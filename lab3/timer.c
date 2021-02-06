#include "i8254.h"
#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>

extern int counter;
int hook_id_timer = TIMER0_IRQ;

int(timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id_timer;

  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id_timer)) {
    printf("Error in sys_irqsetpolicy()\n");
    return 1;
  }
  return 0;
}

int(timer_unsubscribe_int)() {

  if (sys_irqrmpolicy(&hook_id_timer)) {
    printf("Error in sys_irqrmpolicy.\n");
    return 1;
  }
  return 0;
}

void(timer_int_handler)() {
  counter++;
}
