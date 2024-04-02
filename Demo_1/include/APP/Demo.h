#ifndef DEMO1_H_
#define DEMO1_H_

#include"Sched.h"

/********************Stop Watch Buttons***********************/
#define SWITCH_UP       0x10
#define SWITCH_DOWN     0x20
#define SWITCH_RIGHT    0x30
#define SWITCH_LEFT     0x40
#define MODE_BUTTON     0x50

/********************Stop Watch Buttons***********************/
typedef enum 
{
    DefaultView,
    ModeView,
    StopWatchView,
    EditTimeView
}WatchViews;

/********************Stop Watch Buttons***********************/

void WatchInit(void);









#endif

