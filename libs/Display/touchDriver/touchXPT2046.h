#ifndef __touchXPT2046_h__
#define __touchXPT2046_h__

#include "mbed.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void initTouchXPT2046();
bool readTouchXPT2046(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

#ifdef __cplusplus
}
#endif


#endif // __touchXPT2046_h__