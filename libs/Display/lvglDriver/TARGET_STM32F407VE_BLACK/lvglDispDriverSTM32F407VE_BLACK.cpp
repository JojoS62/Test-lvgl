
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

#include "LVGLDispDriverSTM32F407VE_BLACK.h"
#include "ili9341_fsmc.h"

/*
    use FSMC
*/

volatile uint16_t *ili9341_fsmcCommand;
volatile uint16_t *ili9341_fsmcData;

static int fsmc_lcd_init()
{
    // enable clocks
    // workaround, the HAL macros produce warnings
    volatile uint32_t dummy;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    dummy = RCC->AHB1ENR;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    dummy = RCC->AHB1ENR;
    RCC->AHB3ENR |= RCC_AHB3ENR_FSMCEN;
    dummy = RCC->AHB3ENR;
    dummy = dummy;

    // set GPIO Alternate Function FSMC
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Alternate = GPIO_AF12_FSMC;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 |
                             GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                             GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                             GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |
                             GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

    FSMC_NORSRAM_InitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMInitStructure.NSBank = FSMC_NORSRAM_BANK1;                       // ??
    FSMC_NORSRAMInitStructure.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
    FSMC_NORSRAMInitStructure.MemoryType = FSMC_MEMORY_TYPE_SRAM;
    FSMC_NORSRAMInitStructure.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
    FSMC_NORSRAMInitStructure.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;    // disable
    FSMC_NORSRAMInitStructure.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
    FSMC_NORSRAMInitStructure.WrapMode = FSMC_WRAP_MODE_DISABLE;
    FSMC_NORSRAMInitStructure.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
    FSMC_NORSRAMInitStructure.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
    FSMC_NORSRAMInitStructure.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
    FSMC_NORSRAMInitStructure.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
    FSMC_NORSRAMInitStructure.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
    FSMC_NORSRAMInitStructure.WriteBurst = FSMC_WRITE_BURST_DISABLE;  // enable?
    FSMC_NORSRAMInitStructure.ContinuousClock = FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC;
    FSMC_NORSRAMInitStructure.WriteFifo = FSMC_WRITE_FIFO_ENABLE;
    FSMC_NORSRAMInitStructure.PageSize = 0;

    HAL_StatusTypeDef status = HAL_OK;
    status = FSMC_NORSRAM_Init(FSMC_Bank1, &FSMC_NORSRAMInitStructure);

    FSMC_NORSRAM_TimingTypeDef FSMC_NORSRAMTimingInitStructure;
    FSMC_NORSRAMTimingInitStructure.AddressSetupTime = 7; // 100 ns (ADDSET+1)*12.5ns = CS to RW
    FSMC_NORSRAMTimingInitStructure.AddressHoldTime = 0;
    FSMC_NORSRAMTimingInitStructure.DataSetupTime = 3;    // 50 ns (DATAST+1)*12.5ns = RW length
    FSMC_NORSRAMTimingInitStructure.BusTurnAroundDuration = 0;
    FSMC_NORSRAMTimingInitStructure.CLKDivision = 0;
    FSMC_NORSRAMTimingInitStructure.DataLatency = 0;
    FSMC_NORSRAMTimingInitStructure.AccessMode = FSMC_ACCESS_MODE_A;
    FSMC_NORSRAM_Init(FSMC_Bank1, &FSMC_NORSRAMInitStructure);

    status = FSMC_NORSRAM_Timing_Init(FSMC_Bank1,  &FSMC_NORSRAMTimingInitStructure, FSMC_NORSRAM_BANK1);

    /* Enable FSMC NOR/SRAM Bank1 */
    __FSMC_NORSRAM_ENABLE(FSMC_Bank1, FSMC_NORSRAM_BANK1);

    ili9341_fsmcCommand    = (volatile uint16_t *)NOR_MEMORY_ADRESS1;   // clears A18
    ili9341_fsmcData       = (ili9341_fsmcCommand + (1 << 18));                 // sets A18

    return status;
};

LVGLDispSTM32F407VE_BLACK::LVGLDispSTM32F407VE_BLACK(uint32_t nBufferRows) :
    LVGLDispDriver(320, 240),
    _nBufferRows(nBufferRows)
{
    // low level hardware init
    fsmc_lcd_init();

    // tft controller init
    ili9341_fsmc_init();
    ili9341_fsmc_setRotation(1);

    init();
}

void LVGLDispSTM32F407VE_BLACK::init()
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

void LVGLDispSTM32F407VE_BLACK::disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t x;
    int32_t y;
    ili9341_fsmc_setAddrWindow(area->x1, area->y1, area->x2, area->y2);
    ili9341_fsmc_writeCmd(0x2C);
    for (y = area->y1; y <= area->y2; y++) {
        for (x = area->x1; x <= area->x2; x++) {
            ili9341_fsmc_writeData(*((uint16_t *)color_p));
            color_p++;
        }
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

MBED_WEAK LVGLDispDriver *LVGLDispDriver::get_target_default_instance()
{
    static LVGLDispSTM32F407VE_BLACK drv;
    return &drv;
}