#include "macros.h"
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <lcom/timer.h>
#include <stdbool.h>
#include <stdint.h>

extern int timer_counter;
int timer_hook_id = TIMER0_IRQ;

int(timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = timer_hook_id;
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id)) {
    printf("Error in sys_irqsetpolicy()\n");
    return 1;
  }
  return 0;
}

int(timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&timer_hook_id)) {
    printf("Error in sys_irqrmpolicy.\n");
    return 1;
  }
  return 0;
}

void(timer_int_handler)() {
  timer_counter++;
}
