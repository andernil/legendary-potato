#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"
#include "sounds.h"
#define IDLE 0x7FF

unsigned int introlength = 185960;		//Workaround to use the intro
int go = 0;					//Value used to start playing a song
int sound = 0;					//Value for sound selection
int count = 1;					//Value for counting 
/*
 * TIMER1 interrupt handler 
 */
void selection();
void sleep();
void wake();

void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	/*
	 * TODO feed new samples to the DAC remember to clear the pending
	 * interrupt by writing 1 to TIMER1_IFC 
	*/
	*TIMER1_IFC = 1;							//Clear timer interrupts
	if (go == 1){
		*SCR = 2;							//Enter higher level sleep-mode
		switch(sound){	
			case(0):						
				if(count < shoot[0]){				//Play audio until the counter reaches the amount of samples the audio file has
					*DAC0_CH0DATA = shoot[count];
					*DAC0_CH1DATA = shoot[count];
				}
				else{count = 1; go = 0;}			//After the audio is played, count and go are reset.
				break;
			case(1):
				if(count < coin[0]){
					*DAC0_CH0DATA = coin[count];
					*DAC0_CH1DATA = coin[count];
				}
				else{count = 1; go = 0;}
				break;
			case(2):
				if(count < wololo[0]){
					*DAC0_CH0DATA = wololo[count];
					*DAC0_CH1DATA = wololo[count];
				} 
				else{count = 1; go = 0;}
				break;

			case(3):
				if(count < introlength){
					*DAC0_CH0DATA = pacman_intro[count];
					*DAC0_CH1DATA = pacman_intro[count];
				}
				else{count = 1; go = 0;}
				break; 
			default:
				count = 1;
				go = 0;
				break;
		}
	}
	else{
		sleep();							//Function for entering sleep (set IDLE output, enter deep-slepe)
	}
	count++;								//Increment count every interrupt-cycle
}

/*
 * GPIO even pin interrupt handler 
 */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
	/*
	 * TODO handle button pressed event, remember to clear pending
	 * interrupt 
	 */
	selection();				//Read button inputs and play the corresponding audio
	*GPIO_IFC = 0xFF;			//Clear GPIO interrupts
}

/*
 * GPIO odd pin interrupt handler 
 */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
	/*
	 * TODO handle button pressed event, remember to clear pending
	 * interrupt 
	 */
	selection();
	*GPIO_IFC = 0xFF;			//Clear GPIO interrupts
}
void selection (){
	uint8_t keys = *GPIO_IF;		//Get what key is pressed from the interrupt register
	*GPIO_PA_DOUT = ~(*GPIO_IF << 8);	//Logical shift left 8 bits and invert to match up with output register
	count = 1;				//Reset count to initial value
	go = 1;					//Set go to allow for audio playback
	wake();
	switch(keys)
		{
		case (0b00000001):		//If button 1 or 5
		case (0b00010000):
			sound = 0;
			break;
		case (0b00000010):		//If button 2 or 6
		case (0b00100000):
			sound = 1;
			break;
		case (0b00000100):		//If button 3 or 7
		case (0b01000000):
			sound = 2;
			break;
		case (0b00001000):		//If button 4 or 8
		case (0b10000000):
			sound = 3;	
			break;
		default:
			*DAC0_CH0DATA = IDLE;	//Set DAC output to Vcc/2
			*DAC0_CH1DATA = IDLE;
			go = 0;			//Disable audio playback
			sound = 5;		//Assign a non-valid value to sound so that no sound is pre-selected.
			break;
		}
}
void wake (){
	*CMU_HFPERCLKEN0 |= (1 << 17);	        //Enable DAC clock
  	*DAC0_CH0CTRL = *DAC0_CH1CTRL = 1;	//Enable left and right audio
	*SCR = 2;				//Enter sleep-mode 2
}

void sleep (){
	*DAC0_CH0CTRL = *DAC0_CH1CTRL = 0;	//Diable left and right audio
	*DAC0_CH0DATA = IDLE;			
	*DAC0_CH1DATA = IDLE;	
	*GPIO_PA_DOUT = ~0;			//Turn off lights
  	*CMU_HFPERCLKEN0 &= ~(1 << 17);		//Diable DAC clock channels	
	*SCR = 6;				//Enter deep-sleep
}
