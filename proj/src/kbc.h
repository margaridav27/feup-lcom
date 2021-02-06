#ifndef KBC
#define KBC

#include <lcom/lcf.h>

/**
 * @brief Writes cmd in port register
 * @param bit_no´
 * @param cmd
 * @return 0 if succeed, 1 otherwise
*/
int kbc_write(uint8_t port, uint8_t cmd);

/**
 * @brief Reads from OUTPUT BUFFER and stores it in arg
 * @param arg
 * @return 0 if succeed, 1 otherwise
*/
int kbc_read(uint8_t *arg);

/**
 * @brief checks if the kbc interruption is from the mouse
 * @return true if it is a mouse interruption, 0 otherwise
*/
bool mouse_int();

#endif
