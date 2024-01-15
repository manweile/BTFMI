# BTFMI

Bluetooth to FM Tranmsitter 

For audio streaming from Android mobile to FM radio

# Bill of Materials

| Item      | Count     | Link      | Part Number |
| --------- | --------- | --------- | ----------- |
| Arduino Mega2560 R3 | 1 | [Arduino Mega 2560 R3](https://www.sparkfun.com/products/11061) | DEV-11061 |
| Atmel-ICE Basic     | 1 | [Atmel-ICE Basic](https://www.digikey.ca/en/products/detail/microchip-technology/ATATMEL-ICE-BASIC/4753381?utm_adgroup=&utm_source=google&utm_medium=cpc&utm_campaign=PMax%20Product_Low%20ROAS%20Categories&utm_term=&productid=4753381&utm_content=&utm_id=go_cmp-20291741422_adg-_ad-__dev-c_ext-_prd-4753381_sig-EAIaIQobChMIvM6727jdgwMV7zutBh3bswzIEAQYASABEgI7UPD_BwE&gad_source=1&gclid=EAIaIQobChMIvM6727jdgwMV7zutBh3bswzIEAQYASABEgI7UPD_BwE) | ATATMEL-ICE-BASIC-ND |
| 2.8" TFT Touch Shield for Arduino with Capacitve Touch and microSd | 1 | [2.8" TFT Touch Shield](https://www.adafruit.com/product/1947) | 1947 |
| Adafruit Stereo FM Tranmitter with RDS/RDBS Breakout - Si4713 | 1 | [Adafruit Stereo FM Tranmitter](https://www.adafruit.com/product/1958) | 1958 |
| Sparkfun  Audio Bluetooth Breakout - RN-52 | 1 | [Sparkfun Audio Bluetooth Breakout](https://learn.sparkfun.com/tutorials/rn-52-bluetooth-hookup-guide#overview) | WRL-12849 |
| Sparkfun FTDI Basic Breakout - 3.3V | 1 | [Sparkfun FTDI Basic Breakout](https://www.sparkfun.com/products/9873?_gl=1*173untz*_ga*MTc1Mjg5MTM4NC4xNzAxMTA1MDY1*_ga_T369JS7J9N*MTcwNTI3MzMxNC4xNC4xLjE3MDUyNzQyMzcuNjAuMC4w&_ga=2.171073504.1571879566.1705185864-1752891384.1701105065) | DEV-09873 |
| Mini Pushbutton Switch | 7 | [Mini Pushbutton](https://www.sparkfun.com/products/97?_gl=1*36uoho*_ga*MTc1Mjg5MTM4NC4xNzAxMTA1MDY1*_ga_T369JS7J9N*MTcwNTE4NTg2My4xMS4wLjE3MDUxODU4NjMuNjAuMC4w&_ga=2.58816493.1571879566.1705185864-1752891384.1701105065) | COM-0097 |
| Mini Power Switch - SPDT | 1 | [Mini Power Switch](https://www.sparkfun.com/products/102?_gl=1*1s0kwn0*_ga*MTc1Mjg5MTM4NC4xNzAxMTA1MDY1*_ga_T369JS7J9N*MTcwNTE4NTg2My4xMS4xLjE3MDUxODY2OTMuNjAuMC4w&_ga=2.103970880.1571879566.1705185864-1752891384.1701105065) | COM-00102 |
| USB Mini-B Cable | 1 | [Sparkfun USB Minio-B Cable](https://www.sparkfun.com/products/11301?_gl=1*6zq9iw*_ga*MTc1Mjg5MTM4NC4xNzAxMTA1MDY1*_ga_T369JS7J9N*MTcwNTI3MzMxNC4xNC4xLjE3MDUyNzM4MTQuNjAuMC4w&_ga=2.170488672.1571879566.1705185864-1752891384.1701105065) | CAB-11301 |
| Adafruit Perma-Proto 1/2 Sized Breadboard | 2 | add link | add part number |
| Adafruit Perma-Proto 1/4 Sized Breadboard | 1 | add link | add part number |
| 10 uF Polarized Capacitor | 2 | add link | add part number |
| 68 kOhm resistor | 2 | add link | add part number |
| 18 kOhm resistor | 2 | add link | add part number |

**IMPORTANT ARDUINO IDE SETUP**

[Downgrade Arduino AVR Boards to 1.82](https://github.com/LubomirJagos/LabVIEW-Universal-Transcriptor/issues/3)

(use Arduino IDE tools>Board>Board Manager)

# Required Arduino Libraries:

## Pre-installed Arduino hardware libraries:

[SPI](https://www.arduino.cc/reference/en/language/functions/communication/spi/)

[Wire](https://www.arduino.cc/reference/en/language/functions/communication/wire/)

These libraries come installed with Arduino IDE, so all you have to do is refrence them in code.
```
#include <SPI.h>
#include <Wire.h>
```

## Third party function libraries to install:

[ArduinoSTL](https://www.arduino.cc/reference/en/libraries/arduinostl/)
v1.1.0

1. algorithm
2. vector

[PrintEx](https://www.arduino.cc/reference/en/libraries/printex/) v1.2.0

```
#include <algorithm>
#include <vector>
#include <PrintEx.h>
```

## Third party hardware & libraries to install:

[Adafruit_FT6206](https://www.arduino.cc/reference/en/libraries/adafruit-ft6206-library/) v1.03

[Adafruit_GFX](https://www.arduino.cc/reference/en/libraries/adafruit-gfx-library/) v1.5.7

[Adafruit_ILI9341](https://www.arduino.cc/reference/en/libraries/adafruit-ili9341/) v1.5.1

[Adafruit Stereo FM Tranmitter with RDS/RDBS Breakout - Si4713](https://www.arduino.cc/reference/en/libraries/adafruit-si4713-library/) v1.0.0

```
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include <Adafruit_Si4713.h>
```

# Hardware Details

## Arduino Mega 2560 R3

### Hardware Debugging

To hardware debug the Mega 2560 R3, 4 things are required:
1. Cut the reset-en pads  
screenshot here
2. JTAG .5mm pitch 2x5 connection  
Adafruit SWD (2x5 1.27mm) Cable Breakout Board ID: 2743  
screenshot here
3. Atmel-ICE
4. Microchip Studio 7

IPSUM LOREM

### Atmel-ICE Basic

IPSUM LOREM

## Sparkfun  Audio Bluetooth Breakout - RN-52

### Power
**First and foremost, the RN-52 is a 3.3V device.**
It can handle an input voltage of about 3.0 - 3.6V.
Voltages above or below this range can result in the module not working properly or, worse, damaging the module.
Make sure you select a power supply that can provide the correct amount of voltage to the device.

As usual, connect the GND pin to GND, and connect the 3.3V pin to the Vcc pin on the supply.
The PWR_EN pin can also be attached to a button to allow the user to power up the module when it is desired, even if power is already supplied.

This is useful in headset/hands-free applications where a battery is attached to the module but the module doesn't need to be on all the time.

### GPIO4

Restore Factory Defualts with GPIO4

You should connect the GPIO4 pin to a switch, jumper, or resistor so it can be accessed.
You can use this pin to reset the module to its factory default settings, which is critical in situations where the module has been mis-configured.

To reset the module to the factory defaults, GPIO4 should be high on power-up and then *toggle low, high, low, high with a 1 second wait between the transitions.*

### GPIO9

GPIO9 is used to tell the module to enter command mode.

If GPIO9 is HIGH or left floating, the module will remain in its default data mode, streaming audio or data.

In order to enter command mode, GPIO9 must be pulled LOW.
In this example, a switch is used to easily enter and exit command mode.  
Screenshot here

### UART
You will need a way to communicate to the module and send commands.
This will be accomplished with a 3.3V FTDI Basic. Connect GND to GND, TXO to UART_RX, and RXI to UART_TX.
Those are the only connections needed to talk to the module.

**Discovering your device**
1. Connect FTDI to your computer with usb mini-b cable  
*If this is the first time you've plugged in a device of this nature into your computer, you may need to install the drivers.*  
Follow this guide: [FTDI Driver Inatallation Guide](https://learn.sparkfun.com/tutorials/how-to-install-ftdi-drivers/all)
2. Open Windows Device Manager
3. Once you've got Device Manger open, expand the Ports tab
4. You should see "USB Serial Port (COM 9)"
5. Right click on "USB Serial Port"
6. Select Properties
7. Select Driver tab
8. The driver provider should be FTDI

*If you do have multiple devices and are not sure which device is the one you just plugged in, unplug it, watch for whichever COM port disappears, and then plug it back in. The COM port should reappear letting you know that's the device you're looking for.*

#### TeraTerm
Download: [Teraterm Project](https://github.com/TeraTermProject/teraterm/releases)

#### Configure Teraterm
[Teraterm Config](https://learn.sparkfun.com/tutorials/terminal-basics/all)

**Making a Connection**

You should initially be presented with a "TeraTerm: New connection" pop-up within the program. Here, you can select which serial port you'd like to open up.

*Select the "Serial" radio button.*

Then select your port from the drop-down menu.

*Select COM9: USB Serial Port*

If this window doesn't open when you start TeraTerm, you can get here by going to File > New connection.

That'll open up the port. TeraTerm defaults to setting the baud rate at 9600 bps (8-N-1). If you need to adjust the serial settings, go up to Setup > Serial Port.

- Port: COM9
- Speed 115200
- Data: 8 bit
- Parity: none
- Stop bits: 1
- Flow control: none

Adjust what you need to and hit "OK".

### Configure the RN-52 Module

Download and read the [Bluetooth Audio Module Command Reference User's Guide](https://cdn.sparkfun.com/assets/a/2/a/a/d/5217c61f757b7f55758b456f.pdf?_gl=1*122wpwx*_ga*MTc1Mjg5MTM4NC4xNzAxMTA1MDY1*_ga_T369JS7J9N*MTcwNTE4NTg2My4xMS4xLjE3MDUxODc4MTUuNjAuMC4w)

Now that you can actually power up the RN-52 and send it into command mode, let's talk about changing the settings, and thus the behavior, of the Bluetooth module.

1. Make sure the CMD Mode switch is in the OFF position (GPIO9 NOT shorted to GND).
2. Turn on your RN-52.
3. The two status LEDs should both stay solid for about 1 second and then begin flashing alternately.
4. Your device is now ready to make a connection. Rather than connect over Bluetooth, the connection will be made over the serial UART.
5. Open a terminal window on the port (COM 9) your RN-52 has been assigned (115200 Baud, 8,N,1).
6. With the terminal open and connected to the RN-52, flip the CMD Mode switch to the ON position (GPIO9 shorted to GND).
You should now see CMD appear in your terminal.

7. Enable AVRCP  
By default, the RN-52 does not have the AVRCP enabled. This is part of the extended features. When you type D to see the basic settings, you'll notice a line that says ExtFeatures=XX, where XX is some hex value.

8. Notice that bit 0 is the bit we need to enable to activate the AVRCP button functionality. Send the command S%,07(/r) to enable this bit while leaving the other two bits enabled. Then follow it up with a reboot -- R,1(/r). You should now have AVRCP enabled. Type D to double check the settings.

9. To exit command mode, simply flip the switch back to the OFF position. You will see END appear in the window.

#### Connect and Play

You are now ready to connect to your Bluetooth device and start streaming some music. Directions on how to pair and connect to the RN-52 can also be found in section 1.4 of the user guide.

IPSUM LOREM from Secvtion 1.4

### Adafruit Stereo FM Tranmitter with RDS/RDBS Breakout - Si4713

IPSUM LOREM

### 2.8" TFT Touch Shield for Arduino with Capacitve Touch and microSd

IPSUM LOREM
