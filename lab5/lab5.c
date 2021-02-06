
#include "IA32.H"
#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include "video_gr.c"
#include "video_gr.h"
#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

uint8_t data = 0;
extern bool err;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");
  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}
int(video_test_init)(uint16_t mode, uint8_t delay) {
  vg_init(mode);
  sleep(delay);
  vg_exit();
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  vg_init(mode);
  vg_draw_rectangle(x, y, width, height, color);

  uint8_t bit_no;
  KBC_subscribe_int(&bit_no);

  int ipc_status;
  message msg;

  uint32_t KBC_irq_set = BIT(bit_no);

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
            kbc_ih();
            if (err)
              return 1;
          }
      }
    }
  }

  KBC_unsubscribe_int();
  vg_exit();
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  vg_init(mode);
  vg_draw_pattern(mode, no_rectangles, first, step);

  uint8_t bit_no;
  KBC_subscribe_int(&bit_no);
  int ipc_status;
  message msg;
  uint32_t KBC_irq_set = BIT(bit_no);

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
            kbc_ih();
            if (err)
              return 1;
          }
      }
    }
  }

  KBC_unsubscribe_int();
  vg_exit();
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  vg_init(index_mode);
  enum xpm_image_type type = XPM_INDEXED;
  xpm_image_t img;
  uint8_t *map = xpm_load(xpm, type, &img);
  vg_draw_img(img, x, y, map);

  uint8_t bit_no;
  KBC_subscribe_int(&bit_no);
  int ipc_status;
  message msg;
  uint32_t KBC_irq_set = BIT(bit_no);

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
            kbc_ih();
            if (err)
              return 1;
          }
      }
    }
  }

  KBC_unsubscribe_int();
  vg_exit();
  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  vg_init(index_mode);
  enum xpm_image_type type = XPM_INDEXED;
  xpm_image_t img;
  uint8_t *map = xpm_load(xpm, type, &img);
  // move_keyboard(img, map);

  if (xi == xf) {
    vg_move_sprite_v(img, map, yi, yf, xi, speed, fr_rate);
  }
  else if (yi == yf) {
    vg_move_sprite_h(img, map, xi, xf, yi, speed, fr_rate);
  }
  return 0;
}

int(video_test_controller)() {
  // To be completed
  printf("%s(): under construction\n", __func__);

  return 1;
}
