#include "game.h"
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <stdbool.h>
#include <stdint.h>

/*main and auxiliar buffers*/
extern uint32_t *main_buf;
extern uint32_t *aux_buf;

/*keyboard related variables*/
uint32_t keyboard_irq_set;
uint8_t keyboard_data;
bool scancode_2ndbyte;
bool keyboard_err;

/*mouse related variables*/
uint32_t mouse_irq_set;
struct packet mouse_packet;
uint8_t mouse_data;
uint8_t mouse_byte;
unsigned int mouse_byte_tracker;
bool mouse_err;

/*timer related variables*/
uint32_t timer_irq_set;
int timer_counter;

/*rtc related variables*/
uint32_t rtc_irq_set;
struct time_t curr_time;

int i;
int counter = 0;
int presents_rot;
bool moving = false;

struct sprite current_bg;
struct platform *platforms;
struct sprite *cursor;
struct sprite **loading;

struct sprite *presents_animated;
struct present *presents;
struct sprite bigpresent;

struct sprite *sea_front;
struct sprite *sea_back;
struct sprite *sea_middle;

struct sprite **time_numbers;
struct sprite **current_time;

struct sprite **rtc_numbers;
struct sprite **rtc_time;
struct sprite *rtc_sep;

struct sprite **players;

struct sprite *curr_santa_sp;
struct sprite *santa_sp;
struct sprite *covid_santa_sp;

struct sprite current_santa;
struct sprite **current_santa_walk_right;
struct sprite **current_santa_walk_left;
struct sprite **current_santa_jump_left;
struct sprite **current_santa_jump_right;

struct sprite **covid_santa_walk_right;
struct sprite **covid_santa_walk_left;
struct sprite **covid_santa_jump_left;
struct sprite **covid_santa_jump_right;

struct sprite **santa_walk_right;
struct sprite **santa_walk_left;
struct sprite **santa_jump_left;
struct sprite **santa_jump_right;

game_states game_state = INITMENU;
keyboard_events keyboard_event = RELEASED;
santa_states santa_state = STANDING_RIGHT;
player_states char_state = COVID_SANTA;

void set_display() {
  vg_init(0x14c);

  loading = (struct sprite **) malloc(3 * sizeof(struct sprite));
  if (loading == NULL) {
    free(loading);
    return;
  }

  loading[0] = get_sprite((xpm_map_t) init0, 0, 0);
  loading[1] = get_sprite((xpm_map_t) init1, 0, 0);
  loading[2] = get_sprite((xpm_map_t) init2, 73, 378);
  display_init();
  loading[2]->pos_x += 20;
}

struct sprite *get_sprite(xpm_map_t xpm, int x, int y) {
  xpm_image_t img;
  enum xpm_image_type type = XPM_8_8_8_8;
  xpm_map_t map = (xpm_map_t) xpm;
  xpm_load(map, type, &img);

  uint32_t *bytes = (uint32_t *) img.bytes;
  struct sprite *sp = (struct sprite *) malloc(sizeof(struct sprite));

  sp->pos_x = x;
  sp->pos_y = y;
  sp->width = img.width;
  sp->height = img.height;
  sp->data = bytes;

  return sp;
}

void load_xpms() {

  display_init();
  loading[2]->pos_x += 20;

  covid_santa_sp = get_sprite((xpm_map_t) covid_santa_R, 0, 616);
  santa_sp = get_sprite((xpm_map_t) santa_R, 0, 616);
  current_santa = *get_sprite((xpm_map_t) santa_R, 0, 616);
  cursor = get_sprite((xpm_map_t) cursor_, 400, 300);
  sea_front = get_sprite((xpm_map_t) sea, 554, 798);
  sea_back = get_sprite((xpm_map_t) seaback3, 470, 730);
  sea_middle = get_sprite((xpm_map_t) sea, 440, 774);

  loading[2]->pos_x += 20;
  display_init();

  santa_jump_right[0] = get_sprite((xpm_map_t) jump_1, 0, 0);
  santa_jump_right[1] = get_sprite((xpm_map_t) jump_2, 0, 0);
  santa_jump_right[2] = get_sprite((xpm_map_t) jump_3, 0, 0);
  santa_jump_right[3] = get_sprite((xpm_map_t) jump_4, 0, 0);
  santa_jump_right[4] = get_sprite((xpm_map_t) jump_5, 0, 0);

  display_init();
  loading[2]->pos_x += 20;

  covid_santa_jump_right[0] = get_sprite((xpm_map_t) covid_jump_1, 0, 0);
  covid_santa_jump_right[1] = get_sprite((xpm_map_t) covid_jump_2, 0, 0);
  covid_santa_jump_right[2] = get_sprite((xpm_map_t) covid_jump_3, 0, 0);
  covid_santa_jump_right[3] = get_sprite((xpm_map_t) covid_jump_4, 0, 0);
  covid_santa_jump_right[4] = get_sprite((xpm_map_t) covid_jump_5, 0, 0);

  santa_jump_left[0] = get_sprite((xpm_map_t) jump_1_L, 0, 0);
  santa_jump_left[1] = get_sprite((xpm_map_t) jump_2_L, 0, 0);
  santa_jump_left[2] = get_sprite((xpm_map_t) jump_3_L, 0, 0);
  santa_jump_left[3] = get_sprite((xpm_map_t) jump_4_L, 0, 0);
  santa_jump_left[4] = get_sprite((xpm_map_t) jump_5_L, 0, 0);

  display_init();
  loading[2]->pos_x += 20;

  covid_santa_jump_left[0] = get_sprite((xpm_map_t) covid_jump_1_L, 0, 0);
  covid_santa_jump_left[1] = get_sprite((xpm_map_t) covid_jump_2_L, 0, 0);
  covid_santa_jump_left[2] = get_sprite((xpm_map_t) covid_jump_3_L, 0, 0);
  covid_santa_jump_left[3] = get_sprite((xpm_map_t) covid_jump_4_L, 0, 0);
  covid_santa_jump_left[4] = get_sprite((xpm_map_t) covid_jump_5_L, 0, 0);

  santa_walk_right[0] = get_sprite((xpm_map_t) santa_walk_R1, 0, 0);
  santa_walk_right[1] = get_sprite((xpm_map_t) santa_walk_R2, 0, 0);
  santa_walk_right[2] = get_sprite((xpm_map_t) santa_walk_R3, 0, 0);
  santa_walk_right[3] = get_sprite((xpm_map_t) santa_walk_R4, 0, 0);
  santa_walk_right[4] = get_sprite((xpm_map_t) santa_R, 0, 0);

  display_init();
  loading[2]->pos_x += 20;

  covid_santa_walk_right[0] = get_sprite((xpm_map_t) covid_santa_walk_R1, 0, 0);
  covid_santa_walk_right[1] = get_sprite((xpm_map_t) covid_santa_walk_R2, 0, 0);
  covid_santa_walk_right[2] = get_sprite((xpm_map_t) covid_santa_walk_R3, 0, 0);
  covid_santa_walk_right[3] = get_sprite((xpm_map_t) covid_santa_walk_R4, 0, 0);
  covid_santa_walk_right[4] = get_sprite((xpm_map_t) covid_santa_R, 0, 0);

  santa_walk_left[0] = get_sprite((xpm_map_t) santa_walk_L1, 0, 0);
  santa_walk_left[1] = get_sprite((xpm_map_t) santa_walk_L2, 0, 0);
  santa_walk_left[2] = get_sprite((xpm_map_t) santa_walk_L3, 0, 0);
  santa_walk_left[3] = get_sprite((xpm_map_t) santa_walk_L4, 0, 0);
  santa_walk_left[4] = get_sprite((xpm_map_t) santa_L, 0, 0);

  display_init();
  loading[2]->pos_x += 20;

  covid_santa_walk_left[0] = get_sprite((xpm_map_t) covid_santa_walk_L1, 0, 0);
  covid_santa_walk_left[1] = get_sprite((xpm_map_t) covid_santa_walk_L2, 0, 0);
  covid_santa_walk_left[2] = get_sprite((xpm_map_t) covid_santa_walk_L3, 0, 0);
  covid_santa_walk_left[3] = get_sprite((xpm_map_t) covid_santa_walk_L4, 0, 0);
  covid_santa_walk_left[4] = get_sprite((xpm_map_t) covid_santa_L, 0, 0);

  players[0] = get_sprite((xpm_map_t) choose_player_covid, 445, 230);
  players[1] = get_sprite((xpm_map_t) choose_player_happy, 107, 230);

  display_init();
  loading[2]->pos_x += 20;

  backgrounds->background_sp = get_sprite((xpm_map_t) background, 0, 0);
  backgrounds->choose_player_back = get_sprite((xpm_map_t) back_choose_player, 0, 0);
  backgrounds->choose_player_front = get_sprite((xpm_map_t) front_choose_player, 0, 0);
  backgrounds->init_menu_sp = get_sprite((xpm_map_t) initial_menu, 0, 0);
  backgrounds->paused_sp = get_sprite((xpm_map_t) game_pause, 0, 0);

  loading[2]->pos_x += 20;
  display_init();

  backgrounds->level_completed_sp = get_sprite((xpm_map_t) level_completed, 0, 0);
  backgrounds->level_failed_1_sp = get_sprite((xpm_map_t) level_failed_1, 0, 0);
  backgrounds->level_failed_2_sp = get_sprite((xpm_map_t) level_failed_2, 0, 0);
  backgrounds->instructions_sp = get_sprite((xpm_map_t) instructions, 0, 0);
  backgrounds->instructions_txt = get_sprite((xpm_map_t) instructions_txt, 304, 276);
  backgrounds->instructions_front = get_sprite((xpm_map_t) instructions_front, 0, 0);
  display_init();
  loading[2]->pos_x += 20;
  presents_animated[0] = *get_sprite((xpm_map_t) present1, 0, 0);
  presents_animated[1] = *get_sprite((xpm_map_t) present2, 0, 0);
  presents_animated[2] = *get_sprite((xpm_map_t) present3, 0, 0);
  presents_animated[3] = *get_sprite((xpm_map_t) present4, 0, 0);
  presents_animated[4] = *get_sprite((xpm_map_t) present5, 0, 0);
  presents_animated[5] = *get_sprite((xpm_map_t) present6, 0, 0);
  bigpresent = *get_sprite((xpm_map_t) big_present, 0, 0);

  loading[2]->pos_x += 20;
  display_init();

  time_numbers[0] = get_sprite((xpm_map_t) n_0, 0, 0);
  time_numbers[1] = get_sprite((xpm_map_t) n_1, 0, 0);
  time_numbers[2] = get_sprite((xpm_map_t) n_2, 0, 0);
  time_numbers[3] = get_sprite((xpm_map_t) n_3, 0, 0);
  time_numbers[4] = get_sprite((xpm_map_t) n_4, 0, 0);
  loading[2]->pos_x += 20;
  display_init();
  time_numbers[5] = get_sprite((xpm_map_t) n_5, 0, 0);
  time_numbers[6] = get_sprite((xpm_map_t) n_6, 0, 0);
  time_numbers[7] = get_sprite((xpm_map_t) n_7, 0, 0);
  time_numbers[8] = get_sprite((xpm_map_t) n_8, 0, 0);
  time_numbers[9] = get_sprite((xpm_map_t) n_9, 0, 0);

  display_init();
  loading[2]->pos_x += 20;

  current_time[1] = get_sprite((xpm_map_t) sep, 0, 0);
  current_time[0] = get_sprite((xpm_map_t) n_4, 0, 0);
  current_time[2] = get_sprite((xpm_map_t) n_0, 0, 0);
  current_time[3] = get_sprite((xpm_map_t) n_0, 0, 0);

  rtc_numbers[0] = get_sprite((xpm_map_t) rtc_0, 0, 0);
  rtc_numbers[1] = get_sprite((xpm_map_t) rtc_1, 0, 0);
  rtc_numbers[2] = get_sprite((xpm_map_t) rtc_2, 0, 0);
  rtc_numbers[3] = get_sprite((xpm_map_t) rtc_3, 0, 0);
  loading[2]->pos_x += 20;
  display_init();
  rtc_numbers[4] = get_sprite((xpm_map_t) rtc_4, 0, 0);
  rtc_numbers[5] = get_sprite((xpm_map_t) rtc_5, 0, 0);
  rtc_numbers[6] = get_sprite((xpm_map_t) rtc_6, 0, 0);
  rtc_numbers[7] = get_sprite((xpm_map_t) rtc_7, 0, 0);
  rtc_numbers[8] = get_sprite((xpm_map_t) rtc_8, 0, 0);
  rtc_numbers[9] = get_sprite((xpm_map_t) rtc_9, 0, 0);

  rtc_sep = get_sprite((xpm_map_t) rtc_sep_sp, 0, 0);
}

void set_presents() {

  display_init();
  loading[2]->pos_x += 20;

  presents[0].pos_y = 670;
  presents[0].pos_x = 300;
  presents[0].catched = false;

  presents[1].pos_y = 670;
  presents[1].pos_x = 950;
  presents[1].catched = false;

  presents[2].pos_y = 540;
  presents[2].pos_x = 700;
  presents[2].catched = false;

  presents[3].pos_y = 50;
  presents[3].pos_x = 640;
  presents[3].catched = false;

  presents[4].pos_y = 330;
  presents[4].pos_x = 900;
  presents[4].catched = false;

  presents[5].pos_y = 300;
  presents[5].pos_x = 70;
  presents[5].catched = false;
}

void set_buttons() {
  display_init();
  loading[2]->pos_x += 20;

  //screen of 1152 x 864 <=> 22.0 cm x 15.5 cm aproximately

  /*buttons on main menu*/
  buttons->main_single_player_button.pos_x = 363;
  buttons->main_single_player_button.pos_y = 295;
  buttons->main_single_player_button.width = 342;
  buttons->main_single_player_button.height = 93;

  buttons->main_instructions_button.pos_x = 363; //7.2 cm x 8.1 cm
  buttons->main_instructions_button.pos_y = 412;
  buttons->main_instructions_button.width = 342;
  buttons->main_instructions_button.height = 93;

  buttons->main_exit_button.pos_x = 363; //7.2 cm x 10 cm
  buttons->main_exit_button.pos_y = 524;
  buttons->main_exit_button.width = 526;
  buttons->main_exit_button.height = 93;

  /*buttons on game paused menu*/
  buttons->paused_resume_button.pos_x = 398; //7.6 cm x 6.2 cm
  buttons->paused_resume_button.pos_y = 325;
  buttons->paused_resume_button.width = 278;
  buttons->paused_resume_button.height = 84;

  buttons->paused_main_menu_button.pos_x = 398; //7.6 cm x 8.1 cm
  buttons->paused_main_menu_button.pos_y = 424;
  buttons->paused_main_menu_button.width = 278;
  buttons->paused_main_menu_button.height = 84;

  buttons->paused_exit_button.pos_x = 398; //7.6 cm x 10 cm
  buttons->paused_exit_button.pos_y = 524;
  buttons->paused_exit_button.width = 278;
  buttons->paused_exit_button.height = 84;

  /*buttons on game failed menu*/
  buttons->failed_main_menu_button.pos_x = 393; //7.5 cm x 7.9 cm
  buttons->failed_main_menu_button.pos_y = 414;
  buttons->failed_main_menu_button.width = 278;
  buttons->failed_main_menu_button.height = 84;

  buttons->failed_exit_button.pos_x = 393; //7.5 cm x 10.1 cm
  buttons->failed_exit_button.pos_y = 529;
  buttons->failed_exit_button.width = 278;
  buttons->failed_exit_button.height = 84;

  /*buttons on completed game menu*/
  buttons->completed_main_menu_button.pos_x = 382; //7.3 cm x 8.0 cm
  buttons->completed_main_menu_button.pos_y = 419;
  buttons->completed_main_menu_button.width = 278;
  buttons->completed_main_menu_button.height = 84;

  buttons->completed_exit_button.pos_x = 382; //7.3 cm x 10.3 cm
  buttons->completed_exit_button.pos_y = 539;
  buttons->completed_exit_button.width = 278;
  buttons->completed_exit_button.height = 84;

  /*buttons on instructions menu*/
  buttons->instructions_home_button.pos_x = 272; //5.2 cm x 3.0 cm
  buttons->instructions_home_button.pos_y = 157;
  buttons->instructions_home_button.width = 79;
  buttons->instructions_home_button.height = 79;

  /*buttons on the choose player menu*/
  buttons->swipe_left.pos_x = 305;
  buttons->swipe_left.pos_y = 320;
  buttons->swipe_left.width = 93;
  buttons->swipe_left.height = 104;

  buttons->swipe_right.pos_x = 716;
  buttons->swipe_right.pos_y = 320;
  buttons->swipe_right.width = 93;
  buttons->swipe_right.height = 104;

  buttons->start.pos_x = 470;
  buttons->start.pos_y = 550;
  buttons->start.width = 190;
  buttons->start.height = 128;
}

void set_platforms() {
  display_init();
  loading[2]->pos_x += 10;

  platforms[0].pos_x = 0;
  platforms[0].pos_y = 755;
  platforms[0].width = 554;

  platforms[1].pos_x = 840;
  platforms[1].pos_y = 755;
  platforms[1].width = 302;

  platforms[2].pos_x = 552;
  platforms[2].pos_y = 624;
  platforms[2].width = 308;

  platforms[3].pos_x = 370;
  platforms[3].pos_y = 507;
  platforms[3].width = 113;

  platforms[4].pos_x = 0;
  platforms[4].pos_y = 382;
  platforms[4].width = 366;

  platforms[5].pos_x = 755;
  platforms[5].pos_y = 408;
  platforms[5].width = 310;

  platforms[6].pos_x = 394;
  platforms[6].pos_y = 203;
  platforms[6].width = 146;

  platforms[8].pos_x = 457;
  platforms[8].pos_y = 166;
  platforms[8].width = 325;

  platforms[7].pos_x = 896;
  platforms[7].pos_y = 212;
  platforms[7].width = 256;
}

void set_up() {

  mouse_enable_data_report();
  rtc_enable_ints(RTC_REG_B_PI);

  uint8_t rtc_bit_no, timer_bit_no, keyboard_bit_no, mouse_bit_no;

  /*subscribing rtc interrupts*/
  rtc_subscribe_int(&rtc_bit_no);
  rtc_irq_set = BIT(rtc_bit_no);

  /*subscribing timer interrupts*/
  timer_subscribe_int(&timer_bit_no);
  timer_irq_set = BIT(timer_bit_no);
  timer_counter = 0;
  presents_rot = 0;

  /*subscribing keyboard interrupts*/
  kbc_subscribe_int(&keyboard_bit_no);
  keyboard_irq_set = BIT(keyboard_bit_no);

  /*subscribing mouse interrupts*/
  mouse_subscribe_int(&mouse_bit_no);
  mouse_irq_set = BIT(mouse_bit_no);

  /*allocating memory for arrays & structs*/
  players = (struct sprite **) malloc(2 * sizeof(struct sprite));
  if (players == NULL) {
    free(players);
    return;
  }

  display_init();
  loading[2]->pos_x += 20;

  current_santa_walk_right = (struct sprite **) malloc(5 * sizeof(struct sprite));
  if (current_santa_walk_right == NULL) {
    free(current_santa_walk_right);
    return;
  }

  current_santa_walk_left = (struct sprite **) malloc(5 * sizeof(struct sprite));
  if (current_santa_walk_left == NULL) {
    free(current_santa_walk_left);
    return;
  }

  current_santa_jump_right = (struct sprite **) malloc(5 * sizeof(struct sprite));
  if (current_santa_jump_right == NULL) {
    free(current_santa_jump_right);
    return;
  }

  current_santa_jump_left = (struct sprite **) malloc(5 * sizeof(struct sprite));
  if (current_santa_jump_left == NULL) {
    free(current_santa_jump_left);
    return;
  }
  display_init();
  loading[2]->pos_x += 20;

  santa_walk_right = (struct sprite **) malloc(5 * sizeof(struct sprite));
  if (santa_walk_right == NULL) {
    free(santa_walk_right);
    return;
  }

  santa_walk_left = (struct sprite **) malloc(5 * sizeof(struct sprite));
  if (santa_walk_left == NULL) {
    free(santa_walk_left);
    return;
  }

  display_init();
  loading[2]->pos_x += 20;

  santa_jump_right = (struct sprite **) malloc(5 * sizeof(struct sprite));
  if (santa_jump_right == NULL) {
    free(santa_jump_right);
    return;
  }

  santa_jump_left = (struct sprite **) malloc(5 * sizeof(struct sprite));
  if (santa_jump_left == NULL) {
    free(santa_jump_left);
    return;
  }

  covid_santa_walk_right = (struct sprite **) malloc(5 * sizeof(struct sprite));
  if (covid_santa_walk_right == NULL) {
    free(covid_santa_walk_right);
    return;
  }

  covid_santa_walk_left = (struct sprite **) malloc(5 * sizeof(struct sprite));
  if (covid_santa_walk_left == NULL) {
    free(covid_santa_walk_left);
    return;
  }

  covid_santa_jump_right = (struct sprite **) malloc(5 * sizeof(struct sprite));
  if (covid_santa_jump_right == NULL) {
    free(covid_santa_jump_right);
    return;
  }

  covid_santa_jump_left = (struct sprite **) malloc(5 * sizeof(struct sprite));
  if (covid_santa_jump_left == NULL) {
    free(covid_santa_jump_left);
    return;
  }

  time_numbers = (struct sprite **) malloc(10 * sizeof(struct sprite));
  if (time_numbers == NULL) {
    free(time_numbers);
    return;
  }

  current_time = (struct sprite **) malloc(4 * sizeof(struct sprite));
  if (current_time == NULL) {
    free(current_time);
    return;
  }

  rtc_numbers = (struct sprite **) malloc(10 * sizeof(struct sprite));
  if (rtc_numbers == NULL) {
    free(rtc_numbers);
    return;
  }

  rtc_time = (struct sprite **) malloc(5 * sizeof(struct sprite));
  if (rtc_time == NULL) {
    free(rtc_time);
    return;
  }
  display_init();
  loading[2]->pos_x += 20;

  buttons = (struct buttons_ *) malloc(14 * sizeof(struct button));
  if (buttons == NULL) {
    free(buttons);
    return;
  }

  backgrounds = (struct backgrounds_ *) malloc(11 * sizeof(struct sprite));
  if (backgrounds == NULL) {
    free(backgrounds);
    return;
  }

  presents_animated = (struct sprite *) malloc(6 * sizeof(struct sprite));
  if (presents_animated == NULL) {
    free(presents_animated);
    return;
  }

  platforms = (struct platform *) malloc(9 * sizeof(struct platform));
  if (platforms == NULL) {
    free(platforms);
    return;
  }

  display_init();
  loading[2]->pos_x += 20;

  presents = (struct present *) malloc(6 * sizeof(struct present));
  if (presents == NULL) {
    free(presents);
    return;
  }

  santa_sp = (struct sprite *) malloc(sizeof(struct sprite));
  if (santa_sp == NULL) {
    free(santa_sp);
    return;
  }

  covid_santa_sp = (struct sprite *) malloc(sizeof(struct sprite));
  if (covid_santa_sp == NULL) {
    free(covid_santa_sp);
    return;
  }

  cursor = (struct sprite *) malloc(sizeof(struct sprite));
  if (cursor == NULL) {
    free(cursor);
    return;
  }
  loading[2]->pos_x += 20;
  display_init();

  load_xpms();
  set_buttons();
  set_platforms();
  set_presents();
}

void restore() {
  rtc_unsubscribe_int();
  timer_unsubscribe_int();
  kbc_unsubscribe_int();
  mouse_unsubscribe_int();
  mouse_disable_data_report();
  rtc_disable_ints(RTC_REG_B_UI);
  vg_exit();

  free(players);
  free(current_santa_walk_right);
  free(current_santa_walk_left);
  free(current_santa_jump_right);
  free(current_santa_jump_left);
  free(santa_walk_right);
  free(santa_walk_left);
  free(santa_jump_right);
  free(santa_jump_left);
  free(covid_santa_walk_right);
  free(covid_santa_walk_left);
  free(covid_santa_jump_right);
  free(covid_santa_jump_left);
  free(time_numbers);
  free(current_time);
  free(buttons);
  free(backgrounds);
  free(presents_animated);
  free(platforms);
  free(presents);
  free(santa_sp);
  free(covid_santa_sp);
  free(cursor);
}

int init_game() {
  current_bg = *backgrounds->init_menu_sp;

  int r;
  int ipc_status;
  message msg;
  mouse_byte_tracker = 0;
  bool mouse_lag = false;

  while (game_state != EXIT) {
    if ((r = driver_receive(ANY, &msg, &ipc_status))) {
      printf("driver_receive failed with: %d", r);
    }

    if (game_state == PLAYING) {
      mouse_lag = true;
      playing();
    }
    rtc_ih(RTC_REG_C_PF);
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_events_handler();
          }

          if (msg.m_notify.interrupts & mouse_irq_set) {
            mouse_lag = false;

            /*when on any menu, we just need to handle interruptions from the mouse, since we can only interact with the buttons*/
            mouse_ih();

            /*processing mouse bytes until a full packet is constructed*/
            if (!mouse_err) {
              mouse_process_byte();
              mouse_byte_tracker++;
            }

            /*we have processed a full packet from mouse*/
            if (mouse_byte_tracker == 3) {
              mouse_byte_tracker = 0;
              update_cursor();
              mouse_events_handler();
            }
          }
          else if ((game_state == WON || game_state == TIME_OUT) && check_obf() && mouse_lag) {
            util_sys_inb(KBC_OUT_BUF, &mouse_byte); //reading data
          }

          if (msg.m_notify.interrupts & rtc_irq_set) {
            rtc_ih(RTC_REG_C_PF);
          }
          break;
      }
    }
  }
  return 0;
}

int playing() {
  reset_cronometer();
  reset_santa();
  reset_presents();
  update_playing_display();
  int r;
  int ipc_status;
  message msg;
  mouse_byte_tracker = 0;

  while (game_state == PLAYING) {
    if ((r = driver_receive(ANY, &msg, &ipc_status))) {
      printf("driver_receive failed with: %d", r);
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler();
            timer_events_handler();
            counter++;
            check_victory();
          }
          if (msg.m_notify.interrupts & mouse_irq_set) {
            mouse_ih();

            if (mouse_err) {
              continue;
            }

            mouse_process_byte();
            mouse_byte_tracker++;
            if (mouse_byte_tracker == 3) {
              mouse_byte_tracker = 0;
              mouse_events_handler();
              move_santa_mouse();

              if (mouse_packet.delta_x != 0) { /*determines if countdown should start*/
                moving = true;
              }
            }
          }
          else if (santa_state == STANDING_RIGHT) {
            set_santa(current_santa_walk_right[4]);
          }
          else if (santa_state == STANDING_LEFT) {
            set_santa(current_santa_walk_left[4]);
          }

          if (msg.m_notify.interrupts & keyboard_irq_set) {
            kbc_ih();

            if (keyboard_err) {
              continue;
            }

            keyboard_events_handler();
            move_santa_keyboard();

            if (keyboard_event != RELEASED) { /*determines if countdown should start*/
              moving = true;
            }
          }
          else if (santa_state != STANDING_RIGHT || santa_state != STANDING_LEFT) { /*is either jumping or falling*/
            move_santa_keyboard();
          }
          check_victory();
          if (game_state == PAUSE) {
            if (pause() == 0) { /*user paused the game but then decided to keep playing (paused() returned 0)*/
              update_playing_display();
            }
            else {
              if (game_state == INITMENU) { /*user paused the game and decided to return to the main menu (paused() returned 1)*/
                update_display();
              }
            }
          }
          else { /*otherwise display is normally updated*/
            update_playing_display();
          }

          break;
      }
    }
  }
  return 0;
}

int pause() {
  update_display();

  int r;
  int ipc_status;
  message msg;
  mouse_byte_tracker = 0;
  rtc_ih(RTC_REG_C_PF);

  while (game_state == PAUSE) {
    if ((r = driver_receive(ANY, &msg, &ipc_status))) {
      printf("driver_receive failed with: %d", r);
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & mouse_irq_set) {
            mouse_ih();

            if (!mouse_err) {
              mouse_process_byte();
              mouse_byte_tracker++;
            }

            if (mouse_byte_tracker == 3) {
              mouse_byte_tracker = 0;
              update_cursor();
              mouse_events_handler();
              update_display();
            }
          }
          if (msg.m_notify.interrupts & rtc_irq_set) {
            rtc_ih(RTC_REG_C_PF);
          }
          break;
      }
    }
  }
  if (game_state == PLAYING) {
    return 0;
  }
  return 1;
}
