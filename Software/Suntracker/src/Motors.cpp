#include "Motors.h"
#include <Arduino.h>
#include <Servo.h>
#include "Pins.h"
#include <math.h>

Servo servo;  // Servo object for tilt control

// Set servo to specific angle
// Adjusts angle by gear ratio and converts to PWM microseconds
void Set_Servo_angle(float angle)
{
    angle *= Servo_przekladnia;  // Apply gear reduction
    servo.attach(Servo_signal);  // Attach servo to PWM pin
    int pwmValue = map(angle, 0, 180, 1000, 2000);  // Convert to microseconds
    servo.writeMicroseconds(pwmValue);  // Send PWM signal
}

// Set motor PWM value and direction
// direction: true = forward, false = backward
// pwm: 0-255 PWM value
void Motor1_SetPWM(bool direction, int pwm)
{
  pwm = constrain(pwm, 0, 255);  // Limit PWM to valid range

  if (direction)  // Forward direction
  {
    analogWrite(Motor1_in1, pwm);  // Apply PWM to IN1
    digitalWrite(Motor1_in2, LOW);  // Disable IN2
  }
  else  // Backward direction
  {
    digitalWrite(Motor1_in1, LOW);  // Disable IN1
    analogWrite(Motor1_in2, pwm);  // Apply PWM to IN2
  }
}

// Stop motor completely (both pins LOW)
void Motor1_Stop()
{
  digitalWrite(Motor1_in1, LOW);
  digitalWrite(Motor1_in2, LOW);
}

// Rotate motor to specific angle
// Uses 3-phase speed profile: start, constant, stop (for "smooth" acceleration/deceleration)
void Set_Motor1_Direction(float angle)
{
  // No movement needed
  if (angle == 0)
  {
    Motor1_Stop();
    return;
  }

  // Determine direction and magnitude
  bool direction = angle > 0;  // True = clockwise, False = counter-clockwise
  float angleAbs = fabs(angle);

  // Calculate required motor revolutions
  float revolutions = angleAbs / 360.0f;

  // Calculate effective motor speed
  float avgPWM = (speed_start + speed_const + speed_stop) / 3.0f;
  float pwmFactor = avgPWM / 255.0f;
  float effectiveRPS = (MOTOR1_speed / 60.0f) * MOTOR1_przekladnia * pwmFactor;  // Revolutions per second

  if (effectiveRPS <= 0)
    return;

  // Calculate total working time and time for each phase
  float Working_time = revolutions / effectiveRPS;
  unsigned long partTime_ms = (unsigned long)((Working_time / 3.0f) * 1000.0f);  // 1/3 for each phase

  // Phase 1: Acceleration (low speed)
  Motor1_SetPWM(direction, speed_start);
  delay(partTime_ms);

  // Phase 2: Constant speed (cruise)
  Motor1_SetPWM(direction, speed_const);
  delay(partTime_ms);

  // Phase 3: Deceleration (low speed)
  Motor1_SetPWM(direction, speed_stop);
  delay(partTime_ms);

  // Stop motor
  Motor1_Stop();
}
// Move motor 2 (vertical tilt) for specified distance
// Calculates time based on motor speed and distance
void Set_Motor2_Direction(float distance)
{
  // No movement needed
  if (distance == 0)
  {
    digitalWrite(Motor2_in1, LOW);
    digitalWrite(Motor2_in2, LOW);
    return;
  }

  // Determine direction and magnitude
  bool direction = distance > 0;  // True = up, False = down
  float distanceAbs = fabs(distance);

  // Calculate linear speed (mm/s)
  float linearSpeed = (MOTOR2_speed / 60.0f) * 10.0f;  // Convert RPM to mm/s
  float Working_time = distanceAbs / linearSpeed;  // Time needed (seconds)

  // Set direction and start motor
  digitalWrite(Motor2_in1, direction);   // Set direction IN1
  digitalWrite(Motor2_in2, !direction);  // Set opposite direction IN2

  // Wait for movement to complete
  delay((unsigned long)(Working_time * 1000.0f));

  // Stop motor
  digitalWrite(Motor2_in1, LOW);
  digitalWrite(Motor2_in2, LOW);
}