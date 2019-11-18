/* HWTimer class for Mbed
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

#ifndef __HWTimer_h__
#define __HWTimer_h__

#include "mbed.h"

class HWTimer {
public:
    HWTimer(int id, uint32_t period, Callback<void()> cb, bool onePulseMode);

    ~HWTimer();
    void start(uint32_t period = 0);
    void stop();

private:
    void callTimerFn();

private:
    int _id;
    bool _onePulseMode;         
    Callback<void()> _cb;       // user callback on timeout
    CThunk<HWTimer> _irq;       // helper for setting static irq member function
    void *_pConfig;             // generic pointer, used for implementation dependent structures
};
#endif // __HWTimer__h__