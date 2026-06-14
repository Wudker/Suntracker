#pragma once

// ===== MPPT ALGORITHM CONFIGURATION =====
// Maximum Power Point Tracking: adjusts load resistance to extract maximum power from solar panel

#define MPPT_STEP 100.0f              // Resistance step size for each adjustment (Ohms)
#define MPPT_START_RESISTANCE 2500.0f // Starting resistance value (Ohms)

#define MPPT_MIN_RESISTANCE 0.0f      // Minimum allowed resistance (Ohms)
#define MPPT_MAX_RESISTANCE 5000.0f   // Maximum allowed resistance (Ohms)

#define MPPT_UPDATE_TIME 1000UL       // How often to check and adjust resistance (milliseconds)
#define MPPT_MIN_PANEL_VOLTAGE 10.0f  // Minimum panel voltage to consider (Volts)

// Function declarations
bool MPPT_Init();      // Initialize MPPT algorithm and power monitor
void MPPT_menager();   // Main MPPT algorithm - adjust load resistance for maximum power