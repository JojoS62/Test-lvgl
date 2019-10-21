//Port of RFM69 from lowpowerlab
//Sync'd Feb. 6, 2015
//spi register read/write routines from Karl Zweimuller's RF22
//
//
//
// **********************************************************************************
// Driver definition for HopeRF RFM69W/RFM69HW/RFM69CW/RFM69HCW, Semtech SX1231/1231H
// **********************************************************************************
// Copyright Felix Rusu (2014), felix@lowpowerlab.com
// http://lowpowerlab.com/
// **********************************************************************************
// License
// **********************************************************************************
// This program is free software; you can redistribute it 
// and/or modify it under the terms of the GNU General    
// Public License as published by the Free Software       
// Foundation; either version 3 of the License, or        
// (at your option) any later version.                    
//                                                        
// This program is distributed in the hope that it will   
// be useful, but WITHOUT ANY WARRANTY; without even the  
// implied warranty of MERCHANTABILITY or FITNESS FOR A   
// PARTICULAR PURPOSE. See the GNU General Public        
// License for more details.                              
//                                                        
// You should have received a copy of the GNU General    
// Public License along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//                                                        
// Licence can be viewed at                               
// http://www.gnu.org/licenses/gpl-3.0.txt
//
// Please maintain this license information along with authorship
// and copyright notices in any redistribution of this code
// **********************************************************************************// RF22.cpp
//
// Copyright (C) 2011 Mike McCauley
// $Id: RF22.cpp,v 1.17 2013/02/06 21:33:56 mikem Exp mikem $
// ported to mbed by Karl Zweimueller

#include "RFM69.h"

#include "mbed.h"
#include <SPI.h>

volatile uint8_t RFM69::DATA[RF69_MAX_DATA_LEN];
volatile uint8_t RFM69::_mode;        // current transceiver state
volatile uint8_t RFM69::DATALEN;
volatile uint8_t RFM69::SENDERID;
volatile uint8_t RFM69::TARGETID;     // should match _address
volatile uint8_t RFM69::PAYLOADLEN;
volatile uint8_t RFM69::ACK_REQUESTED;
volatile uint8_t RFM69::ACK_RECEIVED; // should be polled immediately after sending a packet with ACK request
volatile int16_t RFM69::RSSI; // most accurate RSSI during reception (closest to the reception)

RFM69::RFM69(SPI &spi, PinName slaveSelectPin, PinName interrupt) :
		_spi(spi),
		_slaveSelectPin(slaveSelectPin),
		_interrupt(interrupt) {

	// Setup the spi for 16 bit data, high steady state clock,
	// second edge capture, with a 1MHz clock rate
	_spi.format(8, 0);
	//_spi.frequency(4000000);
	_mode = RF69_MODE_STANDBY;
	_promiscuousMode = false;
	_powerLevel = 31;
}

RFM69::RFM69(SPI &spi, PinName slaveSelectPin) :
		_spi(spi),
		_slaveSelectPin(slaveSelectPin),
		_interrupt(NC) {

	// Setup the spi for 16 bit data, high steady state clock,
	// second edge capture, with a 1MHz clock rate
	_spi.format(8, 0);
	//_spi.frequency(4000000);
	_mode = RF69_MODE_STANDBY;
	_promiscuousMode = false;
	_powerLevel = 31;
}

bool RFM69::initialize(uint8_t freqBand, uint8_t nodeID, uint8_t networkID) {
	unsigned long start_to;
	const uint8_t CONFIG[][2] =
			{
			/* 0x01 */{ REG_OPMODE, RF_OPMODE_SEQUENCER_ON
					| RF_OPMODE_LISTEN_OFF | RF_OPMODE_STANDBY },
			/* 0x02 */{ REG_DATAMODUL, RF_DATAMODUL_DATAMODE_PACKET
					| RF_DATAMODUL_MODULATIONTYPE_FSK
					| RF_DATAMODUL_MODULATIONSHAPING_00 }, // no shaping
					/* 0x03 */{ REG_BITRATEMSB, RF_BITRATEMSB_55555 }, // default: 4.8 KBPS
					/* 0x04 */{ REG_BITRATELSB, RF_BITRATELSB_55555 },
					/* 0x05 */{ REG_FDEVMSB, RF_FDEVMSB_50000 }, // default: 5KHz, (FDEV + BitRate / 2 <= 500KHz)
					/* 0x06 */{ REG_FDEVLSB, RF_FDEVLSB_50000 },

					/* 0x07 */{ REG_FRFMSB, (uint8_t) (
							freqBand == RF69_315MHZ ?
									RF_FRFMSB_315 :
									(freqBand == RF69_433MHZ ?
											RF_FRFMSB_433 :
											(freqBand == RF69_868MHZ ?
													RF_FRFMSB_868 :
													RF_FRFMSB_915))) },
					/* 0x08 */{ REG_FRFMID, (uint8_t) (
							freqBand == RF69_315MHZ ?
									RF_FRFMID_315 :
									(freqBand == RF69_433MHZ ?
											RF_FRFMID_433 :
											(freqBand == RF69_868MHZ ?
													RF_FRFMID_868 :
													RF_FRFMID_915))) },
					/* 0x09 */{ REG_FRFLSB, (uint8_t) (
							freqBand == RF69_315MHZ ?
									RF_FRFLSB_315 :
									(freqBand == RF69_433MHZ ?
											RF_FRFLSB_433 :
											(freqBand == RF69_868MHZ ?
													RF_FRFLSB_868 :
													RF_FRFLSB_915))) },

					// looks like PA1 and PA2 are not implemented on RFM69W, hence the max output power is 13dBm
					// +17dBm and +20dBm are possible on RFM69HW
					// +13dBm formula: Pout = -18 + OutputPower (with PA0 or PA1**)
					// +17dBm formula: Pout = -14 + OutputPower (with PA1 and PA2)**
					// +20dBm formula: Pout = -11 + OutputPower (with PA1 and PA2)** and high power PA settings (section 3.3.7 in datasheet)
					///* 0x11 */ { REG_PALEVEL, RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF | RF_PALEVEL_PA2_OFF | RF_PALEVEL_OUTPUTPOWER_11111},
					///* 0x13 */ { REG_OCP, RF_OCP_ON | RF_OCP_TRIM_95 }, // over current protection (default is 95mA)

					// RXBW defaults are { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_5} (RxBw: 10.4KHz)
					/* 0x19 */{ REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16
							| RF_RXBW_EXP_2 }, // (BitRate < 2 * RxBw)
					//for BR-19200: /* 0x19 */ { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_3 },
					/* 0x25 */{ REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_01 }, // DIO0 is the only IRQ we're using
					/* 0x26 */{ REG_DIOMAPPING2, RF_DIOMAPPING2_CLKOUT_OFF }, // DIO5 ClkOut disable for power saving
					/* 0x28 */{ REG_IRQFLAGS2, RF_IRQFLAGS2_FIFOOVERRUN }, // writing to this bit ensures that the FIFO & status flags are reset
					/* 0x29 */{ REG_RSSITHRESH, 228 }, // 220 must be set to dBm = (-Sensitivity / 2), default is 0xE4 = 228 so -114dBm
					///* 0x2D */ { REG_PREAMBLELSB, RF_PREAMBLESIZE_LSB_VALUE } // default 3 preamble bytes 0xAAAAAA
					/* 0x2E */{ REG_SYNCCONFIG, RF_SYNC_ON
							| RF_SYNC_FIFOFILL_AUTO | RF_SYNC_SIZE_2
							| RF_SYNC_TOL_0 },
					/* 0x2F */{ REG_SYNCVALUE1, 0x2D }, // attempt to make this compatible with sync1 byte of RFM12B lib
					/* 0x30 */{ REG_SYNCVALUE2, networkID }, // NETWORK ID
					/* 0x37 */{ REG_PACKETCONFIG1, RF_PACKET1_FORMAT_VARIABLE
							| RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_ON
							| RF_PACKET1_CRCAUTOCLEAR_ON
							| RF_PACKET1_ADRSFILTERING_OFF },
					/* 0x38 */{ REG_PAYLOADLENGTH, 66 }, // in variable length mode: the max frame size, not used in TX
					///* 0x39 */ { REG_NODEADRS, nodeID }, // turned off because we're not using address filtering
					/* 0x3C */{ REG_FIFOTHRESH,
							RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY
									| RF_FIFOTHRESH_VALUE }, // TX on FIFO not empty
					/* 0x3D */{ REG_PACKETCONFIG2,
							RF_PACKET2_RXRESTARTDELAY_2BITS
									| RF_PACKET2_AUTORXRESTART_ON
									| RF_PACKET2_AES_OFF }, // RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
					//for BR-19200: /* 0x3D */ { REG_PACKETCONFIG2, RF_PACKET2_RXRESTARTDELAY_NONE | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF }, // RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
					/* 0x6F */{ REG_TESTDAGC, RF_DAGC_IMPROVED_LOWBETA0 }, // run DAGC continuously in RX mode for Fading Margin Improvement, recommended default for AfcLowBetaOn=0
					{ 255, 0 } };

	// Timer for ms waits
	t.start();
	// Setup the spi for 8 bit data : 1RW-bit 7 adressbit and  8 databit
	// second edge capture, with a 10MHz clock rate
	//_spi.format(8,0);
	//_spi.frequency(4000000);

#define TIME_OUT 50

	start_to = t.read_ms();

	do
		writeReg(REG_SYNCVALUE1, 0xaa);
	while ((readReg(REG_SYNCVALUE1) != 0xaa)
			&& t.read_ms() - start_to < TIME_OUT);
	if (t.read_ms()-start_to >= TIME_OUT) return (false);

	// Set time out
	start_to = t.read_ms();
	do
		writeReg(REG_SYNCVALUE1, 0x55);
	while ((readReg(REG_SYNCVALUE1) != 0x55)
			&& t.read_ms() - start_to < TIME_OUT);
	if (t.read_ms() - start_to >= TIME_OUT)
		return (false);

	for (uint8_t i = 0; CONFIG[i][0] != 255; i++)
		writeReg(CONFIG[i][0], CONFIG[i][1]);

	// Encryption is persistent between resets and can trip you up during debugging.
	// Disable it during initialization so we always start from a known state.
	encrypt(0);

	setHighPower(_isRFM69HW); // called regardless if it's a RFM69W or RFM69HW
	setMode(RF69_MODE_STANDBY);
	// Set up interrupt handler
	start_to = t.read_ms();
	while (((readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00)
			&& t.read_ms() - start_to < TIME_OUT); // Wait for ModeReady
	if (t.read_ms() - start_to >= TIME_OUT)
		return (false);

	_interrupt.rise(this, &RFM69::interruptHandler);

	_address = nodeID;
	return true;
}

bool RFM69::initializeOOK() {
	const uint8_t CONFIG[][2] = {
	/* 0x01 */{ REG_OPMODE, RF_OPMODE_SEQUENCER_OFF | RF_OPMODE_LISTEN_OFF
			| RF_OPMODE_STANDBY },
	/* 0x02 */{ REG_DATAMODUL, RF_DATAMODUL_DATAMODE_CONTINUOUSNOBSYNC
			| RF_DATAMODUL_MODULATIONTYPE_OOK
			| RF_DATAMODUL_MODULATIONSHAPING_00 }, // no shaping
			/* 0x03 */{ REG_BITRATEMSB, 0x03 }, // bitrate: 32768 Hz
			/* 0x04 */{ REG_BITRATELSB, 0xD1 },
			/* 0x19 */{ REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24
					| RF_RXBW_EXP_4 }, // BW: 10.4 kHz
			/* 0x1B */ // { REG_OOKPEAK, RF_OOKPEAK_THRESHTYPE_PEAK | RF_OOKPEAK_PEAKTHRESHSTEP_000 | RF_OOKPEAK_PEAKTHRESHDEC_000 },//
			/* 0x1B */{ REG_OOKPEAK, RF_OOKPEAK_THRESHTYPE_FIXED
					| RF_OOKPEAK_PEAKTHRESHSTEP_000
					| RF_OOKPEAK_PEAKTHRESHDEC_000 },
			/* 0x1D */{ REG_OOKFIX, 6 }, // Fixed threshold value (in dB) in the OOK demodulator
			/* 0x26 */{ REG_DIOMAPPING2, RF_DIOMAPPING2_CLKOUT_OFF }, // DIO5 ClkOut disable for power saving
			/* 0x29 */{ REG_RSSITHRESH, 140 }, // 140 RSSI threshold in dBm = -(REG_RSSITHRESH / 2)
			/* 0x6F */{ REG_TESTDAGC, RF_DAGC_IMPROVED_LOWBETA0 }, // run DAGC continuously in RX mode, recommended default for AfcLowBetaOn=0
			{ 255, 0 } };

	wait_ms(50);

	for (uint8_t i = 0; CONFIG[i][0] != 255; i++)
		writeReg(CONFIG[i][0], CONFIG[i][1]);

	setHighPower(_isRFM69HW); // called regardless if it's a RFM69W or RFM69HW
	setMode(RF69OOK_MODE_STANDBY);
	while ((readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00)
		; // Wait for ModeReady

	return true;
}

// return the frequency (in Hz)
uint32_t RFM69::getFrequency() {
	return RF69_FSTEP
			* (((uint32_t) readReg(REG_FRFMSB) << 16)
					+ ((uint16_t) readReg(REG_FRFMID) << 8)
					+ readReg(REG_FRFLSB));
}

// set the frequency (in Hz)
void RFM69::setFrequency(uint32_t freqHz) {
	uint8_t oldMode = _mode;
	if (oldMode == RF69_MODE_TX) {
		setMode(RF69_MODE_RX);
	}
	freqHz /= RF69_FSTEP; // divide down by FSTEP to get FRF
	writeReg(REG_FRFMSB, freqHz >> 16);
	writeReg(REG_FRFMID, freqHz >> 8);
	writeReg(REG_FRFLSB, freqHz);
	if (oldMode == RF69_MODE_RX) {
		setMode(RF69_MODE_SYNTH);
	}
	setMode(oldMode);
}

void RFM69::setMode(uint8_t newMode) {
	if (newMode == _mode)
		return;

	switch (newMode) {
	case RF69_MODE_TX:
		writeReg(REG_OPMODE,
				(readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_TRANSMITTER);
		if (_isRFM69HW)
			setHighPowerRegs(true);
		break;
	case RF69_MODE_RX:
		writeReg(REG_OPMODE, (readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_RECEIVER);
		if (_isRFM69HW)
			setHighPowerRegs(false);
		break;
	case RF69_MODE_SYNTH:
		writeReg(REG_OPMODE,
				(readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_SYNTHESIZER);
		break;
	case RF69_MODE_STANDBY:
		writeReg(REG_OPMODE, (readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_STANDBY);
		break;
	case RF69_MODE_SLEEP:
		writeReg(REG_OPMODE, (readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_SLEEP);
		//writeReg(REG_OPMODE, RF_OPMODE_SLEEP);
		break;
	default:
		return;
	}

	// we are using packet mode, so this check is not really needed
	// but waiting for mode ready is necessary when going from sleep because the FIFO may not be immediately available from previous mode
	while (_mode == RF69_MODE_SLEEP
			&& (readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00)
		; // wait for ModeReady
	_mode = newMode;
}

void RFM69::sleep() {
	setMode(RF69_MODE_SLEEP);
}

void RFM69::setAddress(uint8_t addr) {
	_address = addr;
	writeReg(REG_NODEADRS, _address);
}

void RFM69::setNetwork(uint8_t networkID) {
	writeReg(REG_SYNCVALUE2, networkID);
}

// set output power: 0 = min, 31 = max
// this results in a "weaker" transmitted signal, and directly results in a lower RSSI at the receiver
void RFM69::setPowerLevel(uint8_t powerLevel) {
	_powerLevel = powerLevel;
	writeReg(REG_PALEVEL,
			(readReg(REG_PALEVEL) & 0xE0)
					| (_powerLevel > 31 ? 31 : _powerLevel));
}

bool RFM69::canSend() {
	if (_mode == RF69_MODE_RX && PAYLOADLEN == 0 && readRSSI() < CSMA_LIMIT) // if signal stronger than -100dBm is detected assume channel activity
	{
		setMode(RF69_MODE_STANDBY);
		return true;
	}
	return false;
}

void RFM69::send(uint8_t toAddress, const void* buffer, uint8_t bufferSize,
		bool requestACK) {
	writeReg(REG_PACKETCONFIG2,
			(readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
	uint32_t now = t.read_ms();
	while (!canSend() && t.read_ms() - now < RF69_CSMA_LIMIT_MS)
		receiveDone();
	sendFrame(toAddress, buffer, bufferSize, requestACK, false);
}

// to increase the chance of getting a packet across, call this function instead of send
// and it handles all the ACK requesting/retrying for you :)
// The only twist is that you have to manually listen to ACK requests on the other side and send back the ACKs
// The reason for the semi-automaton is that the lib is interrupt driven and
// requires user action to read the received data and decide what to do with it
// replies usually take only 5..8ms at 50kbps@915MHz
bool RFM69::sendWithRetry(uint8_t toAddress, const void* buffer,
		uint8_t bufferSize, uint8_t retries, int retryWaitTime) {
	int sentTime;
	for (uint8_t i = 0; i <= retries; i++) {
		send(toAddress, buffer, bufferSize, true);
		sentTime = t.read_ms();
		while (t.read_ms() - sentTime < retryWaitTime) {
			if (ACKReceived(toAddress)) {
				//Serial.print(" ~ms:"); Serial.print(t.read_ms() - sentTime);
				return true;
			}
		}
		//Serial.print(" RETRY#"); Serial.println(i + 1);
	}
	return false;
}

// should be polled immediately after sending a packet with ACK request
bool RFM69::ACKReceived(uint8_t fromNodeID) {
	if (receiveDone())
		return (SENDERID == fromNodeID || fromNodeID == RF69_BROADCAST_ADDR)
				&& ACK_RECEIVED;
	return false;
}

// check whether an ACK was requested in the last received packet (non-broadcasted packet)
bool RFM69::ACKRequested() {
	return ACK_REQUESTED && (TARGETID != RF69_BROADCAST_ADDR);
}

// should be called immediately after reception in case sender wants ACK
bool RFM69::NewSendACK(const void* buffer, uint8_t bufferSize, CSMA_STATS *csmaStats) {
	ACK_REQUESTED = 0;   // TWS added to make sure we don't end up in a timing race and infinite loop sending Acks
	setMode(RF69_MODE_RX);

	int16_t _RSSI = RSSI; // save payload received RSSI value
	bool canSendACK = false;

	int maxRssi = -1000;
	int now = t.read_us();
	int loopCount = 0;
	while ((canSendACK == false) && ((t.read_us() - now) < (ACK_CSMA_LIMIT_MS*1000L)))
	{
		int rssi = readRSSI();
		canSendACK = (rssi < CSMA_LIMIT); 	// CSMA_LIMIT = -90
		maxRssi = (rssi > maxRssi) ? rssi : maxRssi;
		if ((loopCount < MAX_RSSI_STAT_VALUES) && (csmaStats))
			csmaStats->rssi[loopCount] = rssi;
		loopCount++;
	}
	int csma_time = t.read_us() - now;

	if (csmaStats)	{
		csmaStats->maxRssi = maxRssi;
		csmaStats->waitTime = csma_time;
		csmaStats->noOfLoops = loopCount;
	}

	// at this point, sending the ack could also be done regardless the RSSI state
	// but by returning the canSendACK state the caller can decide to try the sendACK again or not
	if (canSendACK) {
		writeReg(REG_PACKETCONFIG2,
				(readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
		sendFrame(SENDERID, buffer, bufferSize, false, true);
	}
	RSSI = _RSSI; // restore payload RSSI

	return canSendACK;
}

// should be called immediately after reception in case sender wants ACK
// original algorithm, modified to capture statistics
bool RFM69::sendACK(const void* buffer, uint8_t bufferSize, CSMA_STATS *csmaStats) {
	ACK_REQUESTED = 0;   // TWS added to make sure we don't end up in a timing race and infinite loop sending Acks
	uint8_t sender = SENDERID;
	int16_t _RSSI = RSSI; // save payload received RSSI value
	writeReg(REG_PACKETCONFIG2, (readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks

	int maxRssi = -1000;
	int loopCount = 0;
	int rssi;
	bool canSendACK = false;
	bool timeout = false;
	int now = t.read_us();

//	  while (!canSend() && t.read_ms() - now < RF69_CSMA_LIMIT_MS)
//		  receiveDone();

	while (!canSendACK && !timeout)
	{
		// inserted code from canSend() to get rssi
		rssi = readRSSI();
		if (_mode == RF69_MODE_RX && PAYLOADLEN == 0 && rssi < CSMA_LIMIT) // if signal stronger than -100dBm (actual:-90dBm) is detected assume channel activity
		{
			setMode(RF69_MODE_STANDBY);
			canSendACK = true;
		}
		else
			canSendACK = false;

		timeout = ((t.read_us() - now) >= (RF69_CSMA_LIMIT_MS*1000L));

		if (!canSendACK && !timeout)
			receiveDone();

		if ((loopCount < MAX_RSSI_STAT_VALUES) && csmaStats)
			csmaStats->rssi[loopCount] = rssi;
		loopCount++;
		maxRssi = (rssi > maxRssi) ? rssi : maxRssi;
	}
	int csma_time = t.read_us() - now;

	// store statistics
	if (csmaStats)	{
		csmaStats->maxRssi = maxRssi;
		csmaStats->waitTime = csma_time;
		csmaStats->noOfLoops = loopCount;
	}

	sendFrame(sender, buffer, bufferSize, false, true);
	RSSI = _RSSI; // restore payload RSSI
	return canSendACK;
}


void RFM69::sendFrame(uint8_t toAddress, const void* buffer, uint8_t bufferSize,
		bool requestACK, bool sendACK) {
	setMode(RF69_MODE_STANDBY); // turn off receiver to prevent reception while filling fifo
	while ((readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00)
		; // wait for ModeReady
	writeReg(REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_00); // DIO0 is "Packet Sent"
	if (bufferSize > RF69_MAX_DATA_LEN)
		bufferSize = RF69_MAX_DATA_LEN;

	// control byte
	uint8_t CTLbyte = 0x00;
	if (sendACK)
		CTLbyte = 0x80;
	else if (requestACK)
		CTLbyte = 0x40;

	select();
	_spi.write(REG_FIFO | 0x80);
	_spi.write(bufferSize + 3);
	_spi.write(toAddress);
	_spi.write(_address);
	_spi.write(CTLbyte);

	for (uint8_t i = 0; i < bufferSize; i++)
		_spi.write(((uint8_t*) buffer)[i]);
	unselect();

	// no need to wait for transmit mode to be ready since its handled by the radio
	setMode(RF69_MODE_TX);
	uint32_t txStart = t.read_ms();
	while (_interrupt == 0 && t.read_ms() - txStart < RF69_TX_LIMIT_MS)
		; // wait for DIO0 to turn HIGH signalling transmission finish
	//while (readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PACKETSENT == 0x00); // wait for ModeReady
	setMode(RF69_MODE_STANDBY);
}
// ON  = disable filtering to capture all frames on network
// OFF = enable node/broadcast filtering to capture only frames sent to this/broadcast address
void RFM69::promiscuous(bool onOff) {
	_promiscuousMode = onOff;
	//writeReg(REG_PACKETCONFIG1, (readReg(REG_PACKETCONFIG1) & 0xF9) | (onOff ? RF_PACKET1_ADRSFILTERING_OFF : RF_PACKET1_ADRSFILTERING_NODEBROADCAST));
}

void RFM69::setHighPower(bool onOff) {
	_isRFM69HW = onOff;
	writeReg(REG_OCP, _isRFM69HW ? RF_OCP_OFF : RF_OCP_ON);
	if (_isRFM69HW) // turning ON
		writeReg(REG_PALEVEL,
				(readReg(REG_PALEVEL) & 0x1F) | RF_PALEVEL_PA1_ON
						| RF_PALEVEL_PA2_ON); // enable P1 & P2 amplifier stages
	else
		writeReg(REG_PALEVEL,
				RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF | RF_PALEVEL_PA2_OFF
						| _powerLevel); // enable P0 only
}

void RFM69::setHighPowerRegs(bool onOff) {
	writeReg(REG_TESTPA1, onOff ? 0x5D : 0x55);
	writeReg(REG_TESTPA2, onOff ? 0x7C : 0x70);
}

// for debugging
void RFM69::readAllRegs() {
	uint8_t regVal, regAddr;

	for (regAddr = 1; regAddr <= 0x4F; regAddr++) {
		select();
		_spi.write(regAddr & 0x7F); // send address + r/w bit
		regVal = _spi.write(0);
		regVal = regVal;

		/*   Serial.print(regAddr, HEX);
		 Serial.print(" - ");
		 Serial.print(regVal,HEX);
		 Serial.print(" - ");
		 Serial.println(regVal,BIN);*/
	}
	unselect();
}

uint8_t RFM69::readTemperature(int8_t calFactor) // returns centigrade
		{
	uint8_t oldMode = _mode;

	setMode(RF69_MODE_STANDBY);
	writeReg(REG_TEMP1, RF_TEMP1_MEAS_START);
	while ((readReg(REG_TEMP1) & RF_TEMP1_MEAS_RUNNING))
		;
	setMode(oldMode);

	return ~readReg(REG_TEMP2) + COURSE_TEMP_COEF + calFactor; // 'complement' corrects the slope, rising temp = rising val
} // COURSE_TEMP_COEF puts reading in the ballpark, user can add additional correction

void RFM69::rcCalibration() {
	writeReg(REG_OSC1, RF_OSC1_RCCAL_START);
	while ((readReg(REG_OSC1) & RF_OSC1_RCCAL_DONE) == 0x00)
		;
}
// C++ level interrupt handler for this instance
void RFM69::interruptHandler() {

	if (_mode == RF69_MODE_RX
			&& (readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PAYLOADREADY)) {
		setMode(RF69_MODE_STANDBY);
		select();

		_spi.write(REG_FIFO & 0x7F);
		PAYLOADLEN = _spi.write(0);
		PAYLOADLEN = PAYLOADLEN > 66 ? 66 : PAYLOADLEN; // precaution
		TARGETID = _spi.write(0);
		if (!(_promiscuousMode || TARGETID == _address
				|| TARGETID == RF69_BROADCAST_ADDR) // match this node's address, or broadcast address or anything in promiscuous mode
		|| PAYLOADLEN < 3) // address situation could receive packets that are malformed and don't fit this libraries extra fields
				{
			PAYLOADLEN = 0;
			unselect();
			receiveBegin();
			return;
		}

		DATALEN = PAYLOADLEN - 3;
		SENDERID = _spi.write(0);
		uint8_t CTLbyte = _spi.write(0);

		ACK_RECEIVED = CTLbyte & 0x80; // extract ACK-received flag
		ACK_REQUESTED = CTLbyte & 0x40; // extract ACK-requested flag

		for (uint8_t i = 0; i < DATALEN; i++) {
			DATA[i] = _spi.write(0);
		}
		if (DATALEN < RF69_MAX_DATA_LEN)
			DATA[DATALEN] = 0; // add null at end of string
		unselect();
		setMode(RF69_MODE_RX);
	}
	RSSI = readRSSI();
}

void RFM69::receiveBegin() {
	DATALEN = 0;
	SENDERID = 0;
	TARGETID = 0;
	PAYLOADLEN = 0;
	ACK_REQUESTED = 0;
	ACK_RECEIVED = 0;
	RSSI = 0;
	if (readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PAYLOADREADY)
		writeReg(REG_PACKETCONFIG2,
				(readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
	writeReg(REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_01); // set DIO0 to "PAYLOADREADY" in receive mode
	setMode(RF69_MODE_RX);		// enables interrupts
}

bool RFM69::receiveDone() {
	_interrupt.disable_irq(); 	// re-enabled in unselect() via setMode() or via receiveBegin()
	if (_mode == RF69_MODE_RX && PAYLOADLEN > 0) {
		setMode(RF69_MODE_STANDBY); // enables interrupts
		return true;
	} else if (_mode == RF69_MODE_RX) // already in RX no payload yet
	{
		_interrupt.enable_irq(); // explicitly re-enable interrupts
		return false;
	}
	receiveBegin();
	return false;
}

// To enable encryption: radio.encrypt("ABCDEFGHIJKLMNOP");
// To disable encryption: radio.encrypt(null) or radio.encrypt(0)
// KEY HAS TO BE 16 bytes !!!
void RFM69::encrypt(const char* key) {
	setMode(RF69_MODE_STANDBY);
	if (key != 0) {
		select();
		_spi.write(REG_AESKEY1 | 0x80);
		for (uint8_t i = 0; i < 16; i++)
			_spi.write(key[i]);
		unselect();
	}
	writeReg(REG_PACKETCONFIG2,
			(readReg(REG_PACKETCONFIG2) & 0xFE) | (key ? 1 : 0));
}

int16_t RFM69::readRSSI(bool forceTrigger) {
	int16_t rssi = 0;
	if (forceTrigger) {
		// RSSI trigger not needed if DAGC is in continuous mode
		writeReg(REG_RSSICONFIG, RF_RSSI_START);
		while ((readReg(REG_RSSICONFIG) & RF_RSSI_DONE) == 0x00)
			; // wait for RSSI_Ready
	}
	rssi = -readReg(REG_RSSIVALUE);
	rssi >>= 1;
	return rssi;
}

// Turn the radio into transmission mode
void RFM69::transmitOOKBegin() {
	//_interrupt.rise(NULL);
	setMode(RF69OOK_MODE_TX);
	//_data.mode(PullDown);
	//_data.output();
}

// Turn the radio back to standby
void RFM69::transmitOOKEnd() {
	setMode(RF69OOK_MODE_STANDBY);
}

// Turn the radio into OOK listening mode
void RFM69::receiveOOKBegin() {
	setMode(RF69OOK_MODE_RX);
}

// Turn the radio back to standby
void RFM69::receiveOOKEnd() {
	setMode(RF69OOK_MODE_STANDBY);
	//_interrupt.rise(NULL);
}

// set OOK bandwidth
void RFM69::setBandwidth(uint8_t bw) {
	writeReg(REG_RXBW, (readReg(REG_RXBW) & 0xE0) | bw);
}

// set RSSI threshold
void RFM69::setRSSIThreshold(int8_t rssi) {
	writeReg(REG_RSSITHRESH, -(rssi << 1));
}

// set OOK fixed threshold
void RFM69::setFixedThreshold(uint8_t threshold) {
	writeReg(REG_OOKFIX, threshold);
}

//uint16_t RFM69::readReg(uint8_t addr) {
//	select();
//	uint16_t val = _spi.write((addr & 0x7F) << 8); // Send the address with the write mask off
//	//uint8_t val = _spi.write(0); // The written value is ignored, reg value is read
//	unselect();
//	val &= 0xff;
//	return val;
//}
//
//void RFM69::writeReg(uint8_t addr, uint8_t value) {
//	uint16_t val = ((addr | 0x80) << 8) | value;
//	select();
//	//_spi.write(addr | 0x80); // Send the address with the write mask on
//	_spi.write(val); // New value follows
//	unselect();
//}

uint8_t RFM69::readReg(uint8_t addr)
{
  select();
  _spi.write(addr & 0x7F);
  uint8_t regval = _spi.write(0);
  unselect();
  return regval;
}

void RFM69::writeReg(uint8_t addr, uint8_t value)
{
  select();
  _spi.write(addr | 0x80);
  _spi.write(value);
  unselect();
}

// select the transceiver
void RFM69::select() {
	//_interrupt.disable_irq();    // Disable Interrupts
	_slaveSelectPin = 0;
}

// UNselect the transceiver chip
void RFM69::unselect() {
	_slaveSelectPin = 1;
	 //_interrupt.enable_irq();     // Enable Interrupts
}
