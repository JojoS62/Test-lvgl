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
 * GfxItem.cpp
 *
 *  Created on: 19.11.2017
 *      Author: sn
 */

#include "GfxItem.h"

GfxItem::GfxItem(Adafruit_GFX &gfx, int x, int y, int w, int h, uint16_t color) :
	_parent(0),
	_gfx(gfx),
	_x(x),
	_y(y),
	_w(w),
	_h(h),
	_changeCount(0),
	_color(color) {
}

GfxItem::GfxItem(GfxItem *parent, int x, int y, int w, int h, uint16_t color) :
	_parent(parent),
	_gfx(parent->_gfx),
	_x(x),
	_y(y),
	_w(w),
	_h(h),
    _changeCount(0),
	_color(color) {
}


