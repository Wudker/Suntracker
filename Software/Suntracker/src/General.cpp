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

// Enable/disable servo motor power supply
void Set_Servo_Power(bool state) {
  digitalWrite(Servo_power_enable, state ? HIGH : LOW);
}

// Enable/disable main motor driver power supply
void Set_Motor_Power(bool state) {
  digitalWrite(Motor_driver_enable, state ? HIGH : LOW);
}

// Enable/disable 5V logic power supply
void Set_Logic_5V_Power(bool state) {
  digitalWrite(Logic_5V_enable, state ? HIGH : LOW);
}

// Set 5V logic mode (output voltage selection)
void Set_Logic_5V_Mode(bool mode) {
  digitalWrite(Logic_5V_mode, mode ? HIGH : LOW);
}

// Enable/disable solar panel power output
void Set_Panel_Power(bool state) {
  digitalWrite(Panel_enable, state ? HIGH : LOW);
}

// Initialize system on startup
// 1. Power on all subsystems
// 2. Move panel to initial position
// 3. Find optimal position for sun
// 4. Initialize power tracking (MPPT)
void System_Start()
{
  // Enable 5V logic power supply
  Set_Logic_5V_Power(true);
  delay(100);
  Set_Logic_5V_Mode(true);
  delay(100);
  
  // Enable motor drivers
  Set_Motor_Power(true);
  delay(100);
  Set_Servo_Power(true);
  delay(300);
  
  // Move to initial positions
  Set_Motor2_Direction(40);     ///Move screw actuator
  delay(100);
  Set_Servo_angle(120);         // Tilt panel at 120 degrees
  delay(500);
  
  // Enable panel and find optimal position
  Set_Panel_Power(true);
  int16_t optimal_position = Find_general_optimal_position();  // Get coarse position
  Set_Motor1_Direction(optimal_position);  // Rotate panel to coarse position
  delay(100);
  Find_optimal_position();  // Fine-tune panel position for maximum power

  // Initialize maximum power point tracking
  if (!MPPT_Init())
  {
    Initial_State = FOLD;  // If MPPT fails, fold panel and go to sleep
    return;
  }
}

// Periodically re-optimize panel position to follow sun movement
// Similar to startup but happens every 15 wake cycles (~15 minutes)
void Harvest_Update()
{
  // Power on all subsystems
  Set_Panel_Power(true);
  Set_Logic_5V_Power(true);
  delay(100);
  Set_Logic_5V_Mode(true);
  delay(100);
  Set_Motor_Power(true);
  delay(100);
  Set_Servo_Power(true);
  delay(300);
  
  // Find and move to new optimal position
  int16_t optimal_position = Find_general_optimal_position();  // Get coarse position
  Set_Motor1_Direction(optimal_position);  // Move panel
  delay(100);
  Find_optimal_position();  // Fine-tune position

  // Reinitialize MPPT algorithm
  if (!MPPT_Init())
  {
    Initial_State = FOLD;  // Fold panel if MPPT fails
    return;
  }
}

// Enter low power sleep mode for 60 seconds
// Turn off motors and logic power to save energy
void System_Sleep()
{
  Set_Panel_Power(true);

  // Disable motors and logic to save power
  Set_Motor_Power(false);
  Set_Servo_Power(false);
  Set_Logic_5V_Power(false);

  // Clear motor control pins
  digitalWrite(Motor1_in1, LOW);
  digitalWrite(Motor1_in2, LOW);
  digitalWrite(Motor2_in1, LOW);
  digitalWrite(Motor2_in2, LOW);

  delay(50);

  powerButtonFlag = false;  // Clear any pending button press

  // Enter deep sleep for 60 seconds
  // Will wake up automatically or when power button is pressed
  LowPower.deepSleep(60000);

  if (powerButtonFlag)
  {
    noInterrupts();
    powerButtonFlag = false;
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
