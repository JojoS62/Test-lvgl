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

#include "LVGLTouchDriverDISCO_F746NG.h"

#include "TS_DISCO_F746NG.h"

TS_DISCO_F746NG ts;

/*********************
 *      DEFINES
 *********************/

LVGLTouchDriverDISCO_F746NG::LVGLTouchDriverDISCO_F746NG(LVGLDispDriver *lvglDispDriver) :
    LVGLInputDriver(lvglDispDriver)
{
    ts.Init(480, 272);

    _indev_drv.type = LV_INDEV_TYPE_POINTER; // touchpad
    _indev_drv.read_cb = read;
    _indev_drv.user_data = this;
    /* Register the driver in LittlevGL and save the created input device object*/
    _my_indev = lv_indev_drv_register(&_indev_drv);
}

bool LVGLTouchDriverDISCO_F746NG::read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    TS_StateTypeDef TS_State;

    ts.GetState(&TS_State);

    if (TS_State.touchDetected) {
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    data->point.x = TS_State.touchX[0];
    data->point.y = TS_State.touchY[0];

    return false;
}

MBED_WEAK LVGLInputDriver *LVGLInputDriver::get_target_default_instance_touchdrv(LVGLDispDriver *disp)
{
    static LVGLTouchDriverDISCO_F746NG drv(disp);
    return &drv;
}
