#ifndef I8042_H
#define I8042_H

#include <lcom/lcf.h>

#define KBC_ST_REG 0x64  //KBC status register
#define KBC_CMD_REG 0x64 //KBC command register/byte
#define KBC_OUT_BUF 0x60
#define EN_KBD 0xAE  //enable KBD interface
#define DIS_KBD 0xAD //disable KBD interface
#define READ_CB 0x20 //read command byte
#define WRITE_CB 0x60

#define WAIT_KBC 20000
#define BREAK_ESC 0x81 //should terminate when it reads the breakcode of the ESC key
#define KBC_IRQ 1

//information concerning the status register

#define KBC_OBF BIT(0)     //output buffer full
#define KBC_IBF BIT(1)     //input buffer full
#define AUX BIT(5)         //mouse data
#define KBC_TO_ERR BIT(6)  //timeout error
#define KBC_PAR_ERR BIT(7) //parity error

//information concerning the KBC command byte

#define INT BIT(0)  //enable interrupt on OUT_BUF, from keyboard
#define INT2 BIT(1) //enable interrupt on OUT_BUF, from mouse
#define DIS BIT(4)  //disable keyboard interface
#define DIS2 BIT(5) //disable mouse interface

//TIMER info

#define TIMER0_IRQ 0 /**< @brief Timer 0 IRQ line */

#endif
