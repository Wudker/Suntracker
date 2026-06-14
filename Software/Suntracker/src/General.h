#pragma once

// ===== POWER SUPPLY CONTROL FUNCTIONS =====
void Set_Servo_Power(bool state);         // Enable/disable servo motor power
void Set_Motor_Power(bool state);         // Enable/disable main motor driver
void Set_Logic_5V_Power(bool state);      // Enable/disable 5V logic supply
void Set_Logic_5V_Mode(bool mode);        // Set 5V logic output mode
void Set_Panel_Power(bool state);         // Enable/disable solar panel output

// ===== SYSTEM STATE MANAGEMENT =====
void System_Start();                      // Initialize system at startup
void System_Sleep();                      // Enter low-power sleep mode
void System_Fold();                       // Fold panel to safe position
void Harvest_Update();                    // Periodically update optimal panel position

// ===== HARDWARE INITIALIZATION =====
void Pinout_init();                       // Configure all GPIO pins
void Wire_Init();                         // Initialize I2C communication
//void Interrupt_Init();                  // (Commented out - not used)