#ifndef MACROS
#define MACROS

#include <lcom/lcf.h>

//TIMER

#define TIMER_FREQ 1193182 /**< @brief clock frequency for timer in PC and AT */
#define TIMER0_IRQ 0       /**< @brief Timer 0 IRQ line */

/* I/O port addresses */

#define TIMER_0 0x40    /**< @brief Timer 0 count register */
#define TIMER_1 0x41    /**< @brief Timer 1 count register */
#define TIMER_2 0x42    /**< @brief Timer 2 count register */
#define TIMER_CTRL 0x43 /**< @brief Control register */

#define SPEAKER_CTRL 0x61 /**< @brief Register for speaker control  */

/* Timer control */

/* Timer selection: bits 7 and 6 */

#define TIMER_SEL0 0x00                /**< @brief Control Word for Timer 0 */
#define TIMER_SEL1 BIT(6)              /**< @brief Control Word for Timer 1 */
#define TIMER_SEL2 BIT(7)              /**< @brief Control Word for Timer 2 */
#define TIMER_RB_CMD (BIT(7) | BIT(6)) /**< @brief Read Back Command */

/* Register selection: bits 5 and 4 */

#define TIMER_LSB BIT(4)                      /**< @brief Initialize Counter LSB only */
#define TIMER_MSB BIT(5)                      /**< @brief Initialize Counter MSB only */
#define TIMER_LSB_MSB (TIMER_LSB | TIMER_MSB) /**< @brief Initialize LSB first and MSB afterwards */

/* Operating mode: bits 3, 2 and 1 */

#define TIMER_SQR_WAVE (BIT(2) | BIT(1)) /**< @brief Mode 3: square wave generator */
#define TIMER_RATE_GEN BIT(2)            /**< @brief Mode 2: rate generator */

/* Counting mode: bit 0 */

#define TIMER_BCD 0x01 /**< @brief Count in BCD */
#define TIMER_BIN 0x00 /**< @brief Count in binary */

/* READ-BACK COMMAND FORMAT */

#define TIMER_RB_COUNT_ BIT(5)
#define TIMER_RB_STATUS_ BIT(4)
#define TIMER_RB_SEL(n) BIT((n) + 1)

// KBC

#define KBC_ST_REG 0x64 /**< @brief Status register */

#define KBC_CMD_REG 0x64 /**< @brief Command register */
#define KBC_OUT_BUF 0x60 /**< @brief Output buffer */

#define KBC_OBF BIT(0)     /**< @brief Output buffer full */
#define KBC_IBF BIT(1)     /**< @brief Input buffer full */
#define AUX BIT(5)         /**< @brief Incoming data from mouse*/
#define KBC_TO_ERR BIT(6)  /**< @brief Time Out Error */
#define KBC_PAR_ERR BIT(7) /**< @brief Parity Error*/
#define EN_KBD 0xAE        /**< @brief Enable KBD*/
#define DIS_KBD 0xAD       /**< @brief Disable KBD*/

/*MOUSE RELATED KBC COMMANDS*/
#define READ_CB 0x20     /**< @brief Read command byte*/
#define WRITE_CB 0x60    /**< @brief Write command byte*/
#define EN_MOUSE 0xA8    /**< @brief Enable mouse*/
#define DIS_MOUSE 0xA7   /**< @brief Disable mouse*/
#define CHECK_INTER 0xA9 /**< @brief Check mouse interface*/
#define WRITE_BYTE 0xD4  /**< @brief Write byte to mouse command*/

/*STATUS REGISTER*/
#define PARITY BIT(7)  /**< @brief Parity Error*/
#define TIMEOUT BIT(6) /**< @brief Time Out Error */
#define AUX BIT(5)     /**< @brief Incoming data from mouse*/
#define A2 BIT(3)      //A2 input line, 0: data byte, 1: command byte
#define IBF BIT(1)     /**< @brief Input buffer full */
#define OBF BIT(0)     /**< @brief Output buffer full */

/*KBC COMMAND BYTE*/
#define INT BIT(0)  /**< @brief Enable interrupt on OUT_BUF, from keyboard*/
#define INT2 BIT(1) /**< @brief Enable interrupt on OUT_BUF, from mouse*/
#define DIS BIT(4)  /**< @brief Disable keyboard interface*/
#define DIS2 BIT(5) /**< @brief Disable mouse interface*/

/*MOUSE COMMANDS*/
#define RESET 0xFF           /**< @brief Mouse reset*/
#define RESEND 0xFE          /**< @brief Communication error*/
#define SET_DEFAULTS 0xF6    /**< @brief Set default values*/
#define DISABLE 0xF5         /**< @brief Disable stream mode*/
#define ENABLE 0xF4          /**< @brief Enable stream mode*/
#define SET_SAMPLE_RATE 0xF3 /**< @brief Set sample rate*/
#define SET_REMOTE_MODE 0xF0 /**< @brief Set remote mode*/
#define READ_DATA 0xEB       /**< @brief Polling mouse data*/
#define SET_STREAM_MODE 0xFA /**< @brief Set stream mode*/
#define STATUS_REQUEST 0xF9  /**< @brief Get mouse configuration*/

/*MOUSE CONTROLLER ACKNOWLEDGMENT BYTE*/
#define ACK 0xFA    /**< @brief Acknowledgment byte ok*/
#define NACK 0xFE   /**< @brief Acknowledgment byte invalid*/
#define ERROR 0xFC  /**< @brief Acknowledgment byte error*/
#define REMOTE 0xF0 /**< @brief Remote mode*/
#define STREAM 0xEA /**< @brief  Stream mode*/

#define WAIT_KBC 20000 /**< @brief ms time to wait*/
#define KBC_IRQ 1      /**< @brief KBC IRQ LINE*/
#define TIMER0_IRQ 0   /**< @brief TIMER IRQ LINE*/
#define MOUSE_IRQ 12   /**< @brief MOUSE IRQ LINE*/
#define BREAK_ESC 0x81 /**< @brief ESC BREAK CODE*/

/*COMMAND BYTE 1 BITS MEANING */

#define LB BIT(0)          /**< @brief LEFT BUTTON PRESSED*/
#define RB BIT(1)          /**< @brief RIGHT BUTTON PRESSED*/
#define MB BIT(2)          /**< @brief MID BUTTON PRESSED*/
#define MSB_X_DELTA BIT(4) /**< @brief MOUSE DELTA X*/
#define MSB_Y_DELTA BIT(5) /**< @brief MOUSE DELTA Y*/
#define X_OV BIT(6)        /**< @brief MOUSE DELTA X OVERFLOW*/
#define Y_OV BIT(7)        /**< @brief MOUSE DELTA Y OVERFLOW*/

/*BUTTONS*/

#define MAIN_SINGLE_PLAYER 11      /**< @brief SINGLE PLAYER BUTTON - MAIN MENU*/
#define MAIN_INSTRUCTIONS 12       /**< @brief INSTRUCTIONS BUTTON - MAIN MENU*/
#define MAIN_EXIT 13               /**< @brief EXIT BUTTON - MAIN MENU*/
#define PAUSED_RESUME 21           /**< @brief PAUSE BUTTON - PAUSE MENU*/
#define PAUSED_MAIN_MENU 22        /**< @brief MAIN MENU BUTTON - PAUSE MENU*/
#define PAUSED_EXIT 23             /**< @brief  EXIT BUTTON - PAUSE MENU */
#define WON_MAIN_MENU 31           /**< @brief MAIN MENU BUTTON- WON MENU*/
#define WON_EXIT 32                /**< @brief EXIT BUTTON* - WON MENU*/
#define FAILED_MAIN_MENU 41        /**< @brief MAIN MENU BUTTON - FAILED MENU*/
#define FAILED_EXIT 42             /**< @brief EXIT BUTTON - FAILED MENU*/
#define SWIPE_LEFT 51              /**< @brief SWIPE LEFT BUTTON - CHOOSE PLAYER MENU*/
#define SWIPE_RIGHT 52             /**< @brief SWIPE RIGHT BUTTON - CHOOSE PLAYER MENU*/
#define START 53                   /**< @brief START BUTTON - CHOOSE PLAYER MENU*/
#define INSTRUCTIONS_HOME_BUTTON 6 /**< @brief HOME BUTTON - INSTRUCTIONS MENU*/

/*KEYBOARD*/

#define MAKE_UP_ARROW 0x48    /**< @brief UP ARROW MAKE CODE*/
#define MAKE_LEFT_ARROW 0x4B  /**< @brief LEFT ARROW MAKE CODE*/
#define MAKE_RIGHT_ARROW 0x4D /**< @brief RIGHT ARROW MAKE CODE*/
#define MAKE_UP_W 0x11        /**< @brief W MAKE CODE*/
#define MAKE_LEFT_A 0x1E      /**< @brief A MAKE CODE*/
#define MAKE_RIGHT_B 0x20     /**< @brief B MAKE CODE*/
#define BREAK_ESC 0x81        /**< @brief ESC BREAK CODE*/

/* RTC */
#define RTC_IRQ 8           /**< @brief RTC IRQ LINE */
#define RTC_ADDR_REG 0x70   /**< @brief RTC ADDRESS REGISTER */
#define RTC_DATA_REG 0x71   /**< @brief RTC DATA REGISTER */
#define RTC_SECONDS 0       /**< @brief RTC SECONDS*/
#define RTC_MINUTES 2       /**< @brief RTC MINUTES */
#define RTC_HOURS 4         /**< @brief RTC HOURS */
#define RTC_DAY_OF_MONTH 7  /**< @brief RTC DAY */
#define RTC_MONTH 8         /**< @brief RTC MONTH */
#define RTC_YEAR 9          /**< @brief RTC YEAR */
#define RTC_REG_A 10        /**< @brief RTC REGISTER A */
#define RTC_REG_B 11        /**< @brief RTC REGISTER B */
#define RTC_REG_C 12        /**< @brief RTC REGISTER C */
#define RTC_REG_A_UI BIT(7) /**< @brief RTC REGISTER A PENDING INTERRUPT */
#define RTC_REG_B_PI BIT(6) /**< @brief RTC REGISTER B ENABLE PERIODIC INTERRUPTS */
#define RTC_REG_B_UI BIT(4) /**< @brief RTC REGISTER B ENABLE UPDATE INTERRUPTS */
#define RTC_REG_B_DM BIT(2) /**< @brief RTC BINARY OR BCD DATA */
#define RTC_REG_C_PF BIT(6) /**< @brief RTC REGISTER C PENDING INTERRUPT */

/* OTHER GAME MACROS */

#define WAIT 200             /**< @brief WAIT TIME IN MS */
#define STEP_KB 4            /**< @brief DELTA X SPRITE W/ KEYBOARD INT */
#define STEP_JUMP_X 3        /**< @brief DELTA X JUMPING SPRITE  */
#define STEP_JUMP_Y 11       /**< @brief DELTA Y JUMPING SPRITE  */
#define STEP_TXT 10          /**< @brief DELTA Y TEXT IN INSTRUCTIONS */
#define DELTA_Y 13           /**< @brief DELTA Y INTERVAL FOR CALCULATIONS  */
#define NUM_PRESENTS 6       /**< @brief NUMBER OF PRESENTS  */
#define NUM_PLATFORMS 9      /**< @brief NUMBER OF PLATFORMS  */
#define TIME_LIMIT 240       /**< @brief TIME LIMIT  */
#define SEA_BACK_X0 470      /**< @brief SEA BACK XPOSITION */
#define SEA_MIDDLE_X0 440    /**< @brief SEA MID XPOSITION  */
#define WAVE_WIDTH 77        /**< @brief SEA DELTA X MOVE */
#define HALF_SECOND_TICKS 30 /**< @brief SECONDS TICKS */
#define SECOND_TICKS 60      /**< @brief SECONDS TICKS */
#define IN_POS_X 445         /**< @briefHALF IN THE CHOOSING BOX X POS  */
#define OUT_LEFT_POS_X 80    /**< @briefHALF ON THE LEFT OF THE CHOOSING BOX X POS  */
#define OUT_RIGHT_POS_X 810  /**< @briefHALF ON THE RIGHT OF THE CHOOSING BOX X POS  */
#define IGLO_X 1037          /**<@briefHALF IGLO'S WINNING POS */

#endif
