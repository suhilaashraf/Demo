#include "Switch.h"
#include "Uart.h"
#include "LCDinterface.h"
#include "Demo.h"

/***************************************************/
#define Ascii0      48
#define AsciiColon  58
#define AsciiDash   45
#define START       1
#define PAUSE       2
#define STOP        3
#define Periodicity 50
#define inc         1
#define dec         2
#define MODE        5
#define UP          1
#define DOWN        2
#define RIGHT       3
#define LEFT        4



#define READY   0x0A
#define BUSY    0x0B

#define MONTH   (Date.Month.digit0+Date.Month.digit1*10)
#define DAY     (Date.Day.digit0+Date.Day.digit1*10)
#define NotPressed     6
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
    Digits_t mSeconds;
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
} cursor;

/*************************Variables****************************/

Time_t Time;
Time_t Stopwatch;
uint32_t CurrentPressedSwitch = _SWITCH_NUM;
WatchViews CurrentWatchView = DefaultView;
uint8_t default_counter;
uint8_t mode_counter;
uint8_t EditTime_counter;
uint8_t stopwatch_counter;
uint8_t stopwatchcounter;
uint8_t stopwatchstate = STOP;
uint8_t editstate;
uint8_t arrowcounter;
char DateString[] = "01-04-2024";
char TimeString[10];
char *StopWatchbuffer = TimeString;
uint8_t ReceiveBuffer[1]={NotPressed};
uint8_t SendBuffer [1];

/*************************Functions ProtoTypes****************************/
void CurrDate(void);
void CurrDateAndTime(void);
void DisplayTime(void);
void DisplayDate(void);
void Display_DefaultView();
void Display_ModeView(void);
void SwitchControl(void);
void StopWatch(void);
void DisplayStopWatch(void);
void Display_StopWatch(void);
void Display_EditTime(void);
void EditTime(void);
void DisplayBlinkingTime(void);
void DisplayBlinkingDate(void);

/*****************************Implementation*******************************/
void SwitchControl(void)
{
    uint16_t modestatus;
    uint8_t idx;
    static uint8_t count[_SWITCH_NUM];
    for (idx = 0; idx < _SWITCH_NUM; idx++)
    {
        SWITCH_GETSTATUS(idx, &modestatus);
        if (modestatus == SWITCH_PRESSED)
        {
            if (count[idx] == 0)
            {
                count[idx]++;
                CurrentPressedSwitch = idx+1;
                idx = _SWITCH_NUM;
                SendBuffer[0] = CurrentPressedSwitch;
                Uart_TxBufferAsync(SendBuffer, 1, UART_1, NULL);
            }
        }
        else
        {
            count[idx] = 0;
        }
    }
}

void Demo_Runnable(void)
{
    static uint8_t ControlSwitch = NotPressed;
    SwitchControl();
    Uart_RxBufferAsync(ReceiveBuffer, 1, UART_1,NULL);
    ControlSwitch = ReceiveBuffer[0];
    CurrDateAndTime();
    StopWatch();
    EditTime();

    if (ControlSwitch > 0 && ControlSwitch < NotPressed)
    {
        LCD_clearScreenAsyn();
        ReceiveBuffer[0] = NotPressed;
        switch (ControlSwitch)
        {
        case MODE:
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
        case UP:
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
                EditTime_counter = 0;
                editstate = inc;
                break;
            default:
                break;
            }
            break;
        case DOWN:
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
                EditTime_counter = 0;
                editstate = dec;
                break;
            default:
                break;
            }
            break;

        case RIGHT:
            switch (CurrentWatchView)
            {
            case EditTimeView:
                EditTime_counter = 0;
                arrowcounter++;
                if (arrowcounter == 14)
                {
                    arrowcounter = 0;
                }
                break;

            default:
                break;
            }
            break;

        case LEFT:
            switch (CurrentWatchView)
            {
            case EditTimeView:
                EditTime_counter = 0;
                if (arrowcounter == 0)
                {
                    arrowcounter = 14;
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

    else
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
        DisplayBlinkingDate();
    }
    if (EditTime_counter==3)
    {
        LCD_setCursorPosAsyn(1,0);
    }
    if (EditTime_counter==4)
    {
        DisplayBlinkingTime();
        EditTime_counter=0;
    }
    
}
 // enter every 50 milliseconds
void CurrDateAndTime(void) // enter every 50 milliseconds
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
            if (Time.Minutes.digit1 == 5)
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
        }
        if (Time.Hours.digit0 > 3 && Time.Hours.digit1 == 2 && Time.Minutes.digit0 == 9 && Time.Minutes.digit1 == 5 && Time.Seconds.digit0 == 9 && Time.Seconds.digit1 == 5)
        {
            Time.Hours.digit0 = 0;
            Time.Hours.digit1 = 0;
            Date.Day.digit0++;
        }
        sec_counter = 0;
    }

/* Function that calculate real date */
    if (Date.Day.digit0 == 9)
    {
        Date.Day.digit0 = 0;
        Date.Day.digit1++;
    }
    if (MONTH==0)
    {
        Date.Month.digit0=1;
        Date.Month.digit1=0;
    }
    if (DAY==0)
    {
        Date.Day.digit0=1;
        Date.Day.digit1=0;
    }
    if (DAY>29)
    {
        if (MONTH==2)
        {
            Date.Month.digit0++;
            Date.Day.digit0=1;
            Date.Day.digit1=0;
        }
    }
    if (DAY>30)
    {
        if (MONTH%2==0 && MONTH!=10 && MONTH != 12 )
        {
            Date.Month.digit0++;
            Date.Day.digit0=1;
            Date.Day.digit1=0;
        }
    }
    if (DAY>31)
    {
        if (MONTH%2 || MONTH == 10 || MONTH == 12 )
        {
            Date.Month.digit0++;
            Date.Day.digit0=1;
            Date.Day.digit1=0;
        }

    }
    if (Date.Month.digit0 == 9 && Date.Month.digit1 < 2)
    {
        Date.Month.digit0=0;
        Date.Month.digit1++;
    }
    if(MONTH > 12)
    {
        Date.Month.digit0 = 1;
        Date.Month.digit1 = 0;
    }
    sec_counter += Periodicity;
}

void StopWatch(void)
{
    static uint32_t st_sw_counter = 0;
    if (stopwatchstate == START)
    {
        stopwatchcounter=0;
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
            if (Date.Day.digit1 == 4)
            {
                Date.Day.digit1 = 0;
            }
            else
            {
                Date.Day.digit1++;
            }
            break;
        case 1:
            if (Date.Day.digit0 == 9)
            {
                Date.Day.digit0 = 0;
            }
            else if (Date.Day.digit1 == 3 &&  Date.Day.digit0 >1)
            {
                Date.Day.digit1=0;
            }
            else
            {
                Date.Day.digit0++;
            }
            break;
        case 2:
            if (Date.Month.digit1 == 3)
            {
                Date.Month.digit1 = 0;
            }
            else
            {
                Date.Month.digit1++;
            }
            break;
        case 3:
            if (Date.Month.digit0 == 9)
            {
                Date.Month.digit0 = 0;
            }
            else
            {
                Date.Month.digit0++;
            }
            break;
        case 4:
            if (Date.Year.digit3 == 9)
            {
                Date.Year.digit3 = 0;
            }
            else
            {
                Date.Year.digit3++;
            }
            break;
        case 5:
            if (Date.Year.digit2 == 9)
            {
                Date.Year.digit2 = 0;
            }
            else
            {
                Date.Year.digit2++;
            }
            break;
        case 6:
            if (Date.Year.digit1 == 9)
            {
                Date.Year.digit1 = 0;
            }
            else
            {
                Date.Year.digit1++;
            }
            break;
        case 7:
            if (Date.Year.digit0 == 9)
            {
                Date.Year.digit0 = 0;
            }
            else
            {
                Date.Year.digit0++;
            }
            break;
        case 8:
            if (Time.Hours.digit1 == 2)
            {
                Time.Hours.digit1 = 0;
            }
            else
            {
                Time.Hours.digit1++;
            }
            break;
        case 9:
            if (Time.Hours.digit0 == 9)
            {
                Time.Hours.digit0 = 0;
            }
            else if (Time.Hours.digit1 == 2 && Time.Hours.digit0 >4)
            {
                Time.Hours.digit1=1;
            }
            else
            {
                Time.Hours.digit0++;
            }
            break;
        case 10:
            if (Time.Minutes.digit1 == 5)
            {
                Time.Minutes.digit1 = 0;
            }
            else
            {
                Time.Minutes.digit1++;
            }
            break;
        case 11:
            if (Time.Minutes.digit0 == 9)
            {
                Time.Minutes.digit0 = 0;
            }
            else
            {
                Time.Minutes.digit0++;
            }
            break;
        case 12:
            if (Time.Seconds.digit1 == 5)
            {
                Time.Seconds.digit1 = 0;
            }
            else
            {
                Time.Seconds.digit1++;
            }
            break;
        case 13:
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
            if (Date.Day.digit1 == 0)
            {
                Date.Day.digit1 = 3;
            }
            else
            {
                Date.Day.digit1--;
            }
            break;
        case 1:
            if (Date.Day.digit0 == 0)
            {
                Date.Day.digit0 = 9;
            }
            else
            {
                Date.Day.digit0--;
            }
            break;
        case 2:
            if (Date.Month.digit1 == 0)
            {
                Date.Month.digit1 = 3;
            }
            else
            {
                Date.Day.digit1--;
            }
            break;
        case 3:
            if (Date.Month.digit0 < 0)
            {
                Date.Month.digit0 = 9;
            }
            else
            {
                Date.Month.digit0--;
            }
            break;
        case 4:
            if (Date.Year.digit3 == 0)
            {
                Date.Year.digit3 = 9;
            }
            else
            {
                Date.Year.digit3--;
            }
            break;
        case 5:
            if (Date.Year.digit2 == 0)
            {
                Date.Year.digit2 = 9;
            }
            else
            {
                Date.Year.digit2--;
            }
            break;
        case 6:
            if (Date.Year.digit1 == 0)
            {
                Date.Year.digit1 = 9;
            }
            else
            {
                Date.Year.digit1--;
            }
            break;
        case 7:
            if (Date.Year.digit0 == 0)
            {
                Date.Year.digit0 = 9;
            }
            else
            {
                Date.Year.digit0--;
            }
            break;
        case 8:
            if (Time.Hours.digit1 == 0)
            {
                Time.Hours.digit1 = 2;
            }
            else
            {
                Time.Hours.digit1--;
            }
            break;
        case 9:
            if (Time.Hours.digit0 == 0)
            {
                Time.Hours.digit0 = 9;
            }
            else
            {
                Time.Hours.digit0--;
            }
            break;
        case 10:
            if (Time.Minutes.digit1 == 0)
            {
                Time.Minutes.digit1 = 5;
            }
            else
            {
                Time.Minutes.digit1--;
            }
            break;
        case 11:
            if (Time.Minutes.digit0 == 0)
            {
                Time.Minutes.digit0 = 9;
            }
            else
            {
                Time.Minutes.digit0--;
            }
            break;
        case 12:
            if (Time.Seconds.digit1 == 0)
            {
                Time.Seconds.digit1 = 9;
            }
            else
            {
                Time.Seconds.digit1--;
            }
            break;
        case 13:
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

void DisplayTime(void)
{
    TimeString[0] = 48 + Time.Hours.digit1;
    TimeString[1] = 48 + Time.Hours.digit0;
    TimeString[2] = 58;
    TimeString[3] = 48 + Time.Minutes.digit1;
    TimeString[4] = 48 + Time.Minutes.digit0;
    TimeString[5] = 58;
    TimeString[6] = 48 + Time.Seconds.digit1;
    TimeString[7] = 48 + Time.Seconds.digit0;
    LCD_writeStringAsyn(TimeString, 8);
}

void DisplayBlinkingDate(void)
{
    static uint8_t date_blink;
    CurrDateAndTime();
    switch (arrowcounter)
    {
    case 0:
        if (date_blink < 10)
        {
            DateString[0] ='_';
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = 48 + Date.Year.digit0;
            date_blink++;
        }
        else if (date_blink >= 10)
        {
            DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = 48 + Date.Year.digit0;
            date_blink--;
        }
        LCD_writeStringAsyn(DateString, 10);
        break;
    case 1:
        if (date_blink < 10)
        {
            DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = '_';
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = 48 + Date.Year.digit0;
            date_blink++;
        }
        else if (date_blink >= 10)
        {
            DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = 48 + Date.Year.digit0;
            date_blink--;
        }
        LCD_writeStringAsyn(DateString, 10);
        break;
    case 2:
        if (date_blink < 10)
        {
            DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = '_';
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = 48 + Date.Year.digit0;
            date_blink++;
        }
        else if (date_blink >= 10)
        {
             DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = 48 + Date.Year.digit0;
            date_blink--;
        }
        LCD_writeStringAsyn(DateString, 10);
        break;
    case 3:
        if (date_blink < 10)
        {
             DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] =  '_';
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = 48 + Date.Year.digit0;
            date_blink++;
        }
        else if (date_blink >= 10)
        {
             DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = 48 + Date.Year.digit0;
            date_blink--;
        }
        LCD_writeStringAsyn(DateString, 10);
        break;
    case 4:
     if (date_blink < 10)
        {
             DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = '_' + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = 48 + Date.Year.digit0;
            date_blink++;
        }
        else if (date_blink >= 10)
        {
            DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = 48 + Date.Year.digit0;
            date_blink--;
        }
        LCD_writeStringAsyn(DateString, 10);
        break;
    case 5:
        if (date_blink < 10)
        {
             DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = '_';
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = 48 + Date.Year.digit0;
            date_blink++;
        }
        else if (date_blink >= 10)
        {
            DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = 48 + Date.Year.digit0;
            date_blink--;
        }
        LCD_writeStringAsyn(DateString, 10);
        break;
    case 6:
     if (date_blink < 10)
        {
            DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = '_';
            DateString[9] = 48 + Date.Year.digit0;
            date_blink++;
        }
        else if (date_blink >= 10)
        {
            DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = 48 + Date.Year.digit0;
            date_blink--;
        }
        LCD_writeStringAsyn(DateString, 10);
        break;
        case 7:
        if (date_blink < 10)
        {
            DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = '_';
            date_blink++;
        }
        else if (date_blink >= 10)
        {
            DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = 48 + Date.Year.digit0;
            date_blink--;
        }
        LCD_writeStringAsyn(DateString, 10);
        break;

    default:
            DateString[0] = 48 + Date.Day.digit1;
            DateString[1] = 48 + Date.Day.digit0;
            DateString[2] = 45;
            DateString[3] = 48 + Date.Month.digit1;
            DateString[4] = 48 + Date.Month.digit0;
            DateString[5] = 45;
            DateString[6] = 48 + Date.Year.digit3;
            DateString[7] = 48 + Date.Year.digit2;
            DateString[8] = 48 + Date.Year.digit1;
            DateString[9] = 48 + Date.Year.digit0;
            LCD_writeStringAsyn(DateString,10);
        break;
    }
    editstate = 0;
}
void DisplayBlinkingTime(void)
{
    static uint8_t time_blink;
    switch (arrowcounter)
    {
    case 8:
        if (time_blink < 10)
        {
            TimeString[0] = 48 + Time.Hours.digit1;
            TimeString[1] = 48 + Time.Hours.digit0;
            TimeString[2] = 58;
            TimeString[3] = 48 + Time.Minutes.digit1;
            TimeString[4] = 48 + Time.Minutes.digit0;
            TimeString[5] = 58;
            TimeString[6] = 48 + Time.Seconds.digit1;
            TimeString[7] = 48 + Time.Seconds.digit0;
            time_blink++;
        }
        else if (time_blink >= 10)
        {
            TimeString[0] = '_';
            TimeString[1] = 48 + Time.Hours.digit0;
            TimeString[2] = 58;
            TimeString[3] = 48 + Time.Minutes.digit1;
            TimeString[4] = 48 + Time.Minutes.digit0;
            TimeString[5] = 58;
            TimeString[6] = 48 + Time.Seconds.digit1;
            TimeString[7] = 48 + Time.Seconds.digit0;
            time_blink--;
        }
        LCD_writeStringAsyn(TimeString, 8);
        break;
    case 9:
        if (time_blink < 10)
        {
            TimeString[0] = 48 + Time.Hours.digit1;
            TimeString[1] = 48 + Time.Hours.digit0;
            TimeString[2] = 58;
            TimeString[3] = 48 + Time.Minutes.digit1;
            TimeString[4] = 48 + Time.Minutes.digit0;
            TimeString[5] = 58;
            TimeString[6] = 48 + Time.Seconds.digit1;
            TimeString[7] = 48 + Time.Seconds.digit0;
            time_blink++;
        }
        else if (time_blink >= 10)
        {
            TimeString[0] = 48 + Time.Hours.digit1;
            TimeString[1] = '_';
            TimeString[2] = 58;
            TimeString[3] = 48 + Time.Minutes.digit1;
            TimeString[4] = 48 + Time.Minutes.digit0;
            TimeString[5] = 58;
            TimeString[6] = 48 + Time.Seconds.digit1;
            TimeString[7] = 48 + Time.Seconds.digit0;
            time_blink--;
        }
        LCD_writeStringAsyn(TimeString, 8);
        break;
    case 10:
        if (time_blink < 10)
        {
            TimeString[0] = 48 + Time.Hours.digit1;
            TimeString[1] = 48 + Time.Hours.digit0;
            TimeString[2] = 58;
            TimeString[3] = 48 + Time.Minutes.digit1;
            TimeString[4] = 48 + Time.Minutes.digit0;
            TimeString[5] = 58;
            TimeString[6] = 48 + Time.Seconds.digit1;
            TimeString[7] = 48 + Time.Seconds.digit0;
            time_blink++;
        }
        else if (time_blink >= 10)
        {
            TimeString[0] = 48 + Time.Hours.digit1;
            TimeString[1] = 48 + Time.Hours.digit0;
            TimeString[2] = 58;
            TimeString[3] = '_';
            TimeString[4] = 48 + Time.Minutes.digit0;
            TimeString[5] = 58;
            TimeString[6] = 48 + Time.Seconds.digit1;
            TimeString[7] = 48 + Time.Seconds.digit0;
            time_blink--;
        }
        LCD_writeStringAsyn(TimeString, 8);
        break;
    case 11:
        if (time_blink < 10)
        {
            TimeString[0] = 48 + Time.Hours.digit1;
            TimeString[1] = 48 + Time.Hours.digit0;
            TimeString[2] = 58;
            TimeString[3] = 48 + Time.Minutes.digit1;
            TimeString[4] = 48 + Time.Minutes.digit0;
            TimeString[5] = 58;
            TimeString[6] = 48 + Time.Seconds.digit1;
            TimeString[7] = 48 + Time.Seconds.digit0;
            time_blink++;
        }
        else if (time_blink >= 10)
        {
            TimeString[0] = 48 + Time.Hours.digit1;
            TimeString[1] = 48 + Time.Hours.digit0;
            TimeString[2] = 58;
            TimeString[3] = 48 + Time.Minutes.digit1;
            TimeString[4] = '_';
            TimeString[5] = 58;
            TimeString[6] = 48 + Time.Seconds.digit1;
            TimeString[7] = 48 + Time.Seconds.digit0;
            time_blink--;
        }
        LCD_writeStringAsyn(TimeString, 8);
        break;
    case 12:
        if (time_blink < 10)
        {
            TimeString[0] = 48 + Time.Hours.digit1;
            TimeString[1] = 48 + Time.Hours.digit0;
            TimeString[2] = 58;
            TimeString[3] = 48 + Time.Minutes.digit1;
            TimeString[4] = 48 + Time.Minutes.digit0;
            TimeString[5] = 58;
            TimeString[6] = 48 + Time.Seconds.digit1;
            TimeString[7] = 48 + Time.Seconds.digit0;
            time_blink++;
        }
        else if (time_blink >= 10)
        {
            TimeString[0] = 48 + Time.Hours.digit1;
            TimeString[1] = 48 + Time.Hours.digit0;
            TimeString[2] = 58;
            TimeString[3] = 48 + Time.Minutes.digit1;
            TimeString[4] = 48 + Time.Minutes.digit0;
            TimeString[5] = 58;
            TimeString[6] = '_';
            TimeString[7] = 48 + Time.Seconds.digit0;
            time_blink--;
        }
        LCD_writeStringAsyn(TimeString, 8);
        break;
    case 13:
        if (time_blink < 10)
        {
            TimeString[0] = 48 + Time.Hours.digit1;
            TimeString[1] = 48 + Time.Hours.digit0;
            TimeString[2] = 58;
            TimeString[3] = 48 + Time.Minutes.digit1;
            TimeString[4] = 48 + Time.Minutes.digit0;
            TimeString[5] = 58;
            TimeString[6] = 48 + Time.Seconds.digit1;
            TimeString[7] = 48 + Time.Seconds.digit0;
            time_blink++;
        }
        else if (time_blink >= 10)
        {
            TimeString[0] = 48 + Time.Hours.digit1;
            TimeString[1] = 48 + Time.Hours.digit0;
            TimeString[2] = 58;
            TimeString[3] = 48 + Time.Minutes.digit1;
            TimeString[4] = 48 + Time.Minutes.digit0;
            TimeString[5] = 58;
            TimeString[6] = 48 + Time.Seconds.digit1;
            TimeString[7] = '_';
            time_blink--;
        }
        LCD_writeStringAsyn(TimeString, 8);
        break;
    default:
            TimeString[0] = 48 + Time.Hours.digit1;
            TimeString[1] = 48 + Time.Hours.digit0;
            TimeString[2] = 58;
            TimeString[3] = 48 + Time.Minutes.digit1;
            TimeString[4] = 48 + Time.Minutes.digit0;
            TimeString[5] = 58;
            TimeString[6] = 48 + Time.Seconds.digit1;
            TimeString[7] = 48 + Time.Seconds.digit0;
            LCD_writeStringAsyn(TimeString, 8);
        break;
    }
    editstate = 0;
}

void DisplayDate()
{
    // Date.Day.digit0 = 5;
    // Date.Month.digit0 = 4;
    // Date.Year.digit0 = 4;
    // Date.Year.digit1 = 2;
    // Date.Year.digit2 = 0;
    // Date.Year.digit3 = 2;
    DateString[0] = 48 + Date.Day.digit1;
    DateString[1] = 48 + Date.Day.digit0;
    DateString[2] = 45;
    DateString[3] = 48 + Date.Month.digit1;
    DateString[4] = 48 + Date.Month.digit0;
    DateString[5] = 45;
    DateString[6] = 48 + Date.Year.digit3;
    DateString[7] = 48 + Date.Year.digit2;
    DateString[8] = 48 + Date.Year.digit1;
    DateString[9] = 48 + Date.Year.digit0;

    LCD_writeStringAsyn(DateString, 10);
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


