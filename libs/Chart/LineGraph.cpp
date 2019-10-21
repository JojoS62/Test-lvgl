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
 * LineChart.cpp
 *
 *  Created on: 4 Jan 2018
 *      Author: sn
 */

#include <LineGraph.h>
//#include <math.h>

LineGraph::LineGraph(GfxItem& parent, DataBuffer* data, YScale& yScale, uint16_t color) :
	GraphBase(parent, data, yScale, color) {
	_stopIndex = data->stopIndex();
	_dY = (float)_h / (yScale.maxValue() - yScale.minValue());
}

void LineGraph::draw() {
    // default: draw deltas. Limit startIndex to >= 0
    int startIndex = (_stopIndex < 0) ? 0 : _stopIndex;

	// clear background if new data
	if (_data->stopIndex() < _stopIndex) {
	    if (_changeCount == _parent->changeCount()) {
	        _parent->draw();
	    }

		startIndex = 0;     //
	};
    _changeCount = _parent->changeCount();

	// exit if no data
	if (_data->stopIndex() < 0)
		return;

	int xLast, yLast;
	// draw from last index to new datalength
	int x =_x + startIndex;
	for (int i = startIndex; i <= _data->stopIndex(); i++) {
	    // get value and check clipping
	    float value = _data->_data[i];
        value = fmax(value, _yScale->minValue());
        value = fmin(value, _yScale->maxValue());

		int y = (_y + _h) - (value - _yScale->minValue()) * _dY ;
		if (i == startIndex) {
	        _gfx.drawPixel(x, y, _color);
	        xLast = x;
	        yLast = y;
		}
		else {
            _gfx.drawLine(xLast, yLast, x, y, _color);
            _gfx.drawLine(xLast, yLast-1, x, y-1, _color); // test, stroke 2 pixel
            xLast = x;
            yLast = y;
		}
		x++;
	}

	_stopIndex = _data->stopIndex();
}
