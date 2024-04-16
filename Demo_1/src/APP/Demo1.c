#include "Switch.h"
#include "Uart.h"
#include "LCDinterface.h"
#include "Demo.h"

/***************************************************/
#define Ascii0 48
#define AsciiColon 58
#define AsciiDash 45
#define START 1
#define PAUSE 2
#define STOP 3
#define Periodicity 50
#define inc 1
#define dec 2


#define UP          1
#define DOWN        2
#define RIGHT       3
#define LEFT        4
#define MODE        5  
#define NotPressed  6

#define READY 0x0A
#define BUSY  0x0B

#define ONES 1
#define TENS 10
#define HUNDREDS 100
#define THOUSANDS 1000

#define GET_DIGIT(number, position) ((number / position) % 10)

typedef struct
{
    uint8_t mSeconds;
    uint8_t Seconds;
    uint8_t Minutes;
    uint8_t Hours;
} Time_t;

/*************************Variables****************************/
struct
{
    uint8_t Day;
    uint8_t Month;
    uint16_t Year;
} Date;

Time_t Time;
Time_t Stopwatch;
uint32_t CurrentPressedSwitch = NotPressed;
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
uint8_t ReceiveBuffer[1] = {NotPressed};
uint8_t SendBuffer[1];
uint8_t Txflag;
uint8_t uart_state;

/*************************Functions ProtoTypes****************************/
void app_init();
void CurrDateAndTime(void);
void DisplayTime(void);
void DisplayDate(void);
void Display_DefaultView();
void Display_ModeView(void);
void SwitchControl(void);
void StopWatch(void);
void Stopwatch_helper(void);
void Display_StopWatch(void);
void Display_EditTime(void);
void EditTime(void);
void BlinkTime(void);
void BlinkDate(void);

/*****************************Implementation*******************************/
void app_init()
{
    Date.Day = 17;
    Date.Month = 4;
    Date.Year = 2024;
    Time.Hours = 6;
    Time.Minutes = 0;
    Time.Seconds = 0;
    SWITCH_init();
    LCD_initAsyn();
}
void Demo_Runnable(void)
{
    static uint8_t ControlSwitch = NotPressed;
    SwitchControl();
    Uart_RxBufferAsync(ReceiveBuffer, 1, UART_1, NULL);
    ControlSwitch = ReceiveBuffer[0];
    EditTime();
    CurrDateAndTime();
    StopWatch();
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
                if (arrowcounter == 2 || arrowcounter == 5 || arrowcounter == 12 || arrowcounter == 15)
                {
                    arrowcounter++;
                }
                else if (arrowcounter == 18)
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
                arrowcounter--;
                if (arrowcounter == 2 || arrowcounter == 5 || arrowcounter == 12 || arrowcounter == 15)
                {
                    arrowcounter--;
                }
                if (arrowcounter == 0)
                {
                    arrowcounter = 18;
                }
                
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
            Display_DefaultView();

            break;
        }
    }
}

void DisplayDate()
{
    DateString[0] = Ascii0 + GET_DIGIT(Date.Day, TENS);
    DateString[1] = Ascii0 + GET_DIGIT(Date.Day, ONES);
    DateString[2] = AsciiDash;
    DateString[3] = Ascii0 + GET_DIGIT(Date.Month, TENS);
    DateString[4] = Ascii0 + GET_DIGIT(Date.Month, ONES);
    DateString[5] = AsciiDash;
    DateString[6] = Ascii0 + GET_DIGIT(Date.Year, THOUSANDS);
    DateString[7] = Ascii0 + GET_DIGIT(Date.Year, HUNDREDS);
    DateString[8] = Ascii0 + GET_DIGIT(Date.Year, TENS);
    DateString[9] = Ascii0 + GET_DIGIT(Date.Year, ONES);

    LCD_writeStringAsyn(DateString, 10);
}

void DisplayTime(void)
{
    TimeString[0] = Ascii0 + GET_DIGIT(Time.Hours, TENS);
    TimeString[1] = Ascii0 + GET_DIGIT(Time.Hours, ONES);
    TimeString[2] = AsciiColon;
    TimeString[3] = Ascii0 + GET_DIGIT(Time.Minutes, TENS);
    TimeString[4] = Ascii0 + GET_DIGIT(Time.Minutes, ONES);
    TimeString[5] = AsciiColon;
    TimeString[6] = Ascii0 + GET_DIGIT(Time.Seconds, TENS);
    TimeString[7] = Ascii0 + GET_DIGIT(Time.Seconds, ONES);
    LCD_writeStringAsyn(TimeString, 8);
}

void CurrDateAndTime(void) // enter every 50 milliseconds
{
    static uint32_t seconds = 0;
    if (seconds == 1000)
    {
        Time.Seconds++;
        seconds = 0;
        if (Time.Seconds > 59)
        {
            Time.Seconds = 0;
            Time.Minutes++;
            if (Time.Minutes > 59)
            {
                Time.Minutes = 0;
                Time.Hours++;
                if (Time.Hours > 23)
                {
                    Time.Hours = 0;
                    Date.Day++;
                }
            }
        }
    }
    seconds += Periodicity;
    if (Date.Day > 31)
    {
        Date.Day = 1;
        Date.Month++;
    }
    else if (Date.Day > 30)
    {
        if (Date.Month % 2 == 0 && Date.Month != 10 && Date.Month != 12)
        {
            Date.Day = 1;
            Date.Month++;
        }
    }
    else if (Date.Day>29)
    {
        if (Date.Month==2)
        {
            Date.Day=1;
            Date.Month++;
        }
    }
    else if (Date.Day>28)
    {
        if (Date.Month==2 && Date.Year%4)
        {
            Date.Day=1;
            Date.Month++;   
        }
    }
    if (Date.Month>12)
    {
        Date.Month=1;
    }
    
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
        Stopwatch_helper();
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
        BlinkDate();
    }
    if (EditTime_counter == 3)
    {
        LCD_setCursorPosAsyn(1, 0);
    }
    if (EditTime_counter == 4)
    {
        BlinkTime();
        EditTime_counter = 0;
    }
}
// enter every 50 milliseconds

/*??Do we need a condition for hours??*/
void StopWatch(void)
{
    static uint32_t st_sw_counter = 0;
    if (stopwatchstate == START)
    {
        stopwatchcounter = 0;
        if (st_sw_counter == 100)
        {
            Stopwatch.mSeconds++;
            if (Stopwatch.mSeconds > 9)
            {
                Stopwatch.mSeconds = 0;
                Stopwatch.Seconds++;
                if (Stopwatch.Seconds > 59)
                {
                    Stopwatch.Seconds = 0;
                    Stopwatch.Minutes++;
                    if (Stopwatch.Minutes > 59)
                    {
                        Stopwatch.Minutes = 0;
                        Stopwatch.Hours++;
                    }
                }
            }
            st_sw_counter = 0;
        }
        st_sw_counter += Periodicity;
    }
    if (stopwatchstate == STOP)
    {
        Stopwatch.mSeconds = 0;
        Stopwatch.Seconds = 0;
        Stopwatch.Minutes = 0;
        Stopwatch.Hours = 0;
    }
}

void EditTime(void)
{
    if (editstate == inc)
    {
        switch (arrowcounter)
        {
        case 0:
            if (GET_DIGIT(Date.Day,TENS)==3)
            {
                Date.Day=GET_DIGIT(Date.Day,ONES);
            }
            else
            Date.Day += 10;
            break;
        case 1:
            if (GET_DIGIT(Date.Day,ONES)==9)
            {
                Date.Day=GET_DIGIT(Date.Day,TENS)*10;
            }
            else
            Date.Day++;
            break;
        case 3:
            if (GET_DIGIT(Date.Month,TENS)==1)
            {
                Date.Month=GET_DIGIT(Date.Month,ONES);
            }
            else
            Date.Month += 10;
            break;
        case 4:
            if (GET_DIGIT(Date.Month,ONES)==9)
            {
                Date.Month=GET_DIGIT(Date.Month,TENS)*10;
            }
            else
            Date.Month++;
            break;

        case 6:
            if (GET_DIGIT(Date.Year,THOUSANDS)==9)
            {
                Date.Year=Date.Year%1000;
            }
            else
            Date.Year += 1000;
            break;
        case 7:
            if (GET_DIGIT(Date.Year,HUNDREDS)==9)
            {
                Date.Year = (Date.Year/1000) * 1000 + Date.Year % 100;
            }
            else
            Date.Year += 100;
            break;
        case 8:
            if (GET_DIGIT(Date.Year,TENS)==9)
            {
                Date.Year = (Date.Year/100)*100 + Date.Year % 10;
            }
            else
            Date.Year += 10;

            break;
        case 9:
            if (GET_DIGIT(Date.Year,ONES)==9)
            {
                Date.Year=(Date.Year/10)*10;
            }
            else
            Date.Year++;
            break;
        case 10:
            if (GET_DIGIT(Time.Hours,TENS)== 2)
            {
                Time.Hours = GET_DIGIT(Time.Hours,ONES);
            }
            else
            Time.Hours += 10;
            break;
        case 11:
            if (GET_DIGIT(Time.Hours,ONES)==9 || (GET_DIGIT(Time.Hours,ONES)==3 && GET_DIGIT(Time.Hours,TENS)==2))
            {
                Time.Hours = GET_DIGIT(Time.Hours,TENS)*10;
            }
            else
            Time.Hours++;
            break;
        case 13:
            if (GET_DIGIT(Time.Minutes,TENS)== 5)
            {
                Time.Minutes = GET_DIGIT(Time.Minutes,ONES);
            }
            else
            Time.Minutes += 10;
            break;
        case 14:
            if (GET_DIGIT(Time.Minutes,ONES)== 9)
            {
                Time.Hours = GET_DIGIT(Time.Minutes,TENS)*10;
            }
            else
            Time.Minutes++;
            break;
        case 16:
            if (GET_DIGIT(Time.Seconds,TENS) == 5)
            {
                Time.Seconds = GET_DIGIT(Time.Seconds,ONES);
            }
            else
            Time.Seconds += 10;
            break;
        case 17:
            if (GET_DIGIT(Time.Seconds,ONES) == 9)
            {
                Time.Seconds = GET_DIGIT(Time.Seconds,TENS)*10;
            }
            else
            Time.Seconds++;
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
            if (GET_DIGIT(Date.Day,TENS)==0)
            {
                Date.Day=30+GET_DIGIT(Date.Day,ONES);
            }
            else
            Date.Day -= 10;
            break;
        case 1:
            if (GET_DIGIT(Date.Day,ONES)==0)
            {
                Date.Day=GET_DIGIT(Date.Day,TENS)*10+9;
            }
            else
            Date.Day--;
            break;
        case 3:
            if (GET_DIGIT(Date.Month,TENS)==0)
            {
                Date.Month=10+GET_DIGIT(Date.Month,ONES);
            }
            else
            Date.Month -= 10;
            break;
        case 4:
            if (GET_DIGIT(Date.Month,ONES)==0)
            {
                Date.Month=GET_DIGIT(Date.Month,TENS)*10+9;
            }
            else
            Date.Month--;
            break;

        case 6:
            if (GET_DIGIT(Date.Year,THOUSANDS)==0)
            {
                Date.Year=9000+Date.Year%1000;
            }
            else
            Date.Year -= 1000;
            break;
        case 7:
            if (GET_DIGIT(Date.Year,HUNDREDS)==0)
            {
                Date.Year = 900 + (Date.Year/1000) * 1000 + Date.Year % 100;
            }
            else
            Date.Year -= 100;
            break;
        case 8:
            if (GET_DIGIT(Date.Year,TENS)==0)
            {
                Date.Year = 90 + (Date.Year/100)*100 + Date.Year % 10;
            }
            else
            Date.Year -= 10;

            break;
        case 9:
            if (GET_DIGIT(Date.Year,ONES)==9)
            {
                Date.Year=9+(Date.Year/10)*10;
            }
            else
            Date.Year--;
            break;
        case 10:
            if (GET_DIGIT(Time.Hours,TENS)== 0)
            {
                Time.Hours =20 + GET_DIGIT(Time.Hours,ONES);
            }
            else
            Time.Hours -= 10;
            break;
        case 11:
            if (GET_DIGIT(Time.Hours,ONES) == 0 && GET_DIGIT(Time.Hours,TENS)!=2)
            {
                Time.Hours = 9 + GET_DIGIT(Time.Hours,TENS)*10;
            }
            else if (GET_DIGIT(Time.Hours,ONES) == 0 && GET_DIGIT(Time.Hours,TENS)==2)
            {
                Time.Hours = 3 + GET_DIGIT(Time.Hours,TENS)*10;
            }
            else
            Time.Hours--;
            break;
        case 13:
            if (GET_DIGIT(Time.Minutes,TENS)== 0)
            {
                Time.Minutes = 50 + GET_DIGIT(Time.Minutes,ONES);
            }
            else
            Time.Minutes -= 10;
            break;
        case 14:
            if (GET_DIGIT(Time.Minutes,ONES)== 0)
            {
                Time.Hours = 9+GET_DIGIT(Time.Minutes,TENS)*10;
            }
            else
            Time.Minutes--;
            break;
        case 16:
            if (GET_DIGIT(Time.Seconds,TENS) == 0)
            {
                Time.Seconds = 50 + GET_DIGIT(Time.Seconds,ONES);
            }
            else
            Time.Seconds -= 10;
            break;
        case 17:
            if (GET_DIGIT(Time.Seconds,ONES) == 0)
            {
                Time.Seconds = 9 + GET_DIGIT(Time.Seconds,TENS) * 10;
            }
            else
            Time.Seconds--;
            break;
        default:
            break;
        }
        editstate = 0;
    }
}

void BlinkDate(void)
{
    static uint8_t date_blink;
    if (date_blink < 3)
    {
        DateString[0] = Ascii0 + GET_DIGIT(Date.Day, TENS);
        DateString[1] = Ascii0 + GET_DIGIT(Date.Day, ONES);
        DateString[2] = AsciiDash;
        DateString[3] = Ascii0 + GET_DIGIT(Date.Month, TENS);
        DateString[4] = Ascii0 + GET_DIGIT(Date.Month, ONES);
        DateString[5] = AsciiDash;
        DateString[6] = Ascii0 + GET_DIGIT(Date.Year, THOUSANDS);
        DateString[7] = Ascii0 + GET_DIGIT(Date.Year, HUNDREDS);
        DateString[8] = Ascii0 + GET_DIGIT(Date.Year, TENS);
        DateString[9] = Ascii0 + GET_DIGIT(Date.Year, ONES);
        date_blink++;
        if (date_blink == 3)
            date_blink = 6;
    }
    else if (date_blink > 3 && arrowcounter <= 9)
    {
        DateString[arrowcounter] = '_';
        date_blink--;
        if (date_blink == 3)
        {
            date_blink = 0;
        }
    }
    LCD_writeStringAsyn(DateString, 10);
}
void BlinkTime(void)
{
    static uint8_t time_blink;
    if (time_blink < 3)
    {
        TimeString[0] = Ascii0 + GET_DIGIT(Time.Hours, TENS);
        TimeString[1] = Ascii0 + GET_DIGIT(Time.Hours, ONES);
        TimeString[2] = AsciiColon;
        TimeString[3] = Ascii0 + GET_DIGIT(Time.Minutes, TENS);
        TimeString[4] = Ascii0 + GET_DIGIT(Time.Minutes, ONES);
        TimeString[5] = AsciiColon;
        TimeString[6] = Ascii0 + GET_DIGIT(Time.Seconds, TENS);
        TimeString[7] = Ascii0 + GET_DIGIT(Time.Seconds, ONES);
        time_blink++;
        if (time_blink == 3)
            time_blink = 6;
    }
    else if (time_blink > 3 && arrowcounter > 9)
    {
        TimeString[arrowcounter - 10] = '_';
        time_blink--;
        if (time_blink == 3)
        {
            time_blink = 0;
        }
    }
    LCD_writeStringAsyn(TimeString, 8);
}
void Stopwatch_helper(void)
{
    StopWatchbuffer[0] = Ascii0 + GET_DIGIT(Stopwatch.Hours, TENS);
    StopWatchbuffer[1] = Ascii0 + GET_DIGIT(Stopwatch.Hours, ONES);
    StopWatchbuffer[2] = AsciiColon;
    StopWatchbuffer[3] = Ascii0 + GET_DIGIT(Stopwatch.Minutes, TENS);
    StopWatchbuffer[4] = Ascii0 + GET_DIGIT(Stopwatch.Minutes, ONES);
    StopWatchbuffer[5] = AsciiColon;
    StopWatchbuffer[6] = Ascii0 + GET_DIGIT(Stopwatch.Seconds, TENS);
    StopWatchbuffer[7] = Ascii0 + GET_DIGIT(Stopwatch.Seconds, ONES);
    StopWatchbuffer[8] = AsciiColon;
    StopWatchbuffer[9] = Ascii0 + GET_DIGIT(Stopwatch.mSeconds, TENS);
    StopWatchbuffer[10] = Ascii0 + GET_DIGIT(Stopwatch.mSeconds, ONES);
    LCD_writeStringAsyn(StopWatchbuffer, 11);
}
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
                CurrentPressedSwitch = idx + 1;
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