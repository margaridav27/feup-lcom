#include "keyboard.h"
#include "kbc.h"
#include "macros.h"
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <lcom/timer.h>
#include <stdbool.h>
#include <stdint.h>

int kbc_hook_id = KBC_IRQ;

extern uint32_t kbc_irq_set;
extern uint8_t keyboard_data;
extern bool scancode_2ndbyte;
extern bool keyboard_err;
uint8_t kbc_command;

int(kbc_subscribe_int)(uint8_t *bit_no) {
  *bit_no = kbc_hook_id;
  if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbc_hook_id)) {
    printf("Error in sys_irqsetpolicy()\n");
    return 1;
  }
  return 0;
}

int(kbc_unsubscribe_int)() {
  if (sys_irqrmpolicy(&kbc_hook_id)) {
    printf("Error in sys_irqrmpolicy.\n");
    return 1;
  }
  return 0;
}

void(kbc_ih)() {
  if (kbc_read(&keyboard_data) == 0)
    keyboard_err = false;
  else
    keyboard_err = true;
}

int kbc_issue_command(uint8_t cmd) {
  unsigned int tries = 10;
  while (tries > 0) {
    if (kbc_write(KBC_CMD_REG, cmd) == 0) {
      return 0;
    }
    tries--;
    tickdelay(micros_to_ticks(WAIT_KBC));
  }
  printf("Error issuing command to keyboard.\n");
  return 1;
}

int kbc_get_command() {
  unsigned int tries = 10;
  kbc_issue_command(READ_CB);
  while (tries > 0) {
    if (kbc_read(&kbc_command) == 0) {
      return 0;
    }
    tries--;
    tickdelay(micros_to_ticks(WAIT_KBC));
  }
  printf("Error reading keyboard's command byte.\n");
  return 1;
}

int kbc_restore_command() {
  int tries = 10;
  while (tries > 0) {
    if (kbc_write(KBC_ST_REG, WRITE_CB) == 0) {
      kbc_command = kbc_command | INT;
      if (kbc_write(KBC_OUT_BUF, kbc_command) == 0) {
        return 0;
      }
    }
    tickdelay(micros_to_ticks(WAIT_KBC));
    tries--;
  }
  printf("Error restoring keyboard's command byte.\n");
  return 1;
}

int kbc_read_scancode() {
  if (kbc_read(&keyboard_data) == 0) {
    return 0;
  }
  return 1;
}
