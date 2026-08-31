#include "Rotary.h"

////////////////////////////////////////////////////////////////////////////////////////
//////////                        OBJECTS DEFINITION                          //////////

RotaryPos      rotPos;
RotaryEncoder  rotaryEnc(PIN_ROTENC_DT, PIN_ROTENC_CLK,
                         RotaryEncoder::LatchMode::TWO03);

////////////////////////////////////////////////////////////////////////////////////////
//////////                       FUNCTION DEFINITIONS                         //////////

void
SetUpEncoder()
{
    // Setup Rotary Encoder:
    debugPrint("> Setup Pins for Rotary Encoder... ");
    rotaryEnc.setPosition(ROTARYMIN);
    rotPos.lastPos = ROTARYMIN;
    rotPos.adjustedPos = EncoderNum::UNKNOWN;

    pinMode(PIN_ROTENC_CLK,    INPUT_PULLUP);
    pinMode(PIN_ROTENC_DT,     INPUT_PULLUP);
    pinMode(PIN_ROTENC_SW,     INPUT_PULLUP);

    debugPrintln("Done.");
}


////////////////////////////////////////////////////////////////////////////////////////
//////////                        METHODS DEFINITIONS                         //////////

RotaryPos::RotaryPos() :
    adjustedPos(EncoderNum::UNKNOWN),
    lastPos(0),
    encPressed(false),
    rotationPending(false)
{
}

bool
RotaryPos::UpdateRotaryPosition()
{
    if (!rotationPending) {
        return false;
    }

    rotationPending = false;
    bool isPosChanged = false;

    // get the current physical position and calc the logical position
    int newPos = rotaryEnc.getPosition();
    // Wrap around
    newPos = (newPos < ROTARYMIN) ? ROTARYMAX :
                                    ((newPos > ROTARYMAX) ? ROTARYMIN : newPos);

    rotaryEnc.setPosition(newPos);

    if (lastPos != newPos) {
        lastPos = newPos;
        // Ensure position is within valid enum range
        if (lastPos >= UNKNOWN && lastPos <= POS_10) {
            adjustedPos = static_cast<EncoderNum>(lastPos);
            isPosChanged = true;
        }
    }
    return isPosChanged;
}
