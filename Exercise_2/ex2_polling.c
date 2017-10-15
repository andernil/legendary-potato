#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"
#include "sound1.c"
#include "sound2.c"
#include "sound3.c"
#include "intro.c"

/*
 * TODO calculate the appropriate sample period for the sound wave(s) you 
 * want to generate. The core clock (which the timer clock is derived
 * from) runs at 14 MHz by default. Also remember that the timer counter
 * registers are 16 bits. 
 */
/*
 * The period between sound samples, in clock cycles 
 */
#define SAMPLE_PERIOD  317	//Usually 292
#define IDLE 0x7FF
unsigned int introlength = 185960;
/*
 * Declaration of peripheral setup functions 
 */

void setupTimer (uint32_t period);
void setupDAC ();
void setupNVIC ();
void setupGPIO ();
void selection ();
void playsound ();
/*
 * Your code will start executing here 
 */
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
  //setupNVIC();          //Will not run the while loop coming up.
  /*
   * TODO for higher energy efficiency, sleep while waiting for
   * interrupts instead of infinite loop for busy-waiting 
   */ 
  while(1){
  	selection();
  }
}


void setupNVIC ()
{
  /*
   * TODO use the NVIC ISERx registers to enable handling of
   * interrupt(s) remember two things are necessary for interrupt
   * handling: - the peripheral must generate an interrupt signal - the
   * NVIC must be configured to make the CPU handle the signal You will
   * need TIMER1, GPIO odd and GPIO even interrupt handling for this
   * assignment. 
   */
  /*
   */
  *ISER0 |= 0x802;		//Enable GPIO interrupts
  *ISER0 |= 1 << 12;		//Set bit 12 high to enable timer interrupts
}
void selection (){
uint8_t keys = ~*GPIO_PC_DIN;
*GPIO_PA_DOUT = (*GPIO_PC_DIN << 8);
switch(keys)
	{
	case (0b00010000):
		playsound(0);
		break;
	case (0b00100000):
		playsound(1);
		break;
	case (0b01000000):
		playsound(2);
		break;
	case (0b10000000):
		playsound(3);	
		break;
	default:
		*DAC0_CH0DATA = IDLE;
		*DAC0_CH1DATA = IDLE;
		break;
	}
}
void playsound(int sound){
int time = *TIMER1_TOP*0.9;			//Comparing with *TIMER1_TOP causes slowdown and instability.
	switch(sound){
		case(0):
			for (int count = 1; count <= shoot[0];){
				if(*TIMER1_CNT >= time){		
					*DAC0_CH0DATA = shoot[count];
					*DAC0_CH1DATA = shoot[count];
					count++;
				}
			}
			break;
		case(1):
			for (int count = 1; count <= coin[0];){
				if(*TIMER1_CNT >= time){
					*DAC0_CH0DATA = coin[count];
					*DAC0_CH1DATA = coin[count];
					count++;
				}
			}
			break;
		case(2):
			for (int count = 1; count <= pacman_eat[0];){
				if(*TIMER1_CNT >= time){
					*DAC0_CH0DATA = pacman_eat[count];
					*DAC0_CH1DATA = pacman_eat[count];
					count++;
				}
			} 
			break;

		case(3):
			for (int count = 1; count <= introlength;){
				if(*TIMER1_CNT >= time){
					*DAC0_CH0DATA = pacman_intro[count];
					*DAC0_CH1DATA = pacman_intro[count];
				count++;
				}
			}
			break; 
		default:
			*DAC0_CH0DATA = IDLE;
			*DAC0_CH1DATA = IDLE;
			break;
	}
}
/*
 * if other interrupt handlers are needed, use the following names:
 * NMI_Handler HardFault_Handler MemManage_Handler BusFault_Handler
 * UsageFault_Handler Reserved7_Handler Reserved8_Handler
 * Reserved9_Handler Reserved10_Handler SVC_Handler DebugMon_Handler
 * Reserved13_Handler PendSV_Handler SysTick_Handler DMA_IRQHandler
 * GPIO_EVEN_IRQHandler TIMER0_IRQHandler USART0_RX_IRQHandler
 * USART0_TX_IRQHandler USB_IRQHandler ACMP0_IRQHandler ADC0_IRQHandler
 * DAC0_IRQHandler I2C0_IRQHandler I2C1_IRQHandler GPIO_ODD_IRQHandler
 * TIMER1_IRQHandler TIMER2_IRQHandler TIMER3_IRQHandler
 * USART1_RX_IRQHandler USART1_TX_IRQHandler LESENSE_IRQHandler
 * USART2_RX_IRQHandler USART2_TX_IRQHandler UART0_RX_IRQHandler
 * UART0_TX_IRQHandler UART1_RX_IRQHandler UART1_TX_IRQHandler
 * LEUART0_IRQHandler LEUART1_IRQHandler LETIMER0_IRQHandler
 * PCNT0_IRQHandler PCNT1_IRQHandler PCNT2_IRQHandler RTC_IRQHandler
 * BURTC_IRQHandler CMU_IRQHandler VCMP_IRQHandler LCD_IRQHandler
 * MSC_IRQHandler AES_IRQHandler EBI_IRQHandler EMU_IRQHandler 
 */
