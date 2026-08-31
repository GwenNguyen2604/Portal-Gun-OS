#include "LEDs.h"

////////////////////////////////////////////////////////////////////////////////////////
//////////                        OBJECTS DEFINITION                          //////////

LEDData leds;
CRGB mainArray[LED_COUNT_MAIN];
CRGB clawArray[LED_COUNT_CLAW];


////////////////////////////////////////////////////////////////////////////////////////
//////////                       FUNCTION DEFINITIONS                         //////////

bool
setupLEDs()
{
    // Setup Main and Claw LEDs:
    debugPrint("> Setup LEDs... ");
    FastLED.addLeds<WS2812B, PIN_LED_MAIN, GRB>(mainArray, LED_COUNT_MAIN);
    FastLED.addLeds<WS2812B, PIN_LED_CLAW, GRB>(clawArray, LED_COUNT_CLAW);

    // Set all LEDs to the initial values:
    leds.UpdateBrightnessScale(1.0f);
    fill_solid(mainArray, LED_COUNT_MAIN, CHSV(DEFAULT_COLOR.h, DEFAULT_COLOR.s, DEFAULT_COLOR.v));
    fill_solid(clawArray, LED_COUNT_CLAW, CHSV(DEFAULT_COLOR.h, DEFAULT_COLOR.s, DEFAULT_COLOR.v));
    FastLED.show();
    delay(10);
    debugPrintln("Done.");

    return true;
}


void
IdlingStateColor()
{
    // Update Flickering brightness:
    #if ENABLE_LED_FLICKERING
        leds.UpdateFlickering();
    #endif
    // Update LED brightness:
    leds.UpdateColor();
}

void
ShootingStateColor(uint16_t& flashTick, const ColorHSV& color)
{
    float brightnessScale = 1.0f;

    if(flashTick < FLASH_TICK_DROP)
        // Full LED Brightness (Bright Flash):
        brightnessScale = 1.0f;
    else if(flashTick == FLASH_TICK_DROP)
        // Drop brightness to 0:
        brightnessScale = 0.0f;
    else
    {
        // Rebuild brightness using the same step behavior as the original sketch.
        brightnessScale = (flashTick - FLASH_TICK_DROP) * (FLASH_REBUILD_SLOPE / 255.0f);
    }
    flashTick++;

    leds.UpdateBrightnessScale(brightnessScale);
    leds.UpdateColor(color);
}


////////////////////////////////////////////////////////////////////////////////////////
//////////                        METHODS DEFINITIONS                         //////////

inline
LEDData::LEDData() :
    flickerStep((MAX_BRIGHTNESS + MIN_BRIGHTNESS) / 2),
    currentBrightness(0.0f),
    currentColor(DEFAULT_COLOR)
{
}

void
LEDData::UpdateColor()
{
    const uint8_t mainBrightness = static_cast<uint8_t>(currentColor.v * currentBrightness);

    fill_solid(mainArray, LED_COUNT_MAIN, CHSV(currentColor.h, currentColor.s, mainBrightness));
    fill_solid(clawArray, LED_COUNT_CLAW, CHSV(currentColor.h, currentColor.s, mainBrightness));
    FastLED.show();
}


void
LEDData::UpdateColor(const ColorHSV& color)
{
    // Update stored color then render
    if (!(currentColor == color)) {
        debugPrintln("> Updating to new color state ");
        currentColor = color;
    }
    UpdateColor();
}


void
LEDData::UpdateBrightnessScale(float brightnessScale)
{
    currentBrightness = constrain(brightnessScale, 0.0f, 1.0f);
}


// Update Flickering brightness:
#if ENABLE_LED_FLICKERING
void
LEDData::UpdateFlickering()
{
    flickerStep += random(-FLICKER_STRENGTH + 1, FLICKER_STRENGTH);
    flickerStep = constrain(flickerStep, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    UpdateBrightnessScale(flickerStep / 255.0f);
}
#endif // ENABLE_LED_FLICKERING
