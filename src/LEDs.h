#ifndef LEDS_H
#define LEDS_H

#include <FastLED.h>
#include "configuration.h"

////////////////////////////////////////////////////////////////////////////////////////
//////////                             STRUCTURE                              //////////

// struct for LED Color
struct ColorHSV {
    uint8_t h;
    uint8_t s;
    uint8_t v;

    // Constructor
    ColorHSV(uint8_t h, uint8_t s, uint8_t v) : h(h), s(s), v(v) {};

    // Operator overloading
    bool operator==(const ColorHSV& otherColor) const {
        return (h == otherColor.h) && (s == otherColor.s) && (v == otherColor.v);
    }
};

// struct maintaining LEDs data
struct LEDData {
    uint8_t  flickerStep;
    float    currentBrightness;
    ColorHSV currentColor;

    // Constructor
    LEDData();

    // Struct methods
    void UpdateColor();
    void UpdateColor(const ColorHSV& color);
    void UpdateBrightnessScale(float brightnessScale);
    #if ENABLE_LED_FLICKERING
        void UpdateFlickering();
    #endif
};


////////////////////////////////////////////////////////////////////////////////////////
//////////                             CONSTANTS                              //////////

// Color constants
static const ColorHSV ORANGE_COLOR  = {COL_ORANGE_H, COL_ORANGE_S, COL_ORANGE_V};
static const ColorHSV BLUE_COLOR    = {COL_BLUE_H, COL_BLUE_S, COL_BLUE_V};
static const ColorHSV DEFAULT_COLOR = {COL_INIT_H, COL_INIT_S, COL_INIT_V};          // Initial Color, no Portals yet
static const ColorHSV SETTING_COLOR = {COL_SETTING_H, COL_SETTING_S, COL_SETTING_V}; // Indication Color for settings


////////////////////////////////////////////////////////////////////////////////////////
//////////                              OBJECTS                               //////////

extern LEDData leds;
extern CRGB mainArray[];
extern CRGB clawArray[];


////////////////////////////////////////////////////////////////////////////////////////
//////////                       FUNCTION PROTOTYPES                          //////////

bool setupLEDs(); // Setup
void IdlingStateColor(); // Idling state, LEDs display static color
void ShootingStateColor(uint16_t& flashTick, const ColorHSV& color); // Flashing either blue or orange portal color

#endif // LEDS_H
