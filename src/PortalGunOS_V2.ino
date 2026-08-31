/*
  Portal Gun V2 © 2025 by EVARATE is licensed under CC BY-NC 4.0
*/

// Includes:
#include <Arduino.h>
#include <FastLED.h>
#include <ESP32TimerInterrupt.h>
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
#include <RotaryEncoder.h>
#include <driver/adc.h>
#include "configuration.cpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                           INITIALIZATION                                           //////////

// Class for a LED Color:
class ColorHSV {
public:
  uint8_t h = 0;
  uint8_t s = 0;
  uint8_t v = 0;
  ColorHSV(uint8_t h, uint8_t s, uint8_t v) : h(h), s(s), v(v) {};
};

// Struct containing all LED data:
struct LEDData {
  ColorHSV color_1        = {COL1_H, COL1_S, COL1_V};  // Orange
  ColorHSV color_2        = {COL2_H, COL2_S, COL2_V};  // Blue
  ColorHSV color_current  = {COL0_H, COL0_S, COL0_V};  // Current Color
  ColorHSV color_settings = {COL3_H, COL3_S, COL3_V};  // Settings Color

  float currentBrightness = 0.0;

  CRGB mainArray[LED_COUNT_MAIN];
  CRGB clawArray[LED_COUNT_CLAW];
};
LEDData leds; // Global LED Data

// Struct containing all Sound data:
#if ENABLE_SOUND
  EspSoftwareSerial::UART SoftSerial1(PIN_SPEAKER1_RX, PIN_SPEAKER1_TX);
  EspSoftwareSerial::UART SoftSerial2(PIN_SPEAKER2_RX, PIN_SPEAKER2_TX);
  DFPlayerMini_Fast dfPlayer1; // Idle-Humm     (5W) 
  DFPlayerMini_Fast dfPlayer2; // Sound-Effects (3W)
  uint8_t volume_spk1 = VOLUME_PLAYER1;
  uint8_t volume_spk2 = VOLUME_PLAYER2;
#endif

enum SOUND_IDLE_FILE{   // Soundfiles for Speaker 1
  SOUND0_IDLE = 1
};

enum SOUND_EFFECT_FILE{ // Soundfiles for Speaker 2
  SOUND1_ORANGE_1      = 1,
  SOUND1_ORANGE_2      = 2,
  SOUND1_ORANGE_3      = 3,
  SOUND1_ORANGE_4      = 4,
  SOUND1_BLUE_1        = 5,
  SOUND1_BLUE_2        = 6,
  SOUND1_BLUE_3        = 7,
  SOUND1_BLUE_4        = 8,
  SOUND1_SPEAKER_1     = 9,
  SOUND1_SPEAKER_2     = 10,
  SOUND1_HUE           = 11,
  SOUND1_SATURATION    = 12,
  SOUND1_BRIGHTNESS    = 13,
  SOUND1_CONFIRM       = 14,
  SOUND1_ERROR         = 15,
  SOUND1_CLICK         = 16,
  SOUND1_ACTIVATION    = 17,
  SOUND1_LOW_BATTERY   = 18,
};

// Flickering Variables:
#if ENABLE_LED_FLICKERING
struct flickerData
{
  uint8_t step = (MAX_BRIGHTNESS + MIN_BRIGHTNESS) / 2;
  uint8_t max = MAX_BRIGHTNESS;
  uint8_t min = MIN_BRIGHTNESS;
};
flickerData flicker;
#endif

#define NUM_FLASH_TICKS (FLASH_DURATION / (SM_TICK_RATE / 1000))  // Number of ticks for the entire flash duration
#define FLASH_TICK_DROP NUM_FLASH_TICKS / 2                       // Number of ticks for full brightness. Brightn. then drops to 0 and rebuilds to MIN_BRIGHTNESS
#define FLASH_REBUILD_SLOPE float(((MIN_BRIGHTNESS - FLASH_DROP) / (NUM_FLASH_TICKS - FLASH_TICK_DROP)))  // Slope for rebuilding brightness

// Battery Data:
struct batteryData
{
  uint16_t allReadings[20];
  uint8_t readingIndex = 0;
  uint16_t currentReading = 0;
  uint16_t sumReadings = 0;
  uint16_t averageReading = 0;
  float percentage = 0.0;
};
batteryData battery1;
batteryData battery2;

// Rotary Encoder (Will be created in setupPins()):
RotaryEncoder *rotaryEnc = nullptr;

// State machine states:
enum SM_STATE
{
  IDLE,
  SHOOTING1,
  SHOOTING2
};
SM_STATE sm_state = IDLE;

// Variables and Flags:
volatile bool flagStateMachineTickIsDue = false;
volatile bool flagOrangePressed = false;
volatile bool flagBluePressed = false;
volatile bool flagREleft = false;
volatile bool flagREright = false;
volatile bool flagREpress = false;

// Timer for State Machine Tick:
ESP32Timer ITimer(0);

//////////                                           INITIALIZATION                                           //////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                         FUNCTION PROTOTYPES                                        //////////

// Setup Functions:
bool setupSerial();
bool setupLEDs();
bool setupPins();
bool setupSMTimer();
bool setupSound();

// Button press ISR Functions:
void IRAM_ATTR onOrangeButtonPress();
void IRAM_ATTR onBlueButtonPress();
void IRAM_ATTR onRotaryEncoderRotation();
void IRAM_ATTR onRotaryEncoderPress();

// State Machine Functions:
bool IRAM_ATTR flagStateMachineTick(void* timerNo);   // Flag indicating readyness for state machine tick
bool IRAM_ATTR     stateMachineTick();                // Execute Machine Tick
void stateIdle();                                     // Idle State
void stateShooting1();                                // Shooting State for Orange Portal
void stateShooting2();                                // Shooting State for Blue Portal

// Other Functions:
void Serial_printLogo();
void readAndIndicateBattery();
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);

//////////                                         FUNCTION PROTOTYPES                                        //////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                                SETUP                                               //////////

void setup()
{
  // Setup Serial Communication:
  setupSerial();

  // Setup Sound:
  #if ENABLE_SOUND
    setupSound();
  #endif

  // Setup LEDs:
  setupLEDs();

  // Setup Pins:
  setupPins();

  // Setup State Machine Timer:
  setupSMTimer();

  debugPrint("> Setup Complete. Starting State Machine @ ");
  debugPrint(SM_TICK_RATE / 1000);
  debugPrintln("ms\n//////////////////////////////////////////////");
  Serial.flush();
} 

// Setup Function for Serial Communication:
bool setupSerial()
{
  // Setup Serial and print startup message:
  Serial.begin(115200);
  while(!Serial);
  Serial.println("+------------------------------------------------------------------+");
  Serial.println("|  Portal Gun V2 © 2025 by EVARATE is licensed under CC BY-NC 4.0  |");
  Serial.println("+------------------------------------------------------------------+\n");
  Serial_printLogo();

  Serial.print("Starting Portal Gun V2 on '");
  Serial.print(ARDUINO_BOARD);
  Serial.print("' @ ");
  Serial.print(F_CPU / 1000000);
  Serial.println("MHz\n");
  
  return true;
}

// Setup Function for LEDs:
bool setupLEDs()
{
  // Setup Main and Claw LEDs:
  debugPrint("> Setup LEDs... ");
  FastLED.addLeds<WS2812B, PIN_LED_MAIN, GRB>(leds.mainArray, LED_COUNT_MAIN);
  FastLED.addLeds<WS2812B, PIN_LED_CLAW, GRB>(leds.clawArray, LED_COUNT_CLAW);
  
  // Set all LEDs to the initial color:
  fill_solid(leds.mainArray, LED_COUNT_MAIN, CHSV(leds.color_current.h, leds.color_current.s, leds.color_current.v));
  fill_solid(leds.clawArray, LED_COUNT_CLAW, CHSV(leds.color_settings.h, leds.color_settings.s, leds.color_settings.v));
  FastLED.show();
  delay(10);
  
  debugPrintln("Done.");
  return true;
}

bool setupPins()
{
  debugPrint("> Setup Pins... ");
  // Input Buttons:
  pinMode(PIN_BUTTON_ORANGE, INPUT_PULLUP);
  pinMode(PIN_BUTTON_BLUE,   INPUT_PULLUP);
  pinMode(PIN_ROTENC_CLK,    INPUT_PULLUP);
  pinMode(PIN_ROTENC_DT,     INPUT_PULLUP);
  pinMode(PIN_ROTENC_SW,     INPUT_PULLUP);

  // Input Battery:
  pinMode(PIN_ANALOG_BAT1, INPUT);
  pinMode(PIN_ANALOG_BAT2, INPUT);
  adc1_config_width(ADC_WIDTH_BIT_10);

  // Output LEDs:
  pinMode(PIN_LED_CHARGE_R, OUTPUT);
  pinMode(PIN_LED_CHARGE_G, OUTPUT);

  // Setup Button Interrupts:
  attachInterrupt(PIN_BUTTON_ORANGE, onOrangeButtonPress, FALLING);
  attachInterrupt(PIN_BUTTON_BLUE,   onBlueButtonPress,   FALLING);
  
  // Setup Rotary Encoder:
  rotaryEnc = new RotaryEncoder(PIN_ROTENC_DT, PIN_ROTENC_CLK, RotaryEncoder::LatchMode::FOUR0);
  attachInterrupt(PIN_ROTENC_CLK,    onRotaryEncoderRotation, CHANGE);
  attachInterrupt(PIN_ROTENC_DT,     onRotaryEncoderRotation, CHANGE);
  attachInterrupt(PIN_ROTENC_SW,     onRotaryEncoderPress,    FALLING);

  debugPrintln("Done.");
  return true;
}

bool setupSMTimer()
{
  // Setup Timer for State Machine:
  if(ITimer.attachInterruptInterval(SM_TICK_RATE, flagStateMachineTick)){
    debugPrint("> Setup State Machine Timer @ ");
    debugPrint(SM_TICK_RATE);
    debugPrint("us (");
    debugPrint(1000000 / SM_TICK_RATE);
    debugPrintln("Hz)");
  }
  else{
    debugPrintln("> Failed to start State-Machine Timer.");
    return false;
  }
  return true;
}

#if ENABLE_SOUND
bool setupSound()
{
  // Setup Sound:
  debugPrintln("> Setup Sound... ");

  SoftSerial1.begin(9600);
  SoftSerial2.begin(9600);
  dfPlayer1.begin(SoftSerial1);
  dfPlayer2.begin(SoftSerial2);

  delay(1000);

  dfPlayer1.stop();
  dfPlayer2.stop();

  dfPlayer1.reset();
  dfPlayer2.reset();

  // Attempt to play Idle Sound:
  uint8_t player1_attempts = 0;
  while(dfPlayer1.currentFlashTrack() != SOUND_IDLE_FILE::SOUND0_IDLE && dfPlayer1.currentMode() != 2)
  {
    dfPlayer1.loop(SOUND_IDLE_FILE::SOUND0_IDLE);
    delay(150);
    player1_attempts++;
    if(player1_attempts > 10)
    {
      debugPrintln(">   Error: Player 1 not responding.");
      break;
    }
  }

  // Attempt to play Activation Sound:
  uint8_t player2_attempts = 0;
  while(dfPlayer2.currentFlashTrack() != SOUND_EFFECT_FILE::SOUND1_ACTIVATION && dfPlayer2.currentMode() != 2)
  {
    dfPlayer2.play(SOUND_EFFECT_FILE::SOUND1_ACTIVATION);
    delay(150);
    player2_attempts++;
    if(player2_attempts > 10)
    {
      debugPrintln(">   Error: Player 2 not responding.");
      break;
    }
  }

  dfPlayer1.volume(volume_spk1);
  delay(200); 
  dfPlayer2.volume(volume_spk2);
  delay(200);

  debugPrint(">   Player 1 Tracks: ");
  debugPrintln(dfPlayer1.numSdTracks());
  debugPrint(">   Player 2 Tracks: ");
  debugPrintln(dfPlayer2.numSdTracks());

  debugPrintln("> Done.");

  return true;
}
#endif

//////////                                                SETUP                                               //////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                                LOOP                                                //////////

void loop()
{  
  // Run State Machine:
  if(flagStateMachineTickIsDue)
  {
    stateMachineTick();
  }
  else
  {
    delay(1);
  }
}

//////////                                                LOOP                                                //////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                        FUNCTION DEFINITIONS                                        //////////

// ISR Function for Orange Button Press:
void IRAM_ATTR onOrangeButtonPress()
{
  flagOrangePressed = true;
}

// ISR Function for Blue Button Press:
void IRAM_ATTR onBlueButtonPress()
{
  flagBluePressed = true;
}

void IRAM_ATTR onRotaryEncoderRotation()
{
  rotaryEnc->tick();
}

void IRAM_ATTR onRotaryEncoderPress()
{
  // Debounce:
  static unsigned long lastPressTime = 0;
  unsigned long currentTime = millis();
  if (currentTime - lastPressTime < 300) {
    return;
  }
  lastPressTime = currentTime;

  flagREpress = true;
}

// State Machine Functions:
bool IRAM_ATTR flagStateMachineTick(void* timerNo)
{
  flagStateMachineTickIsDue = true;
  return true;
}

bool IRAM_ATTR stateMachineTick()
{
  // Get rotary Encoder Position:
  rotaryEnc->tick();
  static long rotEnc_lastPosition = 0;
  long rotEnc_position = rotaryEnc->getPosition();
  if(rotEnc_position != rotEnc_lastPosition)
  {
    if(rotEnc_position > rotEnc_lastPosition) flagREright = true;
    else if(rotEnc_position < rotEnc_lastPosition) flagREleft = true;
    rotEnc_lastPosition = rotEnc_position;
    debugPrint("> Rotary Encoder: ");
    debugPrint(flagREleft ? "Left " : "Right");
    debugPrint(" (");
    debugPrint(rotEnc_position);
    debugPrintln(")");
  }

  if(flagREpress) debugPrintln("> Rotary Encoder: Press");


  switch(sm_state)
  {
    case IDLE:
      stateIdle();
      break;
    case SHOOTING1:
      stateShooting1();
      break;
    case SHOOTING2:
      stateShooting2();
      break;
    default:
      break;
  }


  // Reset Flags:
  flagStateMachineTickIsDue = false;
  flagOrangePressed = false;
  flagBluePressed = false;
  flagREleft = false;
  flagREright = false;
  flagREpress = false;

  // Read Battery:
  readAndIndicateBattery();

  // Debug Interval measurement:
  #if 0
    static long tickTimer = 0;
    long tickLength = micros() - tickTimer;
    tickTimer = micros();
    debugPrint("Tick Intervall: ");
    debugPrint(tickLength);
    debugPrintln("us");
  #endif

  return true;
}

void stateIdle()
{
  // Check for Shot-button press:
  if(flagOrangePressed)    sm_state = SHOOTING1;
  else if(flagBluePressed) sm_state = SHOOTING2;

  // Update Flickering brightness:
  #if ENABLE_LED_FLICKERING
    flicker.step += random(-FLICKER_STRENGTH + 1, FLICKER_STRENGTH);
    flicker.step = constrain(flicker.step, flicker.min, flicker.max);
    leds.currentBrightness = flicker.step / 255.0;
  #endif

  // Update LED brightness:
  fill_solid(leds.mainArray, LED_COUNT_MAIN, CHSV(leds.color_current.h, leds.color_current.s, leds.color_current.v * leds.currentBrightness));
  fill_solid(leds.clawArray, LED_COUNT_CLAW, CHSV(leds.color_current.h, leds.color_current.s, leds.color_current.v));
  FastLED.show();
}

// Gets executed when the Orange Button is pressed. Switches Color, plays sound and returns to idle state:
void stateShooting1()
{
  static uint16_t flashTick = 0;

  if(flashTick == 0) 
  {
    debugPrintln("> Shot Portal: Orange");

    // Play random shooting sound:
    #if ENABLE_SOUND
      dfPlayer2.play(random(SOUND1_ORANGE_1, SOUND1_ORANGE_4));
      delay(200);

      // Ensure proper volume:
      dfPlayer1.volume(volume_spk1);
      dfPlayer2.volume(volume_spk2);
    #endif
    flashTick++;
  }

  if(flashTick < FLASH_TICK_DROP)
  {
    // Full LED Brightness (Bright Flash):
    leds.currentBrightness = 1.0;
    flashTick++;
  }
  else if(flashTick == FLASH_TICK_DROP)
  {
    // Drop brightness to 0:
    leds.currentBrightness = 0.0;
    flashTick++;
  }
  else if(flashTick < NUM_FLASH_TICKS)
  {
    // Rebuild brightness:
    leds.currentBrightness += FLASH_REBUILD_SLOPE / 255.0;
    flashTick++;
  }
  else
  {
    flashTick = 0;

    // Return to IDLE:
    sm_state = IDLE;
  }

  // Update LED Color:
  leds.color_current = leds.color_1;
  fill_solid(leds.mainArray, LED_COUNT_MAIN, CHSV(leds.color_current.h, leds.color_current.s, leds.color_current.v * leds.currentBrightness));
  fill_solid(leds.clawArray, LED_COUNT_CLAW, CHSV(leds.color_current.h, leds.color_current.s, leds.color_current.v));
  FastLED.show();
}

void stateShooting2()
{
  // Perform Flashing:
  static uint16_t flashTick = 0;
  if(flashTick == 0) 
  {
    debugPrintln("> Shot Portal: Blue");

    // Play random shooting sound:
    #if ENABLE_SOUND
      dfPlayer2.play(random(SOUND1_BLUE_1, SOUND1_BLUE_4));
      delay(200);

      // Ensure proper volume:
      dfPlayer1.volume(volume_spk1);
      dfPlayer2.volume(volume_spk2);
    #endif
    flashTick++;
  }

  if(flashTick < FLASH_TICK_DROP)
  {
    // Full LED Brightness (Bright Flash):
    leds.currentBrightness = 1.0;
    flashTick++;
  }
  else if(flashTick == FLASH_TICK_DROP)
  {
    // Drop brightness to 0:
    leds.currentBrightness = 0.0;
    flashTick++;
  }
  else if(flashTick < NUM_FLASH_TICKS)
  {
    // Rebuild brightness:
    leds.currentBrightness += FLASH_REBUILD_SLOPE / 255.0;
    flashTick++;
  }
  else
  {
    flashTick = 0;

    // Return to IDLE:
    sm_state = IDLE;
  }

  // Update LED Color:
  leds.color_current = leds.color_2;
  fill_solid(leds.mainArray, LED_COUNT_MAIN, CHSV(leds.color_current.h, leds.color_current.s, leds.color_current.v * leds.currentBrightness));
  fill_solid(leds.clawArray, LED_COUNT_CLAW, CHSV(leds.color_current.h, leds.color_current.s, leds.color_current.v));
  FastLED.show();
}

// Function for printing the Portal Gun Logo:
void Serial_printLogo()
{
  // Taken from https://emojicombos.com/portal-ascii-art
  static const char logo[] =                                                                        
  "⠀⠀⠀⠀⢀⣠⣤⣤⣴⡶⣶⢶⣶⣶⣶⣶⣦⣤⣄⡀\n"
  "⠀⠀⢠⡾⠿⣭⢳⣍⢶⡹⢎⡗⡞⣬⢓⠭⣹⡿⣿⣿⣿⡶⣄⡀\n"
  "⠀⠀⣿⢏⡻⢴⠳⣜⡲⣙⠞⡼⡱⢎⡭⢚⡥⣚⠱⢦⡙⡟⣯⣽⢦⣄\n"
  "⠀⢸⡿⣜⢣⡟⡻⣄⢻⡘⢿⣠⠻⡜⣠⠟⣠⢃⡟⣄⢛⠣⡜⢻⢿⣼⣧⣄⠀⠀⠀⠀⠀⠀⣀⣀⣄⣀\n"
  "⠀⠘⣿⣜⣣⠽⡱⢎⡣⡝⢦⡱⢣⢝⡰⡹⢄⡳⣸⡾⢞⠷⣮⣓⣮⣿⣿⡿⠀⢀⣠⡶⠞⠛⠉⠁⠉⠙⢷⡄\n"
  "⠀⠀⢻⣶⣩⢞⡵⣋⠶⡙⣆⠳⣍⠖⣱⠑⡮⢜⡱⢯⣬⢢⣽⣿⣿⣿⣿⣿⠚⠉⠁⠀⠀⠀⠀⠀⠀⠀⣬⣿⣤⣀⠀⠀⠀⢀⣼⣦⣄⣀\n"
  "⠀⠀⠀⠹⣶⣋⡞⣥⢫⡕⣎⠳⣬⠙⣆⠛⡴⢉⠖⣩⢚⣿⣿⣿⣿⣿⣿⣿⣿⣶⣤⣀⡀⠀⠀⠀⠀⠀⣽⣼⣿⣸⣧⣶⠾⠟⠋⠛⠻⠿⢿⣷⣶⣤⣄⡀⠀⣠⣤⡀\n"
  "⠀⠀⠀⠀⠉⠷⣝⢦⠳⣜⢬⠳⣌⡛⣌⠳⡘⣌⠚⣤⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣦⣠⣶⣾⣿⣿⣿⣿⣅⡀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠙⠛⠿⠿⣿⣷⡀\n"
  "⠀⠀⠀⠀⠀⠀⠈⠻⣗⣜⣺⡷⣿⣿⣬⢧⣱⣢⣽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣇⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢿⣷\n"
  "⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠛⠷⣭⠿⣟⡿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣻⡧\n"
  "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠶⣏⠛⠿⣿⣿⡿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿⠻⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣦⣴⡀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠁\n"
  "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⢦⣄⠀⠀⠀⠙⣻⣿⣿⡿⠿⣍⡳⢥⡛⡴⢂⡛⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠆\n"
  "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢻⣄⠀⠀⠀⣿⣿⣯⡗⡶⣙⠧⣎⢣⡕⢮⡑⢆⡙⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿\n"
  "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠿⣆⠀⠀⠙⢿⣿⣯⠷⣭⢳⡬⢳⡌⢧⡚⣤⢋⢆⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠯⢷⣦⣀⡀\n"
  "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣧⠀⠀⠀⠉⠻⢿⣽⣷⣿⣷⣾⡥⣛⠴⣋⣌⠣⠽⣿⣿⣿⣿⣿⣿⣿⡿⠁⠀⠀⠈⠙⠻⢶⣾⣦⡄\n"
  "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣧⠀⠀⠀⢠⡖⣻⣿⣿⣿⣿⣷⢫⣞⡱⢎⡝⢦⡙⢿⣿⣿⣿⣿⡏⠀⠀⠀⠀⠀⠀⠀⠀⠉⠟⣿⣧\n"
  "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠶⢦⡴⠿⠻⠿⣥⢿⡄⠈⠉⠛⠾⠽⠾⣼⣧⣞⣥⣫⣝⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉\n"
  "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠀⠉⠁\n"
  "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⢻⡄\n"
  "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⣿⣦⣄\n"
  "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢻⣷⣴⢠\n"
  "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠻⣷⣔⡀\n"
  "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢿⣦⡀⢀⣀⣀⣤⣦\n"
  "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⢿⠿⠿⠛⠛⠉⠁\n\n";
  Serial.println(logo);
}

void readAndIndicateBattery()
{
  static uint16_t loopsSkipped = 0;
  if (++loopsSkipped >= BAT_READING_SKIP) {
    loopsSkipped = 0;

    // Subtract old reading from sum:
    battery1.sumReadings -= battery1.allReadings[battery1.readingIndex];
    battery2.sumReadings -= battery2.allReadings[battery2.readingIndex];

    // Read new value:
    battery1.allReadings[battery1.readingIndex] = analogRead(PIN_ANALOG_BAT1);
    battery2.allReadings[battery2.readingIndex] = analogRead(PIN_ANALOG_BAT2);

    // Add new value to sum:
    battery1.sumReadings += battery1.allReadings[battery1.readingIndex];
    battery2.sumReadings += battery2.allReadings[battery2.readingIndex];

    // Increment reading index:
    battery1.readingIndex++;
    battery2.readingIndex++;

    // Wrap index back to 0:
    if (battery1.readingIndex >= 20) battery1.readingIndex = 0;
    if (battery2.readingIndex >= 20) battery2.readingIndex = 0;

    // Calculate average:
    battery1.averageReading = battery1.sumReadings / 20;
    battery2.averageReading = battery2.sumReadings / 20;

    // Calculate percentage:
    battery1.percentage = map(battery1.averageReading, BAT_ADC_MIN, BAT_ADC_MAX, 0, 100);
    battery2.percentage = map(battery2.averageReading, BAT_ADC_MIN, BAT_ADC_MAX, 0, 100);

    // Check if batteries differ by more than  5%:
    if(abs(battery1.averageReading - battery2.averageReading) > 18)
    {
      // Blink charge LED red:
      static bool ledState = false;
      if(ledState)
      {
        digitalWrite(PIN_LED_CHARGE_R, HIGH);
        digitalWrite(PIN_LED_CHARGE_G, LOW);
      }
      else
      {
        digitalWrite(PIN_LED_CHARGE_R, LOW);
        digitalWrite(PIN_LED_CHARGE_G, LOW);

        // Print warning:
        debugPrint("> Warning: Batteries differ by more than 5%: ");
        debugPrint(battery1.averageReading);
        debugPrint(" vs ");
        debugPrint(battery2.averageReading);
        debugPrintln(" (5% = 18)");
      }
    }

    // If lowest battery is above BAT_FULL, set charge LED to green:
    if (battery1.averageReading > BAT_FULL || battery2.averageReading > BAT_FULL)
    {
      digitalWrite(PIN_LED_CHARGE_R, LOW);
      digitalWrite(PIN_LED_CHARGE_G, HIGH);
    }
    // If both batteries are above BAT_LOW, set charge LED to orange:
    else if (battery1.averageReading > BAT_LOW && battery2.averageReading > BAT_LOW)
    {
      digitalWrite(PIN_LED_CHARGE_R, HIGH);
      digitalWrite(PIN_LED_CHARGE_G, HIGH);
    }
    // If either battery is below BAT_LOW, set charge LED to red:
    else
    {
      digitalWrite(PIN_LED_CHARGE_R, HIGH);
      digitalWrite(PIN_LED_CHARGE_G, LOW);
    }

    

  }
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//////////                                        FUNCTION DEFINITIONS                                        //////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
