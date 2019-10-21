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
 * Scale.h
 *
 *  Created on: 18.11.2017
 *      Author: sn
 */

#ifndef LIB_CHART_SCALE_H_
#define LIB_CHART_SCALE_H_

#include "GfxItem.h"

class YScale  : public GfxItem {
public:
	YScale(GfxItem* parent, int offsetLeft, float minValue, float maxValue, uint16_t color);

    float minValue() { return _minValue; };
    float maxValue() { return _maxValue; };
    void setMinValue(float value);
    void setMaxValue(float value);

	virtual void draw();

private:
	float	_minValue;
	float	_maxValue;
};



#endif /* LIB_CHART_SCALE_H_ */
