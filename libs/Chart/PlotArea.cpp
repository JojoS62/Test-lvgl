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
 * PlotArea.cpp
 *
 *  Created on: 19.11.2017
 *      Author: sn
 */

#include "PlotArea.h"

PlotArea::PlotArea(GfxItem& parent, int offsetLeft, int offsetRight, int offsetTop, int offsetBottom, uint16_t color) :
        GfxItem(&parent, parent.x0() + offsetLeft, parent.y0() + offsetTop, parent.width() - offsetLeft - offsetRight,
                parent.height() - offsetTop - offsetBottom, color) {
    _gridVertical = 0;
    _gridHorizontal = 0;
    _yScale = 0;
}

void PlotArea::draw() {
    _gfx.fillRect(_x, _y, _w, _h, _color);

    if (_gridVertical)
        _gridVertical->draw();

    if (_gridHorizontal)
        _gridHorizontal->draw();

    if (_yScale)
        _yScale->draw();
    _changeCount++;
}

Grid& PlotArea::addGridVertical(float division, uint16_t color) {
    _gridVertical = new Grid(this, vertical, division, color);
    return *_gridVertical;
}

Grid& PlotArea::addGridHorizontal(float division, uint16_t color) {
    _gridHorizontal = new Grid(this, horizontal, division, color);
    return *_gridHorizontal;
}

YScale& PlotArea::addYScale(int offset, float minValue, float maxValue, uint16_t color) {
    _yScale = new YScale(this, offset, minValue, maxValue, color);
    return *_yScale;
}
