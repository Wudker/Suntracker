
/*
    Microchip 63 taps Single Digital Potentiometer
    Simple two-wire UP/DOWN interface
    Author: dzalf - Daniel Melendrez
    Date: June 2020 (COVID-19 Vibes)
    Version: 1.1.1  - Initial deployment
             1.1.2  - General cleanup. Implemented new and overloaded methods that allow
                    to select the desired tap. It is now possible to select the nominal 
                    resistance value or override it by setting the measured value

*/

#include "MCP40xx.h"

// ===== CONSTRUCTORS =====

// Create MCP40xx instance with chip select and up/down pins
MCP40xx::MCP40xx(uint8_t CS, uint8_t UD) {

    _CSPin = CS;      // Store chip select pin
    _UDPin = UD;      // Store up/down control pin
    _debug = false;   // Disable debug output
}

// Create MCP40xx instance with debug output enabled/disabled
MCP40xx::MCP40xx(uint8_t CS, uint8_t UD, bool DEBUG) {

    _CSPin = CS;     // Store chip select pin
    _UDPin = UD;     // Store up/down control pin
    _debug = DEBUG;  // Enable/disable debug messages
}

// ===== INITIALIZATION =====

// Configure GPIO pins for digital potentiometer control
void MCP40xx::setup() {

    // Set up control pins as outputs
    pinMode(_UDPin, OUTPUT);   // Up/Down control pin
    pinMode(_CSPin, OUTPUT);   // Chip select pin

    // Set initial pin states (idle/inactive)
    digitalWrite(_CSPin, HIGH);  // CS is active LOW, so keep HIGH at idle
    digitalWrite(_UDPin, HIGH);  // UD idle state
}

// Initialize potentiometer with default 100 kOhm
void MCP40xx::begin() {
    _tapPointer = MCP40xx_DEFAULT_TAP_COUNT;  // Start at middle tap (31 of 63)
    _nominalResistance = MCP40xx_NOMINAL_RESISTANCE;

    if(_debug){
      Serial.println(F("Initializing MCP40xx..."));
    }
}

// Initialize potentiometer with custom maximum resistance value
void MCP40xx::begin(float nomRes) {
    _tapPointer = MCP40xx_DEFAULT_TAP_COUNT;  // Start at middle tap
    _nominalResistance = nomRes;              // Use custom resistance value

    if(_debug){
      Serial.println(F("Initializing MCP40xx..."));
    }
}

// Get current wiper position as fractional value (0.0 to 1.0)
float MCP40xx::wiper() {
    return (_tapPointer / MCP40xx_TAP_NUMBER);
}

// Increment wiper position by one tap (increase resistance)
void MCP40xx::inc() {

    if ((_tapPointer < MCP40xx_TAP_NUMBER)) {
        // Note: digitalWrite is slow enough for the MCP40xx timing

        unsigned long _startIncTime = micros();  // Measure execution time
        digitalWrite(_UDPin, HIGH);   // Set UP direction

        // Pulse chip select to trigger command
        digitalWrite(_CSPin, LOW);    // Start command

        // One increment pulse
        digitalWrite(_UDPin, LOW);    // Pulse UP line
        digitalWrite(_UDPin, HIGH);   // Return to HIGH

        // End command
        digitalWrite(_CSPin, HIGH);   // Release CS

        _tapPointer++;  // Increment tap counter

        // Clamp to maximum
        if (_tapPointer >= MCP40xx_TAP_NUMBER)
            _tapPointer = MCP40xx_TAP_NUMBER;

        _incDelay = micros() - _startIncTime;  // Record execution time
    }
}

// Decrement wiper position by one tap (decrease resistance)
void MCP40xx::dec() {

    if ((_tapPointer > 0)) {

        unsigned long _startDecTime = micros();  // Measure execution time

        digitalWrite(_UDPin, LOW);    // Set DOWN direction

        // Pulse chip select to trigger command
        digitalWrite(_CSPin, LOW);    // Start command

        // One decrement pulse
        digitalWrite(_UDPin, HIGH);   // Pulse UP line
        digitalWrite(_UDPin, LOW);    // Return to LOW

        // End command
        digitalWrite(_CSPin, HIGH);   // Release CS

        _tapPointer--;  // Decrement tap counter

        // Clamp to minimum
        if (_tapPointer <= 0)
            _tapPointer = 0;

        _decDelay = micros() - _startDecTime;  // Record execution time

    }
}

// Get execution time for last increment (microseconds)
unsigned long MCP40xx::incMicros() {
    return _incDelay;
}

// Get execution time for last decrement (microseconds)
unsigned long MCP40xx::decMicros() {
    return _decDelay;
}

// Get execution time for last setValue (microseconds)
unsigned long MCP40xx::setMicros() {
    return _setDelay;
}

// Get current tap number (0-63)
int MCP40xx::taps() {
    return _tapPointer;  // Current position in the resistor ladder
}

uint8_t MCP40xx::setValue(float desiredR) {

    float _currentValue;
    float _distance;
    int _tapTarget;

    unsigned long _startSetTime = micros();

    _tapTarget = round((desiredR * MCP40xx_TAP_NUMBER) / (_nominalResistance));
    _distance = abs(_tapPointer - _tapTarget);

    if (_debug) {
        Serial.print("Distance to target: ");
        Serial.println(_distance);

        Serial.print("Target tap: ");
        Serial.println(_tapTarget);
    }

    if (_tapTarget < _tapPointer) {
        for (int i = _tapPointer; i > _tapTarget; i--) {
            dec();
        }

    } else if (_tapTarget > _tapPointer) {
        for (int i = _tapPointer; i < _tapTarget; i++) {
            inc();
        }

    } else {
        // Leave everything where it is
    }

    _setDelay = micros() - _startSetTime;

    return _tapTarget;
}

uint8_t MCP40xx::setTap(uint8_t desiredTap) {

    float _currentValue;
    float _distance;
    int _tapTarget;

    unsigned long _startSetTime = micros();

    _tapTarget = desiredTap;
    _distance = abs(_tapPointer - _tapTarget);

    if (_debug) {
        Serial.print("Distance to target: ");
        Serial.println(_distance);

        Serial.print("Target tap: ");
        Serial.println(_tapTarget);
    }

    if (_tapTarget < _tapPointer) {
        for (int i = _tapPointer; i > _tapTarget; i--) {
            dec();
        }

    } else if (_tapTarget > _tapPointer) {
        for (int i = _tapPointer; i < _tapTarget; i++) {
            inc();
        }

    } else {
        // No touchy...Leave everything where it is...drink beer or coffee
    }

    _setDelay = micros() - _startSetTime;

    return _tapTarget;
}

void MCP40xx::zeroWiper() {

    // It is possible to do this with the latest setTap() method
    /*

    for (int i = MCP40xx_DEFAULT_TAP_COUNT ; i >= 0; i--) {
        dec();
    }

    */

   setTap(0);

    _tapPointer = 0;
}

void MCP40xx::maxWiper() {

     // It is possible to do this with the latest setTap() method
    /*

     for (int i = _tapPointer; i <= MCP40xx_TAP_NUMBER; i++) {
        inc();
    }

    */
    setTap(MCP40xx_TAP_NUMBER);

    _tapPointer = MCP40xx_TAP_NUMBER;
}

float MCP40xx::readValue() {
    return (_tapPointer / MCP40xx_TAP_NUMBER) * (_nominalResistance);
}
