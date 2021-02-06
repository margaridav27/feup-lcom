#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "i8254.h"

int timer_hook_id = TIMER0_IRQ;
extern int ticks_counter;

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = timer_hook_id;
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id) != OK) {
    printf("sys_irqsetpolicy failed\n");
    return 1;
  }
  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&timer_hook_id) != OK) {
    printf("sys_irqrmpolicy failed\n");
    return 1;
  }
  return 0;
}

void (timer_int_handler)() {
  ticks_counter++;
}
