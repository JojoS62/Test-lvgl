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
 * DataBuffer.h
 *
 *  Created on: 18 Dec 2017
 *      Author: sn
 */

#ifndef LIB_CHART_DATABUFFER_H_
#define LIB_CHART_DATABUFFER_H_

#include <string.h>

class DataBuffer {
public:
	DataBuffer(int maxPoints, float fillValue=0.0f);
	void reset();
	void addPoint(float value);
	bool isFull() {return _stopIndex==_indexMax;};
	void compress();
    int indexMax(){return _indexMax;};
    int stopIndex(){return _stopIndex;};
    int lpp(){return _lpp;};

	float* _data;

private:
	int _indexMax;
	int _startIndex;
	int _stopIndex;
	float X0;
	float deltaX;
	int _lpp;
	int _sumIndex;
	double _sum;
};


#endif /* LIB_CHART_DATABUFFER_H_ */
