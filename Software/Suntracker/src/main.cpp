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

STM32RTC &rtc = STM32RTC::getInstance();
extern INA219 ina219;
extern MCP40xx mcp40xx;

void setup() {
  rtc.setClockSource(STM32RTC::LSI_CLOCK);
  LowPower.begin();

  LowPower.attachInterruptWakeup(
      POWER_ON,
      PowerButton_ISR,
      FALLING,
      DEEP_SLEEP_MODE);
  Pinout_init();
  Wire_Init();
  Interrupt_Init();
}

void loop()
{
  switch (Initial_State)
  {

  case START:
    System_Start();
    if (Initial_State == START)
    {
      Initial_State = Harvest;
    }
    break;

  case Harvest_update:
    Harvest_Update();
    if (Initial_State == Harvest_update)
    {
      Initial_State = Harvest;
    }
    break;

  case Harvest:
    MPPT_menager();
    if (Initial_State == Harvest)
    {
      Initial_State = Sleep;
    }
    break;

  case FOLD:
    System_Fold();
    if (Initial_State == FOLD)
    {
      Initial_State = Sleep;
    }
    break;

  case Sleep:
    System_Sleep();
    break;
  }
}