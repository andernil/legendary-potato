        .syntax unified
	
	      .include "efm32gg.s"

	/////////////////////////////////////////////////////////////////////////////
	//
  // Exception vector table
  // This table contains addresses for all exception handlers
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .section .vectors
	
	      .long   stack_top               /* Top of Stack                 */
	      .long   _reset                  /* Reset Handler                */
	      .long   dummy_handler           /* NMI Handler                  */
	      .long   dummy_handler           /* Hard Fault Handler           */
	      .long   dummy_handler           /* MPU Fault Handler            */
	      .long   dummy_handler           /* Bus Fault Handler            */
	      .long   dummy_handler           /* Usage Fault Handler          */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* SVCall Handler               */
	      .long   dummy_handler           /* Debug Monitor Handler        */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* PendSV Handler               */
	      .long   dummy_handler           /* SysTick Handler              */

	      /* External Interrupts */
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO even handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO odd handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler

	      .section .text

	/////////////////////////////////////////////////////////////////////////////
	//
	// Reset handler
  // The CPU will start executing here after a reset
	//
	/////////////////////////////////////////////////////////////////////////////
	      .globl  _reset
	      .type   _reset, %function
        .thumb_func
_reset: 
	////////////////////////////////////////
	//Enable CMU for GPIO
	// load CMU base address
	ldr r2, =CMU_BASE
	
	// load current value of HFPERCLK ENABLE
	ldr r3, [r2,#CMU_HFPERCLKEN0]
	
	// set bit for GPIO clk
	mov r4, #1
	lsl r4, r4, #CMU_HFPERCLKEN0_GPIO
	orr r3, r3, r4

	// store new value
	str r3, [r2, #CMU_HFPERCLKEN0]
	
	///////////////////////////////////////////
	//Loaded addresses and values

	//Enable LEDs (pins 8-15)
	// set high drive strength for LEDs by writing 0x2 to GPIO_PA_CTRL
	ldr r0, =GPIO_PA_BASE   //DON'T USE THIS REGISTER AGAIN
	mov r3, #2
	str r3, [r0, #GPIO_CTRL]
	
	//set pins 8-15 to output by writing 0x55555555 to GPIO_PA_MODEH
	ldr r3, =0x55555555
	str r3, [r0, #GPIO_MODEH]
	
	//pins 8-15 can be set high or low by writing to bits 8-15 of GPIO_PA_DOUT
	
	
	/////////////////////////////////////////////
	//Enable inputs for GPIO (pins 0-7)
	// Set pins 0-7 to input by writing 0x33333333 to GPIO_PC_MODEL
	ldr r3, =0x33333333
	ldr r1, =GPIO_PC_BASE   //DON'T USE THIS REGISTER AGAIN
	str r3, [r1, #GPIO_MODEL]
	
	// Enable internal pull-up by writing 0xff to GPIO_PC_DOUT (0xff = 255)
	mov r3, #255
	str r3, [r1, #GPIO_DOUT]
	
	//status of pins 0-7 can be found by reading GPIO_PC_DIN
	
	b label1
	
	// BEGIN LOOP
	label1:

	ldr r4, [r1, #GPIO_DIN]
	lsl r4, r4, #8
	str r4, [r0, #GPIO_DOUT]
	
	b label1
	
	





	
	/////////////////////////////////////////////////////////////////////////////
	//
  // GPIO handler
  // The CPU will jump here when there is a GPIO interrupt
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
gpio_handler:  

	      b .  // do nothing
	
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
dummy_handler:  
        b .  // do nothing

