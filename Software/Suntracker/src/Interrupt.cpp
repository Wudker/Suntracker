#include "Interrupt.h"
#include "arduino.h"
#include "Pins.h"
#include "interrupt.h"
#include <HardwareSerial.h>
#include "STM32LowPower.h"
#include "STM32RTC.h"

volatile uint16_t Timer_counter = 0;
volatile state Initial_State = Sleep;
volatile Power_state Power = OFF;
volatile bool powerButtonFlag = false;
volatile bool wakeTickFlag = false;
volatile bool Button_wakeup_flag = false;
HardwareTimer timer1(TIM2);
STM32RTC &rtc = STM32RTC::getInstance();

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
void Interrupts_init()
{
    rtc.setClockSource(STM32RTC::LSI_CLOCK);
    LowPower.begin();

    LowPower.attachInterruptWakeup(
        POWER_ON,
        PowerButton_ISR,
        FALLING,
        DEEP_SLEEP_MODE);
}
