/*
 MIT License

Copyright (c) 2018 JojoS

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*
 */

/*
 * StopWatch.cpp
 *
 *  Created on: 10.02.2018
 *      Author: sn
 */

#include "StopWatch.h"

StopWatch::StopWatch (int nLapTimes)
{
    actLapTime = 0;
    maxLapTimes = nLapTimes;
    lapTimes = new us_timestamp_t[nLapTimes];

    // measure start-stop delay
    offset = 0;
    start();
    int64_t t1 = stopLapTime();
    int64_t t2 = stopLapTime();
    offset = t2 - t1;
}

void StopWatch::start (bool reset)
{
    if (reset) {
        timer.stop();
        timer.reset();
        actLapTime = 0;
    }
    timer.start();
}

void StopWatch::stop ()
{
    timer.stop();
}

us_timestamp_t StopWatch::stopLapTime ()
{
    us_timestamp_t lapTime = timer.read_high_resolution_us();
    if (actLapTime < maxLapTimes)
        lapTimes[actLapTime++] = lapTime;

    return lapTime;
}

us_timestamp_t StopWatch::getLapTime (int n)
{
    if ((actLapTime >= 0)  && (n <= actLapTime))
        return lapTimes[n];
    else
        return 0;
}

int64_t StopWatch::getLapTimeDiff (int n)
{
    if ((actLapTime >= 1)  && (n <= actLapTime-1)) {
        int64_t delta = lapTimes[n+1] - lapTimes[n] - offset;
        return (delta < 0) ? 0 : delta;  // catch -1 µs due to small readout diff
    }
    else
        return 0;
}
