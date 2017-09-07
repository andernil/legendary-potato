        .syntax undefined
            .include "efm32gg.s"
            .include "ex1.s"


//main
<main>:

////////////////////////////////////////
//Enable CMU for GPIO
// load CMU base address
ldr r2, cmu_base_addr

// load current value of HFPERCLK ENABLE
ldr r3, [r2, #CMU_HFPERCLKEN0]

// set bit for GPIO clk
mov r4, #1
lsl r4, r4, #CMU_HFPERCLKEN0_GPIO
orr r3, r3, r4

// store new value
str r3, [r2, #CMU_HFPERCLKEN0]

///////////////////////////////////////////
//Loaded addresses and values
cmu_base_addr:
            .long CMU_BASE
gpio_pa_base_addr:
            .long GPIO_PA_BASE
gpio_pc_base_addr:
            .long GPIO_PC_BASE
enable_LEDs:
            .long 0x55555555
enable_GPIO_IN:
            .long 0x33333333

///////////////////////////////////////////
//Enable LEDs (pins 8-15)
// set high drive strength for LEDs by writing 0x2 to GPIO_PA_CTRL
mov r5, #2
str gpio_pa_base_addr, [r5, #GPIO_CTRL]

//set pins 8-15 to output by writing 0x55555555 to GPIO_PA_MODEH
ldr r6, enable_LEDs
str gpio_pa_base_addr, [r6, #GPIO_MODEH]

//pins 8-15 can be set high or low by writing to bits 8-15 of GPIO_PA_DOUT


/////////////////////////////////////////////
//Enable inpts for GPIO (pins 0-7)
// Set pins 0-7 to input by writing 0x33333333 to GPIO_PC_MODEL
ldr r2, enable_GPIO_IN
str gpio_pc_base_addr, [r2, #GPIO_MODEL]

// Enable internal pull-up by writing 0xff to GPIO_PC_DOUT (0xff = 255)
mov r3, #255
str gpio_pc_base_addr, [r3, #GPIO_DOUT]

//status of pins 0-7 can be found by reading GPIO_PC_DIN

// BEGIN LOOP
loop:

ldr r4, [gpio_pc_base_addr, #GPIO_DIN]
lsl r4, r4, #8
str r4, [gpio_pa_base_addr, #GPIO_DOUT]

loop







