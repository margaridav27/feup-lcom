#include "macros.h"
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <stdbool.h>
#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  *lsb = val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  *msb = val >> 8;
  return 0;
}

int(util_sys_inb)(int port, uint8_t *value) {
  uint32_t val32bit;
  if (sys_inb(port, &val32bit)) {
    printf("Error in sys_inb()\n");
    return 1;
  }
  *value = (uint8_t) val32bit;
  return 0;
}
