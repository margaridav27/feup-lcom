#include "mouse.h"
#include <lcom/lcf.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define MOUSE_IRQ 12
#define ENABLE_MOUSE 0xF4
#define DISABLE_MOUSE 0xF5
#define ACK 0xFA
#define OBF BIT(0)
#define IBF BIT(1)
#define AUX BIT(5)
#define TIMEOUT BIT(6)
#define PARITY BIT(7)

int mouse_hook_id = MOUSE_IRQ;
extern uint8_t byte;
extern unsigned byte_no;
extern bool err;

int check_ib() {
  uint8_t stat;
  util_sys_inb(0x64, &stat);
  if (stat & IBF) //input buffer full, unable to write
    return 1;
  return 0;
}

int check_ob() {
  uint8_t stat;
  uint8_t trash;
  util_sys_inb(0x64, &stat);
  if (stat & OBF) {                  //output buffer full, something to read
    if (stat & (TIMEOUT | PARITY)) { //corrupted data
      util_sys_inb(0x60, &trash);    //corrupted data needs to be read anyway
      return 1;
    }
    return 0;
  }
  return 1; //output buffer not full, nothing to read
}

/* does not work
int enable_data_reporting() {
  uint8_t cmd;
  int tries = 10;
  while (tries > 0) {
    if (check_ib() == 0) {
      sys_outb(0x64, 0x20); //telling i want to read the command byte

      if (check_ob() == 0) {
        util_sys_inb(0x60, &cmd); //reading the command byte from 0x60
        cmd = cmd | BIT(1);       //editing command byte to enable interrupts from mouse

        if (check_ib() == 0) {
          sys_outb(0x64, 0x60); //telling i want to write the command byte

          if (check_ib() == 0) {
            sys_outb(0x60, cmd); //writing the modified command byte
            return 0;
          }
        }
      }
    }
    tries--;
    tickdelay(micros_to_ticks(20000));
  }
  return 1;
}

int disable_data_reporting() {
  uint8_t cmd;
  int tries = 10;
  while (tries > 0) {
    if (check_ib() == 0) {
      sys_outb(0x64, 0x20); //telling i want to read the command byte

      if (check_ob() == 0) {
        util_sys_inb(0x60, &cmd); //reading the command byte from 0x60
        cmd = cmd | BIT(5);       //editing command byte to disable interrupts from mouse

        if (check_ib() == 0) {
          sys_outb(0x64, 0x60); //telling i want to write the command byte

          if (check_ib() == 0) {
            sys_outb(0x60, cmd); //writing the modified command byte
            return 0;
          }
        }
      }
    }
    tries--;
    tickdelay(micros_to_ticks(20000));
  }
  return 1;
}
*/

/* does not work
int enable_data_reporting() {
  uint8_t response = 0x00;
  int tries = 10;
  while (tries > 0) {
    if (check_ib() == 0) {
      sys_outb(0x64, 0xD4); //writing 'write command' byte, 0xD4
      if (check_ib() == 0) {
        sys_outb(0x60, 0xA8); //enabling mouse
        if (check_ob() == 0) {
          util_sys_inb(0x60, &response); //receiving the acknowledgment byte from 0x60
          if (response == ACK) {
            return 0;
          }
        }
      }
    }
    tries--;
    tickdelay(micros_to_ticks(20000));
  }
  return 0;
}

int disable_data_reporting() {
  uint8_t response = 0x00;
  int tries = 10;
  while (tries > 0) {
    if (check_ib() == 0) {
      sys_outb(0x64, 0xD4); //writing 'write command' byte, 0xD4
      if (check_ib() == 0) {
        sys_outb(0x60, 0xA7); //disabling mouse
        if (check_ob() == 0) {
          util_sys_inb(0x60, &response); //receiving the acknowledgment byte from 0x60
          if (response == ACK) {
            return 0;
          }
        }
      }
    }
    tries--;
    tickdelay(micros_to_ticks(20000));
  }
  return 0;
}
*/

int enable_data_reporting() {
  uint8_t response = 0x00;
  int tries = 10;

  while (tries > 0) {
    if (check_ib() == 0) {
      sys_outb(0x64, 0xD4); //writing 'write command' byte, 0xD4

      if (check_ib() == 0) {
        sys_outb(0x60, ENABLE_MOUSE); //enabling stream mode by writing 0xF4 argument to 0x60

        if (check_ob() == 0) {
          util_sys_inb(0x60, &response); //receiving the acknowledgment byte from 0x60

          if (response == ACK) {
            printf("enable_data_reporting() successfully executed\n");
            return 0;
          }
        }
      }
    }
    tries--;
    tickdelay(micros_to_ticks(20000));
  }
  printf("enable_data_reporting() unsuccessfully executed\n");
  return 1;
}

int disable_data_reporting() {
  uint8_t response = 0x00;
  int tries = 10;

  while (tries > 0) {
    if (check_ib() == 0) {
      sys_outb(0x64, 0xD4); //writing 'write command' byte, 0xD4

      if (check_ib() == 0) {
        sys_outb(0x60, DISABLE_MOUSE); //disabling stream mode by writing 0xF5 argument to 0x60

        if (check_ob() == 0) {
          util_sys_inb(0x60, &response); //receiving the acknowledgment byte from 0x60

          if (response == ACK) {
            printf("enable_data_reporting() successfully executed\n");
            return 0;
          }
        }
      }
    }
    tries--;
    tickdelay(micros_to_ticks(20000));
  }
  printf("enable_data_reporting() unsuccessfully executed\n");
  return 1;
}

int(mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = mouse_hook_id;
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id)) {
    printf("Error in sys_irqsetpolicy()\n");
    return 1;
  }
  return 0;
}

int(mouse_unsubscribe_int)() {
  if (sys_irqrmpolicy(&mouse_hook_id)) {
    printf("Error in sys_irqrmpolicy.\n");
    return 1;
  }
  return 0;
}

void(mouse_ih)() {
  if (byte_no == 0) {
    do {
      if (check_ob() == 0) {
        util_sys_inb(0x60, &byte);
      }
      else { //nothing to read from output buffer
        err = true;
        break;
      }
    } while ((byte & BIT(3)) >> 3 == 0); //bit 3 of first byte of a packet is always set
  }
  else {
    if (check_ob() == 0) {
      util_sys_inb(0x60, &byte);
    }
    else {
      err = true;
    }
  }
}

void parse_packet(struct packet *pp) {
  pp->lb = pp->bytes[0] & BIT(0);
  pp->rb = pp->bytes[0] & BIT(1);
  pp->mb = pp->bytes[0] & BIT(2);
  pp->x_ov = pp->bytes[0] & BIT(6);
  pp->y_ov = pp->bytes[0] & BIT(7);

  if (pp->bytes[0] & BIT(4)) //handling x delta sign extension
    pp->delta_x = 0xFF00 | pp->bytes[1];
  else
    pp->delta_x = 0x0000 | pp->bytes[1];

  if (pp->bytes[0] & BIT(5)) //handling y delta sign extension
    pp->delta_y = 0xFF00 | pp->bytes[2];
  else
    pp->delta_y = 0x0000 | pp->bytes[2];
}

bool valid_slop(uint16_t dx, uint16_t dy) {
  return (abs((float) dy / (float) dx) > 1.0);
}

void event_handler(mouse_state_t mouse_state, mouse_event_t *mouse_event, struct packet *pp, uint8_t x_len, uint8_t tolerance, uint8_t *displacement) {
  if (mouse_state == INITIAL || mouse_state == VERTEX) //i only need to worry about these particular situations in case the mouse is in initial or vertex state
  {
    if (pp->lb && !pp->mb && !pp->rb) {
      if (mouse_state != VERTEX) { //if the mouse is in vertex state, the left button was previously released
        *displacement = 0;         //the released of the left button led the mouse to be in vertex state
      }                            //however, is the left button is again pressed when the mouse is in this particular state
      *mouse_event = LB_DOWN;      //that means the upwards movement is to be continued and so the displacement can not be set to 0
      return;
    }

    if (!pp->lb && !pp->mb && pp->rb) {
      *displacement = 0;
      *mouse_event = RB_DOWN;
      return;
    }

    if (!pp->lb && pp->mb && !pp->rb) { //when the right button is pressed
      *displacement = 0;                //if the mouse is in initial state, nothing will really happen since the gesture just starts upon a left button press
      *mouse_event = MB_DOWN;           //however, if the mouse is in vertex state, pressing the right button means a downwards movement might be about to occur
      return;
    }
  }

  if (!pp->lb && !pp->mb && !pp->rb) {
    *mouse_event = B_UP;
    return;
  }

  if (pp->lb && !pp->mb && !pp->rb && (pp->delta_x > 0 || abs(pp->delta_x) <= tolerance) && (pp->delta_y > 0 || abs(pp->delta_y) <= tolerance) && valid_slop(pp->delta_x, pp->delta_y)) {
    if (pp->delta_x > 0) {
      *displacement += pp->delta_x;
    }
    *mouse_event = UP_MOVEMENT;
    return;
  }

  if (!pp->lb && !pp->mb && pp->rb && (pp->delta_x > 0 || abs(pp->delta_x) <= tolerance) && (pp->delta_y < 0 || pp->delta_y <= tolerance) && valid_slop(pp->delta_x, pp->delta_y)) {
    if (pp->delta_x > 0) {
      *displacement += pp->delta_x;
    }
    *mouse_event = DOWN_MOVEMENT;
    return;
  }
}

void state_machine(mouse_state_t *mouse_state, mouse_event_t mouse_event, uint8_t x_len, uint8_t displacement) {
  switch (*mouse_state) {
    case INITIAL:
      if (mouse_event == LB_DOWN)
        *mouse_state = DRAWING_UP;
      break;
    case DRAWING_UP:
      if (mouse_event == UP_MOVEMENT)
        *mouse_state = DRAWING_UP;
      else if (mouse_event == B_UP && displacement >= x_len)
        *mouse_state = VERTEX;
      else
        *mouse_state = INITIAL;
      break;
    case VERTEX:
      if (mouse_event == LB_DOWN)
        *mouse_state = DRAWING_UP;
      else if (mouse_event == RB_DOWN)
        *mouse_state = DRAWING_DOWN;
      else
        *mouse_state = INITIAL;
      break;
    case DRAWING_DOWN:
      if (mouse_event == DOWN_MOVEMENT)
        *mouse_state = DRAWING_DOWN;
      else if (mouse_event == B_UP && displacement >= x_len)
        *mouse_state = COMPLETE;
      else
        *mouse_state = INITIAL;
      break;
    case COMPLETE: //this state only exists for verification purposes
      break;
  }
}
