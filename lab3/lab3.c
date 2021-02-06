#include "i8042.h"
#include "keyboard.h"
#include <lcom/lab3.h>
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>


extern uint8_t data;
extern bool err;
unsigned int counter = 0;
int count = 0;
bool secondbyte = false;


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {

  uint8_t bit_no;
  KBC_subscribe_int(&bit_no);

  int ipc_status;
  message msg;

  uint32_t KBC_irq_set = BIT(bit_no);
  bool make;
  uint8_t size = 1;    //usually a scancode is 1byte long
  uint8_t scancode[2]; //a scancode might be 2bytes long

  while (data != BREAK_ESC) {

    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status))) { //checks if there is a message
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { //received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: //hardware interrupt notification
          if (msg.m_notify.interrupts & KBC_irq_set) {

            kbc_ih(); //handles interruption from the keyboard

            if (err)
              return 1; //invalid information
            ;
            if (data == 0xE0) { //the 1st byte of two byte scancodes is usually 0xE0
              scancode[1] = data;
              size = 2;
              kbc_ih();
              scancode[0] = data;
            }
            else {
              scancode[1] = 0;
              scancode[0] = data;
              size = 1;
            }
            //a key break code is a make code of that same key with the MSB (most significant bit) set to 1
            //if a scancode is 2byte long, only the least significant byte matters, because make and break codes had the most significant byte in common, i.e. 0xE0
            make = ((scancode[0] & BIT(7)) >> 7);
            kbd_print_scancode(!make, size, scancode);
          }
          break;
      }
    }
  }
  kbd_print_no_sysinb(count); //this function prints the number of sys_inb kernel calls
  KBC_unsubscribe_int();
  return 0;
}

int(kbd_test_poll)() {
  count = 0;
  bool make;
  uint8_t size = 1;
  uint8_t scancode[2];

  while (data != BREAK_ESC) {
    if (read_scancode() == 0) {
      if (data == 0xE0) {
        scancode[1] = data;
        size = 2;
        if (read_scancode() == 0) {
          scancode[0] = data;
        }
      }
      else {
        scancode[1] = 0;
        scancode[0] = data;
        size = 1;
      }
      make = ((scancode[0] & BIT(7)) >> 7);
      kbd_print_scancode(!make, size, scancode);
    }
  }
  kbd_print_no_sysinb(count);
  search_command();
  restore_command();
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {

  bool make;
  uint8_t size = 1;
  uint8_t scancode[2];

  //int second = 0;

  uint8_t bit_no_timer;
  timer_subscribe_int(&bit_no_timer); //subscribe interruptions from timer
  uint32_t TIMER_irq_set = BIT(bit_no_timer);

  uint8_t bit_no;
  KBC_subscribe_int(&bit_no); //subscribe interruptions from KBC
  uint32_t KBC_irq_set = BIT(bit_no);

  int ipc_status;
  message msg;

  while (data != BREAK_ESC && counter < n * 60) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status))) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { //received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                   //hardware interrupt notification
          if (msg.m_notify.interrupts & TIMER_irq_set) { //timer interrupt notification
            timer_int_handler();
            //if (!(counter % 60))
            // second++;
          }
          if (msg.m_notify.interrupts & KBC_irq_set) { //kbc interrupt notification
            //second = 0;
            counter = 0;
            kbc_ih(); //handles interruption from the keyboard

            if (err)
              return 1; //invalid information

            if (data == 0xE0) { //the 1st byte of two byte scancodes is usually 0xE0
              scancode[1] = data;
              size = 2;
              secondbyte = true;
              kbc_ih();
              scancode[0] = data;
              secondbyte = false;
            }
            else {
              scancode[1] = 0;
              scancode[0] = data;
              size = 1;
            }
            //a key break code is a make code of that same key with the MSB (most significant bit) set to 1
            //if a scancode is 2byte long, only the least significant byte matters, because make and break codes had the most significant byte in common, i.e. 0xE0
            make = ((scancode[0] & BIT(7)) >> 7);
            kbd_print_scancode(!make, size, scancode);
          }

          break;
      }
    }
  }
  kbd_print_no_sysinb(count); //this function prints the number of sys_inb kernel calls
  KBC_unsubscribe_int();
  return 0;
}
