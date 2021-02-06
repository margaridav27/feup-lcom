#ifndef GAME_CTRL
#define GAME_CTRL

#include "macros.h"
#include "rtc.h"
#include "video_gr.h"
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

struct sprite {
  uint16_t pos_x, pos_y;
  uint16_t width, height;
  uint32_t *data;
};

struct button {
  uint16_t pos_x, pos_y;
  uint16_t width, height;
};

struct present {
  uint16_t pos_x, pos_y;
  bool catched;
};

struct platform {
  uint16_t pos_x, pos_y;
  uint16_t width;
};

struct backgrounds_ {
  struct sprite *background_sp, *init_menu_sp, *paused_sp, *level_failed_1_sp, *level_failed_2_sp, *level_completed_sp, *instructions_sp, *instructions_txt, *instructions_front, *choose_player_front, *choose_player_back;
};

struct buttons_ {
  struct button main_single_player_button, main_instructions_button, main_exit_button;
  struct button paused_resume_button, paused_main_menu_button, paused_exit_button;
  struct button swipe_left, swipe_right, start;
  struct button failed_main_menu_button, failed_exit_button;
  struct button completed_main_menu_button, completed_exit_button;
  struct button instructions_home_button;
};

struct time_t {
  uint8_t minutes;
  uint8_t hours;
};

typedef enum { INITMENU,
               PAUSE,
               CHOOSE_PLAYER,
               INSTRUCTIONS_MENU,
               PLAYING,
               WON,
               TIME_OUT,
               IGLO,
               INSTRUCTIONS,
               EXIT } game_states;

typedef enum { STANDING_RIGHT,
               STANDING_LEFT,
               WALKING_RIGHT,
               WALKING_LEFT,
               JUMPING_RIGHT,
               JUMPING_LEFT,
               FALLING_RIGHT,
               FALLING,
               FALLING_LEFT,
               DEAD } santa_states;

typedef enum { ESC,
               LEFT,
               RIGHT,
               UP,
               RELEASED } keyboard_events;

typedef enum {
  HAPPY_SANTA,
  COVID_SANTA,
} player_states;

extern uint32_t *main_buf;
extern uint32_t *aux_buf;
extern game_states game_state;
extern santa_states santa_state;
extern player_states char_state;
extern keyboard_events keyboard_event;
extern struct sprite *sea_front;
extern struct sprite *sea_middle;
extern struct sprite *sea_back;
extern struct sprite current_bg;
extern struct sprite *cursor;
extern struct sprite **players;
extern struct sprite *curr_santa_sp;
extern struct sprite *santa_sp;
extern struct sprite *covid_santa_sp;
extern struct sprite current_santa;
extern struct sprite **current_santa_walk_right;
extern struct sprite **current_santa_walk_left;
extern struct sprite **current_santa_jump_right;
extern struct sprite **current_santa_jump_left;
extern struct sprite **santa_walk_right;
extern struct sprite **santa_walk_left;
extern struct sprite **santa_jump_right;
extern struct sprite **santa_jump_left;
extern struct sprite **covid_santa_walk_right;
extern struct sprite **covid_santa_walk_left;
extern struct sprite **covid_santa_jump_right;
extern struct sprite **covid_santa_jump_left;
extern struct sprite **loading;
extern struct sprite **time_numbers;
extern struct sprite **current_time;
extern struct sprite **rtc_numbers;
extern struct sprite **rtc_time;
extern struct sprite *rtc_sep;
extern struct present *presents;
extern struct sprite *presents_animated;
extern struct sprite bigpresent;
struct backgrounds_ *backgrounds;
extern struct platform *platforms;
struct buttons_ *buttons;
extern uint8_t keyboard_data;
extern struct packet mouse_packet;
struct time_t curr_time;

extern bool moving;
static int mouse_santa_sp_right_counter = 0;
static int mouse_santa_sp_left_counter = 0;
static int santa_sp_up_counter = 0;
static int santa_sp_down_counter = 0;
static int moving_right_counter = 0;
static int moving_left_counter = 0;
static bool moving_right = false;
static bool moving_left = false;
extern int presents_rot;
static int presents_catched = 0;
extern int timer_counter;
static unsigned cronometer;
static int instructions_ov_y = 270;
static unsigned int choose_player_ix = 0;
extern int counter;

/**
 * @brief Displays the background and cursor
*/
void update_display();

/**
 * @brief Displays the player sprite
*/
void update_display_player();

/**
 * @brief Displays the instructions menu
*/
void update_display_instructions();

/**
 * @brief Displays the background whilst playing mode
*/
void update_playing_display();

/**
 * @brief updates the sea moving pos
*/
void update_sea_pos();

/**
 * @brief Displays the real time
*/
void display_time();

/**
 * @brief Displays the game time count down
*/
void display_cronometer();

/**
 * @brief Displays the loading background
*/
void display_init();

/**
 * @brief Displays the presents
*/
void display_presents();

/**
 * @brief Checks whether or not the a certain button (received as parameter) was pressed
 * @param button
 * @return true if the button passed as parameter was pressed, false otherwise
*/
bool check_button(uint8_t button);

/**
 * @brief Checks whether or not a present was catched
 * @return the number of the present catched, -1 if none
*/
int check_present();

/**
 * @brief Check if the game conditions to verify if it time to end and changes game state accordingly
*/
void check_victory();

/**
 * @brief Checks santa orientation
 * @return 0 if right, 1 if left
*/
int check_right_left();

/**
 * @brief Registers the capture of a present and removes the catched present from display
*/
void catch_present(int present_ix);

/**
 * @brief Updates the time array
*/
void update_time();

/**
 * @brief Updates the cronometer array
*/
void update_cronometer();

/**
 * @brief Updates cursor's position
*/
void update_cursor();

/**
 * @brief Updates player's position
*/
void update_santa_pos();

/**
 * @brief Resets cronometer array to its defaul 4 minutes
*/
void reset_cronometer();

/**
 * @brief Changes the player sprite displayed with the sprite passed as parameter
 * @param new_sp
*/
void set_santa(struct sprite *new_sp);

/**
 * @brief Sets sprites according to player choosed
*/
void set_player();

/**
 * @brief Resets player's initial position
*/
void reset_santa();

/**
 * @brief Resets presents
*/
void reset_presents();

/**
 * @brief Changes santa state according to mouse's input
*/
void mouse_events_handler_while_playing();

/**
 * @brief changes game state according to mouse's input
*/
void mouse_events_handler();

/**
 * @brief Sets keyboard's event according to keyboard's input
*/
void set_keyboard_events();

/**
 * @brief Changes game state and backgrounds according to keyboard's input
*/
void keyboard_events_handler();

/**
 * @brief Handles all the changes that must be done periodically such as displays and updates
*/
void timer_events_handler();

/**
 * @brief Verifies if the player is standing in a platform 
 * @return the number of the platform the player is on, -1 if none
*/
int get_current_platform();

/**
 * @brief Checks the player's colisions and changes its position accordingly (incrementing the position with the step received as parameter)
 * @param step
*/
void check_collision(uint16_t step);

/**
 * @brief Checks the player's moves, keyboard input,and updates the sprite displayed accordingly
*/
void move_santa_keyboard();

/**
 * @brief Checks the player's moves, mouse input,and updates the sprite displayed accordingly
*/
void move_santa_mouse();

#endif
