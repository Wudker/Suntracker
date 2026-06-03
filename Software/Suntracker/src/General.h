#pragma once

void Set_Servo_Power(bool state);
void Set_Motor_Power(bool state);
void Set_Logic_5V_Power(bool state);
void Set_Logic_5V_Mode(bool mode);
void Set_Panel_Power(bool state);
void System_Start();
void System_Sleep();
void System_Fold();
void Harvest_Update();
void Pinout_init();
void Wire_Init();
//void Interrupt_Init();