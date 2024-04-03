#include "Switch.h"
#include "LCDinterface.h"
#include "Demo.h"


/***************************************************/
#define Ascii0          48
#define AsciiColon      58
#define AsciiDash       45
#define START           1
#define PAUSE           2
#define STOP            3
#define Periodicity     100
#define newline         15
#define inc             1
#define dec             2
/***************************************************/

enum
{
    WRITE_TIME,
    WRITE_DATE,
    CURSOR1,
    CURSOR2,
    WRITE_EDITMODE,
    WRITE_STOPWATCHMODE,
    CLEAR
} lcd_commands;

/***************************************************/

typedef struct digits
{
    uint8_t digit0;
    uint8_t digit1;
    uint8_t digit2;
    uint8_t digit3;
} Digits_t;

typedef struct
{
    Digits_t Seconds;
    Digits_t Minutes;
    Digits_t Hours;
} Time_t;

/***************************************************/

struct
{
    Digits_t Day;
    Digits_t Month;
    Digits_t Year;
} Date;

struct
{
    uint8_t xPos;
    uint8_t yPos;
}cursor;

/*************************Variables****************************/

Time_t Time;
Time_t Stopwatch;
uint32_t CurrentPressedSwitch = _SWITCH_NUM;
WatchViews CurrentWatchView = DefaultView;
uint8_t LCD_command = CURSOR1;
uint8_t default_counter;
uint8_t mode_counter;
uint8_t EditTime_counter;
uint8_t stopwatch_counter; 
uint8_t stopwatchcounter;
uint8_t stopwatchstate = STOP;
uint8_t editstate;
uint8_t arrowcounter;
char DateString[] = "01-04-2024";
char *DateBuffer = DateString;
char TimeString[] = "04:13:00";
char *TimeBuffer = TimeString;
char *StopWatchbuffer = TimeString;


/*************************Functions ProtoTypes****************************/
void CurrDate(void);
void CurrTime(void);
void DisplayTime(void);
void DisplayDate(void);
void LCD_handle();
void Display_DefaultView();
void Display_ModeView(void);
void SwitchControl(void);
void StopWatch(void);
void DisplayStopWatch(void);
void Display_StopWatch(void);
void Display_EditTime(void);
void EditTime(void);

/*****************************Implementation*******************************/


void Demo_Runnable(void)
{
    CurrTime();
    StopWatch();
    EditTime();
    SwitchControl();
    if (CurrentPressedSwitch != _SWITCH_NUM)
    {
        LCD_clearScreenAsyn();
        switch (CurrentPressedSwitch)
        {
        case Switch_mode:
            switch (CurrentWatchView)
            {
            case DefaultView:
                mode_counter = 0;
                CurrentWatchView = ModeView;
                break;
            case ModeView:
                default_counter = 0;
                CurrentWatchView = DefaultView;
                break;
            case StopWatchView:
                mode_counter = 0;
                CurrentWatchView = ModeView;
                break;
            case EditTimeView:
                mode_counter = 0;
                CurrentWatchView = ModeView;
                break;
            }
            break;
        case Switch_up:
            switch (CurrentWatchView)
            {
            case ModeView:
                stopwatch_counter = 0;
                CurrentWatchView = StopWatchView;
                break;
            case StopWatchView:
                stopwatchstate = START;
                break;
            case EditTimeView:
                editstate = inc;
                break;    
            default:
                break;
            }
            break;
        case Switch_down:
            switch (CurrentWatchView)
            {
            case ModeView:
                EditTime_counter = 0;
                CurrentWatchView = EditTimeView;
                break;
            case StopWatchView:
                stopwatchcounter++;
                if (stopwatchcounter == 1)
                {
                    stopwatchstate = PAUSE;
                }
                if (stopwatchcounter == 2)
                {
                    stopwatchstate = STOP;
                    stopwatchcounter = 0;
                }
                break;
            case EditTimeView:
                editstate = dec;
                break;    
            default:
                break;
            }
            break;

        case Switch_right:
            switch (CurrentWatchView)
            {
            case EditTimeView: 
                arrowcounter++;
                if (arrowcounter == 8)
                {
                    arrowcounter =0;
                }
                else if (arrowcounter == 2 || arrowcounter == 5)
                {
                    arrowcounter++;
                }
                break;
            
            default:
                break;
            }
            break;


        case Switch_left:
            switch (CurrentWatchView)
            {
            case EditTimeView:
                if (arrowcounter==0)
                {
                    arrowcounter=8;
                }
                else if (arrowcounter == 3 || arrowcounter == 6)
                {
                    arrowcounter--;
                } 
                arrowcounter--;
                break;
            
            default:
                break;
            }
            break;

        default:
            break;
        }
    }

    if (CurrentPressedSwitch == _SWITCH_NUM)
    {
        switch (CurrentWatchView)
        {
        case DefaultView:
            Display_DefaultView();
            break;
        case ModeView:
            Display_ModeView();
            break;

        case StopWatchView:
            Display_StopWatch();
            break;

        case EditTimeView:
            Display_EditTime();
            break;

        default:

            break;
        }
    }
}


void WatchInit(void)
{
    SWITCH_init();
    LCD_initAsyn();
}
void Display_DefaultView()
{
    default_counter++;
    if (default_counter == 1)
    {
        LCD_setCursorPosAsyn(0, 0);
    }
    else if (default_counter == 2)
    {
        DisplayDate();
    }
    else if (default_counter == 3)
    {
        LCD_setCursorPosAsyn(1, 0);
    }
    else
    {
        default_counter = 0;
        DisplayTime();
    }
}

void Display_ModeView(void)
{
    mode_counter++;
    if (mode_counter == 1)
    {
        LCD_setCursorPosAsyn(0, 0);
    }
    else if (mode_counter == 2)
    {
        LCD_writeStringAsyn("Stop Watch", 10);
    }
    else if (mode_counter == 3)
    {
        LCD_setCursorPosAsyn(1, 0);
    }
    else
    {
        mode_counter = 0;
        LCD_writeStringAsyn("Edit Time & Date", 16);
    }
}

void Display_StopWatch(void)
{
    stopwatch_counter++;
    if (stopwatchstate == 0)
    {
        LCD_writeStringAsyn("00:00:00", 8);
        stopwatch_counter = 0;
    }
    if (stopwatch_counter == 1)
    {
        LCD_setCursorPosAsyn(0, 0);
    }
    else if (stopwatch_counter == 2)
    {
        DisplayStopWatch();
        stopwatch_counter = 0;
    }
}

void Display_EditTime(void)
{
    EditTime_counter++;
    if (EditTime_counter == 1)
    {
        LCD_setCursorPosAsyn(0, 0);
    }
    if (EditTime_counter == 2)
    {
        DisplayTime();
        EditTime_counter=0;
    }
}

void CurrTime(void) // enter every 50 milliseconds
{
    static uint32_t sec_counter = 0;
    if (sec_counter == 1000)
    {
        Time.Seconds.digit0++;
        if (Time.Seconds.digit0 > 9)
        {
            if (Time.Seconds.digit1 == 5)
            {
                Time.Seconds.digit0 = 0;
                Time.Seconds.digit1 = 0;
                Time.Minutes.digit0++;
            }
            else
            {
                Time.Seconds.digit0 = 0;
                Time.Seconds.digit1++;
            }
        }
        if (Time.Minutes.digit0 > 9)
        {
            if (Time.Seconds.digit1 == 5)
            {
                Time.Minutes.digit0 = 0;
                Time.Minutes.digit1 = 0;
                Time.Hours.digit0++;
            }
            else
            {
                Time.Minutes.digit0 = 0;
                Time.Minutes.digit1++;
            }
        }
        if (Time.Hours.digit0 > 9)
        {
            Time.Hours.digit0 = 0;
            Time.Hours.digit1++;
            if (Time.Hours.digit0 == 3 && Time.Hours.digit1 == 2)
            {
                Time.Hours.digit0 = 0;
                Time.Hours.digit1 = 0;
                CurrDate();
            }
        }
        sec_counter = 0;
    }
    sec_counter += Periodicity;
}

/* Function that calculate real date */
void CurrDate(void)
{
    Date.Day.digit0++;
    if (Date.Day.digit0 == 9)
    {
        Date.Day.digit0 = 0;
        Date.Day.digit1++;
    }
    if (Date.Month.digit0 % 2 || Date.Month.digit0 == 7 || (Date.Month.digit1 == 1 && (Date.Month.digit0 == 0 || Date.Month.digit0 == 2)))
    {
        if (Date.Day.digit0 == 1 && Date.Day.digit1 == 3)
        {
            Date.Month.digit0++;
        }
    }
    else if (Date.Month.digit0 == 2)
    /* handle feb 29 days every 4 years*/
    {
        if (Date.Day.digit0 == 2 && Date.Day.digit1 == 8)
        {
            Date.Month.digit0++;
        }
    }
    else
    {
        if (Date.Day.digit0 == 0 && Date.Day.digit1 == 3)
        {
            Date.Month.digit0++;
        }
    }
    if (Date.Month.digit0 == 9)
    {
        Date.Month.digit0 = 0;
        Date.Month.digit1++;
        if (Date.Month.digit0 == 2 && Date.Month.digit1 == 1)
        {
            Date.Year.digit0++;
        }
    }
    if (Date.Year.digit0 == 9)
    {
        Date.Year.digit0 = 0;
        Date.Year.digit1++;
        if (Date.Year.digit1 == 9)
        {
            Date.Year.digit1 = 0;
            Date.Year.digit2++;
            if (Date.Year.digit2 == 9)
            {
                Date.Year.digit2 = 0;
                Date.Year.digit3++;
            }
        }
    }
}

void StopWatch(void)
{
    static uint32_t st_sw_counter = 0;
    if (stopwatchstate == START)
    {
        if (st_sw_counter == 1000)
        {
            Stopwatch.Seconds.digit0++;
            if (Stopwatch.Seconds.digit0 > 9)
            {
                if (Stopwatch.Seconds.digit1 == 5)
                {
                    Stopwatch.Seconds.digit0 = 0;
                    Stopwatch.Seconds.digit1 = 0;
                    Stopwatch.Minutes.digit0++;
                }
                else
                {
                    Stopwatch.Seconds.digit0 = 0;
                    Stopwatch.Seconds.digit1++;
                }
            }
            if (Stopwatch.Minutes.digit0 > 9)
            {
                if (Stopwatch.Seconds.digit1 == 5)
                {
                    Stopwatch.Minutes.digit0 = 0;
                    Stopwatch.Minutes.digit1 = 0;
                    Stopwatch.Hours.digit0++;
                }
                else
                {
                    Stopwatch.Minutes.digit0 = 0;
                    Stopwatch.Minutes.digit1++;
                }
            }
            if (Stopwatch.Hours.digit0 > 9)
            {
                Stopwatch.Hours.digit0 = 0;
                Stopwatch.Hours.digit1++;
                if (Stopwatch.Hours.digit0 == 3 && Stopwatch.Hours.digit1 == 2)
                {
                    Stopwatch.Hours.digit0 = 0;
                    Stopwatch.Hours.digit1 = 0;
                }
            }
            st_sw_counter = 0;
        }
        st_sw_counter += Periodicity;
    }
    if (stopwatchstate == STOP)
    {
        Stopwatch.Seconds.digit0 = 0;
        Stopwatch.Seconds.digit1 = 0;
        Stopwatch.Minutes.digit0 = 0;
        Stopwatch.Minutes.digit1 = 0;
        Stopwatch.Hours.digit0 = 0;
        Stopwatch.Hours.digit1 = 0;
    }
}

void EditTime(void)
{
    if (editstate == inc)
    {
        switch (arrowcounter)
        {
        case 0:
            if (Time.Hours.digit1 == 2)
            {
                Time.Hours.digit1 = 0;
            }
            else
            {
                Time.Hours.digit1++;
            }
            break;
        case 1:
            if (Time.Hours.digit0 == 9)
            {
                Time.Hours.digit0 = 0;
            }
            else
            {
                Time.Hours.digit0++;
            }
            break;   
        case 3:
            if (Time.Minutes.digit1 == 5)
            {
                Time.Minutes.digit1 = 0;
            }
            else
            {
                Time.Minutes.digit1++;
            }
            break;
        case 4:
            if (Time.Minutes.digit0 == 9)
            {
                Time.Minutes.digit0 = 0;
            }
            else
            {
                Time.Minutes.digit0++;
            }
            break;
        case 6:
            if (Time.Seconds.digit1 == 5)
            {
                Time.Seconds.digit1 = 0;
            }
            else
            {
                Time.Seconds.digit1++;
            }
            break;
        case 7:
            if (Time.Seconds.digit0 == 9)
            {
                Time.Seconds.digit0 = 0;
            }
            else
            {
                Time.Seconds.digit0++;
            }
            break;
        default:
            break;
        }
        editstate = 0;
    }
    else if (editstate == dec)
    {
        switch (arrowcounter)
        {
        case 0:
            if (Time.Hours.digit1 == 0)
            {
                Time.Hours.digit1 = 2;
            }
            else
            {
                Time.Hours.digit1--;
            }
            break;

        case 1:
            if (Time.Hours.digit0 == 0)
            {
                Time.Hours.digit0 = 9;
            }
            else
            {
                Time.Hours.digit0--;
            }
            break;
        case 3:
            if (Time.Minutes.digit1 == 0)
            {
                Time.Minutes.digit1 = 5;
            }
            else
            {
                Time.Minutes.digit1--;
            }
            break;
        case 4:
            if (Time.Minutes.digit0 == 0)
            {
                Time.Minutes.digit0 = 9;
            }
            else
            {
                Time.Minutes.digit0--;
            }
            break;
        case 6:
            if (Time.Seconds.digit1)
            {
                Time.Seconds.digit1 = 5;
            }
            else
            {
                Time.Seconds.digit1--;
            }
            break;
        case 7:
            if (Time.Seconds.digit0 == 0)
            {
                Time.Seconds.digit0 = 9;
            }
            else
            {
                Time.Seconds.digit0--;
            }
            break;
        default:
            break;
        }
        editstate = 0;
    }
}
void ControlCursor(void)
{

}
void DisplayTime(void)
{
    TimeBuffer[0] = 48 + Time.Hours.digit1;
    TimeBuffer[1] = 48 + Time.Hours.digit0;
    TimeBuffer[2] = 58;
    TimeBuffer[3] = 48 + Time.Minutes.digit1;
    TimeBuffer[4] = 48 + Time.Minutes.digit0;
    TimeBuffer[5] = 58;
    TimeBuffer[6] = 48 + Time.Seconds.digit1;
    TimeBuffer[7] = 48 + Time.Seconds.digit0;
    LCD_writeStringAsyn(TimeBuffer, 8);
}

void DisplayDate()
{
    Date.Day.digit0 = 5;
    Date.Month.digit0 = 4;
    Date.Year.digit0 = 4;
    Date.Year.digit1 = 2;
    Date.Year.digit2 = 0;
    Date.Year.digit3 = 2;
    DateBuffer[0] = 48 + Date.Day.digit1;
    DateBuffer[1] = 48 + Date.Day.digit0;
    DateBuffer[2] = 45;
    DateBuffer[3] = 48 + Date.Month.digit1;
    DateBuffer[4] = 48 + Date.Month.digit0;
    DateBuffer[5] = 45;
    DateBuffer[6] = 48 + Date.Year.digit3;
    DateBuffer[7] = 48 + Date.Year.digit2;
    DateBuffer[8] = 48 + Date.Year.digit1;
    DateBuffer[9] = 48 + Date.Year.digit0;

    LCD_writeStringAsyn(DateBuffer, 10);
}

void DisplayStopWatch(void)
{
    StopWatchbuffer[0] = 48 + Stopwatch.Hours.digit1;
    StopWatchbuffer[1] = 48 + Stopwatch.Hours.digit0;
    StopWatchbuffer[2] = 58;
    StopWatchbuffer[3] = 48 + Stopwatch.Minutes.digit1;
    StopWatchbuffer[4] = 48 + Stopwatch.Minutes.digit0;
    StopWatchbuffer[5] = 58;
    StopWatchbuffer[6] = 48 + Stopwatch.Seconds.digit1;
    StopWatchbuffer[7] = 48 + Stopwatch.Seconds.digit0;
    LCD_writeStringAsyn(StopWatchbuffer, 8);
}

void SwitchControl(void)
{
    uint16_t modestatus;
    uint8_t idx;
    CurrentPressedSwitch = _SWITCH_NUM;
    for (idx = 0; idx < _SWITCH_NUM; idx++)
    {
        SWITCH_GETSTATUS(idx, &modestatus);
        if (modestatus == SWITCH_PRESSED)
        {
            CurrentPressedSwitch = idx;
            idx = _SWITCH_NUM; /*handle the case with multiple pressed switches*/
        }
    }
}