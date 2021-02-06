#include "i8042.h"
#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>

extern uint8_t data;
bool err = false;
extern uint32_t count;
int hook_id_kbc = KBC_IRQ;
extern bool secondbyte;
uint8_t command;

int write_comand(uint8_t port, uint8_t cmd) {
  uint8_t stat;
  int n = 0;
  while (n < 10) {
    util_sys_inb(KBC_ST_REG, &stat);
    if ((stat & KBC_IBF) == 0) {
      sys_outb(port, cmd);
      return 0;
    }
    tickdelay(micros_to_ticks(WAIT_KBC));
    n++;
  }
  return 1;
}
int read_(uint8_t port, uint8_t *arg) {
  uint8_t stat;
  int counter = 0;
  while (counter < 10) {
    util_sys_inb(KBC_ST_REG, &stat);
    if ((stat & KBC_OBF)) {

      util_sys_inb(KBC_OUT_BUF, arg);
      if ((stat & (KBC_PAR_ERR | KBC_TO_ERR | AUX)) == 0) {
        return 0;
      }
    }
    tickdelay(micros_to_ticks(WAIT_KBC));
    counter++;
  }
  return 1;
}
int(KBC_subscribe_int)(uint8_t *bit_no) {

  *bit_no = hook_id_kbc;

  if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_kbc)) {
    printf("Error in sys_irqsetpolicy()\n");
    return 1;
  }
  return 0;
}
int(KBC_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id_kbc)) {
    printf("Error in sys_irqrmpolicy.\n");
    return 1;
  }
  return 0;
}
void(kbc_ih)() {
  if (read_(KBC_OUT_BUF, &data) == 0)
    err = false;
  else
    err = true;
}

int issue_command(uint8_t cmd) {
  while (1) {
    if (write_comand(KBC_CMD_REG, cmd) == 0)
      return 0;
  }
  tickdelay(micros_to_ticks(WAIT_KBC));
}

int search_command() {

  int counter = 0;
  issue_command(READ_CB);
  while (counter < 10) {
    if (read_(KBC_OUT_BUF, &command) == 0)
      return 0;
  }
  tickdelay(micros_to_ticks(WAIT_KBC));
  counter++;
  return 1;
}

int read_scancode() {
  if (read_(KBC_OUT_BUF, &data) == 0)
    return 0;
  return 1;
}

int restore_command() {

  int counter = 0;
  while (counter < 10) {
    if (write_comand(KBC_ST_REG, WRITE_CB) == 0) {
      command = command | INT;
      if (write_comand(KBC_OUT_BUF, command) == 0)
        return 0;
      return 1;
    }
    else {
      tickdelay(micros_to_ticks(WAIT_KBC));
      counter++;
    }
  }
  return 1;
}
