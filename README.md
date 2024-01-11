# BTFMI

## Bluetooth to FM tranmsitter
For audio streaming from Android mobile to FM radio

## Required Libraries:
### Arduino hardware libraries:
SPI 
https://www.arduino.cc/reference/en/language/functions/communication/spi/
Wire 
https://www.arduino.cc/reference/en/language/functions/communication/wire/

### Third party function libraries:
NB: per https://github.com/LubomirJagos/LabVIEW-Universal-Transcriptor/issues/3
Downgrade Arduino AVR Boards to 1.82 (use Arduino IDE tools>Board>Board Manager)
ArduinoSTL
https://www.arduino.cc/reference/en/libraries/arduinostl/
v1.1.0
algorithm
vector

PrintEx
https://www.arduino.cc/reference/en/libraries/printex/
v1.2.0

### Third party hardware & libraries:
2.8" TFT Touch Shield for Arduino with Capacitve Touch and microSd 
https://www.adafruit.com/product/1947

Adafruit_FT6206		
https://www.arduino.cc/reference/en/libraries/adafruit-ft6206-library/
v1.03

Adafruit_GFX
https://www.arduino.cc/reference/en/libraries/adafruit-gfx-library/
v1.5.7

Adafruit_ILI9341
https://www.arduino.cc/reference/en/libraries/adafruit-ili9341/
v1.5.1

Adafruit Stereo FM Tranmitter with RDS/RDBS Breakout - Si4713
https://www.adafruit.com/product/1958
https://www.arduino.cc/reference/en/libraries/adafruit-si4713-library/
v1.0.0

Sparkfun  Audio Bluetooth Breakout - RN-52
https://www.sparkfun.com/products/retired/12849
https://learn.sparkfun.com/tutorials/rn-52-bluetooth-hookup-guide#overview

## Power
First and foremost, the RN-52 is a 3.3V device.
It can handle an input voltage of about 3.0 - 3.6V.
Voltages above or below this range can result in the module not working properly or, worse, damaging the module.
Make sure you select a power supply that can provide the correct amount of voltage to the device.
As usual, connect the GND pin to GND, and connect the 3.3V pin to the Vcc pin on the supply.
The PWR_EN pin can also be attached to a button to allow the user to power up the module when it is desired, even if power is already supplied.
This is useful in headset/hands-free applications where a battery is attached to the module but the module doesn't need to be on all the time.

Screenshot here

## GPIO4
Restore Factory Defualts with GPIO4
You should connect the GPIO4 pin to a switch, jumper, or resistor so it can be accessed.
You can use this pin to reset the module to its factory default settings, which is critical in situations where the module has been mis-configured.
To reset the module to the factory defaults, GPIO4 should be high on power-up and then toggle low, high, low, high with a 1 second wait between the transitions.

Screenshot here

## GPIO9
GPIO9 is used to tell the module to enter command mode.
If GPIO9 is HIGH or left floating, the module will remain in its default data mode, streaming audio or data.
In order to enter command mode, GPIO9 must be pulled LOW.
This can be accomplished by simply connecting GPIO9 to GND with a jumper wire.
In this example, a switch is used to easily enter and exit command mode.

Screenshot here

## UART
You will need a way to communicate to the module and send commands.
This will be accomplished with a 3.3V FTDI Basic. Connect GND to GND, TXO to UART_RX, and RXI to UART_TX.
Those are the only connections needed to talk to the module.

Screenshot here

## Configure the Module
Now that you can actually power up the RN-52 and send it into command mode, let's talk about changing the settings, and thus the behavior, of the Bluetooth module.

Make sure the CMD Mode switch is in the OFF position (GPIO9 NOT shorted to GND).
Turn on your RN-52.
The two status LEDs should both stay solid for about 1 second and then begin flashing alternately.
Your device is now ready to make a connection. Rather than connect over Bluetooth, the connection will be made over the serial UART.
Open a terminal window on the port your RN-52 has been assigned (115200 Baud, 8,N,1).
With the terminal open and connected to the RN-52, flip the CMD Mode switch to the ON position (GPIO9 shorted to GND).
You should now see CMD appear in your terminal.