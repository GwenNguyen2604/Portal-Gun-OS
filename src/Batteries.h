#ifndef BATTERIES_H
#define BATTERIES_H

#include <Arduino.h>
#include "configuration.h"

////////////////////////////////////////////////////////////////////////////////////////
//////////                               ENUMS                                //////////

enum BatteryStates
{
    BATTERY_HIGH = 1,
    BATTERY_MED,
    BATTERY_LOW,
    BATTERY_IMBALANCED,
    UNKNOWN_STATE
};


////////////////////////////////////////////////////////////////////////////////////////
//////////                             CONSTANTS                              //////////

static constexpr uint8_t BATTERY_BUFFER_SIZE = 20;
// Imbalance detection configurations
// If the difference between battery ADC averages exceeds
// BAT_IMBALANCE_THRESHOLD for BAT_IMBALANCE_CONSECUTIVE checks
// the batteries are considered imbalanced.
static constexpr uint8_t  BAT_IMBALANCE_THRESHOLD = 18;
static constexpr uint8_t  BAT_IMBALANCE_CONSECUTIVE = 3; // number of checks


////////////////////////////////////////////////////////////////////////////////////////
//////////                             STRUCTURE                              //////////

struct batteryData
{
    uint8_t  batteryPin;
    uint16_t allReadings[BATTERY_BUFFER_SIZE];
    uint8_t  readIndex;
    uint8_t  validReadings;
    uint32_t sumReadings;
    uint16_t currentReading;
    uint16_t avgReading;
    uint8_t  percentage;

    // Constructor
    batteryData(uint8_t  batteryPin) :
        batteryPin(batteryPin),
        readIndex(0),
        validReadings(0),
        sumReadings(0),
        currentReading(0),
        avgReading(0),
        percentage(0)
    {
        // zero-initialize buffer to avoid using uninitialized values
        for (uint8_t i = 0; i < BATTERY_BUFFER_SIZE; ++i) allReadings[i] = 0;
    }

    // Struct method
    void UpdateBatteryReadings(); // Updates Battery Voltage readings
};

////////////////////////////////////////////////////////////////////////////////////////
//////////                              OBJECTS                               //////////

extern batteryData bat1;
extern batteryData bat2;


////////////////////////////////////////////////////////////////////////////////////////
//////////                       FUNCTION PROTOTYPES                          //////////

bool SetUpBatteries(); // Set up
BatteryStates ReadAndIndicateBattery(); // Periodically read
void BatteryIndicatorLED(BatteryStates batteryState); // Blink or set solid LED color deppending on battery state


#endif // BATTERIES_H
