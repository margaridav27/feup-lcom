#ifndef VIDEO_GR_H
#define VIDEO_GR_H

#include <lcom/lcf.h>

int(get_mode_info)(uint16_t mode);
int(map_vr)();
void *(vg_init)(uint16_t mode);
int(color_pixel)(uint16_t x, uint16_t y, uint32_t color);
int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
uint32_t(get_color)(uint16_t mode, uint32_t first, uint8_t step, uint16_t x, uint16_t y, uint8_t no_rectangles);
int(vg_draw_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);
int(vg_xpm_pixel)(uint16_t x, uint16_t y, uint8_t img);
int(vg_draw_img)(xpm_image_t img, uint16_t x, uint16_t y, uint8_t *map);
int(vg_move_sprite_h)(xpm_image_t img, uint8_t *map, uint16_t xi, uint16_t xf, uint16_t yi, int16_t speed, uint8_t fr_rate);
int(vg_move_sprite_v)(xpm_image_t img, uint8_t *map, uint16_t yi, uint16_t yf, uint16_t xi, int16_t speed, uint8_t fr_rate);

#endif
