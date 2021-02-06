#include "mouse.h"
#include "kbc.h"
#include "macros.h"
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

int mouse_hook_id = MOUSE_IRQ;

extern bool mouse_err;
extern uint8_t mouse_data;
extern uint8_t mouse_byte;
extern struct packet mouse_packet;
extern unsigned int mouse_byte_tracker;

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

int check_obf() {
  uint8_t stat;
  if (util_sys_inb(KBC_ST_REG, &stat) != OK)
    printf("ERROR reading stat\n");
  if (stat & OBF) {
    return 1;
  }
  return 0;
}

void(mouse_ih)() {

  uint8_t stat;
  util_sys_inb(KBC_CMD_REG, &stat);
  if (stat & OBF) //output buffer full, something to read
  {
    if (((stat & (PARITY | TIMEOUT)) != 0) || (stat & AUX) == 0) { //checking errors and making sure it was a mouse interruption other than a keyboard (both kbc peripherals)
      mouse_err = true;
    }
    else {

      mouse_err = false;
      util_sys_inb(KBC_OUT_BUF, &mouse_byte);
    }                                                                 //reading data
    if (mouse_byte_tracker == 0 && (mouse_byte & BIT(3)) >> 3 == 0) { //mouse is not in sync because we know for sure the the bit 3 of mouse's 1st byte is set
      mouse_err = true;
    }
  }
  else {
    mouse_err = true;
  }
}

void mouse_process_byte() {
  if (mouse_byte_tracker == 0) {
    //LEFT BUTTON PRESSED
    if (mouse_byte & LB)
      mouse_packet.lb = 1;
    else
      mouse_packet.lb = 0;

    //RIGHT BUTTON PRESSED
    if (mouse_byte & RB)
      mouse_packet.rb = 1;
    else
      mouse_packet.rb = 0;

    //MIDDLE BUTTON PRESSED
    if (mouse_byte & MB)
      mouse_packet.mb = 1;
    else
      mouse_packet.mb = 0;

    //X OVERFLOW
    if ((mouse_byte & X_OV))
      mouse_packet.x_ov = 1;
    else
      mouse_packet.x_ov = 0;

    //Y OVERFLOW
    if ((mouse_byte & Y_OV))
      mouse_packet.y_ov = 1;
    else
      mouse_packet.y_ov = 0;
  }
  else if (mouse_byte_tracker == 1) {
    bool negative_sign = (mouse_packet.bytes[0] & BIT(4)) >> 4;
    //X SIGN EXTENSION HANDLER
    if (negative_sign) {
      mouse_packet.delta_x = 0xFF00 | mouse_byte;
    }
    else {
      mouse_packet.delta_x = 0x0000 | mouse_byte;
    }
  }
  else if (mouse_byte_tracker == 2) {
    bool negative_sign = (mouse_packet.bytes[0] & BIT(5)) >> 5;
    //Y SIGN EXTENSION HANDLER
    if (negative_sign) {
      mouse_packet.delta_y = 0xFF00 | mouse_byte;
    }
    else {
      mouse_packet.delta_y = 0x0000 | mouse_byte;
    }
  }
  mouse_packet.bytes[mouse_byte_tracker] = mouse_byte;
}

int mouse_write(uint8_t cmd) {
  uint8_t ack_byte;
  unsigned int tries = 10;
  while (tries > 0) {
    if (kbc_write(KBC_CMD_REG, WRITE_BYTE) == 0) {
      if (kbc_write(KBC_OUT_BUF, cmd) == 0) {
        if (kbc_read(&ack_byte) == 0) {
          if (ack_byte == ACK)
            return 0;
        }
      }
    }
    tries--;
  }
  printf("Error writing to mouse.\n");
  return 1;
}

int mouse_enable_data_report() {
  uint8_t ack_byte;
  do {
    if (kbc_write(KBC_CMD_REG, WRITE_BYTE) == OK) {
      if (kbc_write(KBC_OUT_BUF, ENABLE) == OK) {
        kbc_read(&ack_byte);
      }
    }
  } while (ack_byte != ACK);
  return 0;
}

int mouse_disable_data_report() {
  uint8_t ack_byte;
  do {
    if (kbc_write(KBC_CMD_REG, WRITE_BYTE) == OK) {
      if (kbc_write(KBC_OUT_BUF, DISABLE) == OK) {
        kbc_read(&ack_byte);
      }
    }
  } while (ack_byte != ACK);
  return 0;
}
