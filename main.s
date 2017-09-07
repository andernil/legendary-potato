        .syntax undefined
            .include "efm32gg.s"
            .include "ex1.s"


//main
<main>:

loop

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

cmu_base_addr:
            .long CMU_BASE
gpio_pa_base_addr:
            .long GPIO_PA_BASE
enable_LEDs:
            .long 0x55555555


// set high drive strength for LEDs by writing 0x2 to GPIO_PA_CTRL
mov r5, #2
str gpio_pa_base_addr, [r5, #GPIO_CTRL]

//set pins 8-15 to output by writing 0x55555555 to GPIO_PA_MODEH
ldr r5, enable_LEDs
str gpio_pa_base_addr, [r5, #GPIO_MODEH]

//pins 8-15 can be set high or low by writing to bits 8-15 of GPIO_PA_DOUT



