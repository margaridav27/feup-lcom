#ifndef KEYBOARD
#define KEYBOARD

#include <lcom/lcf.h>

int(KBC_subscribe_int)(uint8_t *bit_no);
int(KBC_unsubscribe_int)();
int read_data(uint8_t *data);
void(kbc_ih)();

#endif
