#include "Interrupt.h"
#include "arduino.h"
#include "Pins.h"
#include "interrupt.h"

volatile uint16_t Timer_counter = 0;
volatile state Initial_State = Sleep;
volatile Power_state Power = OFF;
volatile bool powerButtonFlag = false;
volatile bool wakeTickFlag = false;
HardwareTimer timer1(TIM2);
void PowerButton_ISR()
{
    powerButtonFlag = true;
}

void Harvest_Update_interrupt()
{
    wakeTickFlag = true;
}


void Handle_Power_Button()
{
    if (Power == OFF)
    {
        Power = ON;
        Initial_State = START;
        Timer_counter = 0;
    }
    else
    {
        Power = OFF;
        Initial_State = FOLD;
    }
}