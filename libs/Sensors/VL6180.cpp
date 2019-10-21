#include "VL6180.h"
#include "mbed.h"

VL6180::VL6180(PinName sda, PinName scl) : _i2c(sda, scl) {
    char poweron;
    poweron = readI2C(0x16);
    if(poweron == 1) {
        //mandatory initialization
        writeI2C(0x0207,0x01);
        writeI2C(0x0208,0x01);
        writeI2C(0x0096,0x00);
        writeI2C(0x0097,0xfd);
        writeI2C(0x00e3,0x00);
        writeI2C(0x00e4,0x04);
        writeI2C(0x00e5,0x02);
        writeI2C(0x00e6,0x01);
        writeI2C(0x00e7,0x03);
        writeI2C(0x00f5,0x02);
        writeI2C(0x00d9,0x05);
        writeI2C(0x00db,0xce);
        writeI2C(0x00dc,0x03);
        writeI2C(0x00dd,0xf8);
        writeI2C(0x009f,0x00);
        writeI2C(0x00a3,0x3c);
        writeI2C(0x00b7,0x00);
        writeI2C(0x00bb,0x3c);
        writeI2C(0x00b2,0x09);
        writeI2C(0x00ca,0x09);
        writeI2C(0x0198,0x01);
        writeI2C(0x01b0,0x17);
        writeI2C(0x01ad,0x00);
        writeI2C(0x00ff,0x05);
        writeI2C(0x0100,0x05);
        writeI2C(0x0199,0x05);
        writeI2C(0x01a6,0x1b);
        writeI2C(0x01ac,0x3e);
        writeI2C(0x01a7,0x1f);
        writeI2C(0x0030,0x00);
        //recommended initialization
        writeI2C(0x0011,0x10);// Enables polling for ‘New Sample ready’ when measurement completes
        writeI2C(0x010a,0x30);// Set the averaging sample period (compromise between lower noise and increased execution time)
        writeI2C(0x003f,0x46);// Sets the light and dark gain (upper nibble). Dark gain should not be changed.
        writeI2C(0x0031,0xFF);// sets the # of range measurements after which auto calibration of system is performed 
        writeI2C(0x0040,0x63);// Set ALS integration time to 100ms
        writeI2C(0x002e,0x01);// perform a single temperature calibration of the ranging sensor 
        //optional initialization
        writeI2C(0x001b,0x09);// Set default ranging inter-measurement period to 100ms
        writeI2C(0x003e,0x31);// Set default ALS inter-measurement period to 500ms
        writeI2C(0x0014,0x24);// Configures interrupt on ‘New Sample Ready threshold event’
        
        writeI2C(0x016,0x00);//change fresh out of set status to 0
    }
}

float VL6180::read() {
    char status;
    char retn;
    
    writeI2C(0x18, 0x01);
    
    status = readI2C(0x4F);
    while((status & 0x7) != 4) {
        status = readI2C(0x4F);
    }
    
    retn = readI2C(0x62);
    
    writeI2C(0x15, 0x07);
    
    return (float)retn / 10.0;
}

VL6180::operator float() {
    return read();
}


void VL6180::writeI2C(int reg, char data) {
    char dataout[3];
    
    dataout[0] = (reg >> 8) & 0xFF;
    dataout[1] = reg & 0xFF;
    dataout[2] = data & 0xFF;
    
    _i2c.write(_addr, dataout, 3);
}

char VL6180::readI2C(int reg) {
    char dataout[2];
    char datain[1];
    
    dataout[0] = (reg >> 8) & 0xFF;
    dataout[1] = reg & 0xFF;
    
    _i2c.write(_addr, dataout, 2);
    _i2c.read(_addr, datain, 1);
    
    return datain[0];
}
    