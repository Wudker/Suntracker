#pragma once

// ===== COARSE POSITIONING (4-sensor vector tracking) =====
#define VECTOR_DEADZONE 40      // Minimum light difference to detect sun direction
#define MIN_LIGHT_LEVEL 100     // Minimum sensor reading to consider sunlight present
#define ADC_SAMPLES 8           // Number of ADC samples to average per sensor

// ===== FINE POSITIONING (gradient descent optimization) =====
#define FINE_STEP_ANGLE 3.0f    // Small angle steps for fine-tuning (degrees)
#define FINE_MAX_STEPS 8        // Maximum number of fine-tuning steps
#define FINE_SETTLE_TIME 300    // Time for panel to stabilize after moving (ms)
#define FINE_SAMPLES 5          // Number of voltage samples to average
#define MIN_VOLTAGE_IMPROVEMENT 0.03f  // Minimum voltage improvement to continue moving (Volts)

// ===== MOTOR DIRECTION CONSTANTS =====
#define MOTOR1_DIR_PLUS true    // Positive (clockwise) rotation
#define MOTOR1_DIR_MINUS false  // Negative (counter-clockwise) rotation

// ===== STARTUP CONDITIONS =====
#define MIN_PANEL_START_VOLTAGE 10.0f  // Minimum panel voltage needed to start (Volts)

// ===== GLOBAL STATE =====
extern volatile bool Critical_Sunlight;  // Flag indicating insufficient light

// Function declarations
int16_t Find_general_optimal_position();  // Find coarse position using 4 sensors
void Find_optimal_position();             // Fine-tune position for maximum voltage