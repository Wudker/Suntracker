#pragma once

#define SERVO_MIN_ANGLE 0      // Minimum servo angle (degrees)
#define SERVO_MAX_ANGLE 180    // Maximum servo angle (degrees)

// External gear ratio - converts motor rotation to panel angle
#define MOTOR1_przekladnia ((1.0f / 6.0f))  // Gear reduction ratio (1:6)
// Motor speed at 5V (reduced from 100 RPM at 6V)
#define MOTOR1_speed   83       // Speed in RPM at 5V

// Motor speed at 5V (reduced from 400 RPM at 6V)
#define MOTOR2_speed   342      // Speed in RPM at 5V

#define Servo_przekladnia (17.0f / 30.0f)  // Servo gear reduction ratio

#define speed_start 100   // Initial speed (PWM value for acceleration)
#define speed_const 240   // Constant/cruise speed (PWM value)
#define speed_stop 20     // Final speed (PWM value for deceleration)

// Function declarations
void Set_Servo_angle(float angle);           // Set servo to specific angle
void Set_Motor1_Direction(float angle);      // Rotate panel horizontally
void Set_Motor2_Direction(float distance);   // Tilt panel vertically