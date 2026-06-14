// Sun tracking algorithm using 4 light sensors
// Provides coarse position from 4-quadrant photorezistor array
// Provides fine-tuning by measuring panel voltage

#include "INA219.h"
#include "MPPT.h"
#include "Photorezistors.h"
#include "Motors.h"
#include <Pins.h>

extern INA219 ina219;                  // Power monitor
volatile bool Critical_Sunlight = false;  // Flag for insufficient sunlight

// Read average ADC value from photorezistor (multiple samples)
// Filters noise by averaging multiple reads
static int readPhotorezistorAverage(uint16_t pin)
{
  long sum = 0;

  for (int i = 0; i < ADC_SAMPLES; i++)
  {
    sum += analogRead(pin);  // Read sensor
    delayMicroseconds(200);   // Small delay between samples
  }

  return sum / ADC_SAMPLES;  // Return average
}

// Normalize angle to -180 to +180 degrees range
// Ensures consistent angle representation
static int16_t normalizeAngle180(int16_t angle)
{
  while (angle > 180)
    angle -= 360;
  while (angle <= -180)
    angle += 360;
  return angle;
}

// Fast approximation of atan2 in degrees
// Used to calculate sun direction from sensor values
// Avoids slow trigonometry functions
static int16_t fastAtan2Deg(int32_t y, int32_t x)
{
  // No signal case
  if (x == 0 && y == 0)
  {
    return 0;
  }

  // Work with absolute values
  int32_t ax = abs(x);
  int32_t ay = abs(y);

  int16_t angle;

  // Approximate angle based on quadrant
  if (ax >= ay)
  {
    angle = (ay * 45L) / ax;  // Interpolate between 0 and 45 degrees
  }
  else
  {
    angle = 90 - ((ax * 45L) / ay);  // Interpolate between 45 and 90 degrees
  }

  // Adjust for quadrant
  if (x < 0)
  {
    angle = 180 - angle;  // 2nd or 3rd quadrant
  }

  if (y < 0)
  {
    angle = -angle;  // 3rd or 4th quadrant
  }

  return normalizeAngle180(angle);  // Normalize to -180..+180
}

// Find coarse optimal panel position using 4 light sensors
// Uses vector calculation from sensor differences to determine sun direction
// Returns angle in degrees (-180 to +180)
int16_t Find_general_optimal_position()
{
  // Read all 4 photorezistors and average each
  int r1 = readPhotorezistorAverage(Photorezistor_1);  // Top-left
  int r2 = readPhotorezistorAverage(Photorezistor_2);  // Top-right
  int r3 = readPhotorezistorAverage(Photorezistor_3);  // Bottom-left
  int r4 = readPhotorezistorAverage(Photorezistor_4);  // Bottom-right

  // Find min and max sensor values
  int maxValue = max(max(r1, r2), max(r3, r4));
  int minValue = min(min(r1, r2), min(r3, r4));

  // Check if light level is too low
  if (maxValue < MIN_LIGHT_LEVEL)
  {
    return 0;  // Not enough sunlight
  }
  else
  {
    Critical_Sunlight = false;  // Clear insufficient light flag
  }

  // Check if there's enough difference between sensors
  if ((maxValue - minValue) < VECTOR_DEADZONE)
  {
    return 0;  // Sun is too centered, no need to move
  }

  // Calculate X and Y components of light vector
  int32_t x = r1 - r3;  // Left-Right difference (top + bottom)
  int32_t y = r2 - r4;  // Up-Down difference (right + left)

  // Check if vector magnitude is significant
  if ((abs(x) + abs(y)) < VECTOR_DEADZONE)
  {
    return 0;  // Vector too small
  }

  // Calculate angle from X,Y vector
  return fastAtan2Deg(y, x);
}

// Read panel voltage multiple times and return average
// Used during fine-tuning to detect maximum power point
static float ReadPanelVoltageAverage()
{
  float sum = 0.0f;

  for (int i = 0; i < FINE_SAMPLES; i++)
  {
    sum += ina219.getBusVoltage();  // Read panel voltage
    delay(60);  // Allow voltage to settle
  }

  return sum / FINE_SAMPLES;  // Return average
}

// Check if new voltage is significantly better than old voltage
// Avoids small oscillations at maximum power point
static bool IsVoltageBetter(float newVoltage, float oldVoltage)
{
  return newVoltage > oldVoltage + MIN_VOLTAGE_IMPROVEMENT;
}

// Fine-tune panel position for maximum power (voltage)
// Uses gradient descent: try small movements and continue if voltage improves
void Find_optimal_position()
{
  Critical_Sunlight = false;  // Clear the flag at start

  // Read baseline voltage
  float bestVoltage = ReadPanelVoltageAverage();

  // Check minimum voltage requirement
  if (bestVoltage < MIN_PANEL_START_VOLTAGE)
  {
    Critical_Sunlight = true;  // Mark as insufficient sunlight
    return;
  }

  // Step 1: Try positive direction (clockwise)
  Set_Motor1_Direction(FINE_STEP_ANGLE);  // Move small angle clockwise
  delay(FINE_SETTLE_TIME);  // Wait for panel to settle

  float voltagePlus = ReadPanelVoltageAverage();

  // Step 2: If voltage improved, continue in positive direction
  if (IsVoltageBetter(voltagePlus, bestVoltage))
  {
    bestVoltage = voltagePlus;  // Update best voltage

    // Continue moving in positive direction up to FINE_MAX_STEPS times
    for (int i = 1; i < FINE_MAX_STEPS; i++)
    {
      Set_Motor1_Direction(FINE_STEP_ANGLE);  // Move another small angle
      delay(FINE_SETTLE_TIME);

      float newVoltage = ReadPanelVoltageAverage();

      // Stop if voltage stops improving
      if (IsVoltageBetter(newVoltage, bestVoltage))
      {
        bestVoltage = newVoltage;  // Continue in this direction
      }
      else
      {
        // Voltage decreased, step back one position
        Set_Motor1_Direction(-FINE_STEP_ANGLE);
        delay(FINE_SETTLE_TIME);
        break;  // Stop searching
      }
    }

    return;  // Found optimal position
  }

  // Step 3: Clockwise didn't improve, return to start position
  Set_Motor1_Direction(-FINE_STEP_ANGLE);
  delay(FINE_SETTLE_TIME);

  // Step 4: Try negative direction (counter-clockwise)
  Set_Motor1_Direction(-FINE_STEP_ANGLE);
  delay(FINE_SETTLE_TIME);

  float voltageMinus = ReadPanelVoltageAverage();

  // Step 5: If voltage improved, continue in negative direction
  if (IsVoltageBetter(voltageMinus, bestVoltage))
  {
    bestVoltage = voltageMinus;

    for (int i = 1; i < FINE_MAX_STEPS; i++)
    {
      Set_Motor1_Direction(-FINE_STEP_ANGLE);
      delay(FINE_SETTLE_TIME);

      float newVoltage = ReadPanelVoltageAverage();

      if (IsVoltageBetter(newVoltage, bestVoltage))
      {
        bestVoltage = newVoltage;  // Continue in this direction
      }
      else
      {
        // Voltage decreased, step back one position
        Set_Motor1_Direction(FINE_STEP_ANGLE);
        delay(FINE_SETTLE_TIME);
        break;  // Stop searching
      }
    }

    return;  // Found optimal position
  }

  // Step 6: Neither direction improved voltage, return to start position
  Set_Motor1_Direction(FINE_STEP_ANGLE);
  delay(FINE_SETTLE_TIME);
}