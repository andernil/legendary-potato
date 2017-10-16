#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"

/*
 * TODO calculate the appropriate sample period for the sound wave(s) you 
 * want to generate. The core clock (which the timer clock is derived
 * from) runs at 14 MHz by default. Also remember that the timer counter
 * registers are 16 bits. 
 */
/*
 * The period between sound samples, in clock cycles 
 */
#define SAMPLE_PERIOD  317	//Usually 317

/*
 * Declaration of peripheral setup functions 
 */

void setupTimer (uint32_t period);
void setupDAC ();
void setupNVIC ();
void setupGPIO ();


int main (void)
{
  /*
   * Call the peripheral setup functions 
   */
  setupGPIO ();
  setupDAC ();
  setupTimer (SAMPLE_PERIOD);
  /*
   * Enable interrupt handling 
   */
  setupNVIC();          //Will not run the while loop coming up.
  *GPIO_IFS = (1<<3);	//Play intro and enter sleep-mode after playthrough.
}


void setupNVIC ()
{
 *ISER0 |= 1 << 12;		//Set bit 12 high to enable timer interrupts 
 *ISER0 |= 0x802;		//Enable GPIO interrupts
}
