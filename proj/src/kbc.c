#include "kbc.h"
#include "macros.h"
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

int kbc_write(uint8_t port, uint8_t cmd) {
  uint8_t stat;
  int tries = 10;
  while (tries > 0) {
    util_sys_inb(KBC_ST_REG, &stat);
    if ((stat & KBC_IBF) == 0) {
      sys_outb(port, cmd);
      return 0;
    }
    tickdelay(micros_to_ticks(WAIT_KBC));
    tries--;
  }
  return 1;
}

int kbc_read(uint8_t *arg) {
  uint8_t stat;
  int tries = 10;
  while (tries > 0) {
    util_sys_inb(KBC_ST_REG, &stat);
    if ((stat & KBC_OBF)) {
      util_sys_inb(KBC_OUT_BUF, arg);
      if ((stat & (KBC_PAR_ERR | KBC_TO_ERR | AUX)) == 0) {
        return 0;
      }
    }
    tickdelay(micros_to_ticks(WAIT_KBC));
    tries--;
  }
  return 1;
}

bool mouse_int() {
  uint8_t stat;
  util_sys_inb(KBC_ST_REG, &stat);
  if ((stat & AUX) == 0) {
    return 1;
  }
  return 0;
}
