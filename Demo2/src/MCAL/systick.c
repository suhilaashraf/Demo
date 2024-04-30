/*
 * systick.c
 *
 *  Created on: Mar 11, 2024
 *      Author: adel
 */
#include <systick.h>
#define STK_BASE_ADDRESS 			0xE000E010
#define STK     ((volatile STK_Registers_t * const)STK_BASE_ADDRESS)

#define STK_LOAD_MASK	0x00FFFFFF
#define STK_VAL_MASK 	0x00FFFFFF


#define STK_ENABLE_INTERRUPT 0x00000003
#define STK_CNT_FLAG	0x00010000

static systickCBF_t APPCBF=0;


uint32_t STK_Mode=0;
typedef struct
{
	uint32_t CRTL;
	uint32_t LOAD;
	uint32_t VAL;
	uint32_t CALIB;
}STK_Registers_t;

ErrorStatus_t systick_SetTimerMs(uint32_t Time)
{
	ErrorStatus_t RetErrorStatus=NOK;
	//STK->LOAD &= ~(STK_LOAD_MASK);
	STK->LOAD = ((CLK_MHZ/(8*1000))*Time)-1;
	STK->VAL=0;
	return RetErrorStatus;
}
ErrorStatus_t systick_Start(uint32_t Mode)
{
	ErrorStatus_t RetErrorStatus=NOK;
	STK_Mode=Mode;
	//STK->VAL &= ~STK_VAL_MASK;
	STK->CRTL |= 1;
	STK->CRTL |= 1<<1;

	return RetErrorStatus;
}

ErrorStatus_t systick_Stop()
{
	ErrorStatus_t RetErrorStatus=NOK;
	STK->CRTL &= ~STK_VAL_MASK;
	return RetErrorStatus;
}
ErrorStatus_t systick_isExpired(uint8_t* add_value)
{
	ErrorStatus_t RetErrorStatus=NOK;
	*add_value= STK->CRTL & STK_CNT_FLAG;
	return RetErrorStatus;
}

ErrorStatus_t systick_setCallBack(systickCBF_t CallBackFun)
{
	ErrorStatus_t RetErrorStatus=NOK;
	if (CallBackFun)
	{
		APPCBF=CallBackFun;
	}
	return RetErrorStatus;
}
void SysTick_Handler(void)
{
	if (STK_Mode==SINGLE)
	{
		STK->CRTL &= ~STK_VAL_MASK;
		if (APPCBF)
		APPCBF();
	}
	else
	{
		if (APPCBF)
		APPCBF();
	}
}





