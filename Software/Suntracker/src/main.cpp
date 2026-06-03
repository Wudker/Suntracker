#include <Arduino.h>
#include <Wire.h>
#include "Pins.h"
#include "General.h"
#include "Motors.h"
#include "MPPT.h"
#include "Photorezistors.h"
#include "INA219.h"
#include "MCP40xx.h"

 enum state{
    START = 0,          // wake up, initialize, and start the system
    Harvest_update = 1, // find the optimal position and update the panel's position
    Harvest = 2,        // keep the panel in the optimal position, and update MPPT values
    FOLD = 3,           // fold the panel
    Sleep = 4           // enter sleep mode
};
 volatile state Initial_State = START;

 enum Power_state{
    OFF = 0,
    ON = 1};
 volatile Power_state Power = OFF;

 uint32_t Harvest_time = 60000000; // 1 min
 volatile uint16_t Timer_counter = 0;
 extern INA219 ina219;
 extern MCP40xx mcp40xx;
 HardwareTimer timer1(TIM2);

 void PowerButton_ISR()
 {
   if (Initial_State == Sleep)
   {
     Initial_State = START;
     Power = ON;
   }
   else
   {
     Initial_State = FOLD;
     Power = OFF;
   }
 }

void Harvest_Update_interrupt()
{

   if (Initial_State == Sleep && Power == ON)
  {
    Initial_State = Harvest;
    Timer_counter++;
    if(Timer_counter >= 15) 
    {
      Initial_State = Harvest_update;
      Timer_counter = 0;
    }
  }
  
}



void setup() {
  timer1.pause();
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
  Wire.setSCL(PIN_I2C_SCL);
  Wire.setSDA(PIN_I2C_SDA);
  Wire.begin();
  attachInterrupt(digitalPinToInterrupt(POWER_ON), PowerButton_ISR, FALLING);
  uint32_t channel1 = 1;
  timer1.setOverflow(Harvest_time, MICROSEC_FORMAT);                        // 1min
  timer1.attachInterrupt(channel1, Harvest_Update_interrupt);
  timer1.refresh();
  timer1.resume();
  
}




void loop() {
switch(Initial_State) {
  case START:
    System_Start();
    Initial_State = Harvest;
    break;
  case Harvest_update:
    Harvest_Update();    
    Initial_State = Harvest;
    break;
  case Harvest:
    MPPT_menager();
    Initial_State = Sleep;
    break;
  case FOLD:
    System_Fold();
    Initial_State = Sleep;
    break;
  case Sleep:
    System_Sleep();
    // Code for timer interrupt 
    break;
}
}