#include "Motors.h"
#include <Arduino.h>
#include <Servo.h>
#include "Pins.h"

Servo servo;

void Set_Servo_angle(float angle)
{
    angle*=Servo_przekladnia;
    servo.attach(Servo_signal);
    int pwmValue = map(angle, 0, 180, 1000, 2000);
    servo.writeMicroseconds(pwmValue);
}

void Motor1_SetPWM(bool direction, int pwm)
{
  pwm = constrain(pwm, 0, 255);

  if (direction)
  {
    analogWrite(Motor1_in1, pwm);
    digitalWrite(Motor1_in2, LOW);
  }
  else
  {
    digitalWrite(Motor1_in1, LOW);
    analogWrite(Motor1_in2, pwm);
  }
}

void Motor1_Stop()
{
  digitalWrite(Motor1_in1, LOW);
  digitalWrite(Motor1_in2, LOW);
}

void Set_Motor1_Direction(float angle)
{
  if (angle == 0)
  {
    Motor1_Stop();
    return;
  }

  bool direction = angle > 0;
  float angleAbs = fabs(angle);

  float revolutions = angleAbs / 360.0f;

  float avgPWM = (speed_start + speed_const + speed_stop) / 3.0f;
  float pwmFactor = avgPWM / 255.0f;

  float effectiveRPS = (MOTOR1_speed / 60.0f) * MOTOR1_przekladnia * pwmFactor;

  if (effectiveRPS <= 0)
    return;

  float Working_time = revolutions / effectiveRPS;
  unsigned long partTime_ms = (unsigned long)((Working_time / 3.0f) * 1000.0f);

  Motor1_SetPWM(direction, speed_start);
  delay(partTime_ms);

  Motor1_SetPWM(direction, speed_const);
  delay(partTime_ms);

  Motor1_SetPWM(direction, speed_stop);
  delay(partTime_ms);

  Motor1_Stop();
}
void Set_Motor2_Direction(float distance)
{
  if (distance == 0)
  {
    digitalWrite(Motor2_in1, LOW);
    digitalWrite(Motor2_in2, LOW);
    return;
  }

  bool direction = distance > 0;
  float distanceAbs = fabs(distance);

  float linearSpeed = (MOTOR2_speed / 60.0f) * 10.0f; // mm/s
  float Working_time = distanceAbs / linearSpeed;

  digitalWrite(Motor2_in1, direction);
  digitalWrite(Motor2_in2, !direction);

  delay((unsigned long)(Working_time * 1000.0f));

  digitalWrite(Motor2_in1, LOW);
  digitalWrite(Motor2_in2, LOW);
}