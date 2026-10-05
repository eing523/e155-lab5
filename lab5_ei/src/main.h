// main.h
// Josh Brake
// jbrake@hmc.edu
// 10/31/22

#ifndef MAIN_H
#define MAIN_H

#include "STM32L432KC.h"
#include <stm32l432xx.h>

///////////////////////////////////////////////////////////////////////////////
// Custom defines
///////////////////////////////////////////////////////////////////////////////

#define LED_PIN PB3
#define BUTTON_PIN PA7
//#define DELAY_TIM TIM2
#define TIMER TIM2

// sensors
#define A_PIN PA6 
#define B_PIN PA9

// values for variables
#define CW 0
#define CCW 1
#define PPR 408

#endif // MAIN_H