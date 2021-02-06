#include "keyboard.h"
#include "i8042.h"
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

int kbc_hook_id = KBC_IRQ;
bool error;
uint8_t code;

int kbc_subscribe_int(uint8_t *bit_no) {
  *bit_no = kbc_hook_id;
  if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbc_hook_id) != OK) {
    printf("sys_irqsetpolicy failed\n");
    return 1;
  }
  return 0;
}

int kbc_unsubscribe_int() {
  if (sys_irqrmpolicy(&kbc_hook_id) != OK) {
    printf("sys_irqrmpolicy failed\n");
    return 1;
  }
  return 0;
}

int read_(uint8_t port, uint8_t *data) {
  uint8_t status;
  if (util_sys_inb(KBC_ST_REG, &status) != OK) {
    printf("read_ failed - could not read the status register\n");
    return 1;
  }
  if (status & KBC_OBF) {
    if (util_sys_inb(port, data) != OK) {
      printf("read_ failed - could not read the data\n");
      return 1;
    }
    if (status & (KBC_PAR_ERR | KBC_TO_ERR | AUX)) {
      printf("read_ failed - data was corrputed\n");
      return 1;
    }
    return 0;
  }
  printf("read_ failed - output buffer was empty\n");
  return 1;
}

int write_(uint8_t port, uint8_t data) {
  uint8_t status;
  if (util_sys_inb(KBC_ST_REG, &status) != OK) {
    printf("write_ failed - could not read the status register\n");
    return 1;
  }
  if (status & KBC_IBF) {
    printf("write_ failed - input buffer was not empty\n");
    return 1;
  }
  if (sys_outb(port, data) != OK) {
    printf("write_ failed - could not write the data\n");
    return 1;
  }
  return 0;
}

void(kbc_ih)() {
  error = false;
  if (read_(KBC_OUT_BUF, &code) != 0) {
    error = true;
  }
}

int issue_cmd(uint8_t command) {
  return write_(KBC_CMD_REG, command);
}

int read_cmd(uint8_t *command) {
  int tries = 0;
  while (tries < 20) {
    if (issue_cmd(READ_CB) == 0) {
      if (read_(KBC_OUT_BUF, command) == 0) {
        return 0;
      }
    }
    tries++;
    tickdelay(micros_to_ticks(WAIT_KBC));
  }
  printf("read_cmd failed\n");
  return 1;
}

int write_cmd(uint8_t command) {
  int tries = 0;
  while (tries < 20) {
    if (issue_cmd(WRITE_CB) == 0) {
      if (write_(KBC_IN_BUF, command) == 0) {
        return 0;
      }
    }
    tries++;
    tickdelay(micros_to_ticks(WAIT_KBC));
  }
  printf("write_cmd failed\n");
  return 1;
}

int read_scancode() {
  int tries = 0;
  while (tries < 20) {
    if (read_(KBC_OUT_BUF, &code) == 0) {
      return 0;
    }
    tries++;
    tickdelay(micros_to_ticks(WAIT_KBC));
  }
  return 1;
}
