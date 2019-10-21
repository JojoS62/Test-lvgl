// Modifyed to support mbed environment, rewrite the state machine.
// by Zibin Zheng <znbin@qq.com>

// -----
// OneButton.h - Library for detecting button clicks, doubleclicks and long press pattern on a single button.
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// More information on: http://www.mathertel.de/Arduino
// -----


#ifndef OneButton_h
#define OneButton_h

#include "mbed.h"

/**
 * Example:
 * @code
 
#include "mbed.h"
#include "OneButton.h"

Serial pc(SERIAL_TX, SERIAL_RX);

OneButton btn1(PC_13);

void pressed() {
	pc.printf("pressed\r\n");
}

void click() {
	pc.printf("click\r\n");
}

void double_click() {
	pc.printf("double_click\r\n");
}

void long_start() {
	pc.printf("long_start\r\n");
}

void long_hold() {
	pc.printf("long_hold\r\n");
}

void long_stop() {
	pc.printf("long_stop\r\n");
}

int main() {

	btn1.attachClick(&click);
	btn1.attachPress(&pressed);
	btn1.attachDoubleClick(&double_click);
	btn1.attachLongPressStart(&long_start);
	btn1.attachDuringLongPress(&long_hold);
	btn1.attachLongPressStop(&long_stop);

	while(1) {

		btn1.tick(); //loop
	}
}

*/


#if (MBED_LIBRARY_VERSION < 122)
// ----- Callback function types -----

extern "C" {
	typedef void (*callbackFunction)(void);
}
#endif

class OneButton
{
public:

	// ----- Constructor -----
	OneButton(PinName pin, bool active_level = 0);

	// ----- Set runtime parameters -----

	// set # millisec after single click is assumed.
	void setClickTicks(int ticks);

	// set # millisec after press is assumed.
	void setPressTicks(int ticks);

	// attach functions that will be called when button was pressed in the specified way.
    
#if (MBED_LIBRARY_VERSION < 122)
	void attachClick(callbackFunction newFunction);
	void attachDoubleClick(callbackFunction newFunction);
	void attachPress(callbackFunction newFunction); // DEPRECATED, replaced by longPressStart, longPressStop and duringLongPress
	void attachLongPressStart(callbackFunction newFunction);
	void attachLongPressStop(callbackFunction newFunction);
	void attachDuringLongPress(callbackFunction newFunction);
#else
	// click function
	void attachClick(Callback<void()> func){
		clickFn.attach(func);
	};

	// doubleClick function
	void attachDoubleClick(Callback<void()> func){
		doubleClickFn.attach(func);
	};

	// press function
	void attachPress(Callback<void()> func){
		pressFn.attach(func);
	};

	// long press start function
	void attachLongPressStart(Callback<void()> func){
		longPressStartFn.attach(func);
	};

	// long press stop function
	void attachLongPressStop(Callback<void()> func){
		longPressStopFn.attach(func);
	};

	// during long press function
	void attachDuringLongPress(Callback<void()> func){
		duringLongPressFn.attach(func);
	};
    
   	Callback<void()> clickFn;
   	Callback<void()> doubleClickFn;
   	Callback<void()> pressFn;
   	Callback<void()> longPressStartFn;
   	Callback<void()> longPressStopFn;
   	Callback<void()> duringLongPressFn;

#endif
	// ----- State machine functions -----

	// call this function every some milliseconds for handling button events.
	void tick(void);
	bool isLongPressed();

private:
	int _clickTicks; // number of ticks that have to pass by before a click is detected
	int _pressTicks; // number of ticks that have to pass by before a long button press is detected

	int _buttonReleased;
	int _buttonPressed;
	bool _isLongPressed;

	// These variables will hold functions acting as event source.
	// mbed lib < 122 will use a simple function pointer, 
	// for libs >= 122 we can use the new Callback template to avoid compiler warnings

#if (MBED_LIBRARY_VERSION < 122)
	callbackFunction clickFn;
	callbackFunction doubleClickFn;
	callbackFunction pressFn;
	callbackFunction longPressStartFn;
	callbackFunction longPressStopFn;
	callbackFunction duringLongPressFn;
#endif
	// These variables that hold information across the upcoming tick calls.
	// They are initialized once on program start and are updated every time the tick function is called.
	
	int ticks;
	uint8_t _state;
	uint8_t _debounce_cnt;
	uint8_t _debounceTicks; // number of ticks for debounce times.
	bool button_level;
	bool active;
	
	DigitalIn* btn_pin;
	Timer *timer;
};

#endif


