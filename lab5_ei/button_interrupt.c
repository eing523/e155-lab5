// button_interrupt.c
// Josh Brake
// jbrake@hmc.edu
// 10/31/22

#include "main.h"
// Necessary includes for printf to work
#include <stdio.h>
#include "stm32l432xx.h"

// global vars
volatile int direction;
volatile int pulse;
volatile float velocity;

// two stationary digital sensors
int state_a;
int state_b;


// Function used by printf to send characters to the laptop
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}

int main(void) {
    printf("hello\n");
    // Enable LED as output
    gpioEnable(GPIO_PORT_A);
    pinMode(A_PIN, GPIO_INPUT);
    pinMode(B_PIN, GPIO_INPUT);

    GPIOA->PUPDR |= (0b01 << 2*gpioPinOffset(A_PIN)); // Set PA6 as pull-up (PUPD6 = 01)
    GPIOA->PUPDR |= (0b01 << 2*gpioPinOffset(B_PIN)); // Set PA9 as pull-up (PUPD9 = 01)
    // there's like 2 extra line here that hh did but dont think it does anhything? if code no work add it

    // Initialize timer
    //RCC->APB1ENR1 |= (1 << 0); // TIM2EN -- I dont think i have to update this to tim15. if code dont work, change to tim 15 and see.
    RCC->APB2ENR |= (0b01 << 16);
    initTIM(TIMER, 10000); // 10,000 since PSC = 7,999, CLK = 80 MHz

    // 1. Enable SYSCFG clock domain in RCC
    RCC->APB2ENR |= (1 << 0); // SYSCFGEN
    // 2. Configure EXTICR for the input button interrupt
    // EXTI6 is bits [10:8] of EXTICR2 (EXTICR[1] in C). Port A is 0b000, so clearing the field selects PA6.
    SYSCFG->EXTICR[1] &= ~(0b111 << 8);

    // EXTI9 is bits [6:4] of EXTICR3 (EXTICR[2] in C). Port A is 0b000, so clearing the field selects PA9.
    SYSCFG->EXTICR[2] &= ~(0b111 << 4);

    
    // Configure interrupt for falling edge of A GPIO pin for button 6
    EXTI->IMR1 |= (1 << gpioPinOffset(A_PIN));   // 1. Configure mask bit
    EXTI->RTSR1 |= (1 << gpioPinOffset(A_PIN)); // 2. Enable rising edge trigger, because of PWM
    EXTI->FTSR1 |= (1 << gpioPinOffset(A_PIN));  // 3. Enable falling edge trigger

     // Configure interrupt for falling edge of B GPIO pin for button 9
    EXTI->IMR1 |= (1 << gpioPinOffset(B_PIN));   // 1. Configure mask bit
    EXTI->RTSR1 |= (1 << gpioPinOffset(B_PIN)); // 2. Enable rising edge trigger, because of PWM
    EXTI->FTSR1 |= (1 << gpioPinOffset(B_PIN));  // 3. Enable falling edge trigger


    NVIC->ISER[0] |= (1 << 23);                       // 4. Turn on EXTI interrupt in NVIC_ISER (EXTI9_5 is IRQ 23)

     // Enable interrupts globally
    __enable_irq();
    // from PM; NVIC priority interrupts (p.218)
    __NVIC_EnableIRQ(EXTI9_5_IRQn);
    __NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 1);  // give timer priority
    __NVIC_SetPriority(EXTI9_5_IRQn, 2);

    while(1){
        if(TIMER->CNT == 10000){ // checking velocity and direction every 1 second
        // find angular velocity in rps
        velocity = ((float)pulse)/(4*408.0f); // PPR (pulse per rotation) = 408. We have 4 edges per physical pulse, so multiply 408 by 4.
        
        printf("Angular velocity: %f!\n", velocity);
        printf("Direction: %d!\n", direction);

        // update interrupt flag - status register
        TIMER->SR &= ~(1<<0);

        TIMER->CNT = 0; // reset count

        pulse = 0;
        }
    }
}

// EXTI lines 5-9 share this handler
void EXTI9_5_IRQHandler(void){
    // Check that the button was what triggered our interrupt
    if (EXTI->PR1 & (1 << gpioPinOffset(A_PIN))){
        // If so, clear the interrupt (NB: Write 1 to reset.) for A
        EXTI->PR1 |= (1 << gpioPinOffset(A_PIN));

        state_a = digitalRead(A_PIN);
        state_b = digitalRead(B_PIN);
        pulse++;

        if (state_a == 1) {
            if (state_b == 0) {
                direction = CCW;
            } else {
                direction = CW;
            }
        } else {
            if (state_b == 0) {
                direction = CCW;
            } else {
                direction = CW;
            }
        }
    }

    // Check that the button was what triggered our interrupt
    if (EXTI->PR1 & (1 << gpioPinOffset(B_PIN))){
        // If so, clear the interrupt (NB: Write 1 to reset.) for B
        EXTI->PR1 |= (1 << gpioPinOffset(B_PIN));

        state_a = digitalRead(A_PIN);
        state_b = digitalRead(B_PIN);
        pulse++;

        if (state_a == 1) {
            if (state_b == 0) {
                direction = CCW;
            } else {
                direction = CW;
            }
        } else {
            if (state_b == 0) {
                direction = CCW;
            } else {
                direction = CW;
            }
        }
    }


}
