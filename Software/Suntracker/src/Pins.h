#pragma once
#include <Arduino.h>

// Digital potentiometer
const uint16_t POT_data = PB14;
const uint16_t POT_cs = PB15;

// MPPT analog inputs
const uint16_t MPPT_A0 = PB0;
const uint16_t MPPT_A1 = PB1;

// Photorezistors - ADC
const uint16_t Photorezistor_1 = PA4;
const uint16_t Photorezistor_2 = PA5;
const uint16_t Photorezistor_3 = PA6;
const uint16_t Photorezistor_4 = PA7;

// Motor control pins
const uint16_t Motor1_in1 = PB4;
const uint16_t Motor1_in2 = PB5;

const uint16_t Motor2_in1 = PB2;
const uint16_t Motor2_in2 = PB3;

// Servo
const uint16_t Servo_power_enable = PA9;
const uint16_t Servo_signal = PA10;
const uint16_t Servo_2_signal = PA11;

// Power control pins
const uint16_t Motor_driver_enable = PA1;
const uint16_t Logic_5V_enable = PA2;
const uint16_t Logic_5V_mode = PA3;
const uint16_t Panel_enable = PB6;

// Button
const uint16_t POWER_ON = PA8; 

// I2C
const uint32_t PIN_I2C_SCL = PB10;
const uint32_t PIN_I2C_SDA = PB11;