#include "mbed.h"
#include "Adafruit_TFTLCD_16bit_STM32.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "demo.h"
#include "lv_test_theme_2.h"
#include "benchmark.h"
#include "touchXPT2046.h"
#include "libs/util/stepper.h"

#include "src/lvScreens/lvSplashScreen.h"
#include "src/lvScreens/lvParameterScreen.h"

#if !defined(MBED_CPU_STATS_ENABLED) || !defined(DEVICE_LPTICKER) || !defined(DEVICE_SLEEP)
#error [NOT_SUPPORTED] test not supported
#endif

#define USE_HW_TIMER


Thread threadIO;
Thread threadLvHandler;
Thread threadStepper;
Ticker tickerStepper;
Ticker tickerLvgl;

DigitalOut led1(LED1, 1); // onboard LEDs
DigitalOut led2(LED2, 1);
Adafruit_TFTLCD_16bit_STM32 tft(NC);

// global vars for stepper motor

bool motorOn = false;
int motorSpeed = 40;
int motorPos = 0;
int motorSetPos = 0;
int motorDirection = 0;
//DigitalOut motorStep(PE_5);      // stepping pulse
//DigitalOut motorDirection(PE_2); // stepping direction


//
// calc cpu usage
//

#define SAMPLE_TIME             2000    // msec
int cpuUsage = 0;

void calc_cpu_usage()
{
    static uint64_t prev_idle_time = 0;
    mbed_stats_cpu_t stats;
    mbed_stats_cpu_get(&stats);

    uint64_t diff = (stats.idle_time - prev_idle_time);
    int idle = (diff * 100) / (SAMPLE_TIME*1000);    // usec;
    cpuUsage = 100 - ((diff * 100) / (SAMPLE_TIME*1000));    // usec;;
    prev_idle_time = stats.idle_time;

    printf("Idle: %4d Usage: %4d \n", idle, cpuUsage);
}



//
// lv Ticker, executed in interrupt context
//

void fnLvTicker()
{
    lv_tick_inc(2); /*Tell LittlevGL that 2 milliseconds were elapsed*/
}

//
// IO Thread
// control tft backlight by button K1

void threadFnIO()
{
    DigitalIn inButtonK1(PE_3, PullUp);

    PwmOut outTftBL(PB_1);
    float delta = 0.1f;

    outTftBL.period_ms(1); // PWM freq 1kHz
    outTftBL = 1.0f;       // Backlight init with 100 %
    int btnK1Old = inButtonK1.read();

    while (true) {
        int btnK1 = inButtonK1.read();

        if ((btnK1 != btnK1Old) && (btnK1 == 0)) {
            //outTftBL = !outTftBL;
            if (outTftBL > 0.9f) {
                delta = -0.1f;
            } else if (outTftBL < 0.1f) {
                delta = 0.1f;
            }

            outTftBL = outTftBL + delta;
        }

        btnK1Old = btnK1;

        ThisThread::sleep_for(10);
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
        lv_task_handler();
        ThisThread::sleep_for(timeSlice);
        if (sleepTime_ms > 0) {
            elapsedTime += timeSlice;
        }
    }
}

typedef void (*lv_update_cb_t)(bool);

static void lv_screen_update_task(lv_task_t* task)
{
	static lv_obj_t* lastScreen = 0;
	bool firstStart;

	lv_obj_t* actScreen = lv_disp_get_scr_act(NULL);
	firstStart = (actScreen != lastScreen);
	lastScreen = actScreen;

	if (actScreen && actScreen->user_data) {
		((lv_update_cb_t)actScreen->user_data)(firstStart);
	}
}


// main() runs in its own thread in the OS
int main()
{
    printf("Hello from STM32F407VE\n");

    // Request the shared queue
    EventQueue *stats_queue = mbed_event_queue();

    stats_queue->call_every(SAMPLE_TIME, calc_cpu_usage);

    tft.begin();
    tft.setRotation(1);

    tft.fillScreen(RED);

    initTouchXPT2046();

    lv_init();
    lv_port_disp_init();
	// register update handler
	lv_task_create(lv_screen_update_task, 200, LV_TASK_PRIO_MID, 0);

    // setup touchpad
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);          // Basic initialization
    indev_drv.type = LV_INDEV_TYPE_POINTER; // touchpad
    indev_drv.read_cb = readTouchXPT2046;

    /* Register the driver in LittlevGL and save the created input device object*/
    lv_indev_t *my_indev = lv_indev_drv_register(&indev_drv);

    /* Set cursor */
    lv_obj_t *mouse_cursor = lv_img_create(lv_disp_get_scr_act(NULL), NULL);
    lv_img_set_src(mouse_cursor, LV_SYMBOL_PLUS);
    lv_indev_set_cursor(my_indev, mouse_cursor);

    // start threads
    threadIO.start(callback(threadFnIO));
    tickerLvgl.attach_us(&fnLvTicker, 2000);

    //lv_tutorial_hello_world();
    demo_create();
    //lv_test_theme_2();
    //benchmark_create();

    sleepWithLvHandler(0); // sleep forever and call lv_handler
}
