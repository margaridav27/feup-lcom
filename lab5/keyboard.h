#ifndef KEYBOARD
#define KEYBOARD

#include <lcom/lcf.h>

int(KBC_subscribe_int)(uint8_t *bit_no);
int(KBC_unsubscribe_int)();
void(kbc_ih)();
int search_command();
int issue_command(uint8_t cmd);
int restore_command();
int read_scancode();

#endif
