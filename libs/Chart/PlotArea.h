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
 * PlotArea.h
 *
 *  Created on: 19.11.2017
 *      Author: sn
 */

#ifndef LIB_CHART_PLOTAREA_H_
#define LIB_CHART_PLOTAREA_H_

#include "GfxItem.h"
#include "Grid.h"
#include "Scale.h"

class PlotArea : public GfxItem {
public:
	PlotArea(GfxItem &parent, int offsetLeft, int offsetRight, int offsetTop, int offsetBottom, uint16_t color);

	Grid& addGridVertical(float division, uint16_t color);
	Grid& addGridHorizontal(float division, uint16_t color);
	YScale& addYScale(int offset, float minValue, float maxValue, uint16_t color);


	virtual void draw();

private:
	Grid* _gridVertical;
	Grid* _gridHorizontal;
	YScale* _yScale;
};



#endif /* LIB_CHART_PLOTAREA_H_ */
