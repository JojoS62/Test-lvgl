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

DigitalOut	led1(LED1);                 // onboard LEDs
DigitalOut	led2(LED2);
Adafruit_TFTLCD_16bit_STM32 tft(NC);

// global vars for stepper motor 

bool motorOn = false;
int motorSpeed = 40;
DigitalOut motorStep(PE_5);             // stepping pulse
DigitalOut motorDirection(PE_2);        // stepping direction


//
// stepper motor control
//

void fnStepper() {
    static int stepCount = 0;

    if (!motorOn)
        return;

    if (stepCount++ >= motorSpeed) {
        stepCount = 0;
         motorStep = !motorStep;
    }
}

//
// lv Ticker, executed in interrupt context
//

void fnLvTicker ()
{
    lv_tick_inc(2);      /*Tell LittlevGL that 2 milliseconds were elapsed*/
}


//
// lv_task_hanlder thread
//  needs mutex to avoid hardfault!
//

void threadFnLvHandler()
{
    while(1) {
		led1 = !led1;
        lv_task_handler();
		ThisThread::sleep_for(10);
	}
}


//
// IO Thread
// control tft backlight by button K1

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

//
//  print raw and calibrated touch coordinates
//   usess a modified lv_index_data_t, added raw coordinates
//

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

//
//  sleep and call lvHandler cyclic
//   sleeptime_ms: >0: sleep milliseconds, 0: sleep forever

void sleepWithLvHandler(uint32_t sleepTime_ms)
{
    uint32_t elapsedTime = 0;
    const uint32_t timeSlice = 5;

    while (elapsedTime <= sleepTime_ms) {
        led1 = !led1;
        lv_task_handler();
        ThisThread::sleep_for(timeSlice);
        if (sleepTime_ms > 0)
            elapsedTime += timeSlice;
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
    //threadLvHandler.start(callback(threadFnLvHandler));

	//lv_tutorial_hello_world();
    //demo_create();
    //lv_test_theme_2();
    //benchmark_create();
    
	lvSplashScreen();
    sleepWithLvHandler(2000);
    lvParameterScreen();

    sleepWithLvHandler(0);  // sleep forever and call lv_handler
}

