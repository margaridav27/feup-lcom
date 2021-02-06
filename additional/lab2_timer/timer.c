#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "i8254.h"

int timer_hook_id = TIMER0_IRQ;
int ticks_counter = 0;

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint8_t st;
  timer_get_conf(timer, &st);
  uint8_t lsbyte = st & 0x0F;

  uint8_t control = TIMER_LSB_MSB | lsbyte;
  switch (timer)
  {
  case 0:
    control |= TIMER_SEL0;
    break;
  case 1:
    control |= TIMER_SEL1;
    break;
  case 2:
    control |= TIMER_SEL2;
    break;
  default:
    return 1;
  }

  if (sys_outb(TIMER_CTRL, control) != OK) {
    return 1;
  }

  uint16_t value = TIMER_FREQ / freq;
  uint8_t lsb, msb;
  util_get_LSB(value, &lsb);
  util_get_MSB(value, &msb);

  if (sys_outb(TIMER_0 + timer, lsb) != OK) {
    return 1;
  }
  if (sys_outb(TIMER_0 + timer, msb) != OK) {
    return 1;
  }

  return 0;
}

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

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t command = TIMER_RB_SEL(timer) | !TIMER_RB_STATUS_ | TIMER_RB_COUNT_ | TIMER_RB_CMD;
  if (sys_outb(TIMER_CTRL, command) != OK) {
    return 1;
  }
  if (util_sys_inb(TIMER_0 + timer, st) != OK) {
    return 1;
  }
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,  enum timer_status_field field) {
  union timer_status_field_val config;
  switch (field) {
  case tsf_all:
    config.byte = st;
    break;
  case tsf_initial:
    config.in_mode = (st & TIMER_LSB_MSB) >> 4;
    break;
  case tsf_mode:
    config.count_mode = (st & (BIT(3) | TIMER_SQR_WAVE)) >> 1;
    if (config.count_mode == 6 || config.count_mode == 7) {
      config.count_mode -= 4;
    }
    break;
  case tsf_base:
    config.bcd = st & TIMER_BCD;
    break;
  default:
    break;
  }
 
  timer_print_config(timer, field, config);
  return 0;
}
