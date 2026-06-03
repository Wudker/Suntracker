#include "INA219.h"
#include "MPPT.h"
#include "Photorezistors.h"
#include "Motors.h"
#include <Pins.h>

extern INA219 ina219;

static int readPhotorezistorAverage(uint16_t pin)
{
  long sum = 0;

  for (int i = 0; i < ADC_SAMPLES; i++)
  {
    sum += analogRead(pin);
    delayMicroseconds(200);
  }

  return sum / ADC_SAMPLES;
}

static int16_t normalizeAngle180(int16_t angle)
{
  while (angle > 180)
    angle -= 360;
  while (angle <= -180)
    angle += 360;
  return angle;
}

static int16_t fastAtan2Deg(int32_t y, int32_t x)
{
  if (x == 0 && y == 0)
  {
    return 0;
  }

  int32_t ax = abs(x);
  int32_t ay = abs(y);

  int16_t angle;

  if (ax >= ay)
  {
    angle = (ay * 45L) / ax;
  }
  else
  {
    angle = 90 - ((ax * 45L) / ay);
  }

  if (x < 0)
  {
    angle = 180 - angle;
  }

  if (y < 0)
  {
    angle = -angle;
  }

  return normalizeAngle180(angle);
}

int16_t Find_general_optimal_position()
{
  int r1 = readPhotorezistorAverage(Photorezistor_1);
  int r2 = readPhotorezistorAverage(Photorezistor_2);
  int r3 = readPhotorezistorAverage(Photorezistor_3);
  int r4 = readPhotorezistorAverage(Photorezistor_4);

  int maxValue = max(max(r1, r2), max(r3, r4));
  int minValue = min(min(r1, r2), min(r3, r4));

  if (maxValue < MIN_LIGHT_LEVEL)
  {
    return 0;
  }

  if ((maxValue - minValue) < VECTOR_DEADZONE)
  {
    return 0;
  }

  int32_t x = r1 - r3;
  int32_t y = r2 - r4;

  if ((abs(x) + abs(y)) < VECTOR_DEADZONE)
  {
    return 0;
  }

  return fastAtan2Deg(y, x);
}

static float ReadPanelVoltageAverage()
{
  float sum = 0.0f;

  for (int i = 0; i < FINE_SAMPLES; i++)
  {
    sum += ina219.getBusVoltage();
    delay(60);
  }

  return sum / FINE_SAMPLES;
}

static bool IsVoltageBetter(float newVoltage, float oldVoltage)
{
  return newVoltage > oldVoltage + MIN_VOLTAGE_IMPROVEMENT;
}

void Find_optimal_position()
{
  float bestVoltage = ReadPanelVoltageAverage();

  // 1. Testujemy krok w stronę dodatnią
  Set_Motor1_Direction(FINE_STEP_ANGLE);
  delay(FINE_SETTLE_TIME);

  float voltagePlus = ReadPanelVoltageAverage();

  // 2. Jeśli napięcie się poprawiło, idziemy dalej w plus
  if (IsVoltageBetter(voltagePlus, bestVoltage))
  {
    bestVoltage = voltagePlus;

    for (int i = 1; i < FINE_MAX_STEPS; i++)
    {
      Set_Motor1_Direction(FINE_STEP_ANGLE);
      delay(FINE_SETTLE_TIME);

      float newVoltage = ReadPanelVoltageAverage();

      if (IsVoltageBetter(newVoltage, bestVoltage))
      {
        bestVoltage = newVoltage;
      }
      else
      {
        // Ostatni krok pogorszył wynik, więc cofamy się o jeden krok
        Set_Motor1_Direction(-FINE_STEP_ANGLE);
        delay(FINE_SETTLE_TIME);
        break;
      }
    }

    return;
  }

  // 3. Skoro plus nie pomógł, wracamy do pozycji startowej
  Set_Motor1_Direction(-FINE_STEP_ANGLE);
  delay(FINE_SETTLE_TIME);

  // 4. Testujemy krok w stronę ujemną
  Set_Motor1_Direction(-FINE_STEP_ANGLE);
  delay(FINE_SETTLE_TIME);

  float voltageMinus = ReadPanelVoltageAverage();

  // 5. Jeśli napięcie się poprawiło, idziemy dalej w minus
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
        bestVoltage = newVoltage;
      }
      else
      {
        // Ostatni krok pogorszył wynik, więc cofamy się o jeden krok
        Set_Motor1_Direction(FINE_STEP_ANGLE);
        delay(FINE_SETTLE_TIME);
        break;
      }
    }

    return;
  }

  // 6. Skoro ani plus, ani minus nie poprawił wyniku,
  // wracamy do pozycji startowej
  Set_Motor1_Direction(FINE_STEP_ANGLE);
  delay(FINE_SETTLE_TIME);
}