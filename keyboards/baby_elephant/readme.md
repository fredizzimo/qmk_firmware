# baby_elephant

![baby_elephant](imgur.com image replace me!)

*A short description of the keyboard/project*

* Keyboard Maintainer: [Fred Sundvik](https://github.com/Fred Sundvik)
* Hardware Supported: *The PCBs, controllers supported*
* Hardware Availability: *Links to where you can find this hardware*

Make example for this keyboard (after setting up your build environment):

    make baby_elephant:default

Flashing example for this keyboard:

    make baby_elephant:default:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (usually the top left key or Escape) and plug in the keyboard
* **Physical reset button**: Briefly press the button on the back of the PCB - some may have pads you must short instead
* **Keycode in layout**: Press the key mapped to `RESET` if it is available


## Connections
Left Side
PA9  - D1 - USART 1 TX
PA10 - D0 - USART 1 RX
NRST - NRST
GND  - GND
PA12 - D2 - USB DP
PB0  - D3   COL1
PB7  - D4 - COL2
PB6  - D5 - COL3
PB1  - D6 - COL4
PC14 - D7 - Oscillator
PC15 - D8 - Oscillator
PA8  - D9
PA11 - D10 - USB DM
PB5  - D11 - COL5
PB4  - D12 - COL6

Right Side
VIN  - VIN
GND  - GND
NRST - NRST
+5V  - +5V
PA2  - A7 - VCP TX (USART 2)
PA7  - A6 - ROW1
PA6  - A5 - ROW2
PA5  - A4 - ROW3
PA4  - A3 - THUMB
PA3  - A2
PA1  - A1
PA0  - A0
AREF - AREF
+3v3 - +3v3
PB3  - PB3

