#include "Sched.h"

/******************** System Runnables Prototypes********************/

void SWITCH_Runabble(void);
void demo2_runnable();

Runnable_t Runnables_info [RUNNABLES_NUM]=
{
    [0]=
    {
        .name = "DEMO",
        .periodicity_ms = 1,
        .cbf = demo2_runnable,
        .FirstDelay=0,
    },
    [1]=
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




    
