/*
 * Sched.c
 * Author: Suhila Ashraf
 */
#include "Sched.h"

extern const Runnable_t Runnables_info [RUNNABLES_NUM];
uint32_t Pendingticks;


static void Sched(void);
static void TICK_cb(void);

void Sched_init(void)
{
    systick_setCallBack(TICK_cb);
    systick_SetTimerMs(SCH_TICKS_MS);
}

void Sched_Start(void)
{
    systick_Start(PERIODIC);
    while (1)
    {
        if (Pendingticks)
        {
            Pendingticks--;
            Sched();
        }
    }
}

static void Sched(void)
{
    uint32_t Localindex;
    static uint32_t timestamp = 1;
    for (Localindex = 0; Localindex < RUNNABLES_NUM; Localindex++)
    {
        if ((Runnables_info[Localindex].cbf) && (timestamp % Runnables_info[Localindex].periodicity_ms == 0))
        {
            Runnables_info[Localindex].cbf();
        }
    }
    timestamp += SCH_TICKS_MS;

}

static void TICK_cb(void)
{
    Pendingticks++;
}