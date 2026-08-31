#include "Batteries.h"

////////////////////////////////////////////////////////////////////////////////////////
//////////                        OBJECTS DEFINITION                          //////////

batteryData bat1(PIN_ANALOG_BAT1);
batteryData bat2(PIN_ANALOG_BAT2);


////////////////////////////////////////////////////////////////////////////////////////
//////////                       FUNCTION DEFINITIONS                         //////////

bool
SetUpBatteries()
{
    // Input Pins for Battery:
    debugPrint("> Setup Pins for batteries... ");

    // Input Battery:
    pinMode(PIN_ANALOG_BAT1, INPUT);
    pinMode(PIN_ANALOG_BAT2, INPUT);
    analogReadResolution(12);

    // Output LEDs:
    pinMode(PIN_LED_CHARGE_R, OUTPUT);
    pinMode(PIN_LED_CHARGE_G, OUTPUT);

    debugPrint("> Done. ");
    return true;
}


BatteryStates
ReadAndIndicateBattery()
{
    static uint16_t loopsSkipped = 0;
    BatteryStates state = BatteryStates::UNKNOWN_STATE;

    // Corner case
    if (++loopsSkipped < BAT_READING_SKIP) {
        return state;
    }

    loopsSkipped = 0;
    // Read current average battery
    bat1.UpdateBatteryReadings();
    bat2.UpdateBatteryReadings();

    if (bat1.avgReading > BAT_FULL && bat2.avgReading > BAT_FULL) {
        debugPrintln("> Battery High");
        state = BatteryStates::BATTERY_HIGH;
    }
    else if (bat1.avgReading > BAT_LOW && bat2.avgReading > BAT_LOW) {
        debugPrintln("> Battery Medium");
        state = BatteryStates::BATTERY_MED;
    }
    else {
        debugPrintln("> Battery Low");
        state = BatteryStates::BATTERY_LOW;
    }

    // Check if batteries differ by more than configured thresholds. Low battery
    // still wins so the warning sound can trigger when either battery is low.
    static uint8_t imbalanceCount = 0;
    uint16_t readingDifference = abs(static_cast<int32_t>(bat1.avgReading) - static_cast<int32_t>(bat2.avgReading));
    bool isImbalancedNow = readingDifference > BAT_IMBALANCE_THRESHOLD;

    // Only update state as imbalanced after a maximum reading counts is reached
    // to avoid reacting to single noisy samples.
    if (isImbalancedNow) {
        if (imbalanceCount < BAT_IMBALANCE_CONSECUTIVE)
            imbalanceCount++;
        else if (state != BatteryStates::BATTERY_LOW) {
            debugPrint("> Warning: Batteries differ by more than 5%: ");
            debugPrint(bat1.avgReading);
            debugPrint(" vs ");
            debugPrint(bat2.avgReading);
            debugPrintln(" (5% = 18)");
            state = BatteryStates::BATTERY_IMBALANCED;
        }
    } else {
        imbalanceCount = 0;
    }
    BatteryIndicatorLED(state);
    return state;
}


void
BatteryIndicatorLED(BatteryStates batteryState)
{
    switch (batteryState) {
        case BatteryStates::BATTERY_HIGH:
            // Set charge LED to green
            digitalWrite(PIN_LED_CHARGE_R, LOW);
            digitalWrite(PIN_LED_CHARGE_G, HIGH);
            break;

        case BatteryStates::BATTERY_MED:
            // Set charge LED to orange
            digitalWrite(PIN_LED_CHARGE_R, HIGH);
            digitalWrite(PIN_LED_CHARGE_G, HIGH);
            break;

        case BatteryStates::BATTERY_LOW:
            // Set charge LED to red:
            digitalWrite(PIN_LED_CHARGE_R, HIGH);
            digitalWrite(PIN_LED_CHARGE_G, LOW);
            break;

        case BatteryStates::BATTERY_IMBALANCED: {
            // Blink state
            static bool ledState = false;
            static uint32_t lastToggle = 0;
            const uint32_t blinkInterval = 500; // ms

            if (millis() - lastToggle >= blinkInterval) {
                ledState = !ledState;
                lastToggle = millis();
            }
            if (ledState) {
                digitalWrite(PIN_LED_CHARGE_R, HIGH);
                digitalWrite(PIN_LED_CHARGE_G, LOW);
            }
            else {
                digitalWrite(PIN_LED_CHARGE_R, LOW);
                digitalWrite(PIN_LED_CHARGE_G, LOW);
            }
            break;
        }

        default:
            break;
    }
}


////////////////////////////////////////////////////////////////////////////////////////
//////////                        METHODS DEFINITIONS                         //////////

void
batteryData::UpdateBatteryReadings()
{
    // Appends newest reading to allReadings[] and update sumReadings
    // Subtract old reading from sum:
    sumReadings -= allReadings[readIndex];
    // Read new value:
    currentReading = analogRead(batteryPin);
    allReadings[readIndex] = currentReading;
    // Add new value to sum:
    sumReadings += allReadings[readIndex];
    // Increment reading index:
    readIndex++;
    // Wrap index back to 0:
    if (readIndex == BATTERY_BUFFER_SIZE)
    {
        readIndex = 0;
    }
    // During start up, not all readings in buffer is populated yet. Keep track
    // of valid readings to calculate the average
    if (validReadings < BATTERY_BUFFER_SIZE)
    {
        validReadings++;
    }

    // Calculate average:
    avgReading = static_cast<uint16_t>(sumReadings / validReadings);
    percentage = static_cast<uint8_t>(constrain(map(avgReading, BAT_ADC_MIN, BAT_ADC_MAX, 0, 100), 0, 100));
}
