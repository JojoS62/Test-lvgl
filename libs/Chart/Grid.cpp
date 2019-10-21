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
 * Grid.cpp
 *
 *  Created on: 18.11.2017
 *      Author: sn
 */

#include "Grid.h"

Grid::Grid(GfxItem* parent, GridOrientation orientation, float division, uint16_t color) :
	GfxItem(parent, parent->x0(), parent->y0(), parent->width(), parent->height(), color),
	_orientation(orientation),
	_division(division) {
}

void Grid::draw() {
	if (_orientation == vertical) {
		float x = _x;
		while (x < (_x + _w  - 1)) {
			_gfx.drawFastVLine(x, _y, _h, _color);
			x += _division;
		}
	} else
		if (_orientation == horizontal) {
			float y = _y + _h - 1;
			while (y > (_y  - 1)) {
				_gfx.drawFastHLine(_x, y, _w, _color);
				y -= _division;
			}
		};
}
