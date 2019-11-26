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

#include "LVGLTouchDriverXPT2046.h"

/*********************
 *      DEFINES
 *********************/
#define CMD_X_READ  0b10010000
#define CMD_Y_READ  0b11010000

#define XPT2046_HOR_RES     320
#define XPT2046_VER_RES     240
#define XPT2046_X_MIN       200
#define XPT2046_Y_MIN       260
#define XPT2046_X_MAX       3750
#define XPT2046_Y_MAX       3860
#define XPT2046_INV         0

#define XPT2046_XY_SWAP     0
#define XPT2046_X_INV       1
#define XPT2046_Y_INV       1

LVGLTouchDriverXPT2046::LVGLTouchDriverXPT2046(PinName mosi, PinName miso, PinName sclk, PinName ssel, 
                                               PinName penIRQ, 
                                               LVGLDispDriver *lvglDispDriver) :
    LVGLInputDriver(lvglDispDriver),
    _spi(mosi, miso, sclk),
    _irqTouch(penIRQ),
    _csTouch(ssel, 1)
{
    _spi.format(8, 0);
    _spi.frequency(1000000);   

    // enable IRQ
    _csTouch = 0;
    _spi.write(0x80); // Enable PENIRQ
    _csTouch = 1;

    _indev_drv.type = LV_INDEV_TYPE_POINTER; // touchpad
    _indev_drv.read_cb = read;
    _indev_drv.user_data = this;
    /* Register the driver in LittlevGL and save the created input device object*/
    _my_indev = lv_indev_drv_register(&_indev_drv);
}

bool LVGLTouchDriverXPT2046::read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static int16_t last_x = 0;
    static int16_t last_y = 0;
    
    int16_t x = 0;
    int16_t y = 0;
    LVGLTouchDriverXPT2046 *me = (LVGLTouchDriverXPT2046 *)indev_drv->user_data;
    
    uint8_t irq = (me->_irqTouch).read();

    if(irq == 0) {
        uint8_t temp;

        me->_csTouch = 0;  // select

        // We use 7-bits from the first byte and 5-bit from the second byte
        me->_spi.write(CMD_X_READ); // Send command

        temp = me->_spi.write(0);
        x = temp<<8;
        temp = me->_spi.write(CMD_Y_READ);
        x |= temp;
        x >>=3;

        temp = me->_spi.write(0);
        y = temp<<8;
        temp = me->_spi.write(0);
        y |= temp;
        y >>=3;
        
        data->raw.x = x;
        data->raw.y = y;

        me->corr(&x, &y);
        me->avg(&x, &y);

        last_x = x;
        last_y = y;
        me->_csTouch = 1;  // deselect

        data->state = LV_INDEV_STATE_PR;

    } else {
        x = last_x;
        y = last_y;
        me->avg_last = 0;
        data->state = LV_INDEV_STATE_REL;
    }

    data->point.x = x;
    data->point.y = y;

    return false;
}

void LVGLTouchDriverXPT2046::corr(int16_t * x, int16_t * y)
{
#if XPT2046_XY_SWAP != 0
    int16_t swap_tmp;
    swap_tmp = *x;
    *x = *y;
    *y = swap_tmp;
#endif

    if((*x) > XPT2046_X_MIN)(*x) -= XPT2046_X_MIN;
    else(*x) = 0;

    if((*y) > XPT2046_Y_MIN)(*y) -= XPT2046_Y_MIN;
    else(*y) = 0;

    (*x) = (uint32_t)((uint32_t)(*x) * XPT2046_HOR_RES) /
           (XPT2046_X_MAX - XPT2046_X_MIN);

    (*y) = (uint32_t)((uint32_t)(*y) * XPT2046_VER_RES) /
           (XPT2046_Y_MAX - XPT2046_Y_MIN);

#if XPT2046_X_INV != 0
    (*x) =  XPT2046_HOR_RES - (*x);
#endif

#if XPT2046_Y_INV != 0
    (*y) =  XPT2046_VER_RES - (*y);
#endif


}


void LVGLTouchDriverXPT2046::avg(int16_t * x, int16_t * y)
{
    /*Shift out the oldest data*/
    uint8_t i;
    for(i = XPT2046_AVG - 1; i > 0 ; i--) {
        avg_buf_x[i] = avg_buf_x[i - 1];
        avg_buf_y[i] = avg_buf_y[i - 1];
    }

    /*Insert the new point*/
    avg_buf_x[0] = *x;
    avg_buf_y[0] = *y;
    if(avg_last < XPT2046_AVG) avg_last++;

    /*Sum the x and y coordinates*/
    int32_t x_sum = 0;
    int32_t y_sum = 0;
    for(i = 0; i < avg_last ; i++) {
        x_sum += avg_buf_x[i];
        y_sum += avg_buf_y[i];
    }

    /*Normalize the sums*/
    (*x) = (int32_t)x_sum / avg_last;
    (*y) = (int32_t)y_sum / avg_last;
}
