#include "Switch.h"
#include "LCDinterface.h"
#include "Demo.h"

#define Ascii0 48
#define AsciiColon 58
#define AsciiDash 45

typedef enum
{
    WRITE_TIME,
    WRITE_DATE,
    CURSOR1,
    CURSOR2,
    WRITE_EDITMODE,
    WRITE_STOPWATCHMODE,
    CLEAR
} lcd_commands;

/******************/
typedef struct digits
{
    uint8_t digit0;
    uint8_t digit1;
    uint8_t digit2;
    uint8_t digit3;
} Digits_t;

struct
{
    Digits_t Seconds;
    Digits_t Minutes;
    Digits_t Hours;
} Time;

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
} cursor;

/******************/
uint32_t CurrentPressedSwitch = _SWITCH_NUM;
WatchViews CurrentWatchView = DefaultView;
char DateString[] = "01-04-2024";
char *DateBuffer = DateString;
char TimeString[] = "04:13:00";
char *TimeBuffer = TimeString;
uint8_t displayState;
uint8_t LCD_command = CURSOR1;
uint32_t default_counter;
uint32_t mode_counter;
// /******************/

// /*****************/
void CurrDate(void);
void CurrTime(void);
void DisplayTime(void);
void DisplayDate(void);
void LCD_handle();
void Display_DefaultView();
void Display_ModeView(void);
void SwitchControl(void);

/******************/
void Demo_Runnable(void)
{
    CurrTime();
    switch (CurrentWatchView)
    {
    case DefaultView:
        if (CurrentPressedSwitch == _SWITCH_NUM)
        {
            Display_DefaultView();
        }
        else if (CurrentPressedSwitch == Switch_mode)
        {
            LCD_clearScreenAsyn();
            CurrentWatchView = ModeView;
            mode_counter=0;
        }
        break;
    case ModeView:
        if (CurrentPressedSwitch == _SWITCH_NUM)
        {
            Display_ModeView();
        }
        else if (CurrentPressedSwitch == Switch_mode)
        {
            LCD_clearScreenAsyn();
            CurrentWatchView = DefaultView;
            default_counter=0;
        }
        break;
    case StopWatchView:

        break;

    case EditTimeView:

        break;

    default:

        break;
    }
    SwitchControl();
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

void CurrTime(void) // enter every 50 milliseconds
{
    static uint32_t counter = 0;
    if (counter == 1000)
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
        counter = 0;
    }
    counter += 50;
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