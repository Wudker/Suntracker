#include <Arduino.h>
#include <Wire.h>
#include "Pins.h"
#include "General.h"
#include "Motors.h"
#include "MPPT.h"
#include "Photorezistors.h"
#include "INA219.h"
#include "MCP40xx.h"

enum state {
    START = 0,          //wake up, initialize, and start the system
    Harvest_update = 1, //find the optimal position and update the panel's position
    Harvest = 2,        //keep the panel in the optimal position, and update MPPT values 
    FOLD = 3,           //fold the panel 
    Sleep = 4           //enter sleep mode
};
state Initial_State = START;

extern INA219 ina219;
extern MCP40xx mcp40xx;

void setup() {
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

}

void loop() {
if(Initial_State == START) {
    System_Start();
    Initial_State = Harvest;
  }
  else if(Initial_State == Harvest_update) {
    Harvest_Update();    
    Initial_State = Harvest;
  }
  else if(Initial_State == Harvest) {
        MPPT_menager();
        Initial_State = Sleep;
  }
  else if(Initial_State == FOLD) {
    System_Fold();
    Initial_State = Sleep;
  }
  else if(Initial_State == Sleep) {
    System_Sleep();
  }
}
//if wakeup form timer -> updateharvest
//if wakeup from button -> fold
//TODP: replace if to switch case, and adjust state functions