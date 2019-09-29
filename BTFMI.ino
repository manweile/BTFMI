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

#include <Adafruit_Si4713.h>
#include <ArduinoSTL.h>
//#include <PrintEx.h>
#include <Wire.h>
#include <algorithm>
#include <vector>

#define RESETPIN 22				//pin to use on mega 2560
#define FMSTATION 8770			// default station 8770 == 87.70 MHz
#define BAUD 115200				// use default upload speed for convenience
#define MAX_FREQ 10790			// upper end of FM band
#define MIN_FREQ 8770			// lower end of FM band
#define TX_POWER 115			// dBuV, 88-115 max
#define BROADCAST_LEVEL 60		// noise level for broadcasting stations
#define USE_AVAILABLE false
#define SHOW_SCANNED false


Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

void setup() {
	int freq;
	int station;

	Serial.begin(BAUD);
	printf("\nGAM Radio\n");

	// begin with address 0x63 (CS high default)
	if (! radio.begin()) {
		printf("Couldn't find radio Si4713 transciever\n");
		while (1);
	}

	printf("\nScanning for broadcasting stations ...\n\n");
	for (freq = MIN_FREQ; freq <= MAX_FREQ; freq += 20) {
		radio.readTuneMeasure(freq);
		radio.readTuneStatus();
		if( radio.currNoiseLevel >= BROADCAST_LEVEL) {
			// @TODO use PrintEx.h?
			Serial.print("Broadcast station at "); Serial.print(freq/100.00); Serial.print(" Mhz, Current Noise Level: "); Serial.println(radio.currNoiseLevel);
		}
	}

	// station = availableChannels(BROADCAST_LEVEL, FMSTATION, MIN_FREQ, MAX_FREQ, SHOW_SCANNED);

	// @TODO use PrintEx.h?
	Serial.print("\nSet TX power: "); Serial.print(TX_POWER); Serial.println(" dBuV");
	radio.setTXpower(TX_POWER);

	// @TODO use PrintEx.h?
	if(USE_AVAILABLE) {
		station = availableChannels(BROADCAST_LEVEL, FMSTATION, MIN_FREQ, MAX_FREQ, SHOW_SCANNED);
		Serial.print("\nTuning into frequency "); Serial.print(station/100.00); Serial.println(" Mhz\n");
		radio.tuneFM(station);
	}
	else {
		Serial.print("\nTuning into debugging frequency "); Serial.print(FMSTATION/100.00); Serial.println(" Mhz\n");
		radio.tuneFM(FMSTATION);
	}

	// This will tell you the status in case you want to read it from the chip
	// @TODO use PrintEx.h?
	radio.readTuneStatus();
	Serial.print("Curr frequency: "); Serial.print(radio.currFreq/100.00); Serial.print("\tCurr frequency dBuV: ");   Serial.print(radio.currdBuV);
	Serial.print("\tCurr ANT capacitance: "); Serial.print(radio.currAntCap); Serial.print("\n");

	// set GP1 and GP2 to output
	radio.setGPIOctrl(_BV(1) | _BV(2));
}

void loop() {
	// @TODO use PrintEx.h?

	// @TODO I want to see current antenna capacitance
	radio.readTuneStatus();
	// @TODO I only want to see CHANGES in antenna capacitance
	Serial.print("Curr ANT capacitance: \t"); Serial.print(radio.currAntCap);

	radio.readASQ();
	// @TODO I only want to see CHANGES in ASQ and InLevel
    Serial.print("\tCurr ASQ:\t 0x"); Serial.print(radio.currASQ, HEX); Serial.print("\tCurr InLevel:\t"); Serial.println(radio.currInLevel);

	// @TODO if ASQ over modulating (confirm this value in docs), reduce rn-52 output volume 1 step and print to screen
	// @TODO if ASQ is not over modulating, increase rn-52 output volume 1 step and print to screen

	// toggle GPO1 and GPO2
	radio.setGPIO(_BV(1));
	delay(500);
	radio.setGPIO(_BV(2));
	delay(500);
}


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
	printf("\nScanning for available frequencies ...\n\n");
	for (freq = loEnd; freq <= hiEnd; freq += 20) {
		radio.readTuneMeasure(freq);
		radio.readTuneStatus();

		if( radio.currNoiseLevel < maxLevel) {
			// @TODO use PrintEx.h?
			if (showInfo){
				Serial.print("Measuring available frequency "); Serial.print(freq/100.00); Serial.print(" Mhz with Current Noise Level: "); Serial.println(radio.currNoiseLevel);
			}
			scannedFreqs.push_back(std::make_pair(freq, radio.currNoiseLevel));
		}
	}

	// sort by low noise level ascending, take lowest noise level as new broadcast frequency
	sort(scannedFreqs.begin(), scannedFreqs.end(), sort_pred());

	printf("\nSorted available frequencies\n\n");
	for(int i = 0; i < scannedFreqs.size(); i++) {
		if(showInfo){
			// @TODO use PrintEx.h?
			Serial.print("Frequency "); Serial.print(scannedFreqs[i].first/100.00); Serial.print(" Mhz, with noise: "); Serial.print(scannedFreqs[i].second); Serial.print("\n");
		}
	}

	newBroadcast = scannedFreqs[0].first;

	//display new frequency
	printf("\nFound %d frequencies with noise less than %d\n", scannedFreqs.size(), maxLevel);
	// @TODO use PrintEx.h?
	Serial.print("Quietest frequency: "); Serial.print(newBroadcast/100.00); Serial.print(" Mhz with Current Noise Level: "); Serial.println(scannedFreqs[0].second);

	// save new broadcast frequency to either eeprom (preferable) or sd and return it

	return newBroadcast;
}