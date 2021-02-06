#ifndef KEYBOARD
#define KEYBOARD

#include <lcom/lcf.h>

/**
 * @brief Subscribes KBC interrupts by calling sys_irqsetpolicy()
 * @param bit_no
 * @return 0 upon success, 1 otherwise
*/
int(kbc_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes KBC interrupts by calling sys_rmsetpolicy()
 * @return 0 upon success, 1 otherwise
*/
int(kbc_unsubscribe_int)();

/**
 * @brief Handles interrupts from KBC 
*/
void(kbc_ih)();

int kbc_issue_command(uint8_t cmd);

int kbc_get_command();

int kbc_restore_command();

int kbc_read_scancode();

#endif
