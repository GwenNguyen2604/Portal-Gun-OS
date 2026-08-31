/*
    Portal Gun V2 © 2025 by EVARATE is licensed under CC BY-NC 4.0
*/

// Includes:
#include <Arduino.h>
#include <ESP32TimerInterrupt.h>
#include "configuration.h"
#include "ShootingButtons.h"
#include "LEDs.h"
#include "Rotary.h"
#include "Batteries.h"
#include "Speakers.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                           INITIALIZATION                                           //////////

// Variables and Flags:
volatile bool flagStateMachineTickIsDue = false;

// Timer for State Machine Tick:
ESP32Timer ITimer(0);

//////////                                           INITIALIZATION                                           //////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                         FUNCTION PROTOTYPES                                        //////////


// Button press ISR Functions:
void IRAM_ATTR onOrangeButtonPress();
void IRAM_ATTR onBlueButtonPress();
void IRAM_ATTR onRotaryEncoderRotation();
void IRAM_ATTR onRotaryEncoderPress();
// State Machine Functions:
bool IRAM_ATTR flagStateMachineTick(void* timerNo);   // Flag indicating readyness for state machine tick
bool IRAM_ATTR stateMachineTick();                    // Execute Machine Tick

bool SetupSerial();
bool SetupSMTimer();
void ProcessRotaryEncoderStates();
void StateShooting(const ColorHSV& shootingColor);
void ReadBattery();
void Serial_printLogo();

//////////                                         FUNCTION PROTOTYPES                                        //////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                                SETUP                                               //////////

void setup()
{
    // Setup Serial communication
    SetupSerial();

    // Setup Sound, LEDs, Batteries, Rotary encoder, and Buttons
    #if ENABLE_SOUND
        SetupSpeakers();
    #endif
    setupLEDs();
    SetUpBatteries();
    SetUpEncoder();
    SetupButtons();

    // Setup Button Interrupts:
    attachInterrupt(PIN_BUTTON_ORANGE, onOrangeButtonPress, FALLING);
    attachInterrupt(PIN_BUTTON_BLUE,   onBlueButtonPress,   FALLING);

    // Set Up Rotary Encoder interrupts
    attachInterrupt(digitalPinToInterrupt(PIN_ROTENC_CLK), onRotaryEncoderRotation, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ROTENC_DT), onRotaryEncoderRotation, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ROTENC_SW), onRotaryEncoderPress, FALLING);

    // Setup State Machine Timer:
    SetupSMTimer();

    debugPrint("> Setup Complete. Starting State Machine @ ");
    debugPrint(SM_TICK_RATE / 1000);
    debugPrintln("ms\n//////////////////////////////////////////////");
    #if ENABLE_DEBUG_COMMS
        Serial.flush();
    #endif
}

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

// State Machine Functions:
bool IRAM_ATTR flagStateMachineTick(void* timerNo)
{
    flagStateMachineTickIsDue = true;
    return true;
}

bool IRAM_ATTR stateMachineTick()
{
    // Get rotary Encoder Position:
    ProcessRotaryEncoderStates();
    // Determine if Idle or Shooting
    ProcessShootingStates();
    // Periodically Check on Speakers' state
    #if ENABLE_SOUND
        HandleSpeakersIdlingStates();
    #endif
    // Reset Flags:
    flagStateMachineTickIsDue = false;

    // Read Battery:
    ReadBattery();

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


void IRAM_ATTR onRotaryEncoderRotation()
{
    rotaryEnc.tick();
    rotPos.SetRotationPending(true);
}

void IRAM_ATTR onRotaryEncoderPress()
{
    rotPos.SetEncoderPressed(true);
}

// ISR Function for Orange Button Press:
void IRAM_ATTR onOrangeButtonPress()
{
    buttonStates.SetOrangeFlagPressed(true);
}

// ISR Function for Blue Button Press:
void IRAM_ATTR onBlueButtonPress()
{
    buttonStates.SetBlueFlagPressed(true);
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


bool SetupSerial()
{
    randomSeed(analogRead(UNCONNECTED_PIN)); // Seed with noise from unconnected pin

    #if ENABLE_DEBUG_COMMS
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
    #endif

    return true;
}


bool SetupSMTimer()
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


void
ProcessShootingStates()
{
    // Determine if Idle or Shooting
    switch(buttonStates.GetSMState()) {
        case SHOOTING_ORANGE:
            buttonStates.SetOrangeFlagPressed(false);
            buttonStates.SetBlueFlagPressed(false);
            StateShooting(ORANGE_COLOR);
            break;
        case SHOOTING_BLUE:
            buttonStates.SetOrangeFlagPressed(false);
            buttonStates.SetBlueFlagPressed(false);
            StateShooting(BLUE_COLOR);
            break;
        case IDLE:
            buttonStates.UpdateSMState();
            IdlingStateColor();
            break;
        default:
            break;
    }
}


void
StateShooting(const ColorHSV& shootingColor)
{
    // Perform Flashing:
    static uint16_t flashTick = 0;

    // Reset counter if another portal color button is pressed
    if (!(leds.currentColor == shootingColor)) {
        flashTick = 0;
    }

    if(flashTick == 0) {
        String colorStr = shootingColor == BLUE_COLOR ? "Blue" : "Orange";
        debugPrintln("> Shot Portal: " + colorStr);
        // Play random shooting sound:
        #if ENABLE_SOUND
            if (shootingColor == BLUE_COLOR)
                PlayShootingSoundBlue();
            else
                PlayShootingSoundOrange();
        #endif
        flashTick++;
    }

    // Set flashing color
    ShootingStateColor(flashTick, shootingColor);
    // Check if Flash tick has reached NUM_FLASH_TICKS, which mean returning to idling state
    if (flashTick >= NUM_FLASH_TICKS) {
        flashTick = 0;
        buttonStates.sm_state = IDLE;
    }
}


void ProcessRotaryEncoderStates()
{
    rotPos.UpdateRotaryPosition();

    if(rotPos.IsEncoderPressed()) {
        rotPos.SetEncoderPressed(false);
        #if ENABLE_SOUND
            PlayEncoderSelectedSound(rotPos.adjustedPos);
        #endif
    }
}


void ReadBattery()
{
    if (ReadAndIndicateBattery() == BatteryStates::BATTERY_LOW) {
        #if ENABLE_SOUND
            PlayLowBatterySound();
        #endif
    }
}

//////////                                        FUNCTION DEFINITIONS                                        //////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
