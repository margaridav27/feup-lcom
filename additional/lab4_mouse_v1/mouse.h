#ifndef MOUSE
#define MOUSE

#include <lcom/lcf.h>

typedef enum {
  RB_DOWN,
  LB_DOWN,
  MB_DOWN,
  B_UP,
  UP_MOVEMENT,
  DOWN_MOVEMENT,
} mouse_event_t;

typedef enum {
  INITIAL,
  DRAWING_UP,
  DRAWING_DOWN,
  VERTEX,
  COMPLETE
} mouse_state_t;

int enable_data_reporting();

int disable_data_reporting();

int(mouse_subscribe_int)(uint8_t *bit_no);

int(mouse_unsubscribe_int)();

void(mouse_ih)();

void parse_packet(struct packet *pp);

bool valid_slop(uint16_t dx, uint16_t dy);

void event_handler(mouse_state_t mouse_state, mouse_event_t *mouse_event, struct packet *pp, uint8_t x_len, uint8_t tolerance, uint8_t *displacement);

void state_machine(mouse_state_t *mouse_state, mouse_event_t mouse_event, uint8_t x_len, uint8_t displacement);

#endif
