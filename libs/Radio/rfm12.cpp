/*
 * rfm12.cpp
 *
 *  Created on: 27.03.2016
 *      Author: sn
 */

#include "rfm12.h"

RFM12::RFM12(SPI &spi) :
	_spi(spi)
{
	init();		// init RFM for OOK receive
}

void RFM12::init()
{
	wait_ms(100);
	_spi.format(16, 0);

	_spi.write(0xC000);			// AVR CLK: 1MHz
	_spi.write(0x8027);			// Disable FIFO, 868MHz

	_spi.write(0x82C0);			//

	_spi.write(0xC227);			// Data Filter: digital

	_spi.write(0xCA00);			// Set FIFO mode disabled
	_spi.write(0xCC67);			// set PLL
	_spi.write(0xB800);			// Tx write not used
	_spi.write(0xE000);			// disable wakeuptimer
	_spi.write(0xC800);			// disable low duty cycle

	_spi.write(0xC473);			// AFC settings: autotuning: -10kHz...+7,5kHz
}

void RFM12::setBandwidth(unsigned char bandwidth, unsigned char gain, unsigned char drssi)
{
	_spi.write(0x9400|((bandwidth&7)<<5)|((gain&3)<<3)|(drssi&7));
}

void RFM12::setFreq(unsigned short freq)
{	if (freq<96)				// fmin
		freq=96;
	else if (freq>3903)			// fmax
		freq=3903;
	_spi.write(0xA000|freq);
}

void RFM12::setBaud(unsigned short baud)
{
	if (baud<663)
		return;
	if (baud<5400)					// Baudrate= 344827,58621/(R+1)/(1+CS*7)
		_spi.write(0xC680|((43104/baud)-1));
	else
		_spi.write(0xC600|((344828UL/baud)-1));
}

void RFM12::setPower(unsigned char power, unsigned char mod)
{
	_spi.write(0x9800|(power&7)|((mod&15)<<4));
}

void RFM12::ready(void)
{
	// TODO: implement ready function
	//cbi(RF_PORT, CS);
	//while (!(RF_PIN & (1<<SDO))); // wait until FIFO ready
}

void RFM12::txData(unsigned char *data, unsigned char number)
{	unsigned char i;
	_spi.write(0x8238);			// TX on
	ready();
	_spi.write(0xB8AA);
	ready();
	_spi.write(0xB8AA);
	ready();
	_spi.write(0xB8AA);
	ready();
	_spi.write(0xB82D);
	ready();
	_spi.write(0xB8D4);
	for (i=0; i<number; i++)
	{		ready();
		_spi.write(0xB800|(*data++));
	}
	ready();
	_spi.write(0x8208);			// TX off
}

void RFM12::rxData(unsigned char *data, unsigned char number)
{	unsigned char i;
	_spi.write(0x82C8);			// RX on
	_spi.write(0xCA81);			// set FIFO mode
	_spi.write(0xCA83);			// enable FIFO
	for (i=0; i<number; i++)
	{	ready();
		*data++=_spi.write(0xB000);
	}
	_spi.write(0x8208);			// RX off
}

void RFM12::rxOn()
{
	_spi.write(0x82C8);			// RX on
}

void RFM12::powerDown()
{
	_spi.write(0x8201);			// RX on
}


void RFM12::txOn(void)
{
	_spi.write(0x823D);			// TX on
}


void RFM12::txOff(void)
{
	_spi.write(0x820D);			// TX off
}



void RFM12::initOOK()
{
	setFreq(RF12FREQ(868.35));	// Sende/Empfangsfrequenz auf 868,35MHz einstellen

	// Bandwidth		LNA gain		RSSI threshold
	// 0: reserved		0:   0  dB		0: -103 dBm
	// 1: 400 kHz		1:	-6  dB		1: -97  dBm
	// 2: 340 kHz		2:	-14 dB		2: -91  dBm
	// 3: 270 kHz		3:	-20 dB		3: -85  dBm
	// 4: 200 kHz						4: -79  dBm
	// 5: 134 kHz						5: -73  dBm
	// 6:  67 kHz						6: -67  dBm
	// 7: reserved						7: -61  dBm
	setBandwidth(3, 0, 3);		// Bandbreite, Verstärkung, DRSSI threshold
	setBaud(19200);				// 9600 baud
	setPower(0, 6);			// 1mW Ausgangangsleistung, 120kHz Frequenzshift
}

