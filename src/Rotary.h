#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <SoftwareSerial.h>
#include <RotaryEncoder.h>
#include "configuration.h"

////////////////////////////////////////////////////////////////////////////////////////
//////////                               ENUMS                                //////////

enum EncoderNum {
    UNKNOWN = 0, // Default position, do nothing
    POS_1   = 1, // If pressed, plays radio tune (sound 2)
    POS_2   = 2, // If pressed, plays either Cave's or GLaDOS's welcome (sound 3-4)
    POS_3   = 3, // If pressed, plays one of the 2 GLaDOS's cake lines (sound 9-10)
    POS_4   = 4, // If pressed, plays Cave's abestos line (sound 5)
    POS_5   = 5, // If pressed, plays a radom turret greeting (sound 12-15)
    POS_6   = 6, // If pressed, plays GLaDOS's Weight line (sound 11)
    POS_7   = 7, // If pressed, plays Cave's Mantis line (sound 7)
    POS_8   = 8, // If pressed, plays GLaDOS's bird line (sound 8)
    POS_9   = 9, // If pressed, plays Cave's Lemon rant (sound 6)
    POS_10  = 10 // If pressed, plays a radom turret greeting (sound 12-15) (same as POS_5)
};


////////////////////////////////////////////////////////////////////////////////////////
//////////                             CONSTANTS                              //////////

static constexpr int     ROTARYMIN   = 0;
static constexpr int     ROTARYMAX   = 10;


////////////////////////////////////////////////////////////////////////////////////////
//////////                             STRUCTURE                              //////////

struct RotaryPos {
    EncoderNum adjustedPos;
    int        lastPos;
    volatile bool encPressed;
    volatile bool rotationPending;

    // Constructor
    RotaryPos();

    // Struct methods
    bool UpdateRotaryPosition();
    bool IsEncoderPressed() const {
        return encPressed;
    }
    bool IsRotationPending() const {
        return rotationPending;
    }
    void SetEncoderPressed(bool pressed) {
        encPressed = pressed;
    }
    void SetRotationPending(bool pending) {
        rotationPending = pending;
    }
};

////////////////////////////////////////////////////////////////////////////////////////
//////////                              OBJECTS                               //////////

extern RotaryPos      rotPos;
extern RotaryEncoder  rotaryEnc;


////////////////////////////////////////////////////////////////////////////////////////
//////////                       FUNCTION PROTOTYPES                          //////////

void SetUpEncoder();

#endif // ROTARY_ENCODER_H
