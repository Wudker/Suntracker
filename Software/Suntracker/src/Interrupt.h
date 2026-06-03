#pragma once
#include "Arduino.h"

const uint32_t Harvest_time = 60000000UL; // 1 min

enum state
{
    START = 0,          // wake up, initialize, and start the system
    Harvest_update = 1, // find the optimal position and update the panel's position
    Harvest = 2,        // keep the panel in the optimal position, and update MPPT values
    FOLD = 3,           // fold the panel
    Sleep = 4           // enter sleep mode
};

enum Power_state
{
    OFF = 0,
    ON = 1
};

extern volatile uint16_t Timer_counter;
extern volatile state Initial_State;
extern volatile Power_state Power;

void Interrupts_Init();
void PowerButton_ISR();
void Harvest_Update_interrupt();