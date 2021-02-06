#include "keyboard.h"
#include "macros.h"
#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>

int kbc_hook_id = KBC_IRQ;
uint8_t data = 0;
bool err = false;

int(KBC_subscribe_int)(uint8_t *bit_no) {
  *bit_no = kbc_hook_id;
  if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbc_hook_id)) {
    printf("Error in sys_irqsetpolicy()\n");
    return 1;
  }
  return 0;
}

int(KBC_unsubscribe_int)() {
  if (sys_irqrmpolicy(&kbc_hook_id)) {
    printf("Error in sys_irqrmpolicy.\n");
    return 1;
  }
  return 0;
}

int read_data(uint8_t *data) {
  uint8_t stat;
  int tries = 10;

  while (tries > 0) {
    util_sys_inb(0x64, &stat);
    if ((stat & OBF)) {
      util_sys_inb(0x60, data);
      if ((stat & (PARITY | TIMEOUT | AUX)) == 0) //no errors detected
        return 0;
    }
    tickdelay(micros_to_ticks(WAIT_KBC));
    tries--;
  }
  return 1;
}

void(kbc_ih)() {
  if (read_data(&data) == 0)
    err = false;
  else
    err = true;
}
