#include "General.h"
#include <Arduino.h>
#include "Wire.h"
#include "Pins.h"
#include "MPPT.h"
#include "Motors.h"
#include "interrupt.h"
#include "Photorezistors.h"
#include "STM32LowPower.h"
#include "STM32RTC.h"

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
  Set_Motor2_Direction(40);
  delay(100);
  Set_Servo_angle(120);
  delay(500);
  Set_Panel_Power(true);
  int16_t optimal_position = Find_general_optimal_position();
  Set_Motor1_Direction(optimal_position);
  delay(100);
  Find_optimal_position();
  delay(100);
  MPPT_Init();
}

void Harvest_Update()
{
  Set_Panel_Power(true);
  Set_Logic_5V_Power(true);
  delay(100);
  Set_Logic_5V_Mode(true);
  delay(100);
  Set_Motor_Power(true);
  delay(100);
  Set_Servo_Power(true);
  delay(300);
  int16_t optimal_position = Find_general_optimal_position();
  Set_Motor1_Direction(optimal_position);
  delay(100);
  Find_optimal_position();
  delay(100);
  MPPT_Init();
}

void System_Sleep()
{
  Set_Panel_Power(true);

  Set_Motor_Power(false);
  Set_Servo_Power(false);
  Set_Logic_5V_Power(false);

  digitalWrite(Motor1_in1, LOW);
  digitalWrite(Motor1_in2, LOW);
  digitalWrite(Motor2_in1, LOW);
  digitalWrite(Motor2_in2, LOW);

  delay(50);

  Button_wakeup_flag = false;

  LowPower.deepSleep(60000);

  if (Button_wakeup_flag)
  {
    noInterrupts();
    Button_wakeup_flag = false;
    interrupts();

    Handle_Power_Button();
  }
  else
  {
    Harvest_Update_interrupt();
  }
}

void System_Fold()
{
  Set_Panel_Power(false);
  delay(100);
  Set_Logic_5V_Power(true);
  delay(100);
  Set_Logic_5V_Mode(true);
  delay(100);
  Set_Motor_Power(true);
  delay(100);
  Set_Servo_Power(true);
  delay(300);
  Set_Servo_angle(0);
  delay(100);
  Set_Motor2_Direction(-40);
  System_Sleep();
}

void Pinout_init()
{
  pinMode(POT_data, OUTPUT);
  pinMode(POT_cs, OUTPUT);
  pinMode(MPPT_A0, INPUT_ANALOG);
  pinMode(MPPT_A1, INPUT_ANALOG);
  pinMode(Photorezistor_1, INPUT_ANALOG);
  pinMode(Photorezistor_2, INPUT_ANALOG);
  pinMode(Photorezistor_3, INPUT_ANALOG);
  pinMode(Photorezistor_4, INPUT_ANALOG);
  pinMode(Motor1_in1, OUTPUT);
  pinMode(Motor1_in2, OUTPUT);
  pinMode(Motor2_in1, OUTPUT);
  pinMode(Motor2_in2, OUTPUT);
  pinMode(Servo_signal, OUTPUT);
  pinMode(Motor_driver_enable, OUTPUT);
  pinMode(Logic_5V_enable, OUTPUT);
  pinMode(Servo_power_enable, OUTPUT);
  pinMode(Logic_5V_mode, OUTPUT);
  pinMode(Panel_enable, OUTPUT);
  pinMode(POWER_ON, INPUT);
}

void Wire_Init()
{
  Wire.setSCL(PIN_I2C_SCL);
  Wire.setSDA(PIN_I2C_SDA);
  Wire.begin();
}
