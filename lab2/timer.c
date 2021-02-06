#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

#define mask_last4bits  0x0F;
#define mask_lastbit  0x1;

int hook_id = TIMER0_IRQ;
int counter=0;


int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  //1st it is required to read the input timer configuration before changing it -> the last 4bits cannot be changed
  unsigned char st;
  if (freq < 19 || freq > TIMER_FREQ){ 
    printf("Frequency out of range!\n");
    return 1;
  }

  if (timer_get_conf(timer ,&st)) return 1; //error reading conf
    
  uint8_t control_word;
  uint8_t aux = st & mask_last4bits;
  control_word = TIMER_LSB_MSB | aux ;
  switch(timer){
    case 0:
      control_word |= TIMER_SEL0;
      break;
    case 1:
      control_word |= TIMER_SEL1;
      break;
    case 2:
      control_word |= TIMER_SEL2;
      break;
    default:
      return 1;
  }
  if (sys_outb(TIMER_CTRL,control_word))return 1;
  uint16_t initial_value = TIMER_FREQ / freq;

  uint8_t lsb, msb; //divide initial_value in lsb and msb in order to write it
  util_get_LSB(initial_value, &lsb);
  util_get_MSB(initial_value, &msb);

  //writting it
  if (sys_outb(TIMER_0 + timer, lsb))
    return 1;
  if (sys_outb(TIMER_0 + timer, msb))
    return 1;

  return 0;
}

/*this function should be used to subscribe a notification on every interrupt in the input irq_line*/
int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id;

  if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id)) 
  {
    /*   the 2nd argument specifies whether or not the interrupt on that IRQ line should be automatically 
    enabled by the generic interrupt handler, or whether the device driver interrupt handler will do it
         the hook_id argument is used both for input to the call and output from the call
         the caller should initialize it with a value that will be used in the interrupt notifications, as
    described below
         the value returned by the call in this argument must be used in the other calls to 
    specify the interrupt notification to operate on */
    printf("Error in sys_irqsetpolicy()\n");
    return 1;
  }
  return 0;
}

/*this function unsubscribes a previous subscription of the interrupt 
notification associated with the specified hook_id*/
int (timer_unsubscribe_int)() {
  
  if (sys_irqrmpolicy(&hook_id))
  {
    printf("Error in sys_irqrmpolicy.\n");
    return 1;
  }
  return 0;
}

 /*all this function needs to do is to increment a global counter variable*/
void (timer_int_handler)() {
  counter++;
}

/*define read-back command, this will inform timer what will be done;
write rbcmd in the timer control;
read timer config*/
int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  __uint8_t rbcommand = 0;
  rbcommand = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer) ; //ReadBackCommand
  
  if (sys_outb(TIMER_CTRL, rbcommand)) //trying to write the rbcmd in the ctrl port
  {
    printf("Error in sys_outb()\n");
    return 1;  //unable to write 
  }
  if (util_sys_inb(TIMER_0 + timer, st))
  {
    printf("Error in util_sys_inb()\n");
    return 1;
  } //read the timer config
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
                          //this function descodifies the control word (passed in the 2nd argument as a 8bits word)
                          //according to which part of the control word I want to know (given by the 3rd argument)
  union timer_status_field_val conf;
  switch (field)  
  {
  case tsf_all:     //if the argument field is 'tsf_all' means that I want the complete control world in hexadecimal
    conf.byte = st;
    break;
  case tsf_initial: //if the argument field is 'tsf_initial' means I want to know the initialization mode
    conf.in_mode = (uint8_t)(st & TIMER_LSB_MSB) >> 4;
    break;
  case tsf_mode:    //counting mode
    conf.count_mode = (uint8_t)(st & (BIT(3) | TIMER_SQR_WAVE)) >> 1;
    if (conf.count_mode == 6 || conf.count_mode == 7)
      conf.count_mode -=4;
    break;
  case tsf_base:    //counting base
    conf.bcd = st & mask_lastbit; //bit 0 is the only relevant 
    break;
  }
  timer_print_config(timer, field, conf);
  return 0;
}
