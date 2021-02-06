#include "mouse.h"
#include "macros.h"
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

int mouse_hook_id = MOUSE_IRQ;
extern uint8_t packet[3];
extern uint8_t byte_tracker;
extern struct packet pp;
extern bool err;

bool check_ib() { 
  uint8_t st;
  util_sys_inb(ST_REG, &st); 

  if ((st & IBF) != 0) {
    printf("input buffer full\n");
    return false;
  }
  return true;
}

bool check_ob() { 
  uint8_t st, trash;
  util_sys_inb(ST_REG, &st);

  if (st & OBF) {
    if (st & (TO_ERR | PAR_ERR | !AUX)) {
      util_sys_inb(OUT_BUF, &trash);
      printf("corrupted data\n");
      return false;
    }
    return true;
  }
  printf("output buffer empty\n");
  return false;
}

int mouse_write_cmd(uint8_t cmd) {
  uint8_t response;
  do {
    if (check_ib()) {
      if (sys_outb(CMD_REG, WRITE_TO_MOUSE) != OK) {
        printf("mouse_write_cmd failed - could not write command 0xD4 to port 0x64\n");
        return 1;
      }
      if (check_ib()) {
        if (sys_outb(IN_BUF, cmd) != OK) {
          printf("mouse_write_cmd failed - could not write the command to port 0x60\n");
          return 1;
        }
        if (check_ob()) {
          util_sys_inb(OUT_BUF, &response);
        }
      }
    }
  } while (response != ACK);
  return 0;
}

int mouse_enable_data_report() { 
  if (mouse_write_cmd(EN_DR) == 0) return 0;
  printf("mouse_enable_data_report\n");
  return 1;
}

int mouse_disable_data_report() {
  if (mouse_write_cmd(DIS_DR) == 0) return 0;
  printf("mouse_disable_data_report\n");
  return 1;
}

int mouse_subscribe_int(uint8_t *bit_no) {
  *bit_no = mouse_hook_id;
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != OK) {
    printf("sys_irqsetpolicy failed\n");
    return 1;
  }
  return 0;
}

int mouse_unsubscribe_int() {
  if (sys_irqrmpolicy(&mouse_hook_id) != OK) {
    printf("sys_irqrmpolicy failed\n");
    return 1;
  }
  return 0;
}

int mouse_set_mode(uint8_t mode) {
  if (mouse_write_cmd(mode) == 0) return 0;
  printf("mouse_disable_data_report\n");
  return 1;
}

void(mouse_ih)() {
  uint8_t byte;
  err = false;

  if (byte_tracker == 0) {
    do {
      if (!check_ob()) err = true;
      util_sys_inb(OUT_BUF, &byte);
    } while((byte & BIT(3)) >> 3 == 0);
  }
  else {
    util_sys_inb(OUT_BUF, &byte);
  }
  packet[byte_tracker] = byte;
}

int request_byte(uint8_t *byte) {
  do {
    if (mouse_write_cmd(READ_DATA) == 1) {
      printf("mouse_write_cmd failed - could not write command 0xEB (READ DATA)\n");
      return 1;
    }

    if (!check_ob()) {
      printf("request_packet failed - output buffer empty\n");
      return 1;
    }
    util_sys_inb(OUT_BUF, byte);

  } while ((byte_tracker == 0) && ((*byte & BIT(3)) >> 3 == 0));
  return 0;
}

void poll_packet() {
  uint8_t byte;
  do {
    if (request_byte(&byte) == 0) {
      packet[byte_tracker] = byte;
      byte_tracker++;
    }
  } while (byte_tracker <= 2);
}

void parse_packet() {
  pp.bytes[0] = packet[0];
  pp.bytes[1] = packet[1];
  pp.bytes[2] = packet[2];

  pp.lb = packet[0] & BIT(0);
  pp.rb = (packet[0] & BIT(1)) >> 1;
  pp.mb = (packet[0] & BIT(2)) >> 2;
  pp.x_ov = (packet[0] & BIT(6)) >> 6;
  pp.y_ov = (packet[0] & BIT(7)) >> 7;

  uint8_t msb_x = (packet[0] & BIT(4)) >> 4;
  uint8_t msb_y = (packet[0] & BIT(5)) >> 5;

  if (msb_x) pp.delta_x = 0xFF00 | pp.bytes[1];
  else pp.delta_x = 0x0000 | pp.bytes[1];
  
  if (msb_y) pp.delta_y = 0xFF00 | pp.bytes[2];
  else pp.delta_y = 0x0000 | pp.bytes[2];
}
