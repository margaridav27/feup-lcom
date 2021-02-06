
#include "mouse.h"
#include "i8042.h"
#include <lcom/lcf.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

int hook_id_mouse = MOUSE_IRQ;
bool err = false;
extern uint8_t data;
extern struct packet pack;
extern unsigned int counter;
extern uint8_t mouse_byte;

int(KBC_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id_mouse;
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse) != OK) {
    printf("error in sys_irqsetpolicy\n");
    return 1;
  }
  return 0;
}

int(KBC_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id_mouse) != OK) {
    printf("error in sys_irqrmpolicy\n");
    return 1;
  }
  return 0;
}

void(mouse_ih)() {

  //there is no need to check the obf because if an interruption has been received == the obf is full
  //we will check it anyway, who cares about eficiency
  uint8_t stat;
  util_sys_inb(KBC_CMD_REG, &stat);
  if (stat & OBF) //OBF FULL
  {
    if (((stat & (PARITY | TIMEOUT)) != 0) || (stat & AUX) == 0) { //check errors && make sure it was a mouse interruption other than a keyboard (both kbc peripherals)
      err = true;
    }
    err = false;
    util_sys_inb(KBC_OUT_BUF, &mouse_byte); // Read data
  }
}

void(get_pack_data)() {

  //LEFT BUTTON PRESSED
  if (mouse_byte & LB)
    pack.lb = 1;
  else
    pack.lb = 0;
  //RIGHT BUTTON PRESSED
  if (mouse_byte & RB)
    pack.rb = 1;
  else
    pack.rb = 0;
  if (mouse_byte & MB)
    pack.mb = 1;
  else {
    pack.mb = 0;
  }

  //X OVERFLOW
  if ((mouse_byte & X_OV))
    pack.x_ov = 1;
  else
    pack.x_ov = 0;
  //Y OVERFLOW
  if ((mouse_byte & Y_OV))
    pack.y_ov = 1;
  else
    pack.y_ov = 0;
}

int(write_)(uint8_t port, uint8_t msg) {
  uint8_t stat;
  if (util_sys_inb(KBC_ST_REG, &stat) != OK)
    printf("ERROR reading stat\n");
  if ((stat & IBF) != 0) //Input buffer full == can't write
    return 1;
  if (sys_outb(port, msg) != OK)
    printf("ERROR writing to output buffer\n");
  return 0;
}

int(read_)(uint8_t port, uint8_t *arg) {
  uint8_t stat;
  if (util_sys_inb(KBC_ST_REG, &stat) != OK)
    printf("ERROR reading stat\n");
  if ((stat & OBF) == 0)
    return 1;
  if (util_sys_inb(KBC_OUT_BUF, arg) != OK)
    printf("ERROR reading ouput buffer\n");
  return 0;
}

int mouse_enable_data_report() {
  uint8_t ack_byte;
  while (1) {
    if (write_(KBC_CMD_REG, WRITE_BYTE) == 0) {

      if (write_(KBC_OUT_BUF, ENABLE) == 0) {

        if (read_(KBC_OUT_BUF, &ack_byte) == 0) {

          if (ack_byte == ACK)
            return 0;
        }
      }
    }
    printf("Error enabling\n");
  }
}

int mouse_disable_data_report() {
  uint8_t ack_byte;
  while (1) {
    if (write_(KBC_CMD_REG, WRITE_BYTE) == 0) {

      if (write_(KBC_OUT_BUF, DISABLE) == 0) {

        if (read_(KBC_OUT_BUF, &ack_byte) == 0) {

          if (ack_byte == ACK)
            return 0;
        }
      }
    }
    printf("Error disabling\n");
  }
}

int check_obf() {
  uint8_t stat;
  if (util_sys_inb(KBC_ST_REG, &stat) != OK)
    printf("ERROR reading stat\n");
  if (stat & OBF) {
    if ((stat & (BIT(7) | BIT(6))) == 0)
      return 0;
  }
  return 1;
}

void get_byte() {

  if (counter == 0) {
    do {
      read_(KBC_OUT_BUF, &mouse_byte);
    } while ((mouse_byte & BIT(3)) >> 3 == 0); //bit 3 of byte 0 must be 1
                                               //so while we keep reading bytes with bit 3 set to 0, we can be sure that is not byte 0
    if (mouse_byte & BIT(0)) {
      pack.lb = 1;
    }
    else {
      pack.lb = 0;
    }
    if ((mouse_byte & BIT(1)) >> 1) {
      pack.rb = 1;
    }
    else {
      pack.rb = 0;
    }
    if ((mouse_byte & BIT(2)) >> 2) {
      pack.mb = 1;
    }
    else {
      pack.mb = 0;
    }
    if ((mouse_byte & BIT(6)) >> 6) {
      pack.x_ov = 1;
    }
    else {
      pack.x_ov = 0;
    }
    if ((mouse_byte & BIT(7)) >> 7) {
      pack.y_ov = 1;
    }
    else {
      pack.y_ov = 0;
    }
  }
  else if (counter == 1) {
    read_(KBC_OUT_BUF, &mouse_byte);
    if ((pack.bytes[0] & BIT(4)) >> 4 == 0) {
      pack.delta_x = 0x0000 | mouse_byte;
    }
    else {
      pack.delta_x = 0xFF00 | mouse_byte;
    }
  }
  else if (counter == 2) {
    read_(KBC_OUT_BUF, &mouse_byte);
    if ((pack.bytes[0] & BIT(5)) >> 5 == 0) {
      pack.delta_y = 0x0000 | mouse_byte;
    }
    else {
      pack.delta_y = 0xFF00 | mouse_byte;
    }
  }
}

void read_packet() {
  if (check_obf() == 0) {
    counter = 0;
    get_byte();
    pack.bytes[0] = mouse_byte;
    counter++;
    get_byte();
    pack.bytes[1] = mouse_byte;
    counter++;
    get_byte();
    pack.bytes[2] = mouse_byte;
  }
}

int request_packet() {
  uint8_t ack_byte;
  if (write_(WRITE_BYTE, READ_DATA) == 0) {
    if (read_(KBC_OUT_BUF, &ack_byte) == 0) {
      printf("I read the ack\n");
      if (ack_byte == ACK)
        return 0;
    }
  }
  return 1;
}

void mouse_event(struct packet *pack, ev_type_t *ev) {
  if (pack->lb && !pack->mb && !pack->rb)
    *ev = LBDOWN;
  else if (!pack->lb && !pack->mb && pack->rb)
    *ev = RBDOWN;
  else if (!pack->lb && !pack->mb && !pack->rb)
    *ev = BUP;
  else if (!pack->lb && pack->mb && !pack->rb)
    *ev = MBDOWN;
}

bool draw_v(struct packet *pack, uint8_t x_len, uint8_t tol, ev_type_t *ev) {
  static uint16_t x = 0, y = 0;
  static state_t state = INIT;

  switch (state) {
    case INIT: {
      x = 0;
      y = 0;
      if (*ev == LBDOWN) {
        printf("1\n");
        state = DRAWUP;
      }
      break;
    }
    case DRAWUP: {
      if (*ev == BUP) {
        if (x >= x_len && fabs((y / (float) x)) > 1) {
          state = VERTICE;
        }
        else {
          state = INIT;
        }
      }
      else if (*ev == LBDOWN) {
        if ((pack->delta_x > 0 && pack->delta_y > 0) || (abs(pack->delta_y) <= tol && abs(pack->delta_x) <= tol)) {
          x += pack->delta_x;
          y += pack->delta_y;
        }
        else {
          state = INIT;
        }
      }
      break;
    }
    case VERTICE: {
      x = 0;
      y = 0;
      if (*ev == LBDOWN && pack->delta_x == 0 && pack->delta_y == 0)
        state = DRAWUP;
      else if ((pack->delta_x != 0 && pack->delta_y != 0)) {
        state = INIT;
      }
      else if (*ev == RBDOWN) {
        state = DRAWDOWN;
      }
      else {
        state = INIT;
      }

      break;
    }
    case DRAWDOWN: {
      if (*ev == BUP) {
        if (x >= x_len && fabs(y / (float) x) > 1) {
          return true;
        }
        else {
          state = INIT;
        }
      }
      else if (*ev == RBDOWN) {
        if ((pack->delta_x > 0 && pack->delta_y > 0) || (abs(pack->delta_y) <= tol && abs(pack->delta_x) <= tol)) {
          x += pack->delta_x;
          y += pack->delta_y;
        }
        else {
          state = INIT;
        }
      }
      break;
    }
  }
  return false;
}