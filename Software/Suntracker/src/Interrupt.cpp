#include "Interrupt.h"
#include "arduino.h"
#include "Pins.h"
#include "interrupt.h"
#include <HardwareSerial.h>
#include "STM32LowPower.h"
#include "STM32RTC.h"

volatile uint16_t Timer_counter = 0;        // Count wake-up cycles (0-15 before update)
volatile state Initial_State = Sleep;       // Start in sleep state
volatile Power_state Power = OFF;           // Start powered off
volatile bool powerButtonFlag = false;      // Power button event flag
volatile bool wakeTickFlag = false;         // RTC timer wake-up flag
volatile bool Button_wakeup_flag = false;   // Button wake-up status

HardwareTimer timer1(TIM2);                // Hardware timer for timing operations
STM32RTC &rtc = STM32RTC::getInstance();   // Real-time clock singleton


// Called immediately when power button is pressed (in interrupt context)
// Sets flag for main loop to handle
void PowerButton_ISR()
{
    powerButtonFlag = true;  // Signal main loop to handle button press
}

// Wakes system from sleep to perform periodic tasks
void Harvest_Update_interrupt()
{
    wakeTickFlag = true;  // Signal main loop that wake time occurred
}

// Process power button event (called from main loop)
// Toggles power state and sets appropriate system state
void Handle_Power_Button()
{
    if (Power == OFF)
    {
        // Turn system ON
        Power = ON;           // Set power state
        Initial_State = START;  // Start initialization sequence
        Timer_counter = 0;    // Reset wake-up counter
    }
    else
    {
        // Turn system OFF
        Power = OFF;          // Clear power state
        Initial_State = FOLD;  // Fold panel before powering down
    }
}
// Sets up power button interrupt and periodic wake-up timer
void Interrupts_init()
{
    // Configure RTC to use internal low-speed clock
    rtc.setClockSource(STM32RTC::LSI_CLOCK);
    
    // Initialize low power manager
    LowPower.begin();

    // Attach power button interrupt
    // Wakes from deep sleep when button is pressed (falling edge)
    LowPower.attachInterruptWakeup(
        POWER_ON,              // Pin connected to power button
        PowerButton_ISR,       // Interrupt handler function
        FALLING,               // Trigger on button press (active low)
        DEEP_SLEEP_MODE);      // Wake from deep sleep mode
}
