#include <SoftwareSerial.h>

#include "configuration.h"
#include "ShootingButtons.h"

////////////////////////////////////////////////////////////////////////////////////////
//////////                        OBJECTS DEFINITION                          //////////

ButtonStates buttonStates;

////////////////////////////////////////////////////////////////////////////////////////
//////////                       FUNCTION DEFINITIONS                         //////////

bool
SetupButtons()
{
    debugPrint("> Setup Pins for Buttons... ");
    // Input Buttons:
    pinMode(PIN_BUTTON_ORANGE, INPUT_PULLUP);
    pinMode(PIN_BUTTON_BLUE,   INPUT_PULLUP);

    debugPrintln("Done.");
    return true;
}


////////////////////////////////////////////////////////////////////////////////////////
//////////                        METHODS DEFINITIONS                         //////////

void
ButtonStates::UpdateSMState()
{
    // Check for Shot-button press:
    if(flagOrangePressed) {
        sm_state = SHOOTING_ORANGE;
        flagOrangePressed = false;
        flagBluePressed = false;
    }
    else if(flagBluePressed) {
        sm_state = SHOOTING_BLUE;
        flagBluePressed = false;
    }
    else {
        sm_state = IDLE;
    }
}
