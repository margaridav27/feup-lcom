#include "mouse.h"
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

int timer_counter = 0;
uint8_t byte;
unsigned byte_no = 0;
bool err = false;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(mouse_test_packet)(uint32_t cnt) {
  enable_data_reporting();

  uint8_t mouse_bit_no;
  mouse_subscribe_int(&mouse_bit_no);
  uint32_t mouse_irq_set = BIT(mouse_bit_no);

  int r;
  int ipc_status;
  message msg;

  struct packet pack;
  uint32_t received = 0;

  while (received < cnt) {
    while (byte_no < 3) {
      if ((r = driver_receive(ANY, &msg, &ipc_status))) {
        printf("driver_receive failed with: %d", r);
        continue;
      }

      if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE:
            if (msg.m_notify.interrupts & mouse_irq_set) {
              mouse_ih();
              if (err) {
                mouse_unsubscribe_int();
                disable_data_reporting();
                return 1;
              }
              pack.bytes[byte_no] = byte;
              byte_no++;
            }
        }
      }
    }
    parse_packet(&pack);
    mouse_print_packet(&pack);
    received++;
    byte_no = 0;
  }
  mouse_unsubscribe_int();
  disable_data_reporting();
  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  enable_data_reporting();

  uint8_t mouse_bit_no;
  mouse_subscribe_int(&mouse_bit_no);
  uint32_t mouse_irq_set = BIT(mouse_bit_no);

  uint8_t timer_bit_no;
  timer_subscribe_int(&timer_bit_no);
  uint32_t timer_irq_set = BIT(timer_bit_no);

  int r;
  int ipc_status;
  message msg;
  int secs_counter = 0;

  struct packet pack;

  while (secs_counter < idle_time) {
    while (byte_no < 3) {
      if ((r = driver_receive(ANY, &msg, &ipc_status))) {
        printf("driver_receive failed with: %d", r);
        continue;
      }

      if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE:
            if (msg.m_notify.interrupts & timer_irq_set) {
              timer_int_handler();
              if ((timer_counter % 60) == 0) {
                secs_counter++;
                if (secs_counter == idle_time) {
                  timer_unsubscribe_int();
                  mouse_unsubscribe_int();
                  disable_data_reporting();
                  return 0;
                }
              }
            }
            if (msg.m_notify.interrupts & mouse_irq_set) {
              secs_counter = 0;
              timer_counter = 0;
              mouse_ih();
              if (err) {
                timer_unsubscribe_int();
                mouse_unsubscribe_int();
                disable_data_reporting();
                return 1;
              }
              pack.bytes[byte_no] = byte;
              byte_no++;
            }
        }
      }
    }
    parse_packet(&pack);
    mouse_print_packet(&pack);
    byte_no = 0;
  }
  timer_unsubscribe_int();
  mouse_unsubscribe_int();
  disable_data_reporting();
  return 0;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  enable_data_reporting();

  uint8_t mouse_bit_no;
  mouse_subscribe_int(&mouse_bit_no);
  uint32_t mouse_irq_set = BIT(mouse_bit_no);

  int r;
  int ipc_status;
  message msg;

  mouse_state_t mouse_state = INITIAL;
  mouse_event_t mouse_event;
  struct packet pack;
  uint8_t displacement = 0;

  while (mouse_state != COMPLETE) {
    while (byte_no < 3) {
      if ((r = driver_receive(ANY, &msg, &ipc_status))) {
        printf("driver_receive failed with: %d", r);
        continue;
      }

      if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE:
            if (msg.m_notify.interrupts & mouse_irq_set) {
              mouse_ih();
              if (err) {
                mouse_unsubscribe_int();
                disable_data_reporting();
                return 1;
              }
              pack.bytes[byte_no] = byte;
              byte_no++;
            }
        }
      }
    }
    parse_packet(&pack);
    mouse_print_packet(&pack);
    byte_no = 0;
    event_handler(mouse_state, &mouse_event, &pack, x_len, tolerance, &displacement);
    state_machine(&mouse_state, mouse_event, x_len, displacement);
  }
  mouse_unsubscribe_int();
  disable_data_reporting();
  return 0;
}
