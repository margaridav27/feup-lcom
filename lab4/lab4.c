#include "i8042.h"
#include "mouse.h"
#include <lcom/lab4.h>
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
extern bool err;
unsigned int counter = 0;
int count;
uint8_t data;
struct packet pack;
uint8_t mouse_byte;

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
  mouse_enable_data_report();

  int ipc_status;
  message msg;
  uint8_t bit_no;
  KBC_subscribe_int(&bit_no);
  uint32_t KBC_irq_set = BIT(bit_no);
  int r;
  int byte = 0;
  while (cnt > 0) {
    if ((r = driver_receive(ANY, &msg, &ipc_status))) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {       //received notification
      switch (_ENDPOINT_P(msg.m_source)) { //find notifications source
        case HARDWARE:                     //hardware notification
          if (msg.m_notify.interrupts & KBC_irq_set) {

            mouse_ih();
            if (err)
              continue;
            if (byte == 0) //First byte of the pack
            {
              if ((mouse_byte & BIT(3)) == 0) //not the first byte, keep searching
                continue;

              get_pack_data();
            }
            else if (byte == 1) {
              if ((pack.bytes[0] & BIT(4)) >> 4 == 0) {
                pack.delta_x = 0x0000 | mouse_byte;
              }
              else {
                pack.delta_x = 0xFF00 | mouse_byte;
              }
            }
            else if (byte == 2) {
              if ((pack.bytes[0] & BIT(5)) >> 5 == 0) {
                pack.delta_y = 0x0000 | mouse_byte;
              }
              else {
                pack.delta_y = 0xFF00 | mouse_byte;
              }
            }
            pack.bytes[byte] = mouse_byte;
            byte++;
            //Check pack complete
            if (byte == 3) {
              cnt--;
              byte = 0;
              mouse_print_packet(&pack);
            }
          }
      }
    }
  }
  KBC_unsubscribe_int();
  mouse_disable_data_report();
  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {

  mouse_enable_data_report();

  int ipc_status;
  message msg;

  uint8_t bit_no_timer;
  timer_subscribe_int(&bit_no_timer); //subscribe timer interrupts
  uint32_t TIMER_irq_set = BIT(bit_no_timer);

  uint8_t bit_no;
  KBC_subscribe_int(&bit_no); //subscribe mouse interrupts
  uint32_t KBC_irq_set = BIT(bit_no);

  unsigned int second = 0;
  count = 0;

  while (second < idle_time) {
    while (counter < 3) {
      int r;
      if ((r = driver_receive(ANY, &msg, &ipc_status))) {
        printf("driver_receive failed with: %d", r);
        continue;
      }

      if (is_ipc_notify(ipc_status)) { //received notification
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: //hardware interrupt notification
            if (msg.m_notify.interrupts & TIMER_irq_set) {
              timer_int_handler();
              if (!(count % 60)) {
                second++;
                printf("%d", second);
                if (second == idle_time) {
                  KBC_unsubscribe_int();
                  mouse_disable_data_report();
                  return 0;
                }
              }
            }
            if (msg.m_notify.interrupts & KBC_irq_set) {
              second = 0;
              count = 0;
              mouse_ih();
              if (err)
                return 1;
              get_byte();
              pack.bytes[counter] = mouse_byte;
              counter++;
            }
            break;
        }
      }
    }
    counter = 0;
    mouse_print_packet(&pack); //display the packets contents
  }
  timer_unsubscribe_int();
  KBC_unsubscribe_int();
  mouse_disable_data_report();
  return 0;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  uint8_t numPacketsToReceive = cnt;
  while (numPacketsToReceive > 0) {
    tickdelay(micros_to_ticks(period * 1000));
    if (write_(KBC_ST_REG, READ_DATA) == 0) {
      read_packet();
      numPacketsToReceive--;
      mouse_print_packet(&pack); //display the packets content
    }
    tickdelay(micros_to_ticks(period * 1000));
  }
  write_(KBC_ST_REG, STREAM); //sets kbc back to stream mode
  uint8_t reset_command;
  mouse_disable_data_report();
  reset_command = minix_get_dflt_kbc_cmd_byte();
  sys_outb(KBC_ST_REG, WRITE_CB);       //informing to 0x64 i am writing a command byte (through 0x60)
  sys_outb(KBC_OUT_BUF, reset_command); //writing in 0x60 the command byte that resets kbc to its default value
  return 0;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {

  int ipc_status;
  message msg;

  uint8_t bit_no;
  mouse_enable_data_report();
  KBC_subscribe_int(&bit_no); //subscribe the mouse interrupts
  uint32_t KBC_irq_set = BIT(bit_no);

  ev_type_t event;
  bool complete = false;

  while (!complete) {
    while (counter < 3) {
      int r;
      if ((r = driver_receive(ANY, &msg, &ipc_status))) {
        printf("driver_receive failed with: %d", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) {       //received notification
        switch (_ENDPOINT_P(msg.m_source)) { //find notifications source
          case HARDWARE:                     //hardware notification
            if (msg.m_notify.interrupts & KBC_irq_set) {
              mouse_ih();
              if (err)
                return 1;
              get_byte();
              pack.bytes[counter] = mouse_byte;
              counter++;
            }
            break;
        }
      }
    }
    counter = 0;
    mouse_event(&pack, &event);
    complete = draw_v(&pack, x_len, tolerance, &event);
    mouse_print_packet(&pack);
  }
  KBC_unsubscribe_int();
  mouse_disable_data_report();
  return 0;
}