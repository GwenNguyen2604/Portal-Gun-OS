#ifndef CONFIG_H
#define CONFIG_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                            Feature Toggles                                         //////////

#define ENABLE_DEBUG_COMMS    0  // Enable debug communication via USB with Arduino IDE
#define ENABLE_LED_FLICKERING 1  // Enable LED Flickering
#define ENABLE_SOUND          1  // Enable Sound. Disable this if you are not using any speakers.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                           General Settings                                         //////////

#define VOLUME_PLAYER1 20   // Volume for DFPlayer1 (Idle-Humm)
#define VOLUME_PLAYER2 20   // Volume for DFPlayer2 (Sound Effects)
#define VOLUME_ENCODER 15   // Volume for DFPlayer1 encoder-selected voice lines
#define VOICE_FADE_OUT_DURATION_MS 400 // Fade time before interrupting idle hum for voice lines

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                            LED Settings                                            //////////

// Color 0 [0, 255]: Initial Color, no Portals yet. Doubles as Low-Battery-Color for Top Indicator:
#define COL_INIT_H  0
#define COL_INIT_S  255
#define COL_INIT_V  50

// Color 1 [0, 255]: Orange
#define COL_ORANGE_H  13
#define COL_ORANGE_S  255
#define COL_ORANGE_V  255

// Color 2 [0, 255]: Blue
#define COL_BLUE_H  145
#define COL_BLUE_S  255
#define COL_BLUE_V  255

// Color 3 [0, 255]: Indication Color for settings
#define COL_SETTING_H  0
#define COL_SETTING_S  0
#define COL_SETTING_V  255

// Shooting Flash duration:
#define FLASH_DURATION 300  // [ms]
#define FLASH_DROP     MIN_BRIGHTNESS / 2   // [0, 255] Brightness drop after full brightness

// Number of LEDs per strip:
#define LED_COUNT_MAIN  21  // Number of logically connected main WS2812B LEDs (Indicator, Beam, Nozzle)
#define LED_COUNT_CLAW  2   // 3x 2 LEDs in series  (Claws)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                                Pins                                                //////////

// Pins connected to LEDs:
#define PIN_LED_MAIN     13   // Data pin for main LEDs
#define PIN_LED_CLAW     23   // Data pin for claw LEDs
#define PIN_LED_CHARGE_R 32   // Pin to red RGB LED (Charge Indicator)
#define PIN_LED_CHARGE_G 21   // Pin to green RGB LED (Charge Indicator)

// Pins connected to Buttons:
#define PIN_BUTTON_ORANGE 33   // Orange button
#define PIN_BUTTON_BLUE   25   // Blue button
#define PIN_ROTENC_CLK    17   // Rotary Encoder Clock
#define PIN_ROTENC_DT     18   // Rotary Encoder Data
#define PIN_ROTENC_SW     19   // Rotary Encoder Switch

// Pins connected to Sound cards (DFPlayer Mini):
#define PIN_SPEAKER1_RX 26 // RX Pin for DFplayer1 (Idle-Humm)
#define PIN_SPEAKER1_TX 27 // TX Pin for DFplayer1 (Idle-Humm)
#define PIN_SPEAKER2_RX 14 // RX Pin for DFplayer2 (Sound Effects)
#define PIN_SPEAKER2_TX 16 // TX Pin for DFplayer2 (Sound Effects)

// Pins connected to Batteries:
#define PIN_ANALOG_BAT1 36   // Analog Pin reading the battery voltage (1)
#define PIN_ANALOG_BAT2 39   // Analog Pin reading the battery voltage (2)

// Unconnected pin
#define UNCONNECTED_PIN 15

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                          Flickering Settings                                       //////////

#define FLICKER_STRENGTH 7   // [0, 255] Strength of flickering effect
#define MIN_BRIGHTNESS   150 // [0, 255] Flickering wont go below
#define MAX_BRIGHTNESS   230 // [0, 255] Flickering wont go above

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                       State-Machine Settings                                       //////////

#define SM_TICK_RATE 5000            // State machine Tick-Duration in [us] (Inverse of Frequency)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                      Battery Reading Settings                                      //////////

#define BAT_READING_SKIP 100   // Number of ticks to skip before reading battery voltage
#define BAT_ADC_MAX      1100  // ADC value for 100%    battery    (4.2V)   -> Max value
#define BAT_FULL         1035  // ADC value for 'full'  battery    (4.0V)   -> Considered Full
#define BAT_LOW          760   // ADC value for 'empty' battery    (3.2V)   -> Considered Low
#define BAT_ADC_MIN      740   // ADC value for 0%      battery    (3.1V)   -> Considered Critical
#define LOW_BATTERY_WARNING_INTERVAL 8000


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                      Flash tick rate Settings                                      //////////

#define NUM_FLASH_TICKS (FLASH_DURATION / (SM_TICK_RATE / 1000))  // Number of ticks for the entire flash duration
#define FLASH_TICK_DROP NUM_FLASH_TICKS / 2                       // Number of ticks for full brightness. Brightn. then drops to 0 and rebuilds to MIN_BRIGHTNESS
#define FLASH_REBUILD_SLOPE float(((MIN_BRIGHTNESS - FLASH_DROP) / (NUM_FLASH_TICKS - FLASH_TICK_DROP)))  // Slope for rebuilding brightness

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                               Preprocessor debug stuff (DON'T TOUCH)                               //////////
#if ENABLE_DEBUG_COMMS
    #define debugPrint(...) Serial.print(__VA_ARGS__)
    #define debugPrintln(...) Serial.println(__VA_ARGS__)
#else
    #define debugPrint(...)
    #define debugPrintln(...)
#endif

#endif
