#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

int(util_sys_inb)(int port, uint8_t *value) {
  uint32_t val32bit;
  sys_inb(port, &val32bit);
  *value = (uint8_t) val32bit;
  return 0;
}
