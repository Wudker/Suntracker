#pragma once
#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif
    const uint32_t Harvest_time = 60000000UL; // 1 min
    extern volatile uint16_t Timer_counter;


    typedef enum
    {
        START = 0,          // wake up, initialize, and start the system
        Harvest_update = 1, // find the optimal position and update the panel's position
        Harvest = 2,        // keep the panel in the optimal position, and update MPPT values
        FOLD = 3,           // fold the panel
        Sleep = 4           // enter sleep mode
    } state;

    typedef enum
    {
        OFF = 0,
        ON = 1
    } Power_state;



    void PowerButton_ISR();
    void Harvest_Update_interrupt();
    void Handle_Power_Button();

    extern volatile state Initial_State;
    extern volatile Power_state Power;

    extern volatile bool powerButtonFlag;
    extern volatile bool wakeTickFlag;

#ifdef __cplusplus
}
#endif