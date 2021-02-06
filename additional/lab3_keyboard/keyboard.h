#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <lcom/lcf.h>

int kbc_subscribe_int(uint8_t *bit_no);

int kbc_unsubscribe_int();

int read_(uint8_t port, uint8_t *data);

int write_(uint8_t port, uint8_t data);

void(kbc_ih)();

int issue_cmd(uint8_t command);

int read_cmd(uint8_t *command);

int write_cmd(uint8_t command);

#endif
