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

#ifndef __lvglDriverBase_h__
#define __lvglDriverBase_h__

#include "mbed.h"
#include "lvgl.h"


class LVGLDispDriver {
public:
    LVGLDispDriver();
    lv_disp_t* getLVDisp() { return _disp; };
    lv_disp_drv_t* getLVDispDrv() { return &_disp_drv; };

    /** Return the default diplay device
     *
     * Returns the default display driver based on the configuration JSON.
     * Use the components in target.json or application config to change
     * the default display device.
     *
     * An application can override all target settings by implementing
     * lvglDispDriver::get_default_instance() - the default
     * definition is weak, and calls get_target_default_instance().
    */
    static LVGLDispDriver *get_target_default_instance();


protected:
    LVGLDispDriver(uint32_t horRes, uint32_t verRes);

    virtual void init() {};

    lv_disp_drv_t _disp_drv;
    lv_disp_t   *_disp;
};

#endif
