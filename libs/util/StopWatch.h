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
 * StopWatch.h
 *
 *  Created on: 10.02.2018
 *      Author: sn
 */

#ifndef LIB_UTIL_STOPWATCH_H_
#define LIB_UTIL_STOPWATCH_H_

#include "mbed.h"

class StopWatch
{
public:
    StopWatch(int nLapTimes=8);
    void start(bool reset=true);
    void stop();
    us_timestamp_t stopLapTime();
    us_timestamp_t getLapTime(int n);
    int64_t getLapTimeDiff(int n);

private:
    Timer timer;
    us_timestamp_t *lapTimes;
    int maxLapTimes;
    int actLapTime;
    int offset;
};

#endif /* LIB_UTIL_STOPWATCH_H_ */
