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
 * Scale.cpp
 *
 *  Created on: 18.11.2017
 *      Author: sn
 */


#include "Scale.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

YScale::YScale(GfxItem* parent, int offsetLeft, float minValue, float maxValue, uint16_t color) :
	GfxItem(parent, parent->x0(), parent->y0(), parent->width(), parent->height(), color),
	_minValue(minValue),
	_maxValue(maxValue)
{
}

void YScale::draw() {
//	int nScaleValues = 5;
//	int digits = 3;
	int16_t x1=_x, y1=_y;
	uint16_t w=0, h=0;
	char sValue[16];

	sprintf(sValue, "%4.0f", _maxValue);
	_gfx.getTextBounds(sValue, x1, y1, &x1, &y1, &w, &h);

	x1 -= 2;	// offset to grid
	x1 -= w;	// left pos to grid

	_gfx.setFont();	// TODO: set YScale font
	_gfx.setTextColor(_color);

	_gfx.setCursor(x1, y1);
	_gfx.println(sValue);

	_gfx.setCursor(x1, y1+_h-h);
	_gfx.printf("%4.0f", _minValue);
}

void YScale::setMinValue(float value) {
    _minValue = value;
}

void YScale::setMaxValue(float value) {
    _maxValue = value;
}
