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

extern STM32RTC &rtc;
extern INA219 ina219;
extern MCP40xx mcp40xx;
extern HardwareTimer timer1;

void setup()
{
  Pinout_init();
  Wire_Init();
  Interrupts_init();

}

void loop()
{
  if (powerButtonFlag)
  {
    noInterrupts();
    powerButtonFlag = false;
    interrupts();

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

  if (wakeTickFlag)
  {
    noInterrupts();
    wakeTickFlag = false;
    interrupts();

    if (Initial_State == Sleep && Power == ON)
    {
      Timer_counter++;

      if (Timer_counter >= 15)
      {
        Initial_State = Harvest_update;
        Timer_counter = 0;
      }
      else
      {
        Initial_State = Harvest;
      }
    }
  }

  switch (Initial_State)
  {
  case START:
    System_Start();
    if (Initial_State == START)
      Initial_State = Harvest;
    break;

  case Harvest_update:
    Harvest_Update();
    if (Initial_State == Harvest_update)
      Initial_State = Harvest;
    break;

  case Harvest:
    MPPT_menager();
    if (Initial_State == Harvest)
      Initial_State = Sleep;
    break;

  case FOLD:
    System_Fold();
    if (Initial_State == FOLD)
      Initial_State = Sleep;
    break;

  case Sleep:
    System_Sleep();
    break;
  }
}