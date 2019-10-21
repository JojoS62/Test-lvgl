// Modifyed to support mbed environment, rewrite the state machine.
// by Zibin Zheng <znbin@qq.com>

// -----
// OneButton.cpp - Library for detecting button clicks, doubleclicks and long press pattern on a single button.
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// More information on: http://www.mathertel.de/Arduino
// -----


#include "OneButton.h"


// ----- Initialization and Default Values -----

OneButton::OneButton(PinName pin, bool active_level)
{
    btn_pin = new DigitalIn((PinName)pin);      // sets the MenuPin as input

    timer = new Timer;
    timer->start();

    _clickTicks = 400;        // number of millisec that have to pass by before a click is detected.
    _pressTicks = 800;       // number of millisec that have to pass by before a long button press is detected.
    _debounceTicks = 10;

    _state = 0; // starting with state 0: waiting for button to be pressed
    _isLongPressed = false;  // Keep track of long press state

    button_level = (bool)(btn_pin->read());
    active = active_level;

    if (active_level) {
        // button connects VCC to the pin when pressed.
        btn_pin->mode(PullDown);

    } else {
        // turn on pullUp resistor
        btn_pin->mode(PullUp);
    }

#if (MBED_LIBRARY_VERSION < 122)
    clickFn = NULL;
    pressFn = NULL;
    doubleClickFn = NULL;
    longPressStartFn = NULL;
    longPressStopFn = NULL;
    duringLongPressFn = NULL;
#endif
} // OneButton


// explicitly set the number of millisec that have to pass by before a click is detected.
void OneButton::setClickTicks(int ticks)
{
    _clickTicks = ticks;
} // setClickTicks


// explicitly set the number of millisec that have to pass by before a long button press is detected.
void OneButton::setPressTicks(int ticks)
{
    _pressTicks = ticks;
} // setPressTicks


#if (MBED_LIBRARY_VERSION < 122)
// save function for click event
void OneButton::attachClick(callbackFunction newFunction)
{
	clickFn = newFunction;
} // attachClick


// save function for doubleClick event
void OneButton::attachDoubleClick(callbackFunction newFunction)
{
    doubleClickFn = newFunction;
} // attachDoubleClick


// save function for press event
// DEPRECATED, is replaced by attachLongPressStart, attachLongPressStop, attachDuringLongPress,
void OneButton::attachPress(callbackFunction newFunction)
{
    pressFn = newFunction;
} // attachPress

// save function for longPressStart event
void OneButton::attachLongPressStart(callbackFunction newFunction)
{
    longPressStartFn = newFunction;
} // attachLongPressStart

// save function for longPressStop event
void OneButton::attachLongPressStop(callbackFunction newFunction)
{
    longPressStopFn = newFunction;
} // attachLongPressStop

// save function for during longPress event
void OneButton::attachDuringLongPress(callbackFunction newFunction)
{
    duringLongPressFn = newFunction;
} // attachDuringLongPress
#endif

// function to get the current long pressed state
bool OneButton::isLongPressed()
{
    return _isLongPressed;
}

/**
  * @brief  Button driver core function, driver state machine.
  * @param  handle: the button handle strcut.
  * @retval None
  */
void OneButton::tick(void)
{
    int read_gpio_level = btn_pin->read(); // current button signal.

    /*------------button debounce handle---------------*/
    if(ticks != timer->read_ms()) { //1ms scan again.
        if(read_gpio_level != button_level) {
            //continue read 'debounce_cnt' times same new level change
            if(++_debounce_cnt >= _debounceTicks) {
                button_level = read_gpio_level;
                _debounce_cnt = 0;
            }

        } else { //leved not change ,counter reset.
            _debounce_cnt = 0;
        }
    }

    //current (relative) time in msecs.
    ticks = timer->read_ms();

    /*-----------------State machine-------------------*/
    switch (_state) {
        case 0:
            if(button_level == active) {	//start press
                timer->reset();
                _state = 1;
            }
            break;

        case 1:
            if(button_level != active) { //released
                _state = 2;

            } else if(ticks > _pressTicks) {
                if(pressFn) pressFn();	//press event
                if(longPressStartFn) longPressStartFn();
                _isLongPressed = 1;
                _state = 5;
            }
            break;

        case 2:
            if(ticks > _clickTicks) {	//released
                if (clickFn) clickFn();
                _state = 0;	//reset

            } else if(button_level == active) { //press again
                _state = 3;
            }
            break;

        case 3:	//repeat press pressing
            if(button_level != active) {	//double releasd
                //double click event
                if(doubleClickFn) doubleClickFn();
                _state = 0;
            }
            break;

        case 5:
            if(button_level == active) {
                //continue hold trigger
                if(duringLongPressFn) duringLongPressFn();

            } else { //releasd
                if(longPressStopFn) longPressStopFn();
                _isLongPressed = 0;
                _state = 0; //reset
            }
            break;
    }
}

// end.

