#include "video_gr.h"
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <stdbool.h>
#include <stdint.h>

static unsigned h_res;
static unsigned v_res;
static unsigned bits_per_pixel;
static unsigned bytes_per_pixel;
static unsigned int vram_size;
static uint16_t game_mode;
uint32_t *main_buf;
uint32_t *aux_buf;

void(get_mode_info)(uint16_t mode, vbe_mode_info_t *mode_info) {

  mmap_t mm;
  lm_alloc(sizeof(mode_info), &mm);

  reg86_t reg;
  memset(&reg, 0, sizeof(reg));

  reg.intno = 0X10;
  reg.ah = 0x4F;
  reg.al = 0x01;
  reg.cx = mode;
  reg.es = PB2BASE(mm.phys);
  reg.di = PB2OFF(mm.phys);

  if (sys_int86(&reg) != OK) {
    printf("set_vbe_mode: sys_int86() failed\n");
  }

  memcpy(mode_info, mm.virt, sizeof(*mode_info));
  lm_free(&mm);
}

int(map_vr)(vbe_mode_info_t mode_info) {

  struct minix_mem_range mr;
  mr.mr_base = (phys_bytes) mode_info.PhysBasePtr;
  mr.mr_limit = mr.mr_base + vram_size;

  int r;
  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  main_buf = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  if (main_buf == MAP_FAILED) {
    panic("couldn’t map video memory\n");
    return 1;
  }
  return 0;
}

void set_info(uint16_t mode, vbe_mode_info_t mode_info) {
  game_mode = mode;
  h_res = mode_info.XResolution;
  v_res = mode_info.YResolution;
  bits_per_pixel = mode_info.BitsPerPixel;
  bytes_per_pixel = (bits_per_pixel + 7) / 8;
  vram_size = h_res * v_res * bytes_per_pixel;
  aux_buf = malloc(vram_size); //allocates memory for an auxiliary buffer
}

void *(vg_init)(uint16_t mode) {
  mmap_t mm;
  vbe_mode_info_t mode_info;
  lm_alloc(sizeof(mode_info), &mm);
  get_mode_info(mode, &mode_info);
  set_info(mode, mode_info);

  if (map_vr(mode_info) != 0) {
    printf("Error mapping memory!\n");
    return NULL;
  }

  reg86_t reg;
  memset(&reg, 0, sizeof(reg));

  reg.intno = 0X10;
  reg.ah = 0x4F;
  reg.al = 0x02;
  reg.bx = 1 << 14 | mode;

  if (sys_int86(&reg) != OK) {
    printf("set_vbe_mode: sys_int86() failed \n");
    return NULL;
  }

  lm_free(&mm);
  return (void *) mode_info.PhysBasePtr;
}

int set_pixel(uint32_t *buffer, int x, int y, uint32_t img) {
  if ((img == xpm_transparency_color(XPM_8_8_8_8)) || (x < 0) || (x > (int) (get_h_res())) || (y < 0) || (y > (int) (get_v_res())))
    return 0;

  buffer[x + h_res * y] = img;
  return 0;
}

int display_xpm(uint32_t *buffer, uint32_t *map, int x, int y, uint16_t width, uint16_t height) {
  for (int pos_y = 0; pos_y < height; pos_y++) {
    if (pos_y + y < 0) {
      pos_y = -y;
    }
    if (pos_y + y > (int) get_v_res()) {
      break;
    }
    for (int pos_x = 0; pos_x < width; pos_x++) {
      set_pixel(buffer, x + pos_x, y + pos_y, map[pos_x + width * pos_y]);
    }
  }
  return 0;
}

int clear_buffer(uint32_t *buffer) {
  memset(buffer, 0, vram_size);
  return 0;
}

int page_flipping(uint32_t *main_buf, uint32_t *aux_buf) {
  memcpy(main_buf, aux_buf, vram_size);
  return 0;
}

unsigned get_h_res() { return h_res; }

unsigned get_v_res() { return v_res; }
