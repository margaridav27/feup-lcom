#include "keyboard.h"
#include "macros.h"
#include "video_gr.h"
#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

extern uint8_t data;
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

int(video_test_controller)() {
  vg_vbe_contr_info_t ctrl_info;
  get_ctrl_info(&ctrl_info);
  vg_display_vbe_contr_info(&ctrl_info);
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  vg_init(mode);
  draw_rectangle(x, y, width, height, color);

  int r;
  int ipc_status;
  message msg;

  uint8_t kbc_bit_no;
  KBC_subscribe_int(&kbc_bit_no);
  uint32_t kbc_irq_set = BIT(kbc_bit_no);

  while (data != BREAK_ESC) {
    if ((r = driver_receive(ANY, &msg, &ipc_status))) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & kbc_irq_set) {
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
  draw_pattern(no_rectangles, first, step);

  int r;
  int ipc_status;
  message msg;

  uint8_t kbc_bit_no;
  KBC_subscribe_int(&kbc_bit_no);
  uint32_t kbc_irq_set = BIT(kbc_bit_no);

  while (data != BREAK_ESC) {
    if ((r = driver_receive(ANY, &msg, &ipc_status))) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & kbc_irq_set) {
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
  vg_init(0x105);

  //xpm_load() reads the xpm and updates 'img' with the image information
  //img stores the information about an image, i.e. type, size, width, height, *bytes
  //*bytes is a pointer to the start of the actual image data
  xpm_image_t img;
  uint8_t *bytes = xpm_load(xpm, XPM_INDEXED, &img);
  display_xpm(img, bytes, x, y);

  int r;
  int ipc_status;
  message msg;

  uint8_t kbc_bit_no;
  KBC_subscribe_int(&kbc_bit_no);
  uint32_t kbc_irq_set = BIT(kbc_bit_no);

  while (data != BREAK_ESC) {
    if ((r = driver_receive(ANY, &msg, &ipc_status))) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & kbc_irq_set) {
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
  vg_init(0x105);
  xpm_image_t img;
  uint8_t *bytes = xpm_load(xpm, XPM_INDEXED, &img);
  if (yi == yf) {
    move_xpm_h(img, bytes, xi, xf, yi, speed, fr_rate);
  }
  else if (xi == xf) {
    move_xpm_v(img, bytes, yi, yf, xi, speed, fr_rate);
  }
  vg_exit();
  return 0;
}
