
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

#include "LVGLDispDriver_DISCO_F746NG.h"

static LCD_DISCO_F746NG lcd;
static lv_color_t xbuf[LV_HOR_RES_MAX * 272/4];

extern "C" void dmaXFerComplete(DMA2D_HandleTypeDef *hdma2d) 
{
    lv_disp_flush_ready(LVGLDispDriver::get_target_default_instance()->getLVDispDrv());
}


LVGLDispDISCO_F746NG::LVGLDispDISCO_F746NG(uint32_t nBufferRows) :
    LVGLDispDriver(480, 272),
    _nBufferRows(nBufferRows)
{
    lcd.Init();
    lcd.Clear(LCD_COLOR_BLUE);
    lcd.SetBackColor(LCD_COLOR_BLUE);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.FillRect(0, 10, 480, 100);
    //lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"DISCOVERY STM32F746NG", CENTER_MODE);

    init();
}

void LVGLDispDISCO_F746NG::init()
{
    size_t bufferSize = LV_HOR_RES_MAX * _nBufferRows;

    // allocate memory for display buffer
    //_buf1_1 = new lv_color_t[bufferSize];             /* a buffer for n rows */
    //MBED_ASSERT(_buf1_1 != nullptr);
    //memset(_buf1_1, 0, bufferSize*sizeof(lv_color_t));

    /*Used to copy the buffer's content to the display*/
    _disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    _disp_drv.buffer = &_disp_buf_1;

    lv_disp_buf_init(&_disp_buf_1, xbuf, NULL, bufferSize);   /* Initialize the display buffer */

    /*Finally register the driver*/
    _disp = lv_disp_drv_register(&_disp_drv);

    BSP_LCD_SetDMACpltCallback(dmaXFerComplete);
}

void LVGLDispDISCO_F746NG::disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t width = area->x2 - area->x1 + 1;
    uint32_t height =  area->y2 - area->y1 + 1;

    BSP_LCD_TransferBitmap(area->x1, area->y1, width, height, (uint32_t*)color_p);

#if 0
    // working, but slower than DMA xfer
    int32_t x;
    int32_t y;
    for (y = area->y1; y <= area->y2; y++) {
        for (x = area->x1; x <= area->x2; x++) {
            lcd.DrawPixel(x, y, *((uint32_t *)color_p));
            color_p++;
        }
    }
#endif
    
    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    // handled in DMA transfer complete interrupt
    // lv_disp_flush_ready(disp_drv);
}

MBED_WEAK LVGLDispDriver *LVGLDispDriver::get_target_default_instance()
{
    static LVGLDispDISCO_F746NG drv;
    return &drv;
}