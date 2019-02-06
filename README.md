# Env-Module Firmware
This repo contains the firmware for the [Environmental board](https://github.com/IRNAS/environmental-module-firmware). 

To update your boards, [env_firmware.ino](https://github.com/IRNAS/environmental-module-firmware/blob/master/env_firmware/env_firmware.ino) and depending on your module change the following:

- in env_firmware.ino uncomment one of the following lines, based on the chosen module:
```
#define TOP1
//#define MID2
//#define MID3
//#define BOTTOM4
```
Example: for device with number 1, uncomment line #define TOP1 and comment all the others

- in the [can_module.h](https://github.com/IRNAS/environmental-module-firmware/blob/master/env_firmware/can_module.h) change the number for ```SET_CAN_ID``` accordingly: 
```
#define SET_CAN_ID 0x100
```
Example: for module 1, set ```#define SET_CAN_ID 0x100```, for module 2 set it to ```#define SET_CAN_ID 0x200```.


Before uploading select the IRNAS-env-module-L072Z board under the Tlera Corp STM32L0 Boards.


## Hardware Support
- MCU:  CMWX1ZZABZ-091 LoRa®/Sigfox™ module (Murata)
- CAN: MCP25625 (integrated transceiver)
- Multiple Feather compatible sensors

## Software requirements and board definitions
Add https://irnas.github.io/ArduinoCore-stm32l0/package_stm32l0_boards_index.json in the ArduinoIDE under File->Preferences->Additional Boards Manager URLs.

_**Note:** if you had added any previous STM32L0 board definitions, such as https://github.com/GrumpyOldPizza/ArduinoCore-stm32l0/, you need to remove them to avoid double referencing._ 

After you added Boards Manager URL, install Tlera Corp STM32L0 Boards, version 0.0.11 via the Boards Manager in ArduinoIDE. Afterwords you will be able to select IRNAS-env-module-L072Z board under the Tlera Corp STM32L0 Boards.

