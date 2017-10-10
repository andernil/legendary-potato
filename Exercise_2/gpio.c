#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/*
 * function to set up GPIO mode and interrupts
 */
void setupGPIO()
{
	/*
	 * TODO set input and output pins for the joystick 
	 */
    	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO;	/* enable GPIO clock */
    	*GPIO_PC_MODEL = 0x33333333;    /*Enable inputs for GPIO pins 0-7*/
	*GPIO_PC_DOUT = 255;   /*Enable internal pullups for the input pins*/

	*GPIO_PA_MODEH = 0x55555555;	/* set pins A8-15 as output */
	*GPIO_PA_CTRL = 2;    /* set high drive strength */

   	 /*
    	Enable interrupts for GPIO
   	 */
    	*GPIO_EXTIPSELL = 0x22222222;   /* Enable interrupts on all input pins */
    	*GPIO_EXTIFALL = *GPIO_EXTIRISE = *GPIO_IEN = 255; /* Write 255 to enable interrupts, and enable interrupts 		on positive and negative slope */
	//*ISER0 = 0x802;
	/*
	 * Example of HW access from C code: turn on joystick LEDs D4-D8 check 
	 * efm32gg.h for other useful register definitions 
	 */
	//*GPIO_PA_DOUT = 0xFFFFFFFF; /* turn on LEDs D4-D8 (1792) (LEDs are active * low) */
}

