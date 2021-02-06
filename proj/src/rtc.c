#include "rtc.h"
#include "macros.h"
#include "math.h"
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <stdbool.h>
#include <stdint.h>

int rtc_hook_id = RTC_IRQ;

int rtc_subscribe_int(uint8_t *bit_no) {
  *bit_no = rtc_hook_id;
  if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &rtc_hook_id) != OK) {
    printf("Error in sys_irqsetpolicy()\n");
    return 1;
  }
  return 0;
}

int rtc_unsubscribe_int() {
  if (sys_irqrmpolicy(&rtc_hook_id) != OK) {
    printf("Error in sys_irqrmpolicy.\n");
    return 1;
  }
  return 0;
}

int rtc_write_command(uint8_t reg) {
  if (sys_outb(RTC_ADDR_REG, reg) != OK) {
    return 1;
  }
  return 0;
}

int rtc_enable_ints(uint8_t source) {
  uint8_t reg = 0;
  if (rtc_write_command(RTC_REG_B) == 0) {
    util_sys_inb(RTC_DATA_REG, &reg);
    reg = reg | source;
    if (rtc_write_command(RTC_REG_B) == 0) {
      sys_outb(RTC_DATA_REG, reg);
      return 0;
    }
  }
  return 1;
}

int rtc_disable_ints(uint8_t source) {
  uint8_t reg = 0;
  if (rtc_write_command(RTC_REG_B) == 0) {
    util_sys_inb(RTC_DATA_REG, &reg);
    reg = reg | !source;
    if (rtc_write_command(RTC_REG_B) == 0) {
      sys_outb(RTC_DATA_REG, reg);
      return 0;
    }
  }
  return 1;
}

int rtc_set_BCD() {
  uint8_t reg = 0;
  if (rtc_write_command(RTC_REG_B) == 0) {
    util_sys_inb(RTC_DATA_REG, &reg);
    reg = reg & !RTC_REG_B_DM;
    if (rtc_write_command(RTC_REG_B) == 0) {
      sys_outb(RTC_ADDR_REG, reg);
      return 0;
    }
  }
  return 1;
}

void BCD_to_binary(uint8_t *value) {
  *value = (*value & 0x0F) + ((*value & 0xF0) >> 4) * 10;
}

uint8_t binary_to_decimal(uint8_t value) {

  uint8_t decimal = 0;
  for (unsigned int i = 0; i < 8; i++) {
    if (value & BIT(i)) {
      decimal += pow(2, i);
    }
  }
  return decimal;
}

bool rtc_reading_allowed() {
  uint8_t reg = 0;
  if (rtc_write_command(RTC_REG_A) == 0) {
    util_sys_inb(RTC_DATA_REG, &reg);
    if (reg & RTC_REG_A_UI) { /*if set can not access time/date registers*/
      return false;
    }
  }
  return true;
}

void rtc_ih(uint8_t source) {
  if (!rtc_reading_allowed()) {
    return;
  }

  uint8_t reg = 0;
  if (rtc_write_command(RTC_REG_C) == 0) {
    util_sys_inb(RTC_DATA_REG, &reg);
    if (reg & source) {
      rtc_read_time();
      return;
    }
  }
}

int rtc_read_time() {
  if (!rtc_reading_allowed()) {
    return 1;
  }
  rtc_set_BCD();

  if (rtc_write_command(RTC_MINUTES) == 0) {
    if (util_sys_inb(RTC_DATA_REG, &curr_time.minutes) != 0) {

      BCD_to_binary(&curr_time.minutes);
      return 0;
    }
  }

  if (rtc_write_command(RTC_HOURS) == 0) {
    if (util_sys_inb(RTC_DATA_REG, &curr_time.hours) != 0) {
      BCD_to_binary(&curr_time.hours);
      return 0;
    }
  }
  return 0;
}
