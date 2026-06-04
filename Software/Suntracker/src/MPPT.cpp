#include <Arduino.h>
#include <Wire.h>
#include "MPPT.h"
#include "Pins.h"
#include "INA219.h"
#include "MCP40xx.h"
#include "Interrupt.h"
#include "Photorezistors.h"

INA219 ina219(0x40);
MCP40xx mcp40xx(POT_cs, POT_data);

static float mpptResistance = MPPT_START_RESISTANCE;
static float previousPower = 0.0f;
static int mpptDirection = -1;
static unsigned long lastMPPTUpdate = 0;
static bool firstRun = true;

static void MPPT_SetResistance(float resistance)
{
  resistance = constrain(resistance, MPPT_MIN_RESISTANCE, MPPT_MAX_RESISTANCE);
  mpptResistance = resistance;
  mcp40xx.setValue(mpptResistance);
}

bool MPPT_Init()
{
  if (Critical_Sunlight)
  {
    return false;
  }

  ina219.begin();
  ina219.setBusVoltageRange(32);
  ina219.setGain(1);
  ina219.setMaxCurrentShunt(0.5, 0.05);

  mcp40xx.setup();
  mcp40xx.begin(5000.0f);

  MPPT_SetResistance(MPPT_START_RESISTANCE);
  delay(250);

  previousPower = ina219.getPower();
  firstRun = false;

  return true;
}

void MPPT_menager()
{
  if (millis() - lastMPPTUpdate < MPPT_UPDATE_TIME)
  {
    return;
  }

  lastMPPTUpdate = millis();

  float panelVoltage = ina219.getBusVoltage();
  float power = ina219.getPower();

  if (firstRun)
  {
    previousPower = power;
    firstRun = false;
    return;
  }

  if (panelVoltage < MPPT_MIN_PANEL_VOLTAGE)
  {
    mpptResistance += MPPT_STEP;
    MPPT_SetResistance(mpptResistance);
    previousPower = power;
    return;
  }

  if (power < previousPower)
  {
    mpptDirection = -mpptDirection;
  }

  mpptResistance += mpptDirection * MPPT_STEP;
  MPPT_SetResistance(mpptResistance);

  previousPower = power;
}