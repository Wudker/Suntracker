// Solar panel tracker firmware for STM32
// Includes all necessary libraries for motor control, power monitoring, and interrupts
#include <Arduino.h>
#include <Wire.h>
#include "Pins.h"
#include "General.h"
#include "Motors.h"
#include "MPPT.h"
#include "Photorezistors.h"
#include "INA219.h"
#include "MCP40xx.h"
#include "Interrupt.h"
#include "STM32LowPower.h"
#include "STM32RTC.h"

// External objects for RTC, power monitoring, and digital potentiometer
extern STM32RTC &rtc;
extern INA219 ina219;
extern MCP40xx mcp40xx;
extern HardwareTimer timer1;

// Initialize all hardware: pins, I2C communication, and interrupt handlers
void setup()
{
  Pinout_init();          // Configure GPIO pins
  Wire_Init();            // Initialize I2C communication
  Interrupts_init();      // Set up interrupt handlers

}

// Main program loop - handles state machine and interrupts
void loop()
{
  // Handle power button press
  if (powerButtonFlag)
  {
    noInterrupts();       // Disable interrupts to safely modify flags
    powerButtonFlag = false;
    interrupts();         // Re-enable interrupts

    // Toggle power state and set appropriate initial state
    if (Power == OFF)
    {
      Power = ON;           // Turn system ON
      Initial_State = START;  // Start initialization sequence
      Timer_counter = 0;
    }
    else
    {
      Power = OFF;          // Turn system OFF
      Initial_State = FOLD;  // Fold panel before sleeping
    }
  }

  // Handle periodic wake-up from sleep (every 60 seconds)
  if (wakeTickFlag)
  {
    noInterrupts();       // Disable interrupts to safely modify flags
    wakeTickFlag = false;
    interrupts();         // Re-enable interrupts

    if (Initial_State == Sleep && Power == ON)
    {
      Timer_counter++;    // Increment wake counter

      // After 15 wake cycles (~15 minutes), re-optimize panel position
      if (Timer_counter >= 15)
      {
        Initial_State = Harvest_update;  // Update optimal panel position
        Timer_counter = 0;
      }
      else
      {
        Initial_State = Harvest;  // Continue normal operation
      }
    }
  }

  // State machine - manages system operation modes
  switch (Initial_State)
  {
  case START:  // Initialize system and find optimal panel position
    System_Start();
    if (Initial_State == START)
      Initial_State = Harvest;  // Move to harvesting mode
    break;

  case Harvest_update:  // Re-optimize panel position periodically
    Harvest_Update();
    if (Initial_State == Harvest_update)
      Initial_State = Harvest;  // Return to normal harvesting
    break;

  case Harvest:  // Normal operation - track sun and optimize power
    MPPT_menager();  // Run maximum power point tracking algorithm
    if (Initial_State == Harvest)
      Initial_State = Sleep;  // Go to sleep after harvesting
    break;

  case FOLD:  // Fold panel to safe position
    System_Fold();
    if (Initial_State == FOLD)
      Initial_State = Sleep;  // Go to sleep after folding
    break;

  case Sleep:  // Low power sleep mode
    System_Sleep();  // Enter deep sleep and wait for wake-up
    break;
  }
}