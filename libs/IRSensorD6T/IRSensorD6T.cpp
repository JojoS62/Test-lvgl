/*
 * IRSensorD6T.cpp
 *
 *  Created on: 14.10.2018
 *      Author: sn
 */

#include <IRSensorD6T.h>

namespace mbed {

IRSensorD6T::IRSensorD6T(I2C& i2c, IRSensorType sensorType, int i2cAddress) :
    _i2c(i2c),
    _sensorType(sensorType),
    _i2cAddress(i2cAddress)
{
    _dataCount = getDataCount();
    _rawBuffer = new char[2 + 2*_dataCount + 2];   // max. TAmbient + 16 pixel + crc + spare

    //_temperatures = new float[1 + _dataCount];  // TAmbient + IR pixel
}

IRSensorD6T::~IRSensorD6T() {
//    if (_temperatures) {
//        delete _temperatures;
//        _temperatures = NULL;
//    }
    if (_rawBuffer) {
        delete _rawBuffer;
        _rawBuffer = NULL;
    }
}

int IRSensorD6T::read(float* temperatures) {
    int err = 0;
    const char cmdReadData = 0x4c;

    memset(_rawBuffer, 0, 2 + 2*_dataCount + 2);

    err = _i2c.write(_i2cAddress, &cmdReadData, 1, true);
    if (err)
        return err;

    wait_us(100);

    err = _i2c.read(_i2cAddress, _rawBuffer, 19);
    if (err)
        return err;

    for (int i=0; i < (_dataCount); i++) {
        temperatures[i] = (_rawBuffer[i*2] + (_rawBuffer[1+i*2] << 8)) * 0.1f;
    }

    return err;
}

int IRSensorD6T::getDataCount() {
    switch (_sensorType) {
        case D6T06:
            return 1+8;
            break;
        case D6T44:
            return 1+16;
            break;
        default:
            return 0;
    }
}

} /* namespace mbed */
