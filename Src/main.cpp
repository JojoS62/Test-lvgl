#include "mbed.h"
#include "libs/lvgl/lvgl.h"

#if defined(TARGET_STM32F407VE_BLACK)
#include "lvglDispDriverSTM32F407VE_BLACK.h"
#include "lvglTouchDriverXPT2046.h"
#elif defined(TARGET_DISCO_F746NG)
#endif

#include "libs/lvgl/lv_examples/lv_apps/demo/demo.h"
#include "libs/lvgl/lv_examples/lv_tests/lv_test_theme/lv_test_theme_2.h"
#include "libs/lvgl/lv_examples/lv_apps/benchmark/benchmark.h"
#include "libs/lvgl/lv_examples/lv_tutorial/6_images/lv_tutorial_images.h"
#include "src/lvScreens/lvSplashScreen.h"
#include "src/lvScreens/lvParameterScreen.h"

#include "components/sdio-driver/SDIOBlockDevice.h"
#include "FATFileSystem.h"

#include "USBSerial.h"


#if !defined(MBED_CPU_STATS_ENABLED) || !defined(DEVICE_LPTICKER) || !defined(DEVICE_SLEEP)
#error [NOT_SUPPORTED] test not supported
#endif


Thread threadIO;
Thread threadUSBSerial;
Ticker tickerLvgl;

#define LED_OFF (1)
#define LED_ON  (0)

DigitalOut led1(LED1, 1); // onboard LEDs
DigitalOut led2(LED2, 1);

// graphics class, used for initializing tft
#if defined(TARGET_STM32F407VE_BLACK)
lvglDispSTM32F407VE_BLACK   lvglDisplay;
lvglTouchDriverXPT2046 lvglTouch(PB_15, PB_14, PB_13, PB_12, PC_5, &lvglDisplay);
#elif defined(TARGET_DISCO_F746NG)
#endif

// Physical block device, can be any device that supports the BlockDevice API
SDIOBlockDevice bd;
// File system declaration
FATFileSystem fs("sda", &bd);

//
// calc cpu usage
//

#define SAMPLE_TIME             2000    // msec
int cpuUsage = 0;
int idle = 0;

void calc_cpu_usage()
{
    static uint64_t prev_idle_time = 0;
    mbed_stats_cpu_t stats;
    mbed_stats_cpu_get(&stats);

    uint64_t diff = (stats.idle_time - prev_idle_time);
    idle = (diff * 100) / (SAMPLE_TIME*1000);    // usec;
    cpuUsage = 100 - ((diff * 100) / (SAMPLE_TIME*1000));    // usec;;
    prev_idle_time = stats.idle_time;

    printf("Idle: %4d Usage: %4d \n", idle, cpuUsage);
}

//
// test USBSerial communication
// 

void threadFnUSBSerial()
{
    USBSerial usbSerial;     // serial USB device, blocking connect
    usbSerial.printf("USBSerial connected.\r");

    while(1) {
        usbSerial.printf("Idle: %4d Usage: %4d \n", idle, cpuUsage);
        ThisThread::sleep_for(SAMPLE_TIME);
    }
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

    // Mbed CPU perfomance measuring. Has slight impact on perfomance itself!
    //  mbed_app.json needs : 'platform.cpu-stats-enabled": true'
    EventQueue *stats_queue = mbed_event_queue();
    stats_queue->call_every(SAMPLE_TIME, calc_cpu_usage);

	// register update handler. Task will call screen dependent cyclic updates
	lv_task_create(lv_screen_update_task, 200, LV_TASK_PRIO_MID, 0);

    /* Set cursor */
    #ifdef USE_CURSOR
    lv_obj_t *mouse_cursor = lv_img_create(lv_disp_get_scr_act(NULL), NULL);
    lv_img_set_src(mouse_cursor, LV_SYMBOL_PLUS);
    lv_indev_set_cursor(my_indev, mouse_cursor);
    #endif

    // start threads
    threadIO.start(callback(threadFnIO));
    threadUSBSerial.start(callback(threadFnUSBSerial));
    tickerLvgl.attach_us(&fnLvTicker, 2000);

    //lv_tutorial_hello_world();
    //demo_create();
    //lv_test_theme_2();
    //benchmark_create();

    // display splash screen for 2000 ms
    //lvSplashScreen();
    //sleepWithLvHandler(2000);

    // main screen
    //lvParameterScreen();
    lv_tutorial_image();

    // simple main loop, should handle events
    sleepWithLvHandler(0); // sleep forever and call lv_handler
}
