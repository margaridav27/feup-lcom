#include "mouse.h"
#include "macros.h"
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

uint8_t packet[3];
uint8_t byte_tracker;
struct packet pp;
bool err;
int ticks_counter;

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
  if (mouse_enable_data_report() == 1) {
    printf("mouse_enable_data_report failed\n");
    return 1;
  }

  uint8_t mouse_bit_no;
  mouse_subscribe_int(&mouse_bit_no);
  uint32_t mouse_irq_set = BIT(mouse_bit_no);
  
  int ipc_status, r;
  message msg;

  byte_tracker = 0;

  while (cnt > 0) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & mouse_irq_set) {
          mouse_ih();

          if (err) {
            printf("mouse_ih failed\n");
            mouse_disable_data_report();
            mouse_unsubscribe_int();
            return 1;
          }

          if (byte_tracker == 2) {
            parse_packet();
            mouse_print_packet(&pp);
            byte_tracker = 0;
            cnt--;
          } else {
            byte_tracker++;
          }
        }
        break;
      default:
        break;
      }
    }
  }

  if (mouse_disable_data_report() == 1) {
    printf("mouse_disable_data_report failed\n");
    return 1;
  }
  mouse_unsubscribe_int();
  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  if (mouse_enable_data_report() == 1) {
    printf("mouse_enable_data_report failed\n");
    return 1;
  }

  uint8_t mouse_bit_no;
  mouse_subscribe_int(&mouse_bit_no);
  uint32_t mouse_irq_set = BIT(mouse_bit_no);

  uint8_t timer_bit_no;
  timer_subscribe_int(&timer_bit_no);
  uint32_t timer_irq_set = BIT(timer_bit_no);

  int ipc_status, r;
  message msg;

  byte_tracker = 0;
  ticks_counter = 0;

  while (ticks_counter < idle_time * 60) {
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
        if (msg.m_notify.interrupts & mouse_irq_set) {
          mouse_ih();
          ticks_counter = 0;

          if (err) {
            printf("mouse_ih failed\n");
            mouse_disable_data_report();
            mouse_unsubscribe_int();
            return 1;
          }

          if (byte_tracker == 2) {
            parse_packet();
            mouse_print_packet(&pp);
            byte_tracker = 0;
          } else {
            byte_tracker++;
          }
        }
        break;
      default:
        break;
      }
    }
  }

  if (mouse_disable_data_report() == 1) {
    printf("mouse_disable_data_report failed\n");
    return 1;
  }
  mouse_unsubscribe_int();
  timer_unsubscribe_int();
  return 0;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  byte_tracker = 0;
  while(cnt > 0) {
    poll_packet();
    parse_packet();
    mouse_print_packet(&pp);
    byte_tracker = 0;
    cnt--;
    tickdelay(micros_to_ticks(period * 1000));
  }

  if (mouse_set_mode(STREAM_MODE) == 1) {
    printf("mouse_set_mode failed - could not set STREAM MODE\n");
    return 1;
  }
  if (mouse_disable_data_report() == 1) {
    printf("mouse_disable_data_report failed\n");
    return 1;
  }
  uint8_t reset_cmd = minix_get_dflt_kbc_cmd_byte();
  if (sys_outb(CMD_REG, WRITE_CB) != OK) {
    printf("failed to write the 0x60 (WRITE_CB) command to port 0x64\n");
    return 1;
  }
  if (sys_outb(IN_BUF, reset_cmd) != OK) {
    printf("failed to write the command to port 0x60\n");
    return 1;
  }
  return 0;
}

