# komoot-navi-esp-idf

## Overview
This project uses [Komoot Bluetooth Connect](https://www.komoot.com/b2b/connect#bleconnect) to display cycling navigation on a display. So far I am targeting TTGO LoRa v2.0 board but I am planning to switch to a board that doesn't have LoRa but this will work with any ESP32 which has an SSD1306 128x64 display (either I2C or SPI).

## Configuration
To configure the display go to menuconfig option "SSD1306 Configuration" and set the correct display interface and pins. To pair with the phone turn on the ESP32 go to Komoot Settings > Bluetooth Connect > Ciclo HAC bike computer. A pairing request prompt should appear and after confirming you will be prompted again to type in the paring code which will apprear on the ESP32 display.

## Installation
The installation is done via Platformio if you don't have it installed as an Atom or VS Code plugin install the [CLI version](https://platformio.org/install/cli) and run `platformio run --target upload`.