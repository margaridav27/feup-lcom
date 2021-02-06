#include "video_gr.h"
#include "IA32.H"
#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include <lcom/lcf.h>
#include <lcom/video_gr.h>
#include <stdint.h>
#include <stdio.h>

static char *video_mem; /* Process (virtual) address to which VRAM is mapped */
static vbe_mode_info_t mode_info;

static unsigned h_res;          /* Horizontal resolution in pixels */
static unsigned v_res;          /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned bytes_per_pixel;
uint16_t modex;
int counter = 0;
extern uint8_t data;
extern bool err;

int(get_mode_info)(uint16_t mode) {
  //Allocate memory
  mmap_t mm;
  lm_alloc(sizeof(vbe_mode_info_t), &mm);

  //VBE CALL

  reg86_t r;
  memset(&r, 0, sizeof(r)); //initialize variable as null

  r.ah = 0x4F;
  r.al = 0x01;
  r.cx = mode;
  r.es = PB2BASE(mm.phys);
  r.di = PB2OFF(mm.phys);
  r.intno = 0x10;

  //BIOS CALL

  if (sys_int86(&r) != OK) {
    lm_free(&mm);
    printf("Error in bios call: get_mode_info() \n");
    return 1;
  }

  memcpy(&mode_info, mm.virt, sizeof(vbe_mode_info_t));
  lm_free(&mm);
  h_res = mode_info.XResolution;
  v_res = mode_info.YResolution;
  modex = mode;
  bits_per_pixel = mode_info.BitsPerPixel;
  bytes_per_pixel = (bits_per_pixel + 7) / 8;
  return 0;
}

int(map_vr)() {

  //THIS FUNCTION IS PROVIDED IN CLASS

  struct minix_mem_range mr;
  unsigned int vram_base; /* VRAM's physical addresss */
  unsigned int vram_size; /* VRAM's size, but you can use
				                         the frame-buffer size, instead */

  int r;

  /* Use VBE function 0x01 to initialize vram_base and vram_size */
  vram_base = mode_info.PhysBasePtr;
  vram_size = h_res * v_res * bytes_per_pixel;

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  if (video_mem == MAP_FAILED)
    panic("couldn't map video memory");
  return 0;
}

int(set_mode)(uint16_t mode) {

  // VBE CALL

  reg86_t r;
  memset(&r, 0, sizeof(r)); // initialize r to 0
  r.ah = 0x4F;
  r.al = 0x02;
  r.bx = 1 << 14 | mode; // set bit 14: linear framebuffer
  r.intno = 0x10;

  // BIOS CALL

  if (sys_int86(&r) != OK) {
    printf("set_vbe_mode: sys_int86() failed \n");
    return 1;
  }

  return 0;
}

void *(vg_init)(uint16_t mode) {

  get_mode_info(mode);
  map_vr();
  set_mode(mode);
  return (void *) mode_info.PhysBasePtr;
}

int(color_pixel)(uint16_t x, uint16_t y, uint32_t color) {
  char *ptr = video_mem;

  //mode 0x110 is defined by 15bit, therefore, bit 15 must be 0
  if (modex == 0x110) {
    color = color & 0x7FFF;
  }

  //acess memory postion
  ptr += (x + h_res * y) * (bytes_per_pixel);

  //paint pixel
  for (unsigned int i = 0; i < (bytes_per_pixel); i++) {
    *ptr = color;
    ptr++;
    color = color >> 8;
  }

  return 0;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {

  while (len > 0) {
    color_pixel(x, y, color);

    x++; //go forward in the xx axis
    len--;
  }

  return 0;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

  while (height > 0) {
    vg_draw_hline(x, y, width, color);

    y++; //go down in the yy axis
    height--;
  }

  return 0;
}

uint32_t(get_color)(uint16_t mode, uint32_t first, uint8_t step, uint16_t x, uint16_t y, uint8_t no_rectangles) {
  if (mode == index_mode)
    return ((first + (y * no_rectangles + x) * step) % (1 << bits_per_pixel));
  else {
    uint8_t red_first, green_first, blue_first, red, green, blue;

    red_first = first >> mode_info.RedFieldPosition % BIT(mode_info.RedMaskSize);
    green_first = first >> mode_info.GreenFieldPosition % BIT(mode_info.GreenMaskSize);
    blue_first = first >> mode_info.BlueFieldPosition % BIT(mode_info.BlueMaskSize);

    red = (red_first + x * step) % (1 << mode_info.RedMaskSize);
    green = (green_first + y * step) % (1 << mode_info.GreenMaskSize);
    blue = (blue_first + (x + y) * step) % (1 << mode_info.BlueMaskSize);

    uint32_t color = (red << mode_info.RedFieldPosition | green << mode_info.GreenFieldPosition | blue << mode_info.BlueFieldPosition);
    return color;
  }
}

int(vg_draw_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  //get size of each square
  uint16_t width = h_res / no_rectangles;
  uint16_t height = v_res / no_rectangles;

  uint32_t color;

  for (unsigned int y = 0; y < no_rectangles; y++) {
    for (unsigned int x = 0; x < no_rectangles; x++) {
      color = get_color(mode, first, step, x, y, no_rectangles);
      vg_draw_rectangle(x * width, y * height, width, height, color);
    }
  }
  return 0;
}

int(vg_xpm_pixel)(uint16_t x, uint16_t y, uint8_t img) {
  char *ptr = video_mem;
  ptr += (x + h_res * y);
  *ptr = (char) img;
  return 0;
}

int(vg_draw_img)(xpm_image_t img, uint16_t x, uint16_t y, uint8_t *map) {
  uint16_t width = img.width;
  uint16_t height = img.height;

  for (unsigned yy = 0; yy < height; yy++) {
    for (unsigned xx = 0; xx < width; xx++) {
      vg_xpm_pixel(x + xx, y + yy, *(map + yy * width + xx)); // Imagine img as the base lego
                                                              // + yy * width, you go up the rows you have passed
                                                              // in the first for
                                                              // + xx, you go to the colum you are in
                                                              // eg:
                                                              // to get to my position i have to (base + 2 * 4 + 2)
                                                              //          --
                                                              //          ----
                                                              //          ----
    }
  }
  return 0;
}

int(vg_move_sprite_h)(xpm_image_t img, uint8_t *map, uint16_t xi, uint16_t xf, uint16_t yi, int16_t speed, uint8_t fr_rate) {

  int ipc_status;
  message msg;
  int r;

  uint8_t KBC_bit_no;
  KBC_subscribe_int(&KBC_bit_no);
  uint32_t KBC_irq_set = BIT(KBC_bit_no);

  uint8_t timer_bit_no;
  timer_subscribe_int(&timer_bit_no);
  uint32_t timer_irq_set = BIT(timer_bit_no);

  uint8_t ticks_per_frame = 60 / fr_rate; //e.g. if fps = 6, that means that for each second (60 ticks) we need to display 6 frames
                                          //therefore, to display 1 frame, we need to wait 10 ticks (10 interruptions from timer)
                                          //also, the frame rate predominates over speed (and that is why it is on the most exterior if)
                                          //the latter only affects how frames are displayed, but not when or how often
  int frames_counter = 0;
  bool left_right = (xf > xi); //determines if movement occurs from left to right (that will affect the increments)
  uint16_t xx = xi;

  while (data != BREAK_ESC) {
    if ((r = driver_receive(ANY, &msg, &ipc_status))) {
      printf("driver_receive failed with: %d", r);
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:

          if (msg.m_notify.interrupts & KBC_irq_set) {
            kbc_ih(); //handles interruptions from keyboard
          }

          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler(); //handles interruptions from timer

            if (counter % ticks_per_frame == 0) {
              if (xx == xf)
                continue;
              if (speed > 0) { //here speed means pixels between consecutive frames
                memset(video_mem, 0, (h_res * v_res * (bits_per_pixel) / 8));
                if (left_right) {
                  if (abs(xf - xx) < speed) {
                    speed = abs(xf - xx);
                  }

                  xx += speed;
                }
                else {
                  if (abs(xf - xx) < speed)
                    speed = abs(xf - xx);

                  xx -= speed;
                }
              }
              else if (speed < 0) { //here speed means the number of frames required for a displacement of one pixel
                frames_counter++;
                if (frames_counter == -speed) { //the number of frames equivalent to the speed had been shown, so now its time to move
                  memset(video_mem, 0, (h_res * v_res * (bits_per_pixel) / 8));
                  if (left_right) {
                    xx += 1;
                  }
                  else {
                    xx -= 1;
                  }
                  frames_counter = 0;
                }
              }
            }

            vg_draw_img(img, xx, yi, map);
          }
          break;
      }
    }
  }

  timer_unsubscribe_int();
  KBC_unsubscribe_int();
  vg_exit();
  return 0;
}

int(vg_move_sprite_v)(xpm_image_t img, uint8_t *map, uint16_t yi, uint16_t yf, uint16_t xi, int16_t speed, uint8_t fr_rate) {

  int ipc_status;
  message msg;
  int r;

  uint8_t KBC_bit_no;
  KBC_subscribe_int(&KBC_bit_no);
  uint32_t KBC_irq_set = BIT(KBC_bit_no);

  uint8_t timer_bit_no;
  timer_subscribe_int(&timer_bit_no);
  uint32_t timer_irq_set = BIT(timer_bit_no);

  uint8_t ticks_per_frame = 60 / fr_rate; //e.g. if fps = 6, that means that for each second (60 ticks) we need to display 6 frames
                                          //therefore, to display 1 frame, we need to wait 10 ticks (10 interruptions from timer)
                                          //also, the frame rate predominates over speed (and that is why it is on the most exterior if)
                                          //the latter only affects how frames are displayed, but not when or how often
  int frames_counter = 0;
  bool up_down = (yf > yi); //determines if movement occurs from up to down (that will affect the increments)
  uint16_t yy = yi;

  while (data != BREAK_ESC) {
    if ((r = driver_receive(ANY, &msg, &ipc_status))) {
      printf("driver_receive failed with: %d", r);
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:

          if (msg.m_notify.interrupts & KBC_irq_set) {
            kbc_ih(); //handles interruptions from keyboard
          }

          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler(); //handles interruptions from timer

            if (counter % ticks_per_frame == 0) {
              if (yy == yf)
                continue;
              if (speed > 0) { //here speed means pixels between consecutive frames
                memset(video_mem, 0, (h_res * v_res * (bits_per_pixel) / 8));
                if (up_down) {
                  if (yy + speed > yf)
                    speed = yf - yy;
                  yy += speed;
                }
                else {
                  if (yy - speed < yf)
                    speed = yy - yf;

                  yy -= speed;
                }
              }
              else if (speed < 0) { //here speed means the number of frames required for a displacement of one pixel
                frames_counter++;
                if (frames_counter == -speed) { //the number of frames equivalent to the speed had been shown, so now its time to move
                  memset(video_mem, 0, (h_res * v_res * (bits_per_pixel) / 8));
                  if (up_down) {
                    if (yy != yf)
                      yy += 1;
                  }
                  else {
                    if (yy != yf)
                      yy -= 1;
                  }
                  frames_counter = 0;
                }
              }
            }

            vg_draw_img(img, xi, yy, map);
          }
          break;
      }
    }
  }

  timer_unsubscribe_int();
  KBC_unsubscribe_int();
  vg_exit();
  return 0;
}
