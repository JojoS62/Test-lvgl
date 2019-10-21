#ifndef MBED_VL6180_H
#define MBED_VL6180_H

#include "mbed.h"
/** VL6180 control class.
 *
 * Example:
 * @code
 * //Update pc terminal every half second with range reading
 * #include "VL6180.h"
 * #include "mbed.h"
 *
 * VL6180 rf(p9, p10); //I2C sda and scl
 * Serial pc(USBTX, USBRX); //USB serial
 *
 * int main() {
 *   float reading;
 *   while(1) {
 *      reading = rf;
 *      pc.printf("Read %4.1f cm\n", reading);
 *      wait(0.5);
 *  }
 * }
 * @endcode
 */
class VL6180 {
public:
    /** Create a VL6180 object at I2C address 0x29 (7 bit).
     * 
     * @param sda I2C sda pin number
     * @param scl I2C scl pin number
     */
    VL6180(PinName sda, PinName scl);
    
    /** Make a range reading.
     * 
     * @param return Distance to target in cm. Approx 0.0 cm to 20.0 cm.
     */
    float read();
    
    /** Float cast is shorthand for read() */
    operator float();
    
private:
    void writeI2C(int reg, char data);
    char readI2C(int reg);
    I2C _i2c;
    int _res;
    static const int _addr = 0x52;
};

#endif