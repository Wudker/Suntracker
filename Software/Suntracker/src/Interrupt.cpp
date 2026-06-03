#include "Interrupt.h"
#include "arduino.h"
#include "Pins.h"

volatile uint16_t Timer_counter = 0;
volatile state Initial_State = Sleep;
volatile Power_state Power = OFF;
HardwareTimer timer1(TIM2);
void PowerButton_ISR()
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

void Harvest_Update_interrupt()
{

    if (Initial_State == Sleep && Power == ON)
    {
        Initial_State = Harvest;
        Timer_counter++;
        if (Timer_counter >= 15)
        {
            Initial_State = Harvest_update;
            Timer_counter = 0;
        }
    }
}

void Interrupt_Init()
{
    attachInterrupt(digitalPinToInterrupt(POWER_ON), PowerButton_ISR, FALLING);
}