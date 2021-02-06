#ifndef RTC
#define RTC

#include <lcom/lcf.h>
#include "game_ctrl.h"

extern struct time_t curr_time;

/**
 * @brief Subscribes RTC interrupts by calling sys_irqsetpolicy()
 * @param bit_no
 * @return 0 upon success, 1 otherwise
*/
int rtc_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribes RTC interrupts by calling sys_rmsetpolicy()
 * @return 0 upon success, 1 otherwise
*/
int rtc_unsubscribe_int();

/**
 * @brief Writes to RTC port 0x70
 * @param reg data to be written
 * @return 0 upon success, 1 otherwise
*/
int rtc_write_command(uint8_t reg);

/**
 * @brief Enables RTC interrupts of the specified source
 * @param source type of interrupts that one intends to enable
 * @return 0 upon success, 1 otherwise
*/
int rtc_enable_ints(uint8_t source);

/**
 * @brief Disables RTC interrupts of the specified source
 * @param source type of interrupts that one intends to disable
 * @return 0 upon success, 1 otherwise
*/
int rtc_disable_ints(uint8_t source);

/**
 * @brief Reads the time information and outputs it to the proper data structure
 * @return 0 upon success, 1 otherwise
*/
int rtc_read_time();

/**
 * @brief Sets RTC to ouput data in BCD format
 * @return 0 upon success, 1 otherwise
*/
int rtc_set_BCD();

/**
 * @brief Converts value in BCD to binary
 * @param value data to be converted
*/
void BCD_to_binary(uint8_t *value);

uint8_t binary_to_decimal(uint8_t value);

/**
 * @brief Checks if one can read from RTC, meaning bit 7 of register A is not set
 * @return true if allowed, false otherwise
*/
  bool rtc_reading_allowed();

/**
 * @brief Handles interrupts of the specified source from RTC
 * @param type of interrupts that one intends to handle
*/
void rtc_ih(uint8_t source);

#endif
