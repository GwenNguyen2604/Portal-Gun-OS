# Summary

This project is an improved version of EVARATE's [Portal Gun OS Version 3.0](https://github.com/EVARATE/Portal-Gun-OS/tree/Version-3.0). It is the software that runs the Portal Gun prop, controlling the lights, shooting buttons, speakers, battery indicator, and rotary encoder.

Full build instructions are still a work in progress; use the original project and older versions as hardware references for now.

**You can find all 3D files as well as an assembly [here](https://cad.onshape.com/documents/e9792b9eb487303a96ad3879/w/b553b2e39ecd2aecf96e9de1/e/bdf311fa5cd6293295744bfa?renderMode=0&uiState=67f5a42bb2acff2e70fb3628).**

## Added functionality

- The rotary encoder now does something useful. Turning it selects a voice-line slot, and pressing it plays the selected Portal-themed voice line.
- Speaker1 now handles the constant ambient hum and voice lines. Speaker2 still handles shooting, startup, and warning sounds.
- The ambient hum comes back automatically after a voice line finishes.
- Startup sound behavior is more reliable, including the ambient loop and activation sound.
- Voice lines have their own volume setting, separate from the normal ambient and sound-effect volumes. All sound effects can be configured in configuration.h
- Tweaked Battery feedback.
- Tweaked flickering light behavior, including idle flicker and the blue/orange shooting flash.
- The code was reorganized so each major feature is easier to maintain: lights, sound, buttons, rotary encoder, and batteries.

## Current status

This version has been tested on the physical Portal Gun. Shooting sounds, rotary-selected voice lines, and ambient-loop recovery are currently working.
