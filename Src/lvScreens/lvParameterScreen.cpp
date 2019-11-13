/**
 * @file lv_tutorial_objects.c
 *
 */

/*
 * ------------------------------------------------
 * Learn how to create GUI elements on the screen
 * ------------------------------------------------
 *
 * The basic building blocks (components or widgets) in LittlevGL are the graphical objects.
 * For example:
 *  - Buttons
 *  - Labels
 *  - Charts
 *  - Sliders etc
 *
 * In this part you can learn the basics of the objects like creating, positioning, sizing etc.
 * You will also meet some different object types and their attributes.
 *
 * Regardless to the object's type the 'lv_obj_t' variable type is used
 * and you can refer to an object with an lv_obj_t pointer (lv_obj_t *)
 *
 * PARENT-CHILD STRUCTURE
 * -------------------------
 * A parent can be considered as the container of its children.
 * Every object has exactly one parent object (except screens).
 * A parent can have unlimited number of children.
 * There is no limitation for the type of the parent.
 *
 * The children are visible only on their parent. The parts outside will be cropped (not displayed)
 *
 * If the parent is moved the children will be moved with it.
 *
 * The earlier created object (and its children) will drawn earlier.
 * Using this layers can be built.
 *
 * INHERITANCE
 * -------------
 * Similarly to object oriented languages some kind of inheritance is used
 * among the object types. Every object is derived from the 'Basic object'. (lv_obj)
 * The types are backward compatible therefore to set the basic parameters (size, position etc.)
 * you can use 'lv_obj_set/get_...()' function.

 * LEARN MORE
 * -------------
 * - General overview: http://www.gl.littlev.hu/objects
 * - Detailed description of types: http://www.gl.littlev.hu/object-types
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvParameterScreen.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void btnUpDown_event_cb(lv_obj_t * btn, lv_event_t event);
static void btnSetPos_event_cb(lv_obj_t * btn, lv_event_t event);
static void slider_event_cb(lv_obj_t * ddlist, lv_event_t event);
static void lvParameterScreen_update_task(bool firstStart);

static bool motorOn;
static int motorSpeed;
static int motorPos;
static int motorSetPos;
static int motorDirection;

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * slider;
static lv_obj_t* labelPos;
static lv_obj_t* btnUp;
static lv_obj_t* btnDown;
static lv_obj_t* btnLoadPos;
static lv_obj_t* btnBottomPos;
static lv_obj_t* labelCPU;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create some objects
 */
void lvParameterScreen(void)
{

   // my_style.text.font = &Roboto16;

    /********************
     * CREATE A SCREEN
     *******************/
    /* Create a new screen and load it
     * Screen can be created from any type object type
     * Now a Page is used which is an objects with scrollable content*/
    lv_obj_t * scr = lv_page_create(NULL, NULL);

	scr->user_data = (lv_obj_user_data_t)lvParameterScreen_update_task;
    lv_disp_load_scr(scr);

    /****************
     * ADD A TITLE
     ****************/
    lv_obj_t * label = lv_label_create(scr, NULL); /*First parameters (scr) is the parent*/
    lv_label_set_text(label, "Settings");  /*Set the text*/
    lv_obj_set_x(label, 20);                        /*Set the x coordinate*/

    // CPU load
    labelCPU = lv_label_create(scr, NULL);
    lv_obj_set_x(labelCPU, 270);

    /***********************
     * CREATE BUTTONS
     ***********************/
    /*Create Up button*/
    btnUp = lv_btn_create(lv_disp_get_scr_act(NULL), NULL);                     /*Create a button on the currently loaded screen*/
	lv_obj_set_width(btnUp, LV_DPI * 12 / 10);
    lv_obj_set_event_cb(btnUp, btnUpDown_event_cb);                             /*Set function to be called when the button is released*/
    lv_obj_align(btnUp, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);                /*Align below the label*/

    label = lv_label_create(btnUp, NULL);
    lv_label_set_text(label, "up");

    /* create Down button */
    btnDown = lv_btn_create(scr, btnUp);                                        /*Second parameter is an object to copy*/
    lv_obj_align(btnDown, btnUp, LV_ALIGN_OUT_RIGHT_MID, 50, 0);                /*Align next to the prev. button.*/

    label = lv_label_create(btnDown, NULL);
    lv_label_set_text(label, "down");

    /* create Loadpos button*/
    btnLoadPos = lv_btn_create(scr, btnUp);                                     /*Second parameter is an object to copy*/
    lv_obj_set_event_cb(btnLoadPos, btnSetPos_event_cb);                        /*Set function to be called when the button is released*/
    lv_obj_align(btnLoadPos, btnUp, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);           /*Align next to the prev. button.*/

    label = lv_label_create(btnLoadPos, NULL);
    lv_label_set_text(label, "Load");

    /* create BottomPos button */
    btnBottomPos = lv_btn_create(scr, btnLoadPos);                              /*Second parameter is an object to copy*/
    lv_obj_align(btnBottomPos, btnLoadPos, LV_ALIGN_OUT_RIGHT_MID, 50, 0);      /*Align next to the prev. button.*/

    label = lv_label_create(btnBottomPos, NULL);
    lv_label_set_text(label, "Bottom");

    /****************
     * ADD A SLIDER
     ****************/
    slider = lv_slider_create(scr, NULL);                               /*Create a slider*/
    lv_obj_set_size(slider, lv_obj_get_width(scr)  / 2, LV_DPI / 3);    /*Set the size*/
    lv_obj_align(slider, btnLoadPos, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);        /*Align below the first button*/
    lv_obj_set_event_cb(slider, slider_event_cb); 
    lv_slider_set_value(slider, 30, false);                             /*Set the current value*/

	/****************
	 * ADD A Pos Var
	 ****************/
	labelPos = lv_label_create(scr, NULL);
	lv_obj_align(labelPos, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);               /* Align below slider */
	lv_label_set_text(labelPos, "Pos:    0");

#if 0
    /***********************
     * ADD A DROP DOWN LIST
     ************************/
    lv_obj_t * ddlist = lv_ddlist_create(scr, NULL);                     /*Create a drop down list*/
    lv_obj_align(ddlist, slider, LV_ALIGN_OUT_RIGHT_TOP, 50, 0);         /*Align next to the slider*/
    lv_obj_set_top(ddlist, true);                                        /*Enable to be on the top when clicked*/
    lv_ddlist_set_options(ddlist, "None\nLittle\nHalf\nA lot\nAll");     /*Set the options*/
    lv_obj_set_event_cb(ddlist, ddlist_event_cb);                        /*Set function to call on new option is chosen*/

    /****************
     * CREATE A CHART
     ****************/
    lv_obj_t * chart = lv_chart_create(scr, NULL);                         /*Create the chart*/
    lv_obj_set_size(chart, lv_obj_get_width(scr) / 2, lv_obj_get_width(scr) / 4);   /*Set the size*/
    lv_obj_align(chart, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 50);                   /*Align below the slider*/
    lv_chart_set_series_width(chart, 3);                                            /*Set the line width*/

    /*Add a RED data series and set some points*/
    lv_chart_series_t * dl1 = lv_chart_add_series(chart, LV_COLOR_RED);
    lv_chart_set_next(chart, dl1, 10);
    lv_chart_set_next(chart, dl1, 25);
    lv_chart_set_next(chart, dl1, 45);
    lv_chart_set_next(chart, dl1, 80);

    /*Add a BLUE data series and set some points*/
    lv_chart_series_t * dl2 = lv_chart_add_series(chart, lv_color_make(0x40, 0x70, 0xC0));
    lv_chart_set_next(chart, dl2, 10);
    lv_chart_set_next(chart, dl2, 25);
    lv_chart_set_next(chart, dl2, 45);
    lv_chart_set_next(chart, dl2, 80);
    lv_chart_set_next(chart, dl2, 75);
    lv_chart_set_next(chart, dl2, 505);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lvParameterScreen_update_task(bool firstStart)
{
    char buffer[32];

    static int motorPosOld = 0;
    if (firstStart || (motorPos != motorPosOld)) {			// update only if value has changed
        snprintf(buffer, sizeof(buffer), "Pos: %4d", motorPos);
        lv_label_set_text(labelPos, buffer);
        motorPosOld = motorPos;
    }

    static int cpuUsageOld = 0;
    if (firstStart || (cpuUsage != cpuUsageOld)) {
        snprintf(buffer, sizeof(buffer), "%4d %%", cpuUsage);
        lv_label_set_text(labelCPU, buffer);
        cpuUsageOld = cpuUsage;
    }

}


/**
 * Called when a button is released
 * @param btn pointer to the released button
 * @param event the triggering event
 * @return LV_RES_OK because the object is not deleted in this function
 */
static void btnUpDown_event_cb(lv_obj_t * btn, lv_event_t event)
{
    if (event == LV_EVENT_PRESSED) {
        if (btn == btnUp)
            motorDirection = 0;
        if (btn == btnDown)
            motorDirection = 1;
        motorOn = true;
    } else
    if ((event == LV_EVENT_RELEASED)  || (event == LV_EVENT_PRESS_LOST )) {
        motorOn = false;
    }
}

static void btnSetPos_event_cb(lv_obj_t * btn, lv_event_t event)
{
    if (event == LV_EVENT_PRESSED) {
        if (btn == btnLoadPos) {
            motorDirection = 0;
            motorSetPos = 0;
        }
        if (btn == btnBottomPos) {
            motorDirection = 1;
            motorSetPos = 16*200;
        }
        motorOn = true;
    } 
}

/**
 * Called when a slider is moved
 * @param slider pointer to the slider
 * @param event the triggering event
 * @return LV_RES_OK because the object is not deleted in this function
 */
static void slider_event_cb(lv_obj_t * slider, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED ) {
        motorSpeed = (1000 - lv_slider_get_value(slider)*10);
    }
}

#if 0
/**
 * Called when a new option is chosen in the drop down list
 * @param ddlist pointer to the drop down list
 * @param event the triggering event
 * @return LV_RES_OK because the object is not deleted in this function
 */
static  void ddlist_event_cb(lv_obj_t * ddlist, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        uint16_t opt = lv_ddlist_get_selected(ddlist);            /*Get the id of selected option*/

        lv_slider_set_value(slider, (opt * 100) / 4, true);       /*Modify the slider value according to the selection*/
    }

}
#endif

