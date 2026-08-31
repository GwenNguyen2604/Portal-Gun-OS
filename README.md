# Summary

This project is an improved version of EVARATE's [Portal Gun OS Version 3.0](https://github.com/EVARATE/Portal-Gun-OS/tree/Version-3.0). It is the software that runs the Portal Gun prop, controlling the lights, shooting buttons, speakers, battery indicator, and rotary encoder.

Full build instructions are still a work in progress as per EVARATE's page; use the original project and older versions, as well as provided pdf for circuit and hardware references for now.

**You can find all 3D files as well as an assembly [here](https://cad.onshape.com/documents/e9792b9eb487303a96ad3879/w/b553b2e39ecd2aecf96e9de1/e/bdf311fa5cd6293295744bfa?renderMode=0&uiState=67f5a42bb2acff2e70fb3628).**


## Added functionalities and changes compared to EVARATE's portal gun

- The rotary encoder now does something useful. Turning it selects a voice-line slot, and pressing it plays the selected Portal-themed voice line. Voicelines played are listed below
    - Rotary position 1 - If pressed, plays radio tune (sound 2)
    - Rotary position 2 - If pressed, plays either Cave's or GLaDOS's welcome (sound 3-4)
    - Rotary position 3 - If pressed, plays one of the 2 GLaDOS's cake lines (sound 9-10)
    - Rotary position 4 - If pressed, plays Cave's abestos line (sound 5)
    - Rotary position 5 - If pressed, plays a radom turret greeting (sound 12-15)
    - Rotary position 6 - If pressed, plays GLaDOS's Weight line (sound 11)
    - Rotary position 7 - If pressed, plays Cave's Mantis line (sound 7)
    - Rotary position 8 - If pressed, plays GLaDOS's bird line (sound 8)
    - Rotary position 9 - If pressed, plays Cave's Lemon rant (sound 6)
    - Rotary position 10 - If pressed, plays a radom turret greeting (sound 12-15) (same as POS_5)

- Speaker1 now handles the constant ambient hum and voice lines. Speaker2 still handles shooting, startup, and warning sounds.
- The ambient hum comes back automatically after a voice line finishes.
- Startup sound behavior is more reliable, including the ambient loop and activation sound.
- Voice lines have their own volume setting, separate from the normal ambient and sound-effect volumes. All sound effects can be configured in configuration.h
- Tweaked Battery feedback.
- Tweaked flickering light behavior, including idle flicker and the blue/orange shooting flash.
- The code was reorganized so each major feature is easier to maintain: lights, sound, buttons, rotary encoder, and batteries.

## Current status

This version has been tested on the physical Portal Gun. Shooting sounds, rotary-selected voice lines, and ambient-loop recovery are currently working.

## Known Issue
- Sometimes the voiceline may not play after clicking.
- Due to the short elapse time of the Turret voiceline, playing it can make it loop nonstop at times. Fix this by playing another voiceline or rebooting.
