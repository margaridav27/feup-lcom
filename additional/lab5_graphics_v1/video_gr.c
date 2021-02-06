#include "video_gr.h"
#include "keyboard.h"
#include "macros.h"
#include <lcom/lcf.h>
#include <lcom/video_gr.h>
#include <stdint.h>
#include <stdio.h>

static char *video_mem;
static unsigned h_res;
static unsigned v_res;
static unsigned bits_per_pixel;
static unsigned bytes_per_pixel;
static unsigned vram_size;
static uint8_t red_mask_size, red_field_position;
static uint8_t green_mask_size, green_field_position;
static uint8_t blue_mask_size, blue_field_position;
static uint8_t rsvd_mask_size, rsvd_field_position;
static uint16_t curr_mode;

int timer_counter = 0;
extern uint8_t data;

int map_vram(vbe_mode_info_t mode_info) {
  struct minix_mem_range mr;
  int r;

  mr.mr_base = (phys_bytes) mode_info.PhysBasePtr;
  mr.mr_limit = mr.mr_base + vram_size;

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    return 1;
  }

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  if (video_mem == MAP_FAILED) {
    panic("couldn't map video memory\n");
    return 1;
  }

  return 0;
}

void get_mode_info(uint16_t mode, vbe_mode_info_t *mode_info) {
  mmap_t map;
  lm_alloc(sizeof(mode_info), &map);

  reg86_t reg;
  memset(&reg, 0, sizeof(reg));

  reg.intno = BIOS_INTERRUPT;
  reg.ah = VBE_ACESS;
  reg.al = GET_MODE_INFO; //get vbe mode info
  reg.cx = mode;          //indicates mode from which the information is to be obtained
  reg.es = PB2BASE(map.phys);
  reg.di = PB2OFF(map.phys);

  if (sys_int86(&reg) != OK) {
    printf("sys_int86() failed\n");
    return;
  }

  memcpy(mode_info, map.virt, 256);
  lm_free(&map);
}

int set_mode(uint16_t mode) {
  reg86_t reg;
  memset(&reg, 0, sizeof(reg));

  reg.intno = BIOS_INTERRUPT;
  reg.ah = VBE_ACESS;
  reg.al = SET_MODE;
  reg.bx = LINEAR_FB | mode; //bit 14 of the BX register should be set, in order to set the linear frame buffer model, which facilitates access to VRAM

  if (sys_int86(&reg) != OK) {
    printf("sys_int86() failed\n");
    return 1;
  }
  return 0;
}

void set_info(vbe_mode_info_t mode_info) {
  h_res = mode_info.XResolution;
  v_res = mode_info.YResolution;
  bits_per_pixel = mode_info.BitsPerPixel;
  bytes_per_pixel = (bits_per_pixel + 7) / 8;
  red_mask_size = mode_info.RedMaskSize;
  red_field_position = mode_info.RedFieldPosition;
  green_mask_size = mode_info.GreenMaskSize;
  green_field_position = mode_info.GreenFieldPosition;
  blue_mask_size = mode_info.BlueMaskSize;
  blue_field_position = mode_info.BlueFieldPosition;
  rsvd_mask_size = mode_info.RsvdMaskSize;
  rsvd_field_position = mode_info.RsvdFieldPosition;
  vram_size = h_res * v_res * bytes_per_pixel;
}

void *(vg_init)(uint16_t mode) {
  vbe_mode_info_t mode_info;
  get_mode_info(mode, &mode_info);

  curr_mode = mode;
  set_info(mode_info);

  if (map_vram(mode_info) != 0) {
    printf("map_vram() failed\n");
    return NULL;
  }

  if (set_mode(mode) != 0) {
    printf("mode could not be set\n");
    return NULL;
  }

  return (void *) mode_info.PhysBasePtr;
}

void color_pixel(uint16_t x, uint16_t y, uint32_t color) {
  char *pixel_ptr = video_mem;
  pixel_ptr += (x + h_res * y) * bytes_per_pixel;

  /*if the mode is 0x105, then the resolution is 1024 x 768, 
  color mode is indexed and each pixel takes 1 byte
  thus the argument color must not be larger than 255*/
  if (curr_mode == INDEXED_COLOR) {
    color = color % INDEXED_MASK; //just to make sure that the color given as parameter actually exists (in indexed mode there are 256 representable colors)
  }

  /*R:G:B := 15((1:)5:5:5)*/
  if (curr_mode == 0x110) {
    color = color & 0x7FFF; //need to handle that anoying msb
  }

  for (unsigned i = 0; i < bytes_per_pixel; i++) {
    *pixel_ptr = color;
    pixel_ptr++;
    color = color >> 8;
  }
}

void draw_line(uint16_t x, uint16_t y, uint16_t width, uint32_t color) {
  for (uint16_t x_pos = x; x_pos < x + width; x_pos++) {
    if (x_pos > h_res)
      break;
    color_pixel(x_pos, y, color);
  }
}

void draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (uint16_t y_pos = y; y_pos < y + height; y_pos++) {
    if (y_pos > v_res)
      break;
    draw_line(x, y_pos, width, color);
  }
}

uint32_t get_color(uint8_t no_rectangles, uint32_t first, uint8_t step, uint16_t col, uint16_t row) {
  uint32_t color;

  if (curr_mode == INDEXED_COLOR) {
    color = (first + (row * no_rectangles + col) * step) % (1 << bits_per_pixel);
    return color;
  }

  uint32_t r_first, g_first, b_first, r, g, b;

  r_first = first >> red_field_position % (1 << red_mask_size); //finally i understand this!!!!!
  g_first = first >> green_field_position % (1 << green_mask_size);
  b_first = first >> blue_field_position % (1 << blue_mask_size);

  r = (r_first + col * step) % (1 << red_mask_size);
  g = (g_first + row * step) % (1 << green_mask_size);
  b = (b_first + (col + row) * step) % (1 << blue_mask_size);

  color = ((r << red_field_position) | (g << green_field_position) | (b << blue_field_position));
  return color;
}

void draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step) {

  uint16_t width = h_res / no_rectangles, height = v_res / no_rectangles;
  uint32_t curr_color;

  for (unsigned row = 0; row < no_rectangles; row++) {
    for (unsigned col = 0; col < no_rectangles; col++) {
      curr_color = get_color(no_rectangles, first, step, col, row);
      draw_rectangle(col, row, width, height, curr_color);
    }
  }
}

void fill_xpm_pixel(uint16_t x, uint16_t y, uint8_t filler) {
  char *pixel_ptr = video_mem;
  pixel_ptr += x + h_res * y;
  *pixel_ptr = filler;
}

void display_xpm(xpm_image_t img, uint8_t *bytes, uint16_t x, uint16_t y) {
  uint16_t width = img.width, height = img.height;

  for (uint16_t y_pos = 0; y_pos < height; y_pos++) {
    for (uint16_t x_pos = 0; x_pos < width; x_pos++) {
      fill_xpm_pixel(x + x_pos, y + y_pos, *(bytes + y_pos * width + x_pos));
    }
  }
}

int move_xpm_h(xpm_image_t img, uint8_t *bytes, uint16_t xi, uint16_t xf, uint16_t yi,
               int16_t speed, uint8_t fr_rate) {

  int r;
  int ipc_status;
  message msg;

  uint8_t kbc_bit_no;
  KBC_subscribe_int(&kbc_bit_no);
  uint32_t kbc_irq_set = BIT(kbc_bit_no);

  uint8_t timer_bit_no;
  timer_subscribe_int(&timer_bit_no);
  uint32_t timer_irq_set = BIT(timer_bit_no);

  int ticks_per_frame = 60 / fr_rate;
  int frames_until_move = 0;
  bool left_right = (xf > xi);
  uint16_t x_pos = xi;
  display_xpm(img, bytes, x_pos, yi); //lets just start by just displaying the frame and later we handle the rest

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
          }
          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler();
            if (x_pos != xf) {
              //its time to display another frame
              //also, by using modulo %, there is no need to reset 'timer_counter' to 0
              if (timer_counter % ticks_per_frame == 0) {
                //speed is positive it should be understood as the displacement in pixels between consecutive frames
                if (speed > 0) {                          
                  if (left_right) {
                    //checking boundaries -> if the frame can not be moved the pixels correspondent to speed,
                    if (x_pos + speed > xf) {
                      //lets just move it the pixels that missing until it reaches the final position
                      x_pos += (xf - x_pos);  
                    }
                    else {
                      x_pos += speed;
                    }
                  }
                  else {
                    //checking boundaries -> if the frame can not be moved the pixels correspondent to speed,
                    if (x_pos - speed < xf) {
                      //lets just move it the pixels that missing until it reaches the final position
                      x_pos -= (x_pos - xf);  
                    }
                    else {
                      x_pos -= speed;
                    }
                  }
                }
                //if the speed is negative it should be understood as the number of frames required for a displacement of one pixel
                else if (speed < 0) { 
                  frames_until_move++;
                  //also, by using modulo %, there is no need to reset 'frames_until_move' to 0
                  if (frames_until_move % abs(speed) == 0) { 
                    if (left_right) {
                      x_pos += 1; 
                    }
                    else {
                      x_pos -= 1;
                    }
                  }
                }
                memset(video_mem, 0, h_res * v_res * bytes_per_pixel);
                display_xpm(img, bytes, x_pos, yi);
              }
            }
          }

          break;
      }
    }
  }

  timer_unsubscribe_int();
  KBC_unsubscribe_int();
  return 0;
}

int move_xpm_v(xpm_image_t img, uint8_t *bytes, uint16_t yi, uint16_t yf, uint16_t xi,
               int16_t speed, uint8_t fr_rate) {

  int r;
  int ipc_status;
  message msg;

  uint8_t kbc_bit_no;
  KBC_subscribe_int(&kbc_bit_no);
  uint32_t kbc_irq_set = BIT(kbc_bit_no);

  uint8_t timer_bit_no;
  timer_subscribe_int(&timer_bit_no);
  uint32_t timer_irq_set = BIT(timer_bit_no);

  int ticks_per_frame = 60 / fr_rate;
  int frame_counter = 0;
  bool up_down = (yf > yi);
  uint16_t y_pos = yi;

  display_xpm(img, bytes, xi, y_pos);

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
          }
          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler();
            if (y_pos != yf) {
              if (timer_counter % ticks_per_frame == 0) {
                if (speed > 0) {

                  if (up_down) {
                    if (y_pos + speed > yf) {
                      y_pos += (yf - y_pos);
                    }
                    else {
                      y_pos += speed;
                    }
                  }
                  else {
                    if (y_pos - speed < yf) {
                      y_pos -= (y_pos - yf);
                    }
                    else {
                      y_pos -= speed;
                    }
                  }
                }
                else if (speed < 0) {
                  if (frame_counter % abs(speed) == 0) {
                    if (up_down) {
                      y_pos += 1;
                    }
                    else {
                      y_pos -= 1;
                    }
                  }
                }
                memset(video_mem, 0, h_res * v_res * bytes_per_pixel);
                display_xpm(img, bytes, xi, y_pos);
              }
            }
          }

          break;
      }
    }
  }

  timer_unsubscribe_int();
  KBC_unsubscribe_int();
  return 0;
}
