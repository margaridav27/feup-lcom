#ifndef I8042_H
#define I8042_H

#include <lcom/lcf.h>

#define KBC_ST_REG 0x64  //status register
#define KBC_CMD_REG 0x64 //command register/byte
#define KBC_OUT_BUF 0x60 //output buffer

#define EN_KBD 0xAE  //enable KBD interface
#define DIS_KBD 0xAD //disable KBD interface

/*MOUSE RELATED KBC COMMANDS*/
#define READ_CB 0x20     //read command byte
#define WRITE_CB 0x60    //write command byte
#define EN_MOUSE 0xA8    //enable mouse
#define DIS_MOUSE 0xA7   //disable mouse
#define CHECK_INTER 0xA9 //check mouse interface
#define WRITE_BYTE 0xD4  //write byte to mouse

/*STATUS REGISTER*/
#define PARITY BIT(7)  //parity error
#define TIMEOUT BIT(6) //timeout error
#define AUX BIT(5)     //mouse data
#define A2 BIT(3)      //A2 input line, 0: data byte, 1: command byte
#define SYS BIT(2)     //system flag, 0: system in power-on reset, 1: system already initialized
#define IBF BIT(1)     //input buffer full
#define OBF BIT(0)     //output buffer full

/*KBC COMMAND BYTE*/
#define INT BIT(0)  //enable interrupt on OUT_BUF, from keyboard
#define INT2 BIT(1) //enable interrupt on OUT_BUF, from mouse
#define DIS BIT(4)  //disable keyboard interface
#define DIS2 BIT(5) //disable mouse interface

/*MOUSE COMMANDS*/
#define RESET 0xFF           //mouse reset
#define RESEND 0xFE          //for serial communications errors
#define SET_DEFAULTS 0xF6    //set default values
#define DISABLE 0xF5         //in stream mode, should be sent before any other command
#define ENABLE 0xF4          //in stream mode only
#define SET_SAMPLE_RATE 0xF3 //sets state sampling rate
#define SET_REMOTE_MODE 0xF0 //send data on request only
#define READ_DATA 0xEB       //send data packet request
#define SET_STREAM_MODE 0xFA //send data on events
#define STATUS_REQUEST 0xF9  //get mouse configuration
#define SET_RESOLUTION 0xF8
#define SET_SCALING_21 0xF7 //acceleration mode
#define SET_SCALING_11 0xF6 //linear mode

/*MOUSE CONTROLLER ACKNOWLEDGMENT BYTE*/
#define ACK 0xFA   //everything ok
#define NACK 0xFE  //invalid byte
#define ERROR 0xFC //second consecutive invalid byte
#define REMOTE 0xF0
#define STREAM 0xEA

#define WAIT_KBC 20000
#define KBC_IRQ 1
#define TIMER0_IRQ 0 //timer 0 line
#define MOUSE_IRQ 12 //mouse line

/*COMMAND BYTE 1 BITS MEANING */

#define LB BIT(0)
#define RB BIT(1)
#define MB BIT(2)
#define MSB_X_DELTA BIT(4)
#define MSB_Y_DELTA BIT(5)
#define X_OV BIT(6)
#define Y_OV BIT(7)

#endif
