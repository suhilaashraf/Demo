#include "Sched.h"

/******************** System Runnables Prototypes********************/
void LCD_task();
void Demo_Runnable(void);
void SWITCH_Runabble(void);

const Runnable_t Runnables_info [RUNNABLES_NUM]=
{
    [0]=
    {
        .name = "LCD_TASK",
        .periodicity_ms = 2,
        .cbf = LCD_task,
    },
    [1]=
    {
        .name = "Demo1",
        .periodicity_ms = 25,
        .cbf = Demo_Runnable,
    },
    [2]=
    {
        .name = "Switch",
        .periodicity_ms = 5,
        .cbf = SWITCH_Runabble ,
    },


/*
   [1]=
    {
        .name = "Demo1",
        .periodicity_ms = 500,
        .priority = 1,
        .cbf = Deafult_WatchViewRunnable,
    },
    [2]=
    {
        .name = "Switch",
        .periodicity_ms = 5,
        .priority = 2,
        .cbf = SWITCH_Runabble,
    },*/
};


   
   /* [0]=
    {
        .name = "Traffic",
        .periodicity_ms = 1000,
        .priority = 0,
        .cbf = Traffic_Runnable,
    },
    
    

    [1] =
    {
        .name = "APP1",
        .periodicity_ms = 50,
        .priority = 1,
        .cbf = APP1,
    }
    
        [0] =
    {
        .name = "Uart_TxBuffer",
        .periodicity_ms = 1,
        .priority = 0,
        .cbf = Uart_TxBuffer,
    },    
    [1]=
    {
        .name = "Uart_RxBuffer",
        .periodicity_ms = 1,
        .priority = 1,
        .cbf = Uart_Rxbuffer,
    },   */
    
