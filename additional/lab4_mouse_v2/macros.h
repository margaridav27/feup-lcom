#ifndef MACROS
#define MACROS

#include <lcom/lcf.h>

#define MOUSE_IRQ 12
#define ST_REG 0x64  
#define CMD_REG 0x64 
#define OUT_BUF 0x60
#define IN_BUF 0x60
#define EN_KBD 0xAE  
#define DIS_KBD 0xAD
#define WRITE_TO_MOUSE 0xD4
#define READ_CB 0x20 
#define WRITE_CB 0x60
#define DIS_MOUSE 0xA7
#define EN_MOUSE 0xA8
#define DIS_DR 0xF5
#define EN_DR 0xF4
#define STREAM_MODE 0xEA
#define REMOTE_MODE 0xF0
#define READ_DATA 0xEB
#define ACK 0xFA
#define WAIT_KBC 20000
#define OBF BIT(0)     
#define IBF BIT(1)     
#define AUX BIT(5)     
#define TO_ERR BIT(6)  
#define PAR_ERR BIT(7) 
#define INT BIT(0)  
#define INT2 BIT(1) 
#define DIS BIT(4) 
#define DIS2 BIT(5) 

#endif
