/***************************************************
  This is an example for the Si4713 FM Radio Transmitter with RDS

  Designed specifically to work with the Si4713 breakout in the
  adafruit shop
  ----> https://www.adafruit.com/products/1958

  These transmitters use I2C to communicate, plus reset pin.
  3 pins are required to interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution

  Many thx to https://github.com/phrm/fmtx/blob/master/firmware/firmware.ino !

 ****************************************************/

//arduino libraries
#include <ArduinoSTL.h>
#include <algorithm>
#include <vector>
#include <SPI.h>
#include <Wire.h>

//third party function libraries
#include <PrintEx.h>

//third party hardware libraries
#include <Adafruit_Si4713.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>

// communications definitions
#define SERIAL_BAUD 115200		// use default upload speed for convenience
#define RN52_BAUD 38400			// per BAL docs, best baud rate for RN52

//fm definitions
#define RESETPIN 22				// pin to use on mega 2560 for Si4713
#define FMSTATION 8770			// default station 8770 == 87.70 MHz
#define MAX_FREQ 10790			// upper end of FM band
#define MIN_FREQ 8770			// lower end of FM band
#define TX_POWER 115			// dBuV, 88-115 max
#define BROADCAST_LEVEL 60		// noise level for broadcasting stations

//normal operation & debugging flags
#define USE_AVAILABLE false
#define DEBUG_ON true

//capacitive touch definitions
#define SD_CS 4
#define TFT_CS 10
#define TFT_DC 9

#define FRAME_X 10
#define FRAME_Y 10
#define FRAME_W 100
#define FRAME_H 50

// @TODO just for reference now, delete once direct control buttons are set up
//#define REDBUTTON_X FRAME_X
//#define REDBUTTON_Y FRAME_Y
//#define REDBUTTON_W (FRAME_W/2)
//#define REDBUTTON_H FRAME_H
//
//#define GREENBUTTON_X (REDBUTTON_X + REDBUTTON_W)
//#define GREENBUTTON_Y FRAME_Y
//#define GREENBUTTON_W (FRAME_W/2)
//#define GREENBUTTON_H FRAME_H

Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

Adafruit_FT6206 ts = Adafruit_FT6206();
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// @TODO just for reference now, delete once direct control buttons are set up
//boolean recordOn = false;

uint16_t prevFreq;
uint8_t prevdBuV, prevAntCap, prevNoiseLevel, prevASQ;
int8_t prevInLevel;

void setup() {
	int freq;
	int station = FMSTATION;

	Serial.begin(SERIAL_BAUD);

	// start the cap screen
	tft.begin();

	// error and exit
	if (!ts.begin(40)) {
		// serial 'cause tft screen failed
		Serial.println("Unable to start touchscreen");
		delay(500);
		exit(1);
	} else {
		// config tft screen
		tft.setRotation(1);
		tft.fillScreen(ILI9341_BLACK);
		tft.setCursor(0, 0);
		tft.setTextColor(ILI9341_WHITE);
		tft.setTextSize(3);
	}

	// Banner text
	tft.println("GAM Radio");

	// reset for screen space saving
	tft.setTextSize(1);

	// begin with address 0x63 (CS high default)
	if (!radio.begin()) {
		tft.println("Couldn't find Si4713 transceiver");
		delay(500);
		exit(2);
	}

	if(DEBUG_ON) {
		Serial.println("Scanning for broadcasting stations ...");
		for (freq = MIN_FREQ; freq <= MAX_FREQ; freq += 20) {
			radio.readTuneMeasure(freq);
			radio.readTuneStatus();
			if( radio.currNoiseLevel >= BROADCAST_LEVEL) {
				Serial.print("Broadcast at "); Serial.print(freq/100.00); Serial.print(" Mhz, Current Noise Level: "); Serial.print(radio.currNoiseLevel);
				Serial.println();
			}
		}
	}

	// normal operation
	if(USE_AVAILABLE) {
		// @TODO setup scan frequencies button

		// @TODO offset text to right for button before printing following
		tft.println("Scanning for available frequencies ...");
		station = availableChannels(BROADCAST_LEVEL, FMSTATION, MIN_FREQ, MAX_FREQ);

		// @TODO clear "Scanning for available frequencies ... " text before printing following
		tft.print("\nTuning into frequency "); tft.print(station/100.00); tft.print(" Mhz");
		tft.println();
	}
	else {
		// scan button non-extant if USE_AVAILABE is false and no need to offset text
		tft.print("\nTuning into default frequency "); tft.print(FMSTATION/100.00); tft.print(" Mhz");
		tft.println();
	}

	/*
	readTuneMeasure(freq) in avalaibleChannels enters receive mode (disables transmitter output power)
	so have to call setTxPower after avalaibleChannels called
	*/
	radio.setTXpower(TX_POWER);
	radio.tuneFM(station);

	// This will tell you the status in case you want to read it from the chip
	radio.readTuneStatus();
	prevAntCap = radio.currAntCap;
	// @TODO prob don't need prevFreq and prevdBuv
	prevFreq = radio.currFreq/100.00;
	prevdBuV = radio.currdBuV;

	radio.readASQ();
	prevASQ = radio.currASQ;
	prevInLevel = radio.currInLevel;

	// set GP1 and GP2 to output
	//radio.setGPIOctrl(_BV(1) | _BV(2));

	// @TODO set up buttons
	//			vol up
	//	prev	play/pause	next
	//			vol dn

	// @TODO just for reference now, delete once direct control buttons are set up
	// redBtn();
}

void loop() {
	// react to cap screen use input actions
	if (ts.touched())
	{
		// Retrieve a point
		TS_Point p = ts.getPoint();

		// rotate coordinate system & flip it around to match the screen.
		p.x = map(p.x, 0, 240, 240, 0);
		p.y = map(p.y, 0, 320, 320, 0);
		int y = tft.height() - p.x;
		int x = p.y;

		// @TODO just for reference now, delete once direct control buttons are set up
		//if (recordOn)
		//{
			//if((x > REDBUTTON_X) && (x < (REDBUTTON_X + REDBUTTON_W))) {
				//if ((y > REDBUTTON_Y) && (y <= (REDBUTTON_Y + REDBUTTON_H))) {
					////Serial.print("Red btn hit\n");
					//redBtn();
				//}
			//}
		//}
		//else //Record is off (recordOn == false)
		//{
			//if((x > GREENBUTTON_X) && (x < (GREENBUTTON_X + GREENBUTTON_W))) {
				//if ((y > GREENBUTTON_Y) && (y <= (GREENBUTTON_Y + GREENBUTTON_H))) {
					////Serial.print("Green btn hit\n");
					//greenBtn();
				//}
			//}
		//}
	}

	if(DEBUG_ON) {
		// for changes in antenna capacitance
		radio.readTuneStatus();

		if( radio.currAntCap != prevAntCap ) {
			Serial.print("Curr ANT capacitance: "); Serial.print(radio.currAntCap);
			Serial.println();
			prevAntCap = radio.currAntCap;
		}

		// for changes in ASQ and InLevel
		radio.readASQ();
		if( radio.currASQ != prevASQ ) {
			Serial.print("Curr ASQ: 0x"); Serial.print(radio.currASQ, HEX); Serial.print("\tCurr InLevel: "); Serial.print(radio.currInLevel);
			Serial.println();
			prevASQ = radio.currASQ;
		}

		// currInLevel changes too often by too little, so need a floor delta value
		if( abs(radio.currInLevel - prevInLevel) > 10 ) {
			Serial.print("Curr ASQ: 0x"); Serial.print(radio.currASQ, HEX); Serial.print("\tCurr InLevel: "); Serial.print(radio.currInLevel);
			Serial.println();
			prevInLevel = radio.currInLevel;
		}
	}

	// @TODO if ASQ over modulating (confirm this value in docs), reduce rn-52 output volume 1 step and print to screen
	// @TODO if ASQ is not over modulating, increase rn-52 output volume 1 step and print to screen

	// @TODO what is this code for???
	// toggle GPO1 and GPO2
	//radio.setGPIO(_BV(1));
	//delay(500);
	//radio.setGPIO(_BV(2));
	//delay(500);
}


/**
* FM Functions
*/

/**
* scans FM band for stations that are not being used and returns lowest noise level frequency
* @param{Int} maxLevel The maximum allowable noise level for an unused frequency
* @param{Int} defBroadcast The defualt frequency
* @param{Int} loEnd The low end of the FM band
* @param{Int} hiEnd The high end of the FM band
* @return{Int} newBroadcast The new frequency to broadcast on
*/
int availableChannels(int maxLevel, int defBroadcast, int loEnd, int hiEnd)
{
	int freq;
	int newBroadcast;
	std::vector<std::pair<int, int>> scannedFreqs;

	// sort predicate for vector where left side of pair is frequency and right side of pair is noise level
	struct sort_pred {
		bool operator()(const std::pair<int,int> &left, const std::pair<int,int> &right) {
			return left.second < right.second;
		}
	};

	//set failsafe return value
	newBroadcast = defBroadcast;

	//scan the fm band from loEnd to hiEnd in .2 Mhz increments, save frequencies with low enough noise level
	for (freq = loEnd; freq <= hiEnd; freq += 20) {
		radio.readTuneMeasure(freq);
		radio.readTuneStatus();

		if( radio.currNoiseLevel < maxLevel) {
			if (DEBUG_ON){
				Serial.print("Available frequency "); Serial.print(freq/100.00); Serial.print(" Mhz, Noise Level: "); Serial.print(radio.currNoiseLevel);
				Serial.println();
			}
			scannedFreqs.push_back(std::make_pair(freq, radio.currNoiseLevel));
		}
	}

	// sort by low noise level ascending, take lowest noise level as new broadcast frequency
	sort(scannedFreqs.begin(), scannedFreqs.end(), sort_pred());
	newBroadcast = scannedFreqs[0].first;

	if(DEBUG_ON) {
		Serial.print("\nSorted available frequencies\n");

		// reverse print the sorted scanned frequencies because I want the quietest frequency to be last datum printed
		for(int i = scannedFreqs.size() -1; i >= 0; i--) {
			Serial.print("Frequency "); Serial.print(scannedFreqs[i].first/100.00); Serial.print(" Mhz, Noise Level: "); Serial.print(scannedFreqs[i].second);
			Serial.println();
		}

		// display new frequency info
		Serial.print("\nFound "); Serial.print(scannedFreqs.size()); Serial.print(" frequencies with noise less than "); Serial.print(maxLevel);
		Serial.println();

		Serial.print("Quietest frequency: "); Serial.print(newBroadcast/100.00); Serial.print(" Mhz, Noise Level: "); Serial.print(scannedFreqs[0].second);
		Serial.println();
	}

	return newBroadcast;
}

/**
* Capacitive Touch Functions
*/

// @TODO function header
void drawFrame()
{
	tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, ILI9341_BLACK);
}


// @TODO just for reference now, delete once direct control buttons are set up
//void redBtn()
//{
	//tft.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, ILI9341_RED);
	//tft.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, ILI9341_BLUE);
	//drawFrame();
	//tft.setCursor(GREENBUTTON_X + 6 , GREENBUTTON_Y + (GREENBUTTON_H/2));
	//tft.setTextColor(ILI9341_WHITE);
	//tft.setTextSize(1);
	//tft.println("ON");
	//recordOn = false;
//}

// @TODO just for reference now, delete once direct control buttons are set up
//void greenBtn()
//{
	//tft.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, ILI9341_GREEN);
	//tft.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, ILI9341_BLUE);
	//drawFrame();
	//tft.setCursor(REDBUTTON_X + 6 , REDBUTTON_Y + (REDBUTTON_H/2));
	//tft.setTextColor(ILI9341_WHITE);
	//tft.setTextSize(1);
	//tft.println("OFF");
	//recordOn = true;
//}

//@TODO not sure I need this anymore
/**
* Prints supplied text to touch screen
* The input string is expected to be fully formatted by calling function
* Ie. the input string contains all desired tabs, newlines, etc.
* Eg.
* Serial.print("Quietest frequency: "); Serial.print(newBroadcast/100.00); Serial.print(" Mhz, Noise Level: "); Serial.print(scannedFreqs[0].second);
* Serial.println();
* is expected to be:
* String msg = "Quietest frequency: " + atoi??(newBroadcast/100.00) + " Mhz, Noise Level: " + atoi??(scannedFreqs[0].second) + "\n";
* @param{String} inputText the text to print
*/
void tftPrint(String inputText)
{
	tft.print(inputText);
}

//@TODO not sure I need this anymore
/**
* Prints text to Serial monitor
* The input string is expected to be fully formatted by calling function
* Ie. the input string contains all desired tabs, newlines, etc.
* Eg.
* Serial.print("Quietest frequency: "); Serial.print(newBroadcast/100.00); Serial.print(" Mhz, Noise Level: "); Serial.print(scannedFreqs[0].second);
* Serial.println();
* is expected to be:
* String msg = "Quietest frequency: " + atoi??(newBroadcast/100.00) + " Mhz, Noise Level: " + atoi??(scannedFreqs[0].second) + "\n";
* @param(String) inputText the text to print
*/
void serialPrint(String inputText)
{
	Serial.print(inputText);
}