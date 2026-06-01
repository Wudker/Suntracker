#pragma once

#define MPPT_STEP 100.0f
#define MPPT_START_RESISTANCE 2500.0f

#define MPPT_MIN_RESISTANCE 0.0f
#define MPPT_MAX_RESISTANCE 5000.0f

#define MPPT_UPDATE_TIME 1000UL
#define MPPT_MIN_PANEL_VOLTAGE 10.0f

void MPPT_Init();
void MPPT_menager();