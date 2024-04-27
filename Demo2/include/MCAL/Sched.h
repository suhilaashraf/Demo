/*
 * Sched.h
 * Author: Suhila Ashraf
 */
#ifndef SCHED_H_
#define SCHED_H_

#include "std_types.h"
#include "Errors.h"
#include "systick.h"

#define RUNNABLES_NUM 2
#define SCH_TICKS_MS 1

typedef void (*Runnable_Cb_t)(void);

typedef struct
{
    char * name;
    uint32_t periodicity_ms;
    Runnable_Cb_t cbf ;
    uint32_t FirstDelay;
} Runnable_t;


void Sched_init(void);
void Sched_Start(void);

#endif
