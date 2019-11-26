#include "touchXPT2046.h"

/*********************
 *      DEFINES
 *********************/
#define CMD_X_READ  0b10010000
#define CMD_Y_READ  0b11010000

#define XPT2046_HOR_RES     320
#define XPT2046_VER_RES     240
#define XPT2046_X_MIN       200
#define XPT2046_Y_MIN       260
#define XPT2046_X_MAX       3750
#define XPT2046_Y_MAX       3860
#define XPT2046_AVG         4
#define XPT2046_INV         0

#define XPT2046_XY_SWAP     0
#define XPT2046_X_INV       1
#define XPT2046_Y_INV       1

#define XPT2046_MISO    PB_14
#define XPT2046_MOSI    PB_15
#define XPT2046_CLK     PB_13
#define XPT2046_CS      PB_12
#define XPT2046_IRQ     PC_5

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void xpt2046_corr(int16_t * x, int16_t * y);
static void xpt2046_avg(int16_t * x, int16_t * y);

/**********************
 *  STATIC VARIABLES
 **********************/
int16_t avg_buf_x[XPT2046_AVG];
int16_t avg_buf_y[XPT2046_AVG];
uint8_t avg_last;

//static DigitalOut csTouch(XPT2046_CS);
SPI spiTouch(XPT2046_MOSI, XPT2046_MISO, XPT2046_CLK); 
DigitalOut csTouch(XPT2046_CS, 1);
DigitalIn irqTouch(XPT2046_IRQ, PullUp);

void initTouchXPT2046()
{
    spiTouch.format(8, 0);
    spiTouch.frequency(1000000);   

    // enable IRQ
    csTouch = 0;
    spiTouch.write(0x80); // Enable PENIRQ
    csTouch = 1;
}


bool readTouchXPT2046(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static int16_t last_x = 0;
    static int16_t last_y = 0;

    int16_t x = 0;
    int16_t y = 0;

    uint8_t irq = irqTouch.read();

    if(irq == 0) {
        uint8_t temp;

        csTouch = 0;  // select

        // We use 7-bits from the first byte and 5-bit from the second byte
        spiTouch.write(CMD_X_READ); // Send command

        temp = spiTouch.write(0);
        x = temp<<8;
        temp = spiTouch.write(CMD_Y_READ);
        x |= temp;
        x >>=3;

        temp = spiTouch.write(0);
        y = temp<<8;
        temp = spiTouch.write(0);
        y |= temp;
        y >>=3;
        
        data->raw.x = x;
        data->raw.y = y;

        xpt2046_corr(&x, &y);
        xpt2046_avg(&x, &y);

        last_x = x;
        last_y = y;
        csTouch = 1;  // deselect

        data->state = LV_INDEV_STATE_PR;

    } else {
        x = last_x;
        y = last_y;
        avg_last = 0;
        data->state = LV_INDEV_STATE_REL;
    }

    data->point.x = x;
    data->point.y = y;

    return false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void xpt2046_corr(int16_t * x, int16_t * y)
{
#if XPT2046_XY_SWAP != 0
    int16_t swap_tmp;
    swap_tmp = *x;
    *x = *y;
    *y = swap_tmp;
#endif

    if((*x) > XPT2046_X_MIN)(*x) -= XPT2046_X_MIN;
    else(*x) = 0;

    if((*y) > XPT2046_Y_MIN)(*y) -= XPT2046_Y_MIN;
    else(*y) = 0;

    (*x) = (uint32_t)((uint32_t)(*x) * XPT2046_HOR_RES) /
           (XPT2046_X_MAX - XPT2046_X_MIN);

    (*y) = (uint32_t)((uint32_t)(*y) * XPT2046_VER_RES) /
           (XPT2046_Y_MAX - XPT2046_Y_MIN);

#if XPT2046_X_INV != 0
    (*x) =  XPT2046_HOR_RES - (*x);
#endif

#if XPT2046_Y_INV != 0
    (*y) =  XPT2046_VER_RES - (*y);
#endif


}


static void xpt2046_avg(int16_t * x, int16_t * y)
{
    /*Shift out the oldest data*/
    uint8_t i;
    for(i = XPT2046_AVG - 1; i > 0 ; i--) {
        avg_buf_x[i] = avg_buf_x[i - 1];
        avg_buf_y[i] = avg_buf_y[i - 1];
    }

    /*Insert the new point*/
    avg_buf_x[0] = *x;
    avg_buf_y[0] = *y;
    if(avg_last < XPT2046_AVG) avg_last++;

    /*Sum the x and y coordinates*/
    int32_t x_sum = 0;
    int32_t y_sum = 0;
    for(i = 0; i < avg_last ; i++) {
        x_sum += avg_buf_x[i];
        y_sum += avg_buf_y[i];
    }

    /*Normalize the sums*/
    (*x) = (int32_t)x_sum / avg_last;
    (*y) = (int32_t)y_sum / avg_last;
}
