
#if 0
#include "mbed.h"

DigitalOut led1(LED1, 1); // onboard LEDs

int main()
{
    printf("hello world\n");

    while(1) {
        led1 = !led1;
        ThisThread::sleep_for(100);
    }
}
#endif