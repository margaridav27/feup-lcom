#ifndef MACROS
#define MACROS

#include <lcom/lcf.h>

/*TIMER 0*/
#define TIMER0_IRQ 0

/*GRAPHICS*/
#define BIOS_INTERRUPT 0x10
#define VBE_ACESS 0x4F
#define GET_MODE_INFO 0x01
#define GET_CTRL_INFO 0x00
#define SET_MODE 0x02
#define DEFAULT_TEXT_MODE 0x03
#define LINEAR_FB 1 << 14
#define INDEXED_COLOR 0x105
#define INDEXED_MASK BIT(8)

/*KEYBOARD*/
#define WAIT_KBC 20000
#define KBC_IRQ 1
#define OBF BIT(0)
#define IBF BIT(1)
#define AUX BIT(5)
#define TIMEOUT BIT(6)
#define PARITY BIT(7)
#define BREAK_ESC 0x81

#endif
