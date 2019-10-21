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
 * GfxItem.h
 *
 *  Created on: 19.11.2017
 *      Author: sn
 */

#ifndef LIB_CHART_GFXITEM_H_
#define LIB_CHART_GFXITEM_H_

#include "Adafruit_GFX.h"
//#include <vector>

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

class GfxItem;

class GfxItem {
public:
	GfxItem(Adafruit_GFX &gfx, int x, int y, int w, int h, uint16_t color);	// root object

	GfxItem(GfxItem *parent, int x, int y, int w, int h, uint16_t color);

	inline int x0(){return _x;};
	inline int y0(){return _y;};
	inline int width(){return _w;};
	inline int height(){return _h;};
	inline int color(){return _color;};
	inline uint32_t changeCount() {return _changeCount;};

	virtual void draw() {};

	GfxItem *_parent;
	Adafruit_GFX &_gfx;

protected:
	int _x;
	int _y;
	int _w;
	int _h;
	uint32_t _changeCount;
	uint16_t _color;
};




#endif /* LIB_CHART_GFXITEM_H_ */
