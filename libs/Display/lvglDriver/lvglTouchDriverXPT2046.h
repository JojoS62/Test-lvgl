/* lvglDriver for Mbed
 * Copyright (c) 2019 Johannes Stratmann
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __LVGLTouchDriverXPT2046_h__
#define __LVGLTouchDriverXPT2046_h__

#include "LVGLInputDriverBase.h"

#define XPT2046_AVG         4

class LVGLTouchDriverXPT2046 : public LVGLInputDriver {
public:
    /*
        construct touch driver from SPI pins
        Disp is optional, if null then lvgl will use default display
    */
    LVGLTouchDriverXPT2046(PinName mosi, PinName miso, PinName sclk, PinName ssel, PinName penIRQ, LVGLDispDriver *lvglDispDriver = nullptr);

private:
    SPI _spi;
    DigitalIn _irqTouch;
    DigitalOut _csTouch;

    int16_t avg_buf_x[XPT2046_AVG];
    int16_t avg_buf_y[XPT2046_AVG];
    uint8_t avg_last;

    static bool read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
    void corr(int16_t * x, int16_t * y);
    void avg(int16_t * x, int16_t * y);
};

#endif
