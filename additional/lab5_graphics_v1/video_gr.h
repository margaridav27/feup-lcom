#ifndef VIDEO_GR_H
#define VIDEO_GR_H

#include <lcom/lcf.h>

int map_vram(vbe_mode_info_t mode_info);
void get_ctrl_info(vg_vbe_contr_info_t *ctrl_info);
void get_mode_info(uint16_t mode, vbe_mode_info_t *mode_info);
int set_mode(uint16_t mode);
void set_info(vbe_mode_info_t mode_info);
void *(vg_init)(uint16_t mode);
void color_pixel(uint16_t x, uint16_t y, uint32_t color);
void draw_line(uint16_t x, uint16_t y, uint16_t width, uint32_t color);
void draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
uint32_t get_color(uint8_t no_rectangles, uint32_t first, uint8_t step, uint16_t col, uint16_t row);
void draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step);
void fill_xpm_pixel(uint16_t x, uint16_t y, uint8_t filler);
void display_xpm(xpm_image_t img, uint8_t *bytes, uint16_t x, uint16_t y);
int move_xpm_h(xpm_image_t img, uint8_t *bytes, uint16_t xi, uint16_t xf, uint16_t yi,
               int16_t speed, uint8_t fr_rate);
int move_xpm_v(xpm_image_t img, uint8_t *bytes, uint16_t yi, uint16_t yf, uint16_t xi,
               int16_t speed, uint8_t fr_rate);

#endif
