#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  *lsb = val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  *msb = val >> 8;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t val32bit;
  if (sys_inb(port, &val32bit)) {
    printf("Error in sys_inb()\n");
    return 1;
  }
  printf("status = 0x", val32bit,"\n");
  *value = (uint8_t)val32bit;   //Truncation won't be a problem in this specific case
  return 0;
}
