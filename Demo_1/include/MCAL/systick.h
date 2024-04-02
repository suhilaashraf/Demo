#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>
#include <Errors.h>

#define CLK_MHZ		16000000
#define PERIODIC	2
#define SINGLE		1

typedef void (*systickCBF_t)(void);


ErrorStatus_t systick_SetTimerMs(uint32_t Time);
ErrorStatus_t systick_Start(uint32_t Mode);
ErrorStatus_t systick_Stop();
ErrorStatus_t systick_isExpired(uint8_t* add_value);
ErrorStatus_t systick_setCallBack(systickCBF_t CallBackFun);



#endif
