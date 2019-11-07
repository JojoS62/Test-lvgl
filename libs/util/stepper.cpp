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
#include "stepper.h"
#include "mbed.h"

// define the Stepper Motor save start/stop speed
#define START_STOP_SPEED 1000

// define Library version number
#define VERSION 0.3

#define CLK_DELAY   10

// led4, used for testing the direction signal
//DigitalOut led4(LED4);
//DigitalOut sync(P1_27);

stepper::stepper(PinName clk, PinName dir) : _clk(clk), _dir(dir)
{
    _clk = 0, _dir = 0;
}

void stepper::step(int n_steps, bool direction, int speed, bool accel)
{
    int accelspeed;
    if (accel) {
        accelspeed = START_STOP_SPEED;
    } else {
        accelspeed = speed;
    }
    for (int i = 0; i < n_steps; i++) {
        if (accel) { //linear acceleration
            if (i < START_STOP_SPEED) if (--accelspeed <= speed) {
                    accelspeed++;
                }
            if (i > (n_steps - (START_STOP_SPEED-speed))) if (++accelspeed >= START_STOP_SPEED) {
                    accelspeed--;
                }
        }
        //_dir = led4 = direction;
        _dir = direction;
        _clk = 1;
        wait_us(CLK_DELAY);
        _clk = 0;
        wait_us(CLK_DELAY);
        wait_us(accelspeed);
    }
}

// version() returns the version number of the library
float stepper::version(void)
{
    return VERSION;
}