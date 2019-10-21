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
 * DataSeries.cpp
 *
 *  Created on: 18 Dec 2017
 *      Author: sn
 */

#include "DataBuffer.h"

DataBuffer::DataBuffer(int maxPoints, float fillValue) {
	_data = new float[maxPoints];
	_indexMax = maxPoints - 1;
	reset();
}

void DataBuffer::addPoint(float value) {
    // integrate until lpp reached
    _sumIndex++;
    _sum += value;
    if (_sumIndex < _lpp) {
        return;
    }
    else {
        value = _sum / _sumIndex;
        _sum = 0.0;
        _sumIndex = 0;
    }

    // add to data buffer
	if (_stopIndex < _indexMax) {
		_stopIndex++;
		_data[_stopIndex] = value;
	}
}

void DataBuffer::reset() {
	_startIndex = 0;
	_stopIndex = -1;
	_lpp = 1;
	_sumIndex = 0;
	_sum = 0.0;
}

void DataBuffer::compress() {
    for (int i=0; i<_stopIndex; i++) {
        _data[i] = (_data[2*i] + _data[2*i+1]) / 2.0f;
    }
    _stopIndex /= 2;
    _lpp *= 2;
}
