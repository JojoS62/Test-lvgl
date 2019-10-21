/*
 * rfm12.h
 *
 *  Created on: 27.03.2016
 *      Author: sn
 */

#ifndef SRC_RFM12_H_
#define SRC_RFM12_H_

#include "mbed.h"

#define RF12FREQ(freq)	((freq-860.0)/0.005)							// macro for calculating frequency value out of frequency in MHz

class RFM12
{
public:
	RFM12(SPI &spi);
	void initOOK();
	void setBandwidth(unsigned char bandwidth, unsigned char gain, unsigned char drssi);
	void setFreq(unsigned short freq);
	void setBaud(unsigned short baud);
	void setPower(unsigned char power, unsigned char mod);
	void ready(void);
	void txData(unsigned char *data, unsigned char number);
	void rxData(unsigned char *data, unsigned char number);
	void rxOn();
	void powerDown();
	void txOn(void);
	void txOff(void);


private:
	SPI &_spi;
	void init();

};



#endif /* SRC_RFM12_H_ */
