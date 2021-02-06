#ifndef MOUSE
#define MOUSE

#include <lcom/lcf.h>

bool check_ib();

bool check_ob();

int mouse_write_cmd(uint8_t cmd);

int mouse_enable_data_report();

int mouse_disable_data_report();

int mouse_subscribe_int(uint8_t *bit_no);

int mouse_unsubscribe_int();

int mouse_set_mode(uint8_t mode);

void(mouse_ih)();

int request_byte(uint8_t *byte);

void poll_packet();

void parse_packet();

#endif
