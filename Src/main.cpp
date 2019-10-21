#include "mbed.h"
#include "Adafruit_TFTLCD_16bit_STM32.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "demo.h"
#include "lv_test_theme_2.h"
#include "benchmark.h"
#include "touchXPT2046.h"

#include "src/myScreens/lvSplashScreen.h"
#include "src/myScreens/lvParameterScreen.h"

Thread threadIO;
Thread threadLvHandler;
Ticker tickerStepper;
Ticker tickerLvgl;

DigitalOut	led1(LED1);
DigitalOut	led2(LED2);
Adafruit_TFTLCD_16bit_STM32 tft(NC);

bool motorOn = false;
int motorSpeed = 40;
DigitalOut motorStep(PE_5);
DigitalOut motorDirection(PE_2);

void fnStepper() {
    static int stepCount = 0;

    if (!motorOn)
        return;

    if (stepCount++ >= motorSpeed) {
        stepCount = 0;
         motorStep = !motorStep;
    }
}

void fnLvTicker ()
{
    lv_tick_inc(2);      /*Tell LittlevGL that 2 milliseconds were elapsed*/
}

void threadFnLvHandler()
{
    while(1) {
		led1 = !led1;
        lv_task_handler();
		ThisThread::sleep_for(10);
	}
}

void threadFnIO() {
    DigitalIn inButtonK1(PE_3, PullUp);

    PwmOut outTftBL(PB_1);
    float delta = 0.1f;

    outTftBL.period_ms(1);  // PWM freq 1kHz
    outTftBL = 1.0f;  // Backlight init with 100 %
    int btnK1Old = inButtonK1.read();

    while (true) {
        int btnK1 = inButtonK1.read();

        if ((btnK1 != btnK1Old) && (btnK1 == 0)) {
            //outTftBL = !outTftBL;
            if (outTftBL > 0.9f)
                delta = -0.1f;
            else if (outTftBL < 0.1f)
                delta = 0.1f;

            outTftBL = outTftBL + delta;
        }

        btnK1Old = btnK1;

        ThisThread::sleep_for(10);
    }
}

void testTouch() 
{
    while(1) {
		led1 = !led1;
        lv_indev_data_t touchData;
        volatile bool isTouched = readTouchXPT2046(NULL, &touchData);

        if  (isTouched) {
            printf("x: %4d   y: %4d  x: %4d   y: %4d\n", touchData.raw.x, touchData.raw.y, touchData.point.x, touchData.point.y);
        }

		ThisThread::sleep_for(100);
	}
}

// main() runs in its own thread in the OS
int main() 
{
    printf("Hello from STM32F407VE\n");

    tft.begin();
    tft.setRotation(1);

    tft.fillScreen(RED);

	initTouchXPT2046();

    //testTouch();

    lv_init();
    lv_port_disp_init();

    // setup touchpad
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);              // Basic initialization
    indev_drv.type = LV_INDEV_TYPE_POINTER;     // touchpad
    indev_drv.read_cb = readTouchXPT2046;

    /* Register the driver in LittlevGL and save the created input device object*/
    lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv);

    /* Set cursor */
    lv_obj_t * mouse_cursor = lv_img_create(lv_disp_get_scr_act(NULL), NULL);
    lv_img_set_src(mouse_cursor, LV_SYMBOL_PLUS);
    lv_indev_set_cursor(my_indev, mouse_cursor);

    threadIO.start(callback(threadFnIO));
    tickerStepper.attach_us(&fnStepper, 10);
    tickerLvgl.attach_us(&fnLvTicker, 2000);
    threadLvHandler.start(callback(threadFnLvHandler));

	//lv_tutorial_hello_world();
    //demo_create();
    //lv_test_theme_2();
    //benchmark_create();
    
	lvSplashScreen();
    ThisThread::sleep_for(2000);
    lvParameterScreen();

    while(1) {
#if 0
   		led1 = !led1;
        lv_task_handler();
#endif
		ThisThread::sleep_for(10);
	}
}

