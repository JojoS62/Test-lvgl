/*
 * IRSensorD6T.h
 *
 *  Created on: 14.10.2018
 *      Author: sn
 */

#ifndef LIB_IRSENSORD6T_H_
#define LIB_IRSENSORD6T_H_

#include "mbed.h"

namespace mbed {

class IRSensorD6T {
public:
    enum IRSensorType { D6T06, D6T44 };
    IRSensorD6T(I2C &i2c, IRSensorType sensorType=D6T06, int i2cAddress=0x14);
    virtual ~IRSensorD6T();
    int read(float *temperatures);
    int getDataCount();

private:
    I2C &_i2c;
    IRSensorType _sensorType;
    int _i2cAddress;
    //float *_temperatures;
    char *_rawBuffer;
    int _dataCount;
};

} /* namespace mbed */

#endif /* LIB_IRSENSORD6T_H_ */
