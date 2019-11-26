#include "LVGLTouchDriverXPT2046.h"


MBED_WEAK LVGLInputDriver *LVGLInputDriver::get_target_default_instance_touchdrv(LVGLDispDriver *disp)
{
    static LVGLTouchDriverXPT2046 lvglTouch(PB_15, PB_14, PB_13, PB_12, PC_5, disp);
    return &lvglTouch;
}
