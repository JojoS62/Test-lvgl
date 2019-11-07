/* mbed Stepper Library
 * Copyright (c) 2010 fachatz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MBED_STEPPER_H
#define MBED_STEPPER_H

#include "mbed.h"

/** Stepper control class 
 *
 * Example:
 * @code
 * // apply number of steps, direction, speed and 
 * // a linear acceleration/deceleration to a Stepper Motor Controller
 * #include "mbed.h"
 * #include "stepper.h"
 *
 * #define ACCEL_ON  1
 * #define ACCEL_OFF 0
 * #define SPEED 50
 * 
 * stepper x(P18,P21);
 * stepper y(P19,P22);
 * stepper z(P20,P23);
 *
 * int main()
 * {
 *   x.step(1000,1,SPEED,ACCEL_ON); 
 *   y.step(5000,0,SPEED,ACCEL_ON); 
 *   z.step(2000,1,SPEED,ACCEL_ON);
 *
 * }
 * @endcode
 */
class stepper {

public:

    /** Create a stepper object connected to the specified clk pin and dir pin
     *
     * @param pin clk pin to connect to 
     * @param pin dir pin to connect to  
     */
    stepper(PinName clk, PinName dir);
    
    /** Set number of steps to direction with speed and 
     *  a linear acceleration/deceleration [on/off] 
     *  to a Stepper Motor Controller
     *
     * @param n_steps number of steps to go
     * @param direction 1 or 0
     * @param time value for value in us
     * @param accel 1 or 0 for accel/decel [on/off]
     */
    void step(int n_steps, bool direction, int speed, bool accel);  
    
    /** get version number
     *
     * @param returns Library version number
     */
    float version(void);   
  
private:  
    DigitalOut _clk;
    DigitalOut _dir;
     
};

#endif