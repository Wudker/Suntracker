#pragma once
#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

// ===== TIMING CONSTANTS =====
const uint32_t Harvest_time = 60000000UL;  // Harvest duration before sleep: 1 minute

// ===== GLOBAL STATE VARIABLES =====
extern volatile uint16_t Timer_counter;    // Counter for wake-up cycles (0-15)

// ===== SYSTEM STATES =====
// Enumeration of all possible system states
typedef enum
{
    START = 0,          // Initialize: power on, find optimal position, start MPPT
    Harvest_update = 1, // Periodic update: re-find optimal position every ~15 minutes
    Harvest = 2,        // Normal operation: track sun, optimize power with MPPT
    FOLD = 3,           // Safety mode: fold panel to protected position
    Sleep = 4           // Low power mode: sleep for 60 seconds, wake periodically
} state;

// ===== POWER STATES =====
typedef enum
{
    OFF = 0,  // System powered off
    ON = 1    // System powered on
} Power_state;



// ===== INTERRUPT SERVICE ROUTINES =====
void PowerButton_ISR();                 // Called when power button is pressed
void Harvest_Update_interrupt();        // Called by RTC timer every 60 seconds
void Handle_Power_Button();             // Processes power button event

// ===== GLOBAL STATE VARIABLES =====
extern volatile state Initial_State;    // Current system state
extern volatile Power_state Power;      // Power ON/OFF state

extern volatile bool powerButtonFlag;   // Set to true when power button pressed
extern volatile bool wakeTickFlag;      // Set to true on RTC timer tick (every 60 sec)
extern volatile bool Button_wakeup_flag;// Flag for button wake-up

// ===== INITIALIZATION =====
void Interrupts_init();                 // Initialize interrupt handlers and RTC timer

#ifdef __cplusplus
    class STM32RTC;
    extern STM32RTC &rtc;
#endif

#ifdef __cplusplus
}
#endif