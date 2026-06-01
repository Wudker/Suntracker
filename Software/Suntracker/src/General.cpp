#include "General.h"
#include <Arduino.h>
#include "Pins.h"
#include "MPPT.h"
#include "Motors.h"
#include "Photorezistors.h"



void Set_Servo_Power(bool state) {
  digitalWrite(Servo_power_enable, state ? HIGH : LOW);
}

void Set_Motor_Power(bool state) {
  digitalWrite(Motor_driver_enable, state ? HIGH : LOW);
}

void Set_Logic_5V_Power(bool state) {
  digitalWrite(Logic_5V_enable, state ? HIGH : LOW);
}

void Set_Logic_5V_Mode(bool mode) {
  digitalWrite(Logic_5V_mode, mode ? HIGH : LOW);
}

void Set_Panel_Power(bool state) {
  digitalWrite(Panel_enable, state ? HIGH : LOW);
}

void System_Start()
{
  Set_Logic_5V_Power(true);
  delay(100);
  Set_Logic_5V_Mode(true);
  delay(100);
  Set_Motor_Power(true);
  delay(100);
  Set_Servo_Power(true);
  delay(300);
  Set_Motor2_Direction(true, 40);
  delay(100);
  Set_Servo_angle(120);
  delay(500);
  Set_Panel_Power(true);
  uint16_t optimal_position = Find_general_optimal_position();
  Set_Motor1_Direction(true, optimal_position);  //add direction decision
  delay(100);
  Find_optimal_position();
  delay(100);
  MPPT_Init();
}

void Harvest_Update()
{
  Set_Panel_Power(true);
  uint16_t optimal_position = Find_general_optimal_position();
  Set_Motor1_Direction(true, optimal_position); // add direction decision
  delay(100);
  Find_optimal_position();
  delay(100);
  MPPT_Init();
}

void System_Sleep()
{
  Set_Motor_Power(false);
  Set_Servo_Power(false);
  Set_Logic_5V_Power(false);
  //add sleep

}

void System_Fold()
{
  Set_Panel_Power(false);
  delay(100);
  Set_Servo_angle(0);
  delay(100);
  Set_Motor2_Direction(false, 40);
  System_Sleep();
}

