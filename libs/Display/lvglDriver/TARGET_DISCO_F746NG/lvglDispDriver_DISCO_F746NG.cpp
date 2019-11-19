
/* lvglDriver for Mbed
 * Copyright (c) 2019 Johannes Stratmann
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "lvglDispDriver_DISCO_F746NG.h"

lvglDispDISCO_F746NG::lvglDispDISCO_F746NG(uint32_t nBufferRows) :
    lvglDispDriverBase(480, 272),
    _nBufferRows(nBufferRows)
{
    lcd.Init();
    lcd.Clear(LCD_COLOR_BLUE);
    lcd.SetBackColor(LCD_COLOR_BLUE);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"DISCOVERY STM32F746NG", CENTER_MODE);

    init();
}

void lvglDispDISCO_F746NG::init()
{
    size_t bufferSize = LV_HOR_RES_MAX * _nBufferRows;

    // allocate memory for display buffer
    _buf1_1 = new lv_color_t[bufferSize];             /* a buffer for n rows */
    MBED_ASSERT(_buf1_1 != nullptr);
    memset(_buf1_1, 0, bufferSize*sizeof(lv_color_t));

    /*Used to copy the buffer's content to the display*/
    _disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    _disp_drv.buffer = &_disp_buf_1;

    lv_disp_buf_init(&_disp_buf_1, _buf1_1, NULL, bufferSize);   /* Initialize the display buffer */

    /*Finally register the driver*/
    _disp = lv_disp_drv_register(&_disp_drv);
}

void lvglDispDISCO_F746NG::disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t x;
    int32_t y;
    for (y = area->y1; y <= area->y2; y++) {
        for (x = area->x1; x <= area->x2; x++) {
//            ili9341_fsmc_writeData(*((uint16_t *)color_p));
            color_p++;
        }
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}