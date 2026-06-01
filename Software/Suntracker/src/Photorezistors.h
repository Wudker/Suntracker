#pragma once

#define FINE_STEP_ANGLE 3.0f
#define FINE_MAX_STEPS 8
#define FINE_SETTLE_TIME 300
#define FINE_SAMPLES 5
#define MIN_VOLTAGE_IMPROVEMENT 0.03f

#define MOTOR1_DIR_PLUS true
#define MOTOR1_DIR_MINUS false

#define ADC_SAMPLES 8
#define VECTOR_DEADZONE 40
#define MIN_LIGHT_LEVEL 100

int16_t Find_general_optimal_position();
void Find_optimal_position();