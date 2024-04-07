#include "Sched.h"

/******************** System Runnables Prototypes********************/
void LCD_task();
void Demo_Runnable(void);
void SWITCH_Runabble(void);
void SwitchControl(void);
void Uart_Runnable (void);
void Uart_TxRunnable(void);
void Uart_RxRunnable(void);

Runnable_t Runnables_info [RUNNABLES_NUM]=
{
    [0]=
    {
        .name = "LCD_TASK",
        .periodicity_ms = 2,
        .cbf = LCD_task,
        .FirstDelay=0,
    },
    [1]=
    {
        .name = "Demo1",
        .periodicity_ms = 50,
        .cbf = Demo_Runnable,
        .FirstDelay=3,
    },
    [2]=
    {
        .name = "Switch",
        .periodicity_ms = 5,
        .cbf = SWITCH_Runabble,
        .FirstDelay=0,
    },
/*    [4]=
    {
        .name = "uart runnable",
        .periodicity_ms = 100,
        .cbf =Uart_TxRunnable,
        .FirstDelay=0,          
    }
  [3]=
    {
        .name = "Switch control",
        .periodicity_ms = 50,
        .cbf =SwitchControl,
        .FirstDelay=0,       
    },*/
};




    
