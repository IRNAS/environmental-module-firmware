# Env-Module Firmware
This repo contains the firmware for the [Environmental board](https://github.com/IRNAS/environmental-module-hardware).

## Hardware Support
- MCU:  CMWX1ZZABZ-091 LoRa®/Sigfox™ module (Murata)
- CAN: MCP25625 (integrated transiver)
- Multiple Feather compatible sensors

## Software requirements
This core https://github.com/GrumpyOldPizza/ArduinoCore-stm32l0/ is required with a custom core definition to match the pin numbers to the pins or adjust accordingly for the selected board.

Note the custom board definition is not yet published.

