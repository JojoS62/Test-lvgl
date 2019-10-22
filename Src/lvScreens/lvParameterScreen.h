/**
 * @file lv_tutorial_objects.h
 *
 */

#ifndef __lvParameterScreen_h__
#define __lvParameterScreen_h__

#include "mbed.h"


#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#include "lv_ex_conf.h"
#else
#include "../../lvgl/lvgl.h"
#include "../../lv_ex_conf.h"
#endif

extern bool motorOn;
extern int motorSpeed;
extern DigitalOut motorDirection;

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lvParameterScreen(void);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __lvParameterScreen_h__ */
