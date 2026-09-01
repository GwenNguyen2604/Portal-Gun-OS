# Summary

This project is an improved version of EVARATE's [Portal Gun OS Version 3.0](https://github.com/EVARATE/Portal-Gun-OS/tree/Version-3.0). It is the software that runs the Portal Gun prop, controlling the lights, shooting buttons, speakers, battery indicator, and rotary encoder.

Full build instructions are still a work in progress as per EVARATE's page; use the original project and older versions, as well as provided pdf for circuit and hardware references for now.

**You can find all 3D files as well as an assembly [here](https://cad.onshape.com/documents/e9792b9eb487303a96ad3879/w/b553b2e39ecd2aecf96e9de1/e/bdf311fa5cd6293295744bfa?renderMode=0&uiState=67f5a42bb2acff2e70fb3628). Credits go to EVARATE**


## Added functionalities and changes

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


## Part list for V3

- The part list is similar to that of V2, with some changes. Below is the list of parts that I used to assemble the Portal Gun, plus where to find them

### Electronics Parts

| Amount | Part Number | Part | Info |
|---|---|---|---|
| 1 | 201 | [USB-C Breakout Box ](https://www.adafruit.com/product/4090) |  |
| 2 | 202 | [PowerBoost 500 Charger](https://www.adafruit.com/product/1944) | *Use a PowerBoost 1000 Charger if you want faster charging. Works identical and allowes higher current.* |
| 2 | 203 | [3.7V LiPo Battery](https://www.adafruit.com/product/353) | Must be rated for at least 1500mA discharge rate! |
| 1 | 204 | [Arduino ESP32 Board](https://www.digikey.com/short/pr0z2b77) | Preferrably an ESP32 board. Other 5V boards may work but will need code and pin changes. |
| 9 | 205 | [WS2812B RGB LED](https://a.co/d/0cML1SAu) |  |
| 1 | 206 | [5V RGB LED Ring (WS2812B)](https://www.digikey.com/short/fj5nv5nm) |  |
| 1 | 207 | [5V RGB LED Module (WS2812B)](https://www.digikey.com/short/vnv4wrpf) |  |
| 1 | 208 | [5mm RGB LED](https://www.digikey.com/short/z7jfjdcw) |  |
| 2 | 209 | [DFPlayer](https://www.digikey.com/short/c5787bj8) | Audio module |
| 2 | 210 | [MicroSD Card](https://www.digikey.com/short/qjf5t299) | For DF Player |
| 1 | 211 | [Bi-Directional Logic Converter](https://www.digikey.com/short/q7p3jhnc) |  |
| 1 | 212 | [3W, 3" diameter Speaker](https://www.digikey.com/short/2tb3nhnw) | Keep the speakers between 3-5W, 4-8Oh, has to be 3" |
| 1 | 213 | [3W, 2" diameter Speaker](https://www.digikey.com/short/4z3f23hh) | Keep the speakers between 3-5W, 4-8Oh, has to be 2" |
| 1 | 214 | [Male Micro-USB connector](https://a.co/d/04QRruIk) | To connect the DevKit to the USB-C Breakout Board |
| 1 | 215.1 | [16mm Momentary Push Button - Blue](https://www.digikey.com/short/13vpp27h) | You can get 2 with the same color. The ones I'm using are actually slightly bigger than the opening holes on the 3D model, so modifies will be needed |
| 1 | 215.2 | [16mm Momentary Push Button - Red](https://www.digikey.com/short/z4qhvmb8) |  |
| 1 | 216 | [3PDT ON/ON Toggle Switch](https://a.co/d/02wVgcIb) |  |
| 1 | 217 | [KY-040 Rotary Module](https://a.co/d/0guLndFy) |  |
| 1 | 218 | [Colored Wires](https://www.digikey.com/short/b15v0mwf) | Must be rated for at least 1A. Anything similar works here obviously |
| 2 | 219.1 | [150Ohm Resistor](https://www.digikey.com/short/dcm10drh) | Resistance needs for the RGB LED |
| 2 | 219.2 | [4.3kOhm Resistor](https://www.digikey.com/short/5dt18jqm) | Resistance needs for the VN and VP pins connected to the batteries |
| ~2 | 220.1 | [100uF Capacitors](https://www.digikey.com/short/b3zp4zqb) | Accross power rail |
| ~5 | 220.2 | [10uF Capacitors](https://www.digikey.com/short/3n3crz35) | Accross ESP32 VIN and GND |
| ~5 | 220.3 | [0.1uF Capacitors](https://www.digikey.com/short/n8mnrvb4) | Accross ESP32 VIN and GND, for the DFPlayers |
| 2 | 220.4 | [1mF Capacitors]() | For the DFPlayers |
| 2 | 221 | [PCB Solder Board](https://www.digikey.com/short/vf18zdq0) |  |

### Other Parts
Consult EVARATE's part list [here](https://www.thingiverse.com/thing:6191828). I found V3 to have a similar part list as V2, albeit with more screws needed. The number of screws can be examined in the [model file here](https://cad.onshape.com/documents/e9792b9eb487303a96ad3879/w/b553b2e39ecd2aecf96e9de1/e/bdf311fa5cd6293295744bfa?renderMode=0&uiState=67f5a42bb2acff2e70fb3628).


## Current status

This version has been tested on the physical Portal Gun. Shooting sounds, rotary-selected voice lines, and ambient-loop recovery are currently working.


## Known Issue
- Sometimes the voiceline may not play after clicking.
- Due to the short elapse time of the Turret voiceline, playing it can make it loop nonstop at times. Fix this by playing another voiceline or rebooting.
