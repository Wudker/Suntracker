#include <Arduino.h>
#include <Wire.h>
#include "MPPT.h"
#include "Pins.h"
#include "INA219.h"
#include "MCP40xx.h"
#include "Interrupt.h"
#include "Photorezistors.h"

// Power monitor (INA219 on I2C address 0x40)
INA219 ina219(0x40);
// Digital potentiometer for adjusting MPPT
MCP40xx mcp40xx(POT_cs, POT_data);

static float mpptResistance = MPPT_START_RESISTANCE;  // Current MPPT position
static float previousPower = 0.0f;                     // Power from previous measurement
static int mpptDirection = -1;                         // Direction to adjust resistance (-1 or +1)
static unsigned long lastMPPTUpdate = 0;               // Time of last MPPT adjustment
static bool firstRun = true;                           // Flag for first initialization

// Set digital potentiometer to specific resistance value
// Limits to min/max bounds
static void MPPT_SetResistance(float resistance)
{
  resistance = constrain(resistance, MPPT_MIN_RESISTANCE, MPPT_MAX_RESISTANCE);
  mpptResistance = resistance;
  mcp40xx.setValue(mpptResistance);  // Update digital potentiometer
}

// Initialize MPPT system
// Sets up power monitor and digital potentiometer
bool MPPT_Init()
{
  // Check if there's enough sunlight
  if (Critical_Sunlight)
  {
    return false;  // Not enough sunlight, abort
  }

  // Initialize power monitor (INA219)
  ina219.begin();
  ina219.setBusVoltageRange(32);         // 32V range
  ina219.setGain(1);                     // Gain = 1
  ina219.setMaxCurrentShunt(0.5, 0.05);  // Max current 0.5A, shunt 0.05 Ohm

  // Initialize digital potentiometer
  mcp40xx.setup();              // Configure pins
  mcp40xx.begin(5000.0f);       // Total resistance 5000 Ohm

  // Set starting resistance and measure initial power
  MPPT_SetResistance(MPPT_START_RESISTANCE);
  delay(250);  // Wait for stabilization

  // Record initial power for MPPT algorithm
  previousPower = ina219.getPower();
  firstRun = false;

  return true;
}

// Main MPPT algorithm
// Adjust resistance to find maximum power point
// Uses "Perturb and Observe" algorithm
void MPPT_menager()
{
  // Only update every MPPT_UPDATE_TIME milliseconds
  if (millis() - lastMPPTUpdate < MPPT_UPDATE_TIME)
  {
    return;  // Not time yet, skip
  }

  lastMPPTUpdate = millis();

  // Read current voltage and power from panel
  float panelVoltage = ina219.getBusVoltage();
  float power = ina219.getPower();

  // Handle first run
  if (firstRun)
  {
    previousPower = power;  // Record baseline power
    firstRun = false;
    return;
  }

  // Check if voltage is too low
  if (panelVoltage < MPPT_MIN_PANEL_VOLTAGE)
  {
    // Increase resistance (reduce current) when voltage is low
    mpptResistance += MPPT_STEP;
    MPPT_SetResistance(mpptResistance);
    previousPower = power;
    return;
  }

  // MPPT Algorithm: Perturb and Observe
  // If power decreased, change direction
  if (power < previousPower)
  {
    mpptDirection = -mpptDirection;  // Reverse adjustment direction
  }

  // Adjust resistance in current direction
  mpptResistance += mpptDirection * MPPT_STEP;
  MPPT_SetResistance(mpptResistance);

  // Save current power for next iteration
  previousPower = power;
}