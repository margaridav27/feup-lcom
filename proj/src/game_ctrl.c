#include "game_ctrl.h"
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <stdbool.h>
#include <stdint.h>

void update_display() {
  clear_buffer(aux_buf);
  display_xpm(aux_buf, current_bg.data, current_bg.pos_x, current_bg.pos_y, current_bg.width, current_bg.height);
  display_time();
  display_xpm(aux_buf, cursor->data, cursor->pos_x, cursor->pos_y, cursor->width, cursor->height);
  page_flipping(main_buf, aux_buf);
}

void update_display_player() {
  clear_buffer(aux_buf);
  display_xpm(aux_buf, backgrounds->choose_player_back->data, backgrounds->choose_player_back->pos_x, backgrounds->choose_player_back->pos_y, backgrounds->choose_player_back->width, backgrounds->choose_player_back->height);
  display_xpm(aux_buf, players[0]->data, players[0]->pos_x, players[0]->pos_y, players[0]->width, players[0]->height);
  display_xpm(aux_buf, players[1]->data, players[1]->pos_x, players[1]->pos_y, players[1]->width, players[1]->height);
  display_xpm(aux_buf, backgrounds->choose_player_front->data, backgrounds->choose_player_front->pos_x, backgrounds->choose_player_front->pos_y, backgrounds->choose_player_front->width, backgrounds->choose_player_front->height);
  display_xpm(aux_buf, cursor->data, cursor->pos_x, cursor->pos_y, cursor->width, cursor->height);
  page_flipping(main_buf, aux_buf);
}

void update_display_instructions() {
  clear_buffer(aux_buf);
  display_xpm(aux_buf, current_bg.data, current_bg.pos_x, current_bg.pos_y, current_bg.width, current_bg.height);
  display_xpm(aux_buf, backgrounds->instructions_txt->data, backgrounds->instructions_txt->pos_x, instructions_ov_y, backgrounds->instructions_txt->width, backgrounds->instructions_txt->height);
  display_xpm(aux_buf, backgrounds->instructions_front->data, backgrounds->instructions_front->pos_x, backgrounds->instructions_front->pos_y, backgrounds->instructions_front->width, backgrounds->instructions_front->height);
  display_xpm(aux_buf, cursor->data, cursor->pos_x, cursor->pos_y, cursor->width, cursor->height);
  page_flipping(main_buf, aux_buf);
}

void update_playing_display() {
  clear_buffer(aux_buf);
  display_xpm(aux_buf, sea_back->data, sea_back->pos_x, sea_back->pos_y, sea_back->width, sea_back->height);
  display_xpm(aux_buf, sea_middle->data, sea_middle->pos_x, sea_middle->pos_y, sea_middle->width, sea_middle->height);
  display_xpm(aux_buf, sea_front->data, sea_front->pos_x, sea_front->pos_y, sea_front->width, sea_front->height);
  display_xpm(aux_buf, current_bg.data, current_bg.pos_x, current_bg.pos_y, current_bg.width, current_bg.height);
  display_presents();
  display_xpm(aux_buf, current_santa.data, current_santa.pos_x, current_santa.pos_y, current_santa.width, current_santa.height);
  display_cronometer();
  page_flipping(main_buf, aux_buf);
}

void update_sea_pos() {
  if (!(counter % 30))
    presents_rot++;

  static int counter_sea = 0;
  counter_sea++;

  sea_back->pos_x++;
  sea_middle->pos_x--;
  if (!(counter_sea % WAVE_WIDTH)) {
    sea_back->pos_x = SEA_BACK_X0;
    sea_middle->pos_x = SEA_MIDDLE_X0;
  }
}

void display_init() {
  clear_buffer(aux_buf);
  display_xpm(aux_buf, loading[0]->data, loading[0]->pos_x, loading[0]->pos_y, loading[0]->width, loading[0]->height);
  display_xpm(aux_buf, loading[2]->data, loading[2]->pos_x, loading[2]->pos_y, loading[2]->width, loading[2]->height);
  display_xpm(aux_buf, loading[1]->data, loading[1]->pos_x, loading[1]->pos_y, loading[1]->width, loading[1]->height);
  page_flipping(main_buf, aux_buf);
}

void display_time() {
  update_time();
  uint16_t pos_x = get_h_res();
  uint16_t pos_y = 0;
  for (int i = 4; i >= 0; i--) {
    pos_x -= rtc_time[i]->width;
    display_xpm(aux_buf, rtc_time[i]->data, pos_x, pos_y, rtc_time[i]->width, rtc_time[i]->height);
  }
}

void display_cronometer() {
  uint16_t pos_x = get_h_res();
  uint16_t pos_y = 0;
  for (int i = 3; i >= 0; i--) {
    pos_x -= current_time[i]->width;
    display_xpm(aux_buf, current_time[i]->data, pos_x, pos_y, current_time[i]->width, current_time[i]->height);
  }
}

void display_presents() {
  for (unsigned i = 0; i < NUM_PRESENTS; i++) {
    if (presents[i].catched == false) {
      display_xpm(aux_buf, presents_animated[presents_rot % 6].data, presents[i].pos_x, presents[i].pos_y, presents_animated[i].width, presents_animated[i].height);
    }
  }
}

bool check_button(uint8_t button) {
  switch (button) {
    case MAIN_SINGLE_PLAYER:
      return (cursor->pos_x >= buttons->main_single_player_button.pos_x && cursor->pos_x <= buttons->main_single_player_button.pos_x + buttons->main_single_player_button.width && cursor->pos_y >= buttons->main_single_player_button.pos_y && cursor->pos_y <= buttons->main_single_player_button.pos_y + buttons->main_single_player_button.height);
      break;
    case SWIPE_LEFT:
      return (cursor->pos_x >= buttons->swipe_left.pos_x && cursor->pos_x <= buttons->swipe_left.pos_x + buttons->swipe_left.width && cursor->pos_y >= buttons->swipe_left.pos_y && cursor->pos_y <= buttons->swipe_left.pos_y + buttons->swipe_left.height);
      break;
    case SWIPE_RIGHT:
      return (cursor->pos_x >= buttons->swipe_right.pos_x && cursor->pos_x <= buttons->swipe_right.pos_x + buttons->swipe_right.width && cursor->pos_y >= buttons->swipe_right.pos_y && cursor->pos_y <= buttons->swipe_right.pos_y + buttons->swipe_right.height);
      break;
    case START:
      return (cursor->pos_x >= buttons->start.pos_x && cursor->pos_x <= buttons->start.pos_x + buttons->start.width && cursor->pos_y >= buttons->start.pos_y && cursor->pos_y <= buttons->start.pos_y + buttons->start.height);
      break;
    case MAIN_INSTRUCTIONS:
      return (cursor->pos_x >= buttons->main_instructions_button.pos_x && cursor->pos_x <= buttons->main_instructions_button.pos_x + buttons->main_instructions_button.width && cursor->pos_y >= buttons->main_instructions_button.pos_y && cursor->pos_y <= buttons->main_instructions_button.pos_y + buttons->main_instructions_button.height);
      break;
    case MAIN_EXIT:
      return (cursor->pos_x >= buttons->main_exit_button.pos_x && cursor->pos_x <= buttons->main_exit_button.pos_x + buttons->main_exit_button.width && cursor->pos_y >= buttons->main_exit_button.pos_y && cursor->pos_y <= buttons->main_exit_button.pos_y + buttons->main_exit_button.height);
      break;
    case PAUSED_RESUME:
      return (cursor->pos_x >= buttons->paused_resume_button.pos_x && cursor->pos_x <= buttons->paused_resume_button.pos_x + buttons->paused_resume_button.width && cursor->pos_y >= buttons->paused_resume_button.pos_y && cursor->pos_y <= buttons->paused_resume_button.pos_y + buttons->paused_resume_button.height);
      break;
    case PAUSED_MAIN_MENU:
      return (cursor->pos_x >= buttons->paused_main_menu_button.pos_x && cursor->pos_x <= buttons->paused_main_menu_button.pos_x + buttons->paused_main_menu_button.width && cursor->pos_y >= buttons->paused_main_menu_button.pos_y && cursor->pos_y <= buttons->paused_main_menu_button.pos_y + buttons->paused_main_menu_button.height);
      break;
    case PAUSED_EXIT:
      return (cursor->pos_x >= buttons->paused_exit_button.pos_x && cursor->pos_x <= buttons->paused_exit_button.pos_x + buttons->paused_exit_button.width && cursor->pos_y >= buttons->paused_exit_button.pos_y && cursor->pos_y <= buttons->paused_exit_button.pos_y + buttons->paused_exit_button.height);
      break;
    case WON_MAIN_MENU:
      return (cursor->pos_x >= buttons->completed_main_menu_button.pos_x && cursor->pos_x <= buttons->completed_main_menu_button.pos_x + buttons->completed_main_menu_button.width && cursor->pos_y >= buttons->completed_main_menu_button.pos_y && cursor->pos_y <= buttons->completed_main_menu_button.pos_y + buttons->completed_main_menu_button.height);
      break;
    case WON_EXIT:
      return (cursor->pos_x >= buttons->completed_exit_button.pos_x && cursor->pos_x <= buttons->completed_exit_button.pos_x + buttons->completed_exit_button.width && cursor->pos_y >= buttons->completed_exit_button.pos_y && cursor->pos_y <= buttons->completed_exit_button.pos_y + buttons->completed_exit_button.height);
      break;
    case FAILED_MAIN_MENU:
      return (cursor->pos_x >= buttons->failed_main_menu_button.pos_x && cursor->pos_x <= buttons->failed_main_menu_button.pos_x + buttons->failed_main_menu_button.width && cursor->pos_y >= buttons->failed_main_menu_button.pos_y && cursor->pos_y <= buttons->failed_main_menu_button.pos_y + buttons->failed_main_menu_button.height);
      break;
    case FAILED_EXIT:
      return (cursor->pos_x >= buttons->failed_exit_button.pos_x && cursor->pos_x <= buttons->failed_exit_button.pos_x + buttons->failed_exit_button.width && cursor->pos_y >= buttons->failed_exit_button.pos_y && cursor->pos_y <= buttons->failed_exit_button.pos_y + buttons->failed_exit_button.height);
      break;
    case INSTRUCTIONS_HOME_BUTTON:
      return (cursor->pos_x >= buttons->instructions_home_button.pos_x && cursor->pos_x <= buttons->instructions_home_button.pos_x + buttons->instructions_home_button.width && cursor->pos_y >= buttons->instructions_home_button.pos_y && cursor->pos_y <= buttons->instructions_home_button.pos_y + buttons->instructions_home_button.height);
      break;
    default:
      break;
  }
  return 0;
}

int check_present() {
  for (unsigned i = 0; i < NUM_PRESENTS; i++) {
    if (!presents[i].catched) {
      if (((current_santa.pos_x + current_santa.width / 2) >= presents[i].pos_x) && ((current_santa.pos_x + current_santa.width / 2) <= presents[i].pos_x + presents_animated[i].width)) {
        if ((current_santa.pos_y < presents[i].pos_y) && ((current_santa.pos_y + current_santa.height) > presents[i].pos_y)) {
          return i;
        }
      }
    }
  }
  return -1;
}

void check_victory() {
  if (presents_catched == NUM_PRESENTS && game_state == TIME_OUT) {
    current_bg = *backgrounds->level_failed_2_sp;
  }
  else if (game_state == IGLO && presents_catched == NUM_PRESENTS) {
    current_bg = *backgrounds->level_completed_sp;
    game_state = WON;
  }
  else if (game_state == IGLO || game_state == TIME_OUT || santa_state == DEAD) {
    current_bg = *backgrounds->level_failed_1_sp;
    game_state = TIME_OUT;
  }
}

int check_right_left() {
  for (unsigned i = 0; i <= 4; i++) {
    if (current_santa.data == current_santa_jump_right[i]->data || current_santa.data == current_santa_walk_right[i]->data) {
      return 0; /*santa was turned to the right while falling so it will be standing to the right when he lands*/
    }
  }
  return 1; /*santa was turned to the left while falling so it will be standing to the left when he lands*/
}

void catch_present(int present_ix) {
  presents[present_ix].catched = true;
  display_xpm(main_buf, bigpresent.data, presents[present_ix].pos_x - 40, presents[present_ix].pos_y - 40, bigpresent.width, bigpresent.height);
  display_xpm(main_buf, current_santa.data, current_santa.pos_x, current_santa.pos_y, current_santa.width, current_santa.height);
  tickdelay(micros_to_ticks(1500));
  presents_catched++;
}

void update_time() {
  BCD_to_binary(&curr_time.hours);
  BCD_to_binary(&curr_time.minutes);
  curr_time.hours = binary_to_decimal(curr_time.hours);
  curr_time.minutes = binary_to_decimal(curr_time.minutes);

  rtc_time[0] = rtc_numbers[curr_time.hours / 10];
  rtc_time[1] = rtc_numbers[curr_time.hours % 10];
  rtc_time[2] = rtc_sep;
  rtc_time[3] = rtc_numbers[curr_time.minutes / 10];
  rtc_time[4] = rtc_numbers[curr_time.minutes % 10];
}

void update_cronometer() {
  int mins, secs_1, secs_2;
  mins = cronometer / 60;
  secs_1 = (cronometer % 60) / 10;
  secs_2 = (cronometer % 60) % 10;
  current_time[0] = time_numbers[mins];
  current_time[2] = time_numbers[secs_1];
  current_time[3] = time_numbers[secs_2];
}

void update_cursor() {
  /*checking if x cursor coordinates, if updated, would exceed the background limits*/
  if (mouse_packet.delta_x > 0 && cursor->pos_x + mouse_packet.delta_x + cursor->width > (uint16_t) get_h_res()) { /*mouse_packet.delta_x > 0 => trying to move rightwards*/
    cursor->pos_x = (uint16_t) get_h_res() - 1;
  }
  else if (mouse_packet.delta_x < 0 && cursor->pos_x - abs(mouse_packet.delta_x) < 0) { /*mouse_packet.delta_x < 0 => trying to move leftwards*/
    cursor->pos_x = 0;
  }
  else { /*we can safely update the x coordinate*/
    if (mouse_packet.delta_x > 0) {
      cursor->pos_x += mouse_packet.delta_x;
    }
    else if (mouse_packet.delta_x < 0) {
      cursor->pos_x -= abs(mouse_packet.delta_x);
    }
  }

  /*checking if y cursor coordinates, if updated, would exceed the background limits*/
  if (mouse_packet.delta_y < 0 && cursor->pos_y + abs(mouse_packet.delta_y) > (uint16_t) get_v_res()) { /*mouse_packet.delta_y => trying to move downwards*/
    cursor->pos_y = (uint16_t) get_v_res() - 1;
  }
  else if (mouse_packet.delta_y > 0 && cursor->pos_y - mouse_packet.delta_y < 0) { /*trying to move upwards*/
    cursor->pos_y = 0;
  }
  else { /*we can safely update the y coordinate*/
    if (mouse_packet.delta_y > 0) {
      cursor->pos_y -= mouse_packet.delta_y;
    }
    else if (mouse_packet.delta_y < 0) {
      cursor->pos_y += abs(mouse_packet.delta_y);
    }
  }
}

void set_santa(struct sprite *new_sp) {
  current_santa.data = new_sp->data;
  current_santa.height = new_sp->height;
  current_santa.width = new_sp->width;
}

void set_player() {
  if (char_state == COVID_SANTA) {
    current_santa = *covid_santa_sp;
    for (unsigned int i = 0; i < 5; i++) {
      current_santa_walk_right[i] = covid_santa_walk_right[i];
      current_santa_walk_left[i] = covid_santa_walk_left[i];
      current_santa_jump_left[i] = covid_santa_jump_left[i];
      current_santa_jump_right[i] = covid_santa_jump_right[i];
    }
    curr_santa_sp = covid_santa_sp;
  }
  else if (char_state == HAPPY_SANTA) {
    current_santa = *santa_sp;
    for (unsigned int i = 0; i < 5; i++) {
      current_santa_walk_right[i] = santa_walk_right[i];
      current_santa_walk_left[i] = santa_walk_left[i];
      current_santa_jump_left[i] = santa_jump_left[i];
      current_santa_jump_right[i] = santa_jump_right[i];
    }
    curr_santa_sp = santa_sp;
  }
}

void reset_santa() {
  moving = false;
  sea_back->pos_x = SEA_BACK_X0;
  sea_middle->pos_x = SEA_MIDDLE_X0;
  reset_presents();
  reset_cronometer();
  current_santa.data = curr_santa_sp->data;
  current_santa.height = curr_santa_sp->height;
  current_santa.width = curr_santa_sp->width;
  current_santa.pos_x = curr_santa_sp->pos_x;
  current_santa.pos_y = curr_santa_sp->pos_y;
  santa_state = STANDING_RIGHT;
}

void reset_presents() {
  presents_catched = 0;

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

void reset_cronometer() {
  cronometer = TIME_LIMIT;
  update_cronometer();
}

void mouse_events_handler_while_playing() {
  if (mouse_packet.delta_x > 0) { /*trying to move to the right*/
    switch (santa_state) {
      case JUMPING_RIGHT:
        santa_state = JUMPING_RIGHT;
        break;
      case JUMPING_LEFT:
        santa_state = JUMPING_RIGHT;
        break;
      case FALLING_RIGHT:
        santa_state = FALLING_RIGHT;
        break;
      case FALLING_LEFT:
        santa_state = FALLING_RIGHT;
        break;
      default:
        santa_state = WALKING_RIGHT;
        break;
    }
  }
  else if (mouse_packet.delta_x < 0) { /*trying to move to the left*/
    switch (santa_state) {
      case JUMPING_RIGHT:
        santa_state = JUMPING_LEFT;
        break;
      case JUMPING_LEFT:
        santa_state = JUMPING_LEFT;
        break;
      case FALLING_RIGHT:
        santa_state = FALLING_LEFT;
        break;
      case FALLING_LEFT:
        santa_state = FALLING_LEFT;
        break;
      default:
        santa_state = WALKING_LEFT;
        break;
    }
  }
}

void mouse_events_handler() {
  if (game_state == PLAYING) {
    mouse_events_handler_while_playing();
  }
  else if (!mouse_packet.lb && game_state != INSTRUCTIONS) { //no relevant button was pressed
    return;
  }
  switch (game_state) {
    case INITMENU:
      if (check_button(MAIN_SINGLE_PLAYER)) {
        game_state = CHOOSE_PLAYER;
      }
      else if (check_button(MAIN_INSTRUCTIONS)) {
        game_state = INSTRUCTIONS;
        current_bg = *backgrounds->instructions_sp;
      }
      else if (check_button(MAIN_EXIT)) {
        game_state = EXIT;
      }
      break;
    case INSTRUCTIONS:
      if (check_button(INSTRUCTIONS_HOME_BUTTON) && mouse_packet.lb) {
        game_state = INITMENU;
        current_bg = *backgrounds->init_menu_sp;
        instructions_ov_y = backgrounds->instructions_txt->pos_y;
      }
      if (mouse_packet.delta_y > 0) { /*moving instructions text downwards*/
        if (instructions_ov_y != -540)
          instructions_ov_y -= STEP_TXT;
      }
      else if (mouse_packet.delta_y < 0) { /*moving instructions text upwards*/
        if (instructions_ov_y != 270)
          instructions_ov_y += STEP_TXT;
      }
      break;
    case CHOOSE_PLAYER:
      if (check_button(SWIPE_RIGHT)) {
        if (moving_left) {
          moving_right = true;
          struct sprite *aux = players[0];
          players[0] = players[1];
          players[1] = aux;
        }
        else if (!moving_left && !moving_right) {
          moving_right = true;
          players[(choose_player_ix % 2)]->pos_x = IN_POS_X;
          players[((choose_player_ix + 1) % 2)]->pos_x = OUT_LEFT_POS_X;
        }
        moving_left = false;
      }
      else if (check_button(SWIPE_LEFT)) {
        if (moving_right) {
          moving_left = true;
          struct sprite *aux = players[0];
          players[0] = players[1];
          players[1] = aux;
        }
        else if (!moving_left && !moving_right) {
          moving_left = true;
          players[(choose_player_ix % 2)]->pos_x = IN_POS_X;
          players[((choose_player_ix + 1) % 2)]->pos_x = OUT_RIGHT_POS_X;
        }
        moving_right = false;
      }
      else if (check_button(START)) {
        game_state = PLAYING;
        set_player();
        current_bg = *backgrounds->background_sp;
      }
      break;
    case PAUSE:
      if (check_button(PAUSED_RESUME)) {
        game_state = PLAYING;
        current_bg = *backgrounds->background_sp;
      }
      else if (check_button(PAUSED_MAIN_MENU)) {
        game_state = INITMENU;
        current_bg = *backgrounds->init_menu_sp;
      }
      else if (check_button(PAUSED_EXIT)) {
        game_state = EXIT;
      }
      break;
    case WON:
      if (check_button(WON_MAIN_MENU)) {
        game_state = INITMENU;
        current_bg = *backgrounds->init_menu_sp;
      }
      else if (check_button(WON_EXIT)) {
        game_state = EXIT;
      }
      break;
    case TIME_OUT:
      if (check_button(FAILED_MAIN_MENU)) {
        game_state = INITMENU;
        current_bg = *backgrounds->init_menu_sp;
      }
      else if (check_button(FAILED_EXIT)) {
        game_state = EXIT;
      }
      break;
    case EXIT:
      break;
    default:
      break;
  }
}

void set_keyboard_events() {
  if (keyboard_data == BREAK_ESC)
    keyboard_event = ESC;
  else if (keyboard_data == MAKE_LEFT_ARROW || keyboard_data == MAKE_LEFT_A) {
    keyboard_event = LEFT;
  }
  else if (keyboard_data == MAKE_RIGHT_ARROW || keyboard_data == MAKE_RIGHT_B) {
    keyboard_event = RIGHT;
  }
  else if (keyboard_data == MAKE_UP_W || keyboard_data == MAKE_UP_ARROW) {
    keyboard_event = UP;
  }
  else {
    keyboard_event = RELEASED;
  }
}

void keyboard_events_handler() {
  set_keyboard_events();
  switch (keyboard_event) {
    case ESC:
      if (keyboard_event == ESC) {
        game_state = PAUSE;
        current_bg = *backgrounds->paused_sp;
      }
      break;
    case LEFT:
      switch (santa_state) {
        case FALLING:
          santa_state = FALLING_LEFT;
          break;
        case FALLING_RIGHT:
          santa_state = FALLING_LEFT;
          break;
        case FALLING_LEFT:
          santa_state = FALLING_LEFT;
          break;
        case JUMPING_RIGHT:
          santa_state = JUMPING_LEFT;
          break;
        case JUMPING_LEFT:
          santa_state = JUMPING_LEFT;
          break;
        default:

          santa_state = WALKING_LEFT;

          break;
      }
      break;
    case RIGHT:
      switch (santa_state) {
        case FALLING:
          santa_state = FALLING_RIGHT;
          break;
        case FALLING_RIGHT:
          santa_state = FALLING_RIGHT;
          break;
        case FALLING_LEFT:
          santa_state = FALLING_RIGHT;
          break;
        case JUMPING_RIGHT:
          santa_state = JUMPING_RIGHT;
          break;
        case JUMPING_LEFT:
          santa_state = JUMPING_RIGHT;
          break;
        default:
          santa_state = WALKING_RIGHT;

          break;
      }
      break;
    case UP:
      switch (santa_state) {
        case STANDING_LEFT:
          santa_state = JUMPING_LEFT;
          break;
        case WALKING_LEFT:
          santa_state = JUMPING_LEFT;
          break;
        case JUMPING_LEFT:
          santa_state = JUMPING_LEFT;
          break;
        case FALLING_LEFT:
          santa_state = JUMPING_LEFT;
          break;
        case STANDING_RIGHT:
          santa_state = JUMPING_RIGHT;
          break;
        case WALKING_RIGHT:
          santa_state = JUMPING_RIGHT;
          break;
        case JUMPING_RIGHT:
          santa_state = JUMPING_RIGHT;
          break;
        case FALLING_RIGHT:
          santa_state = JUMPING_RIGHT;
          break;
        default:
          if (check_right_left() == 0)
            santa_state = JUMPING_RIGHT;
          else {
            santa_state = JUMPING_LEFT;
          }
          break;
      }
      break;
    case RELEASED:
      switch (santa_state) {
        case STANDING_RIGHT:
          santa_state = STANDING_RIGHT;
          break;
        case WALKING_RIGHT:
          santa_state = STANDING_RIGHT;
          break;
        case JUMPING_RIGHT:
          santa_state = JUMPING_RIGHT;
          break;
        case FALLING_RIGHT:
          santa_state = FALLING_RIGHT;
          break;
        case STANDING_LEFT:
          santa_state = STANDING_LEFT;
          break;
        case WALKING_LEFT:
          santa_state = STANDING_LEFT;
          break;
        case JUMPING_LEFT:
          santa_state = JUMPING_LEFT;
          break;
        case FALLING_LEFT:
          santa_state = FALLING_LEFT;
          break;
        default:
          if (check_right_left() == 0)
            santa_state = FALLING_RIGHT;
          else {
            santa_state = FALLING_LEFT;
          }
          break;
      }
      break;
  }
}

void timer_events_handler() {
  if (game_state == PLAYING) {
    update_sea_pos();
    if ((timer_counter % 60 == 0) && moving) {
      cronometer--;
      update_cronometer();
    }
    if (cronometer == 0) {
      game_state = TIME_OUT;
      reset_cronometer();
    }
  }
  else if (game_state == INSTRUCTIONS) {
    update_display_instructions();
  }
  else if (game_state == CHOOSE_PLAYER) {
    if (moving_right) {
      if (players[((choose_player_ix + 1) % 2)]->pos_x == IN_POS_X) {
        moving_right = false;
        moving_left = false;
        choose_player_ix++;
        switch (char_state) {
          case COVID_SANTA:
            char_state = HAPPY_SANTA;
            break;
          case HAPPY_SANTA:
            char_state = COVID_SANTA;
          default:
            break;
        }
      }
      else {
        players[0]->pos_x++;
        players[1]->pos_x++;
      }
    }
    else if (moving_left) {
      if (players[((choose_player_ix + 1) % 2)]->pos_x == IN_POS_X) {
        moving_right = false;
        moving_left = false;
        choose_player_ix++;
        switch (char_state) {
          case COVID_SANTA:
            char_state = HAPPY_SANTA;
            break;
          case HAPPY_SANTA:
            char_state = COVID_SANTA;
          default:
            break;
        }
      }
      else {
        players[0]->pos_x--;
        players[1]->pos_x--;
      }
    }
    update_display_player();
  }
  else {
    update_display();
  }
}

int get_current_platform() {
  for (unsigned i = 0; i < NUM_PLATFORMS; i++) {
    if (((current_santa.pos_y + current_santa.height) <= platforms[i].pos_y + DELTA_Y) && ((current_santa.pos_y + current_santa.height) >= platforms[i].pos_y - DELTA_Y)) {
      if (((current_santa.pos_x + current_santa.width / 2) >= platforms[i].pos_x) && ((current_santa.pos_x + current_santa.width / 2) <= (platforms[i].pos_x + platforms[i].width))) {
        return (int) i;
      }
    }
  }
  return -1;
}
/*
int get_current_platform() {
  for (unsigned i = 0; i < NUM_PLATFORMS; i++) {
    if (((current_santa.pos_y + current_santa.height) <= platforms[i].pos_y + DELTA_Y) && ((current_santa.pos_y + current_santa.height) >= platforms[i].pos_y - DELTA_Y)) {
      if (current_santa.pos_x <= current_santa.width / 2) {
        if (((current_santa.pos_x + (current_santa.width / 2)) <= (platforms[i].pos_x + platforms[i].width)) && (current_santa.pos_x >= platforms[i].pos_x)) {
          return ((int) i);
        }
      }
      else if (((current_santa.pos_x) <= (platforms[i].pos_x + platforms[i].width)) && ((current_santa.pos_x + (current_santa.width / 3)) >= platforms[i].pos_x)) {
        return ((int) i);
      }
      if (i == 3 || i == 6) {
        if ((((current_santa.pos_x >= platforms[i].pos_x) && (current_santa.pos_x <= (platforms[i].pos_x + platforms[i].width))) || (((current_santa.pos_x + current_santa.width) >= platforms[i].pos_x) && (current_santa.pos_x + current_santa.width) <= (platforms[i].pos_x + platforms[i].width)))) {
          return (int) i;
        else if (((current_santa.pos_x + current_santa.width/2 ) >platforms[i].pos_x ) && ((current_santa.pos_x + current_santa.width/2) <(platforms[i].pos_x + platforms[i].width)))
        }
      }
    }
  }
  return -1;
}
*/
void check_collision(uint16_t step) {
  int curr_platform;
  switch (santa_state) {
    case WALKING_LEFT:
      curr_platform = get_current_platform();
      switch (curr_platform) {
        case 0:
          if ((int) current_santa.pos_x - (int) step < 0) {
            current_santa.pos_x = 0;
          }
          break;
        case 1:
          if ((current_santa.pos_x + (current_santa.width / 2) - step) < platforms[curr_platform].pos_x) {
            set_santa(current_santa_jump_left[4]);
            santa_state = FALLING;
          }
          break;
        case 2:
          if ((current_santa.pos_x + (current_santa.width / 2) - step) < platforms[curr_platform].pos_x) {
            set_santa(current_santa_jump_left[4]);
            santa_state = FALLING;
          }
          break;
        case 3:
          if ((current_santa.pos_x + (current_santa.width / 2) - step) < platforms[curr_platform].pos_x) {
            set_santa(current_santa_jump_left[4]);
            santa_state = FALLING;
          }
          break;
        case 4:
          if ((current_santa.pos_x + (current_santa.width / 2) - step) < platforms[curr_platform].pos_x) {
            current_santa.pos_y = platforms[curr_platform].pos_y - current_santa.height;
            santa_state = STANDING_LEFT;
          }
          break;
        case 5:
          if ((current_santa.pos_x + (current_santa.width / 2) - step) < platforms[curr_platform].pos_x) {
            set_santa(current_santa_jump_left[4]);
            santa_state = FALLING;
          }
          break;
        case 6:
          if ((current_santa.pos_x + (current_santa.width / 2) - step) < platforms[curr_platform].pos_x + 15) {
            set_santa(current_santa_jump_left[4]);
            santa_state = FALLING;
          }
          break;
        case 7:
          if ((current_santa.pos_x + (current_santa.width / 2) - step) < platforms[curr_platform].pos_x) {
            set_santa(current_santa_jump_left[4]);
            santa_state = FALLING;
          }
          break;
        case 8:
          if ((current_santa.pos_x + (current_santa.width / 2) - step) < platforms[curr_platform].pos_x) {
            set_santa(current_santa_jump_left[4]);
            santa_state = FALLING;
          }
          break;
        default:
          set_santa(current_santa_jump_left[4]);
          santa_state = FALLING;
          break;
      }
      if (santa_state != STANDING_LEFT && ((int) current_santa.pos_x - step) > 0) {
        current_santa.pos_x -= step;
      }
      if (step == abs(mouse_packet.delta_x) && santa_state != FALLING) { /*santa was moved using the mouse*/
        santa_state = STANDING_LEFT;
      }
      break;
    case WALKING_RIGHT:
      if (current_santa.pos_x + current_santa.width + step > get_h_res())
        return;
      curr_platform = get_current_platform();

      switch (curr_platform) {
        case 0:
          if ((current_santa.pos_x + (current_santa.width / 2) + step) > (platforms[curr_platform].pos_x + platforms[curr_platform].width)) {
            set_santa(current_santa_jump_right[4]);
            santa_state = FALLING;
          }
          break;
        case 1:
          if ((current_santa.pos_x + (current_santa.width / 2) + step) > (platforms[curr_platform].pos_x + platforms[curr_platform].width)) {
            santa_state = STANDING_RIGHT;
            current_santa.pos_y = platforms[curr_platform].pos_y - current_santa.height;
          }
          break;
        case 2:
          if ((current_santa.pos_x + (current_santa.width / 2) + step) > (platforms[curr_platform].pos_x + platforms[curr_platform].width)) {
            set_santa(current_santa_jump_right[4]);
            santa_state = FALLING;
          }
          break;
        case 3:
          if ((current_santa.pos_x + (current_santa.width / 2) + step) > (platforms[curr_platform].pos_x + platforms[curr_platform].width)) {
            set_santa(current_santa_jump_right[4]);
            santa_state = FALLING;
          }
          break;
        case 4:
          if ((current_santa.pos_x + (current_santa.width / 2) + step) > (platforms[curr_platform].pos_x + platforms[curr_platform].width)) {
            set_santa(current_santa_jump_right[4]);
            santa_state = FALLING;
          }
          break;
        case 5:
          if ((current_santa.pos_x + (current_santa.width / 2) + step) > (platforms[curr_platform].pos_x + platforms[curr_platform].width)) {
            set_santa(current_santa_jump_right[4]);
            santa_state = FALLING;
          }
          break;
        case 6:
          if ((current_santa.pos_x + (current_santa.width / 2) + step) > (platforms[curr_platform].pos_x + 99)) {
            set_santa(current_santa_jump_right[4]);
            santa_state = FALLING;
          }
          break;
        case 7:
          if ((current_santa.pos_x + (current_santa.width / 2) + step) >= IGLO_X) {
            game_state = IGLO;
          }
          break;
        case 8:
          if ((current_santa.pos_x + (current_santa.width / 2) + step) > (platforms[curr_platform].pos_x + platforms[curr_platform].width)) {
            santa_state = FALLING;
            set_santa(current_santa_jump_right[4]);
          }
          break;
        default:
          set_santa(current_santa_jump_right[4]);
          santa_state = FALLING;
          break;
      }
      if (santa_state != STANDING_RIGHT && (current_santa.pos_x + step < get_h_res())) {
        current_santa.pos_x += step;
      }
      if (step == abs(mouse_packet.delta_x) && santa_state != FALLING) { /*santa was moved using the mouse*/
        santa_state = STANDING_RIGHT;
        current_santa.pos_y = platforms[curr_platform].pos_y - current_santa.height;
      }
      if (current_santa.pos_x > get_h_res())
        current_santa.pos_x = get_h_res() - current_santa.width;
      break;
    case JUMPING_RIGHT:

      if (current_santa.pos_x + current_santa.width + step > get_h_res()) {
        current_santa.pos_x = get_h_res() - current_santa.width;
      }
      else if (current_santa.pos_y - STEP_JUMP_Y < 0) {
        set_santa(current_santa_jump_right[4]);
        santa_state = FALLING;
      }
      else if (current_santa.pos_x + current_santa.width + STEP_JUMP_X > get_h_res()) {
        current_santa.pos_y -= STEP_JUMP_Y;
      }
      else {
        current_santa.pos_x += STEP_JUMP_X;
        current_santa.pos_y -= STEP_JUMP_Y;
      }
      break;
    case JUMPING_LEFT:
      if ((int) current_santa.pos_x - STEP_JUMP_X < 0) {
        current_santa.pos_y -= STEP_JUMP_Y;
        set_santa(current_santa_jump_left[4]);
        santa_state = FALLING;
      }
      else if (current_santa.pos_y - STEP_JUMP_Y < 0 || current_santa.pos_x - step < 0) {
        set_santa(current_santa_jump_left[4]);
        santa_state = FALLING;
      }
      else {
        current_santa.pos_x -= STEP_JUMP_X;
        current_santa.pos_y -= STEP_JUMP_Y;
      }
      break;
    case FALLING:

      for (unsigned i = 0; i < NUM_PLATFORMS; i++) {
        if (((current_santa.pos_y + current_santa.height + STEP_JUMP_Y * (2)) >= platforms[i].pos_y) && ((current_santa.pos_y + current_santa.height) <= platforms[i].pos_y)) {
          /*santa would cross a plataform so he will stay standing on it*/
          if (i == 6 || i == 8) {
            if ((((current_santa.pos_x) >= platforms[i].pos_x) && ((current_santa.pos_x) <= (platforms[i].pos_x + platforms[i].width))) && (((current_santa.pos_x + current_santa.width) >= platforms[i].pos_x) && ((current_santa.pos_x + current_santa.width) <= (platforms[i].pos_x + platforms[i].width)))) {
              current_santa.pos_y = platforms[i].pos_y - current_santa.height;
              santa_sp_down_counter = 0;
              if (check_right_left() == 0) {
                santa_state = STANDING_RIGHT;
                set_santa(current_santa_walk_right[4]);
              }
              else {
                santa_state = STANDING_LEFT;
                set_santa(current_santa_walk_left[4]);
              }
              break;
            }
          }
          if (((current_santa.pos_x + current_santa.width / 2) >= platforms[i].pos_x) && ((current_santa.pos_x + current_santa.width / 2) <= (platforms[i].pos_x + platforms[i].width))) {
            current_santa.pos_y = platforms[i].pos_y - current_santa.height;
            santa_sp_down_counter = 0;
            if (check_right_left() == 0) {
              santa_state = STANDING_RIGHT;
              set_santa(current_santa_walk_right[4]);
            }
            else {
              santa_state = STANDING_LEFT;
              set_santa(current_santa_walk_left[4]);
            }
          }
        }

        update_playing_display();
        break;
      }

      if (santa_state != STANDING_RIGHT && santa_state != STANDING_LEFT) {
        current_santa.pos_y += STEP_JUMP_Y;
      }
      if (current_santa.pos_y + current_santa.height > get_v_res()) {
        current_santa.height -= STEP_JUMP_Y;
      }
      if (current_santa.pos_y >= get_v_res() - DELTA_Y) {
        santa_state = DEAD;
      }
      break;
    case FALLING_RIGHT:
      if (current_santa.pos_x + current_santa.width > get_h_res()) {
        santa_state = FALLING;
        current_santa.pos_x = get_h_res() - current_santa.width;
        break;
      }
      for (unsigned i = 0; i < NUM_PLATFORMS; i++) {
        if (((current_santa.pos_y + current_santa.height + STEP_JUMP_Y * 2) >= platforms[i].pos_y) && ((current_santa.pos_y + current_santa.height) <= platforms[i].pos_y)) {
          if (i == 6 || i == 8) {
            if ((((current_santa.pos_x) >= platforms[i].pos_x) && ((current_santa.pos_x) <= (platforms[i].pos_x + platforms[i].width))) && (((current_santa.pos_x + current_santa.width) >= platforms[i].pos_x) && ((current_santa.pos_x + current_santa.width) <= (platforms[i].pos_x + platforms[i].width)))) {
              current_santa.pos_y = platforms[i].pos_y - current_santa.height;
              santa_sp_down_counter = 0;
              if (check_right_left() == 0) {
                santa_state = STANDING_RIGHT;
                set_santa(current_santa_walk_right[4]);
              }
              else {
                santa_state = STANDING_LEFT;
                set_santa(current_santa_walk_left[4]);
              }
              break;
            }
          }
          if (((current_santa.pos_x + current_santa.width / 2) >= platforms[i].pos_x) && ((current_santa.pos_x + (current_santa.width / 2)) <= (platforms[i].pos_x + platforms[i].width))) {
            current_santa.pos_x += step;
            current_santa.pos_y = platforms[i].pos_y - current_santa.height;
            santa_sp_down_counter = 0;
            santa_state = STANDING_RIGHT;
            set_santa(curr_santa_sp);
            update_playing_display();
            break;
          }
        }
      }
      if (santa_state != STANDING_RIGHT) {
        current_santa.pos_x += STEP_JUMP_X;
        current_santa.pos_y += STEP_JUMP_Y;
      }
      if (current_santa.pos_y + current_santa.height > get_v_res()) {
        current_santa.height -= STEP_JUMP_Y;
      }
      if (current_santa.pos_y >= get_v_res() - DELTA_Y) {
        santa_state = DEAD;
      }
      update_playing_display();

      break;
    case FALLING_LEFT:
      if ((int) current_santa.pos_x - STEP_JUMP_X < 0) {
        current_santa.pos_x = 0;
        santa_state = FALLING;
        break;
      }
      for (unsigned i = 0; i < NUM_PLATFORMS; i++) {
        if (((current_santa.pos_y + current_santa.height + STEP_JUMP_Y * 2) >= platforms[i].pos_y) && ((current_santa.pos_y + current_santa.height) <= platforms[i].pos_y)) {
          if (i == 6 || i == 8) {
            if ((((current_santa.pos_x) >= platforms[i].pos_x) && ((current_santa.pos_x) <= (platforms[i].pos_x + platforms[i].width))) && (((current_santa.pos_x + current_santa.width) >= platforms[i].pos_x) && ((current_santa.pos_x + current_santa.width) <= (platforms[i].pos_x + platforms[i].width)))) {
              current_santa.pos_y = platforms[i].pos_y - current_santa.height;
              santa_sp_down_counter = 0;
              if (check_right_left() == 0) {
                santa_state = STANDING_RIGHT;
                set_santa(current_santa_walk_right[4]);
              }
              else {
                santa_state = STANDING_LEFT;
                set_santa(current_santa_walk_left[4]);
              }
              break;
            }
          }
          if (((current_santa.pos_x + current_santa.width / 2) >= platforms[i].pos_x) && ((current_santa.pos_x + (current_santa.width / 2)) <= (platforms[i].pos_x + platforms[i].width))) {
            current_santa.pos_x += step;
            current_santa.pos_y = platforms[i].pos_y - current_santa.height;
            santa_sp_down_counter = 0;
            santa_state = STANDING_LEFT;
            set_santa(curr_santa_sp);
            update_playing_display();
            break;
          }
        }
      }
      if (santa_state != STANDING_LEFT) {
        current_santa.pos_x -= STEP_JUMP_X;
        current_santa.pos_y += STEP_JUMP_Y;
      }
      if (current_santa.pos_y + current_santa.height > get_v_res()) {
        current_santa.height -= STEP_JUMP_Y;
      }
      if (current_santa.pos_y == get_v_res() - DELTA_Y) {
        santa_state = DEAD;
      }

      break;

    default:
      break;
  }
  int present_catched = check_present();
  if (present_catched != -1) {
    catch_present(present_catched);
  }
}

void move_santa_keyboard() {
  int kb_santa_sp_left_counter, kb_santa_sp_right_counter;
  switch (santa_state) {
    case WALKING_LEFT:
      kb_santa_sp_left_counter = 0;
      while (kb_santa_sp_left_counter < 5) {
        set_santa(current_santa_walk_left[kb_santa_sp_left_counter % 5]);
        check_collision(STEP_KB);
        if (santa_state == FALLING)
          break;
        kb_santa_sp_left_counter++;

        tickdelay(micros_to_ticks(WAIT));

        update_sea_pos();
        counter++;

        update_playing_display();
      }
      break;
    case WALKING_RIGHT:
      kb_santa_sp_right_counter = 1;
      while (kb_santa_sp_right_counter < 5) {
        set_santa(current_santa_walk_right[kb_santa_sp_right_counter % 5]);
        check_collision(STEP_KB);
        if (santa_state == FALLING)
          break;
        kb_santa_sp_right_counter++;

        update_sea_pos();
        counter++;

        tickdelay(micros_to_ticks(WAIT));
        update_playing_display();
      }
      break;
    case JUMPING_RIGHT:
      set_santa(current_santa_jump_right[santa_sp_up_counter % 5]);
      check_collision(STEP_KB);
      santa_sp_up_counter++;
      tickdelay(micros_to_ticks(WAIT));
      update_playing_display();
      if (santa_sp_up_counter % 11 == 0) {
        santa_sp_up_counter = 0;
        santa_state = FALLING_RIGHT;
      }
      break;
    case JUMPING_LEFT:
      set_santa(current_santa_jump_left[santa_sp_up_counter % 5]);
      check_collision(STEP_KB);
      santa_sp_up_counter++;
      update_playing_display();
      if (santa_sp_up_counter % 11 == 0) {
        santa_sp_up_counter = 0;
        santa_state = FALLING_LEFT;
      }
      break;
    case FALLING_RIGHT:
      set_santa(current_santa_jump_right[4]);
      check_collision(STEP_KB);
      santa_sp_down_counter++;
      update_playing_display();
      break;
    case FALLING_LEFT:
      set_santa(current_santa_jump_left[4]);
      check_collision(STEP_KB);
      santa_sp_down_counter++;
      update_playing_display();
      break;
    case FALLING:
      check_collision(STEP_KB);
      santa_sp_down_counter++;
      update_playing_display();
      break;
    default:
      break;
  }
}

void move_santa_mouse() {
  switch (santa_state) {
    case WALKING_LEFT:
      set_santa(current_santa_walk_left[mouse_santa_sp_left_counter % 5]);
      check_collision(abs(mouse_packet.delta_x));
      mouse_santa_sp_left_counter++;
      break;
    case WALKING_RIGHT:
      set_santa(current_santa_walk_right[mouse_santa_sp_right_counter % 5]);
      check_collision(abs(mouse_packet.delta_x));
      mouse_santa_sp_right_counter++;
      break;
    case JUMPING_RIGHT:
      set_santa(current_santa_jump_right[santa_sp_up_counter % 5]);
      check_collision(abs(mouse_packet.delta_x));
      santa_sp_up_counter++;
      if (santa_sp_up_counter % 11 == 0) {
        update_sea_pos();
        santa_sp_up_counter = 0;
        santa_state = FALLING_RIGHT;
      }
      break;
    case JUMPING_LEFT:
      set_santa(current_santa_jump_left[santa_sp_up_counter % 5]);
      check_collision(abs(mouse_packet.delta_x));
      santa_sp_up_counter++;
      if (santa_sp_up_counter % 11 == 0) {
        update_sea_pos();
        santa_sp_up_counter = 0;
        santa_state = FALLING_LEFT;
      }
      break;
    case FALLING_RIGHT:
      set_santa(current_santa_jump_right[4 - (santa_sp_down_counter % 5)]);
      check_collision(abs(mouse_packet.delta_x));
      santa_sp_down_counter++;
      break;
    case FALLING_LEFT:
      set_santa(current_santa_jump_left[4 - (santa_sp_down_counter % 5)]);
      check_collision(abs(mouse_packet.delta_x));
      santa_sp_down_counter++;
      break;
    default:
      break;
  }
  tickdelay(micros_to_ticks(WAIT));
}
