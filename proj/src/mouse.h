#ifndef MOUSE
#define MOUSE

#include <lcom/lcf.h>

/**
 * @brief Subscribes mouse interrupts by calling sys_irqsetpolicy()
 * @param bit_no
 * @return 0 upon success, 1 otherwise
*/
int(mouse_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes mouse interrupts by calling sys_rmsetpolicy()
 * @return 0 upon success, 1 otherwise
*/
int(mouse_unsubscribe_int)();

/**
 * @brief Handles interrupts from mouse 
*/
void(mouse_ih)();

/**
 * @brief Parses information of the mouse packet received
*/
void mouse_process_byte();

/**
 * @brief Writes data to mouse
 * @param cmd data to be written
 * @return 0 upon success, 1 otherwise
*/
int mouse_write(uint8_t cmd);

/**
 * @brief Checks if the output buffer is full and therefore if there is any data to be read
 * @return 1 if full, 0 otherwise
*/
int check_obf();

/**
 * @brief Enables mouse data reporting
 * @return 0 upon success, 1 otherwise
*/
int mouse_enable_data_report();

/**
 * @brief Disables mouse data reporting
 * @return 0 upon success, 1 otherwise
*/
int mouse_disable_data_report();

#endif
