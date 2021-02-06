#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include <lcom/lab3.h>
#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdbool.h>
#include <stdint.h>

extern bool error;
extern uint8_t code;
int ticks_counter = 0;

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
  uint8_t kbc_bit_no; 
  kbc_subscribe_int(&kbc_bit_no);
  uint32_t kbc_irq_set = BIT(kbc_bit_no);

  int ipc_status, r;
  message msg;

  uint8_t scancode[2];
  uint8_t scancode_size;
  bool two_bytes = false;
  bool make;

  while (code != BREAK_ESC) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & kbc_irq_set) {
          kbc_ih();
          if (error) {
            kbc_unsubscribe_int();
            return 1;
          }

          if (code == 0xE0) {
            two_bytes = true;
            scancode[1] = code;
            scancode_size = 2;
          } else if (two_bytes) {
            scancode[0] = code;
            two_bytes = false;
            make = code & MSB;
            kbd_print_scancode(!make, scancode_size, scancode);
          } else {
            scancode[1] = 0;
            scancode[0] = code;
            scancode_size = 1;
            make = code & MSB;
            kbd_print_scancode(!make, scancode_size, scancode);
          }
        }
        break;
      default:
        break;
      }
    }
  }
  kbc_unsubscribe_int();
  return 0;
}

int(kbd_test_poll)() {
  uint8_t scancode[2];
  uint8_t scancode_size;
  bool two_bytes = false;
  bool make;
  int tries = 0;

  while (code != BREAK_ESC && tries < 30) {
    if (read_(KBC_OUT_BUF, &code) == 0) {
      tries = 0;
      if (code == 0xE0) {
        two_bytes = true;
        scancode[1] = code;
        scancode_size = 2;
      } else if (two_bytes) {
        scancode[0] = code;
        two_bytes = false;
        make = code & MSB;
        kbd_print_scancode(!make, scancode_size, scancode);
      } else {
        scancode[1] = 0;
        scancode[0] = code;
        scancode_size = 1;
        make = code & MSB;
        kbd_print_scancode(!make, scancode_size, scancode);
      }
    }
    else {
      tries++;
    }
  }

  uint8_t command;
  read_cmd(&command); // reading the command byte, which needs to be restored
  command = command & INT; // enabling the keyboard interrupts on the output buffer
  write_cmd(command); // restoring the command 
  return 0; 
}

int(kbd_test_timed_scan)(uint8_t idle) {
  uint8_t kbc_bit_no;
  kbc_subscribe_int(&kbc_bit_no);
  uint32_t kbc_irq_set = BIT(kbc_bit_no);

  uint8_t timer_bit_no;
  timer_subscribe_int(&timer_bit_no);
  uint32_t timer_irq_set = BIT(timer_bit_no);

  int ipc_status, r;
  message msg;

  uint8_t scancode[2];
  uint8_t scancode_size;
  bool two_bytes = false;
  bool make;

  while (code != BREAK_ESC && ticks_counter < idle * 60) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & timer_irq_set) {
          timer_int_handler();
        }

        if (msg.m_notify.interrupts & kbc_irq_set) {
          kbc_ih();
          if (error) {
            timer_unsubscribe_int();
            kbc_unsubscribe_int();
            return 1;
          }

          ticks_counter = 0;

          if (code == 0xE0) {
            two_bytes = true;
            scancode[1] = code;
            scancode_size = 2;
          } else if (two_bytes) {
            scancode[0] = code;
            two_bytes = false;
            make = (code & MSB);
            kbd_print_scancode(!make, scancode_size, scancode);
          } else {
            scancode[1] = 0;
            scancode[0] = code;
            scancode_size = 1;
            make = code & MSB;
            kbd_print_scancode(!make, scancode_size, scancode);
          }
        }
        break;
      default:
        break;
      }
    }
  }
  timer_unsubscribe_int();
  kbc_unsubscribe_int();
  return 0;
}
