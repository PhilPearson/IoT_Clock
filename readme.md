# NTP Clock

### Parts:

The clock is made up of two simple items, a 4x1 MAX7219 LED lattice module [Aliexpress](https://www.aliexpress.com/item/32907492270.html) and an ESP32 Development board [Aliexpress](https://www.aliexpress.com/item/1005006154020092.html) (be sure to pick the usb-c 30 pin option).

This case on [Thingiverse](https://www.thingiverse.com/thing:2885225) suits the above kit.

Note that you will also need a 5v USB power supply and cable.

### Arduino Studio

Grab yourself a copy of [Arduino Studio](https://www.arduino.cc/en/software/).

Using the boards manager, install support for the esp32 by Espressif Systems.

Using the library manager, install the MD_Parola library by majicDesigns (be sure to install all dependancies)

### To compile:

1. create a new file called `secrets.h` as per below, update the ssid, pass and hostname as required

```
#pragma once

#define WIFI_SSID "my_ssid"
#define WIFI_PASS "my_ssid_pass"
#define WIFI_HOSTNAME "IoT Clock_"
```

2. Open the project in [Arduino Studio](https://www.arduino.cc/en/software/). Select "ESP32 Dev Board" from the list of boards and the correct comms port (will show up when the board is plugged into your pc).

3. Press the upload button to compile and send the board or just Verify to compile

## Wiring

Connect VCC and GND of the MAX7219 to the 5v and GND pins on the ESP32

CLK connects to GPIO18, DIN connects to GPIO23 and CS connects to GPIO5
