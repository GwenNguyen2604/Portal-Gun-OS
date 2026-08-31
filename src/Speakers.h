// Speaker handling for DFPlayer Mini (kept inline-safe)
#ifndef SPEAKERS_H
#define SPEAKERS_H

#include <DFPlayerMini_Fast.h>
#include "configuration.h"
#include "Rotary.h"

#if ENABLE_SOUND

////////////////////////////////////////////////////////////////////////////////////////
//////////                               ENUMS                                //////////

enum Speaker1SoundFiles {   // Soundfiles for Speaker 1
    IDLE_AMBIENT     = 1,
    RADIO_TUNE       = 2,
    CAVE_WELCOME     = 3,
    GLADOS_WELCOME   = 4,
    CAVE_ABESTOS     = 5,
    CAVE_LEMON       = 6,
    CAVE_MANTIS      = 7,
    GLADOS_BIRD      = 8,
    GLADOS_CAKE_1    = 9,
    GLADOS_CAKE_2    = 10,
    GLADOS_WEIGHT    = 11,
    TURRET_1         = 12,
    TURRET_2         = 13,
    TURRET_3         = 14,
    TURRET_4         = 15,
};

enum Speaker2SoundFiles {  // Soundfiles for Speaker 2
    ORANGE_1      = 1,
    ORANGE_2      = 2,
    ORANGE_3      = 3,
    ORANGE_4      = 4,
    BLUE_1        = 5,
    BLUE_2        = 6,
    BLUE_3        = 7,
    BLUE_4        = 8,
    SPEAKER_1     = 9,
    SPEAKER_2     = 10,
    HUE           = 11,
    SATURATION    = 12,
    BRIGHTNESS    = 13,
    CONFIRM       = 14,
    ERROR         = 15,
    CLICK         = 16,
    ACTIVATION    = 17,
    LOW_BATTERY   = 18,
};


////////////////////////////////////////////////////////////////////////////////////////
//////////                             CONSTANTS                              //////////

// Array that stores sound files' lengths in ms, covering, in order, the
// speaker1soundfiles from RADIO_TUNE(2) to TURRET_4(15). Each index in the
// array correspond to the enum - 2
static const uint32_t SoundFile1LengthInMilli[14] = {
    45100, 10700, 6000,  // RADIO_TUNE,    CAVE_WELCOME,  GLADOS_WELCOME
    11500, 32400, 19300, // CAVE_ABESTOS,  CAVE_LEMON,    CAVE_MANTIS
    2500,  6600,  600,   // GLADOS_BIRD,   GLADOS_CAKE_1, GLADOS_CAKE_2
    9000,  600,   1600,  // GLADOS_WEIGHT, TURRET_1,      TURRET_2
    900,   2500          // TURRET_3,      TURRET_4
};

static const size_t SoundFile1LengthArrSize = sizeof(SoundFile1LengthInMilli) / sizeof(SoundFile1LengthInMilli[0]);

////////////////////////////////////////////////////////////////////////////////////////
//////////                             STRUCTURE                              //////////

// Struct for the speaker and its corresponding DfPlayer:
struct Speaker {
    EspSoftwareSerial::UART SoftSerial;
    DFPlayerMini_Fast       dfPlayer;
    uint8_t                 volumePlayer;
    uint8_t                 speakerNum;
    uint32_t                playingEndTime;  // sound length (ms) + time of playing
    bool                    isIdle;
    bool                    idleLoopStarted;

    // Constructor
    Speaker(uint8_t rxPin, uint8_t txPin, uint8_t volumePlayer, uint8_t speakerNum) :
        SoftSerial(rxPin, txPin),
        volumePlayer(volumePlayer),
        speakerNum(speakerNum),
        playingEndTime(0),
        isIdle(true),
        idleLoopStarted(false)
    {
    }

    // Struct methods
    void FadeOut(uint16_t durationMs); // Fade current sound out
    void StopAndFlush(uint16_t settleMs = 200, bool restoreVolume = true); // Stop playback and clear pending serial responses
    void PlaySound(uint8_t soundFile, bool isLooping = false, uint16_t bufferGapMs = 200); // Play a sound
    void UpdateIdlingState(); // Check speaker if it is not playing anything
};

////////////////////////////////////////////////////////////////////////////////////////
//////////                              OBJECTS                               //////////

extern Speaker speaker1;
extern Speaker speaker2;


////////////////////////////////////////////////////////////////////////////////////////
//////////                       FUNCTION PROTOTYPES                          //////////

// Set up speaker1 and speaker 2 in the setup loop
bool SetupSpeakers();
// Check speakers for idling states. speaker1 plays the idling loop if idles
void HandleSpeakersIdlingStates();
// Play shooting for blue and orange portals (player 2)
void PlayShootingSoundBlue();
void PlayShootingSoundOrange();
// Play sound effect noted by current encoder's position (player 1)
void PlayEncoderSelectedSound(EncoderNum encNum);
// Play warning low battery (player 2)
void PlayLowBatterySound();

#endif // ENABLE_SOUND
#endif // SPEAKERS_H
