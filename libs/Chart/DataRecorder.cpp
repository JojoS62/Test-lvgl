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
 * DataRecorder.cpp
 *
 *  Created on: 6 Jan 2018
 *      Author: sn
 */

#include "mbed.h"
#include "DataRecorder.h"

DataRecorder::DataRecorder(uint32_t numPoints, RecordMode recordMode) :
	_recordMode(recordMode) {
	dataBuffer = new DataBuffer(numPoints);
}


void DataRecorder::addYValue(float value) {
	if (dataBuffer->isFull()) {
	    switch(_recordMode) {
	        case noWrap:
	            dataBuffer->reset();
	            break;

	        case compress:
	            dataBuffer->compress();
	            break;

	        default:
	            dataBuffer->reset();
	    }
	}
	dataBuffer->addPoint(value);
}


void DataRecorder::reset(float xMin, float xMax) {
}


