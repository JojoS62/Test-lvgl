/**
 * @file lv_tutorial_hello_world
 *
 */

#ifndef __lvSplashScreen_h__
#define __lvSplashScreen_h__

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


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lvSplashScreen(void);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __lvSplashScreen_h__ */
