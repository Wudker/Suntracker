#pragma once
#include <Arduino.h>

// ===== DIGITAL POTENTIOMETER (for MPPT load resistance) =====
const uint16_t POT_data = PB14;  // Data pin for digital potentiometer
const uint16_t POT_cs = PB15;    // Chip select pin for digital potentiometer

// ===== MPPT FEEDBACK ANALOG INPUTS =====
const uint16_t MPPT_A0 = PB0;    // MPPT feedback input 0
const uint16_t MPPT_A1 = PB1;    // MPPT feedback input 1

// ===== LIGHT SENSORS (4 photorezistors for sun position detection) =====
const uint16_t Photorezistor_1 = PA4;  // Top-left
const uint16_t Photorezistor_2 = PA5;  // Top-right
const uint16_t Photorezistor_3 = PA6;  // Bottom-left
const uint16_t Photorezistor_4 = PA7;  // Bottom-right

// ===== MOTOR 1 CONTROL PINS (Horizontal rotation) =====
const uint16_t Motor1_in1 = PB4;  // Motor 1 direction control IN1
const uint16_t Motor1_in2 = PB5;  // Motor 1 direction control IN2

// ===== MOTOR 2 CONTROL PINS (Vertical tilt) =====
const uint16_t Motor2_in1 = PB2;  // Motor 2 direction control IN1
const uint16_t Motor2_in2 = PB3;  // Motor 2 direction control IN2

// ===== SERVO CONTROL PINS =====
const uint16_t Servo_power_enable = PA9;   // Servo power enable
const uint16_t Servo_signal = PA10;        // Servo PWM signal (main servo)
const uint16_t Servo_2_signal = PA11;      // Servo PWM signal (secondary servo)

// ===== POWER ENABLE/CONTROL PINS =====
const uint16_t Motor_driver_enable = PA1;  // Enable main motor driver
const uint16_t Logic_5V_enable = PA2;      // Enable 5V logic power supply
const uint16_t Logic_5V_mode = PA3;        // Select 5V logic mode (output level)
const uint16_t Panel_enable = PB6;         // Enable solar panel output

// ===== USER INPUT =====
const uint16_t POWER_ON = PA8;  // Power button input (active LOW)

// ===== COMMUNICATION =====
const uint32_t PIN_I2C_SCL = PB10;  // I2C clock line (for power monitor)
const uint32_t PIN_I2C_SDA = PB11;  // I2C data line (for power monitor)