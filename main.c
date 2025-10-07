/**
 * ESL Lab, IIT Dharwad - Lab 7 Assignment
 * Author(s): 1) Pranav Deshpande (EE25DP004), 2) Sujit Nayak (EE25MR009)
 * Problem:
 *   - Toggle the Red LED (PF1) using SysTick interrupt.
 *   - Toggle the Blue LED (PF2) using a GPIO Port F falling edge interrupt
 *     triggered by the SW2 (PF0) switch.
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"

void GPIOF_Handler(void);
void SysTick_Handler(void);

void gpioInit(void) {
    // 1. Enable clock for Port F
    SYSCTL_RCGCGPIO_R |= 0x20;
    while((SYSCTL_RCGCGPIO_R & 0x20) == 0);

    // 2. Unlock PF0 (SW2)
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R   = 0x1F;   // allow changes on PF0–PF4

    // 3. Set directions: PF1, PF2, PF3 outputs (LEDs); PF0, PF4 inputs (switches)
    GPIO_PORTF_DIR_R  = 0x0E;

    // 4. Enable pull-ups on PF0, PF4
    GPIO_PORTF_PUR_R  = 0x11;

    // 5. Enable digital on PF0–PF4
    GPIO_PORTF_DEN_R  = 0x1F;

    // --------- GPIO Interrupt setup for SW2 (PF0) ---------
    GPIO_PORTF_IS_R  &= ~0x01;   // PF0 edge-sensitive
    GPIO_PORTF_IBE_R &= ~0x01;   // not both edges
    GPIO_PORTF_IEV_R &= ~0x01;   // falling edge
    GPIO_PORTF_ICR_R  =  0x01;   // clear any prior interrupt
    GPIO_PORTF_IM_R  |=  0x01;   // unmask PF0 interrupt

    // NVIC enable for Port F (IRQ 30)
    NVIC_EN0_R |= (1 << 30);
}

void SysTick_Init(uint32_t ticks) {
    NVIC_ST_CTRL_R = 0;                // disable SysTick
    NVIC_ST_RELOAD_R = ticks - 1;      // reload value
    NVIC_ST_CURRENT_R = 0;             // clear current value
    NVIC_ST_CTRL_R = 0x07;             // enable SysTick with INTEN + CLK_SRC + ENABLE
}

void SysTick_Handler(void) {
    GPIO_PORTF_DATA_R ^= 0x02;   // Toggle PF1 (Red LED)
}

void GPIOF_Handler(void) {
    if (GPIO_PORTF_RIS_R & 0x01) {     // SW2 (PF0 pressed)
        GPIO_PORTF_ICR_R = 0x01;       // clear interrupt flag
        GPIO_PORTF_DATA_R ^= 0x04;     // toggle Blue LED (PF2)
    }
}

int main(void) {
    gpioInit();

    // SysTick every 0.5s for 16 MHz clock
    SysTick_Init(8000000);

    while(1) {
        // main loop does nothing — LEDs handled by interrupts
    }
}
