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
#include <SPI.h>

//third party function libraries
//#include <PrintEx.h>
#include <algorithm>
#include <vector>

//third party hardware libraries
#include <Adafruit_Si4713.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include <Wire.h>

//fm definitions
#define RESETPIN 22				//pin to use on mega 2560
#define FMSTATION 8770			// default station 8770 == 87.70 MHz
#define BAUD 115200				// use default upload speed for convenience
#define MAX_FREQ 10790			// upper end of FM band
#define MIN_FREQ 8770			// lower end of FM band
#define TX_POWER 115			// dBuV, 88-115 max
#define BROADCAST_LEVEL 60		// noise level for broadcasting stations

//fm debugging flags
#define USE_AVAILABLE true
#define SHOW_SCANNED true
#define DEBUG_ON false

//capacitive touch definitions
#define SD_CS 4
#define TFT_CS 10
#define TFT_DC 9

#define FRAME_X 10
#define FRAME_Y 10
#define FRAME_W 100
#define FRAME_H 50

#define REDBUTTON_X FRAME_X
#define REDBUTTON_Y FRAME_Y
#define REDBUTTON_W (FRAME_W/2)
#define REDBUTTON_H FRAME_H

#define GREENBUTTON_X (REDBUTTON_X + REDBUTTON_W)
#define GREENBUTTON_Y FRAME_Y
#define GREENBUTTON_W (FRAME_W/2)
#define GREENBUTTON_H FRAME_H

Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

Adafruit_FT6206 ts = Adafruit_FT6206();
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

boolean recordOn = false;
uint16_t prevFreq;
uint8_t prevdBuV, prevAntCap, prevNoiseLevel, prevASQ;
int8_t prevInLevel;

void setup() {
	int freq;
	int station = FMSTATION;

	Serial.begin(BAUD);

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

		// now we can use tft.print and println
		Serial.println("GAM Radio");
		tft.println("GAM Radio");
	}

    // reset for screen space saving
	tft.setTextSize(1);

	// begin with address 0x63 (CS high default)
	if (! radio.begin()) {
		Serial.print("Couldn't find Si4713 transceiver");
		tft.println("Couldn't find Si4713 transceiver");
		delay(500);
		exit(2);
	}

	Serial.println("Scanning for broadcasting stations ...");
	tft.println("Scanning for broadcasting stations ...");
	Serial.println();
	tft.println();

	for (freq = MIN_FREQ; freq <= MAX_FREQ; freq += 20) {
		radio.readTuneMeasure(freq);
		radio.readTuneStatus();
		if( radio.currNoiseLevel >= BROADCAST_LEVEL) {
			Serial.print("Broadcast at "); Serial.print(freq/100.00); Serial.print(" Mhz, Current Noise Level: "); Serial.print(radio.currNoiseLevel);
			tft.print("Broadcast at "); tft.print(freq/100.00); tft.print(" Mhz, Current Noise Level: "); tft.print(radio.currNoiseLevel);
			Serial.println();
			tft.println();
		}
	}

	//
	if(USE_AVAILABLE) {
		station = availableChannels(BROADCAST_LEVEL, FMSTATION, MIN_FREQ, MAX_FREQ, SHOW_SCANNED);

		Serial.print("\nTuning into frequency "); Serial.print(station/100.00); Serial.print(" Mhz");
		Serial.println();

		tft.print("\nTuning into frequency "); tft.print(station/100.00); tft.print(" Mhz");
		tft.println();
	}
	else {
		Serial.print("\nTuning into debugging frequency "); Serial.print(FMSTATION/100.00); Serial.print(" Mhz");
		Serial.println();

		tft.print("\nTuning into debugging frequency "); tft.print(FMSTATION/100.00); tft.print(" Mhz");
		tft.println();
	}

	/*
	one of readTuneMeasure(freq) or readTuneStatus() in avalaibleChannels
	is resetting transmission power to zero
	so have to call setTxPower after avalaibleChannels called
	suspect its readTuneMeasure because:
	void Adafruit_Si4713::readTuneMeasure(uint16_t freq) {
		// check freq is multiple of 50khz
		if (freq % 5 != 0) {
			freq -= (freq % 5);
		}
		//Serial.print("Measuring "); Serial.println(freq);
		_i2ccommand[0] = SI4710_CMD_TX_TUNE_MEASURE;
		_i2ccommand[1] = 0;
		_i2ccommand[2] = freq >> 8;
		_i2ccommand[3] = freq;
		_i2ccommand[4] = 0;

		sendCommand(5);
		while (getStatus() != 0x81) delay(10);
	}
	_i2ccommand[4], which is antenna capacitance, is being reset to zero
	*/
	radio.setTXpower(TX_POWER);

	radio.tuneFM(station);

	Serial.print("Setting TX power: "); Serial.print(TX_POWER); Serial.print(" dBuV");
	Serial.println();
	tft.print("Setting TX power: "); tft.print(TX_POWER); tft.print(" dBuV");
	tft.println();

	// This will tell you the status in case you want to read it from the chip
	radio.readTuneStatus();

	Serial.print("\nFrequency: "); Serial.print(radio.currFreq/100.00);
	tft.print("\nFrequency: "); tft.print(radio.currFreq/100.00);

	Serial.print("\nFrequency dBuV: "); Serial.print(radio.currdBuV);
	tft.print("\nFrequency dBuV: "); tft.print(radio.currdBuV);

	Serial.print("\nANT capacitance: "); Serial.print(radio.currAntCap);
	Serial.println("\n");
	tft.print("\nANT capacitance: "); tft.print(radio.currAntCap);
	tft.println();

	prevFreq = radio.currFreq/100.00;
	prevdBuV = radio.currdBuV;
	prevAntCap = radio.currAntCap;

	radio.readASQ();
	prevASQ = radio.currASQ;
	prevInLevel = radio.currInLevel;

	// set GP1 and GP2 to output
	//radio.setGPIOctrl(_BV(1) | _BV(2));

	//tft.fillScreen(ILI9341_BLACK);
	// origin = left,top landscape (USB left upper)
	//tft.setRotation(1);
	// redBtn();
}

void loop() {
	// cap screen
	if (ts.touched())
	{
		// Retrieve a point
		TS_Point p = ts.getPoint();

		// rotate coordinate system & flip it around to match the screen.
		p.x = map(p.x, 0, 240, 240, 0);
		p.y = map(p.y, 0, 320, 320, 0);
		int y = tft.height() - p.x;
		int x = p.y;

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


	radio.readTuneStatus();
	// I only want to see CHANGES in antenna capacitance and only for debugging purposes
	if( radio.currAntCap != prevAntCap ) {
		Serial.print("Curr ANT capacitance: "); Serial.print(radio.currAntCap);
		Serial.println();
		prevAntCap = radio.currAntCap;
	}

	radio.readASQ();
	// I only want to see CHANGES in ASQ and InLevel and only for debugging purposes
	if( radio.currASQ != prevASQ ) {
		Serial.print("Curr ASQ: 0x"); Serial.print(radio.currASQ, HEX); Serial.print("\tCurr InLevel: "); Serial.println(radio.currInLevel);
		prevASQ = radio.currASQ;
	}

	// currInLevel changes to much by too little, so need a floor delta value
	if( abs(radio.currInLevel - prevInLevel) > 10 ) {
		Serial.print("Curr ASQ: 0x"); Serial.print(radio.currASQ, HEX); Serial.print("\tCurr InLevel: "); Serial.println(radio.currInLevel);
		prevInLevel = radio.currInLevel;
	}

	// @TODO if ASQ over modulating (confirm this value in docs), reduce rn-52 output volume 1 step and print to screen
	// @TODO if ASQ is not over modulating, increase rn-52 output volume 1 step and print to screen

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
* @param{Boolean} showinfo Flag to print information
* @return{Int} newBroadcast The new frequency to broadcast on
*/
int availableChannels(int maxLevel, int defBroadcast, int loEnd, int hiEnd, boolean showInfo)
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
	//printf("\nScanning for available frequencies ...\n\n");
	Serial.print("\nScanning for available frequencies ...\n");
	tft.print("\nScanning for available frequencies ...\n");
	for (freq = loEnd; freq <= hiEnd; freq += 20) {
		radio.readTuneMeasure(freq);
		radio.readTuneStatus();

		if( radio.currNoiseLevel < maxLevel) {
			if (showInfo){
				// Serial print cause I don't want to clutter up tft screen
				Serial.print("Available frequency "); Serial.print(freq/100.00); Serial.print(" Mhz, Noise Level: "); Serial.println(radio.currNoiseLevel);
			}
			scannedFreqs.push_back(std::make_pair(freq, radio.currNoiseLevel));
		}
	}

	// sort by low noise level ascending, take lowest noise level as new broadcast frequency
	sort(scannedFreqs.begin(), scannedFreqs.end(), sort_pred());
	newBroadcast = scannedFreqs[0].first;

	//printf("\nSorted available frequencies\n\n");
	Serial.print("\nSorted available frequencies\n");
	//for(int i = 0; i < scannedFreqs.size(); i++) {
	for(int i = scannedFreqs.size() -1; i >= 0; i--) {
		if(showInfo){
			//  Serial print cause I don't want to clutter up tft screen
			Serial.print("Frequency "); Serial.print(scannedFreqs[i].first/100.00); Serial.print(" Mhz, Noise Level: "); Serial.print(scannedFreqs[i].second); Serial.print("\n");
		}
	}

	// display new frequency
	//printf("\nFound %d frequencies with noise less than %d\n", scannedFreqs.size(), maxLevel);
	Serial.print("\nFound "); Serial.print(scannedFreqs.size()); Serial.print(" frequencies with noise less than "); Serial.print(maxLevel);
	Serial.println();
	tft.print("\nFound "); tft.print(scannedFreqs.size()); tft.print(" frequencies with noise less than "); tft.print(maxLevel);
	tft.println();

	Serial.print("Quietest frequency: "); Serial.print(newBroadcast/100.00); Serial.print(" Mhz, Noise Level: "); Serial.print(scannedFreqs[0].second);
	Serial.println();
	tft.print("Quietest frequency: "); tft.print(newBroadcast/100.00); tft.print(" Mhz, Noise Level: "); tft.print(scannedFreqs[0].second);
	tft.println();

	// @TODO save new broadcast frequency to either eeprom (preferable) or sd

	return newBroadcast;
}

/**
* Capacitive Touch Functions
*/

void drawFrame()
{
	tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, ILI9341_BLACK);
}

void redBtn()
{
	tft.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, ILI9341_RED);
	tft.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, ILI9341_BLUE);
	drawFrame();
	tft.setCursor(GREENBUTTON_X + 6 , GREENBUTTON_Y + (GREENBUTTON_H/2));
	tft.setTextColor(ILI9341_WHITE);
	tft.setTextSize(1);
	tft.println("ON");
	recordOn = false;
}

void greenBtn()
{
	tft.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, ILI9341_GREEN);
	tft.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, ILI9341_BLUE);
	drawFrame();
	tft.setCursor(REDBUTTON_X + 6 , REDBUTTON_Y + (REDBUTTON_H/2));
	tft.setTextColor(ILI9341_WHITE);
	tft.setTextSize(1);
	tft.println("OFF");
	recordOn = true;
}

/**
* Prints text to touch screen
* @param{String} inputText the text to print to screen
*/
void printText(String inputText)
{
	//set cursor position

	//set text color

	// set text size

	// print the input text

}