#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/*
 * function to setup the timer 
 */
void
setupTimer (uint16_t period)
{
  /*
   * TODO enable and set up the timer
   * 
   * 1. Enable clock to timer by setting bit 6 in CMU_HFPERCLKEN0 2.
   * Write the period to register TIMER1_TOP 3. Enable timer interrupt
   * generation by writing 1 to TIMER1_IEN 4. Start the timer by writing 
   * 1 to TIMER1_CMD
   * 
   * This will cause a timer interrupt to be generated every (period)
   * cycles. Remember to configure the NVIC as well, otherwise the
   * interrupt handler will not be invoked. 
   */
  *CMU_HFPERCLKEN0 |= 1 << 6;	/* Enable the clock to the timer module */
  *TIMER1_TOP = period;		/*The number of ticks between interrupts. Based on the base clock being 14MHz with 48kHz         sampling rate. f_clk / f_sample = 292, or can be defined by the input period */
  *TIMER1_IEN = 1;		//Enable timer interrupt generation
  *TIMER1_CMD = 1;		//Start timer by writing 1
}
