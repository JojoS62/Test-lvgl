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

#ifndef __lvglInputDriverBase_h__
#define __lvglInputDriverBase_h__

#include "mbed.h"
#include "lvgl.h"
#include "lvglDispDriverBase.h"


class lvglInputDriverBase {
public:
        lv_disp_t* getLVDisp() { return _lvglDispDriverBase != nullptr ?_lvglDispDriverBase->getLVDisp() : nullptr; };
        lv_indev_drv_t* getLVInDev() { return &_indev_drv; };

protected:
    lvglInputDriverBase(lvglDispDriverBase *disp = nullptr);

    virtual void init() {};

    lv_indev_drv_t _indev_drv;
    lvglDispDriverBase*  _lvglDispDriverBase;
    lv_indev_t *_my_indev;
};

#endif