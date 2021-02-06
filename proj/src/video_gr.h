#ifndef VIDEO_GR
#define VIDEO_GR

#include <lcom/lcf.h>

/**
 * @brief Gets information about the specified VBE mode and outputs it to the propor data structure
 * @param mode VBE mode from which one intends to get information
 * @param mode_info data structure to output the mode information 
*/
void(get_mode_info)(uint16_t mode, vbe_mode_info_t *mode_info);

/**
 * @brief Maps memory into the current process adress space
 * @param mode_info 
 * @return 0 upon sucess, 1 otherwise
*/
int(map_vr)(vbe_mode_info_t mode_info);

/**
 * @brief Initializes global variables of the mode just set
 * @param mode VBE mode
 * @param mode_info data structure containing the relevant information about the mode
*/
void set_info(uint16_t mode, vbe_mode_info_t mode_info);

/**
 * @brief Initializes the video module in graphics mode
 * @param mode VBE mode to set
 * @return Virtual address VRAM was mapped to, NULL upon failure
 */
void *(vg_init)(uint16_t mode);

/**
 * @brief Sets a pixel of the screen accordingly to the value passed as argument
 * @param buffer buffer on which the pixel is to be set
 * @param x coordinate of the pixel to be set
 * @param y coordinate of the pixel to be set
*/
int set_pixel(uint32_t *buffer, int x, int y, uint32_t img);

/**
 * @brief Renders image to the specified buffer
 * @param buffer buffer on which the image is to be rendered
 * @param map image actual data 
 * @param x coordinate of the upper left corner of the image
 * @param y coordinate of the upper left corner of the image
 * @param width image's width
 * @param height image's height
*/
int display_xpm(uint32_t *buffer, uint32_t *map, int x, int y, uint16_t width, uint16_t height);

/**
 * @brief Clears specified buffer
 * @param buffer buffer to one intends to clear
*/
int clear_buffer(uint32_t *buffer);

/**
 * @brief Handles double buffering
 * @param main_buf destiny buffer
 * @param aux_buf source buffer
*/
int page_flipping(uint32_t *main_buf, uint32_t *aux_buf);

/**
 * @return screen horizontal resolution
*/
unsigned get_h_res();

/**
 * @return screen vertical resolution
*/
unsigned get_v_res();

#endif
