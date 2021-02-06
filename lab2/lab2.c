#include <lcom/lab2.h>
#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>
extern int counter;


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t st;
  if (timer_get_conf(timer, &st)) {
    printf("Error in timer_get_conf()\n");
    return 1;
  }
  if (timer_display_conf(timer, st, field)) {
    printf("Error in timer_display_conf()\n");
    return 1;
  }
  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  if (timer_set_frequency(timer, freq)) {
    printf("Error in timer_set_frequency()\n");
    return 1;
  }
  return 0;
}

int(timer_test_int)(uint8_t time) {
  uint8_t bit_no;
  int ipc_status;
  message msg;

  timer_subscribe_int(&bit_no);

  uint32_t irq_set = BIT(bit_no); //this is a
  printf("%u", bit_no);
  printf("%X", &bit_no);

  while (time > 0) {
    int r;

    //get a request message
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { //checks if there is a message
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { //received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             //hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set) { //this is 0 if there is an interruption in the timer
            timer_int_handler();
            if (!(counter % 60)) {
              timer_print_elapsed_time(); //this should be invoqued once per sec
              time--;
            }
          }
          break;
      }
    }
  }
  return 0;
}
