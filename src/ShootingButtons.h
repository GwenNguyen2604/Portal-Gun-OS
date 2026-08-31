#ifndef SHOOTING_BUTTONS_H
#define SHOOTING_BUTTONS_H

////////////////////////////////////////////////////////////////////////////////////////
//////////                               ENUMS                                //////////

enum SM_STATE
{
  IDLE,
  SHOOTING_ORANGE,
  SHOOTING_BLUE
};


////////////////////////////////////////////////////////////////////////////////////////
//////////                             STRUCTURE                              //////////

struct ButtonStates {
    SM_STATE sm_state;
    volatile bool flagOrangePressed;
    volatile bool flagBluePressed;

    // Constructor
    ButtonStates() :
        sm_state(IDLE),
        flagOrangePressed(false),
        flagBluePressed(false)
    {}

    // Struct methods
    void SetOrangeFlagPressed(bool isPressed) {
        flagOrangePressed = isPressed;
    }
    void SetBlueFlagPressed(bool isPressed) {
        flagBluePressed = isPressed;
    }
    bool GetOrangeFlag() {
        return flagOrangePressed;
    }
    bool GetBlueFlag() {
        return flagBluePressed;
    }
    SM_STATE GetSMState() {
        return sm_state;
    }
    void UpdateSMState();
};

////////////////////////////////////////////////////////////////////////////////////////
//////////                              OBJECT                                //////////

extern ButtonStates buttonStates;


////////////////////////////////////////////////////////////////////////////////////////
//////////                       FUNCTION PROTOTYPES                          //////////

bool SetupButtons();

#endif // SHOOTING_BUTTONS_H