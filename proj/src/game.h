#ifndef GAME
#define GAME

#include "game_ctrl.h"
#include "kbc.h"
#include "keyboard.h"
#include "macros.h"
#include "mouse.h"
#include "rtc.h"
#include "video_gr.h"
#include "xpms.h"
#include <lcom/lcf.h>

/*struct with background sprites*/
struct backgrounds_ *backgrounds;

/*struct with buttons info*/
struct buttons_ *buttons;

/**
 * @brief Sets graphic mode
*/
void set_display();

/**
 * @brief Creates a sprite from a xpm file
 * @param bit_no
 * @param x
 * @param y
 * @return the created sprite
*/
struct sprite *get_sprite(xpm_map_t xpm, int x, int y);

/**
 * @brief Load all the xpms to sprites and allocates them
*/
void load_xpms();

/**
 * @brief Sets the present's positions
*/
void set_presents();

/**
 * @brief Sets the buttons's positions
*/
void set_buttons();

/**
 * @brief Sets the platforms's positions
*/
void set_platforms();

/**
 * @brief Subscribes interruptions and allocates memory
*/
void set_up();

/**
 * @brief Restores all data changed, frees memory and sets text mode
*/
void restore();

/**
 * @brief Main game loop
 * @return 0 when finished
*/
int init_game();

/**
 * @brief Pause menu loop 
 * @return 0 if the game continues, 1 otherwise
*/
int pause();

/**
 * @brief Playing game loop
 * @return 0 when finished
*/
int playing();

#endif
