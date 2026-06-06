#pragma once
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180
#define MOTOR1_przekladnia ((1.0f / 6.0f) ) //przedładnia zewnetrzna
#define MOTOR1_speed   83       // 100 RPM is for 6V, here we use 5V, so we reduce the speed proportionally
#define MOTOR2_speed   342      // 400 RPM is for 6V, here we use 5V, so we reduce the speed proportionally
#define Servo_przekladnia (17.0f / 30.0f)
#define speed_start 100
#define speed_const 240
#define speed_stop 20

void Set_Servo_angle(float angle);
void Set_Motor1_Direction(float angle);
void Set_Motor2_Direction(float distance);