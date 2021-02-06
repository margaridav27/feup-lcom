#ifndef KEYBOARD
#define KEYBOARD

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
typedef enum { INIT = 0,
               DRAWUP,
               DRAWDOWN,
               VERTICE } state_t;
typedef enum { RBDOWN = 0,
               LBDOWN,
               BUP,
               MBDOWN } ev_type_t;

int(KBC_subscribe_int)(uint8_t *bit_no);

int(KBC_unsubscribe_int)();

void(mouse_ih)();

int write_(uint8_t port, uint8_t cmd);

int read_(uint8_t port, uint8_t *arg);

void get_pack_data();

void read_packet();
void get_byte();
int check_obf();

int request_packet();
int mouse_enable_data_report();

int mouse_disable_data_report();

void mouse_event(struct packet *pack, ev_type_t *ev);
bool draw_v(struct packet *pack, uint8_t x_len, uint8_t tol, ev_type_t *ev);

#endif