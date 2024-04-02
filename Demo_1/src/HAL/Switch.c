/*
 * Switch.c
 * Created on: Feb 26, 2024
 * Author: Suhila Ashraf
 */
#include "Switch.h"

extern SWITCHCFG_str Switches[_SWITCH_NUM];

uint8_t SwitchesStates[_SWITCH_NUM];

ErrorStatus_t SWITCH_init(void)
{
	ErrorStatus_t RETURN_ERRORSTATUS = NOK;
	uint8_t localCounter, Errorcheck = 0;
	GPIOPIN_t Switch;
	Switch.gpioSPEED = GPIO_HIGHSPEED;
	for (localCounter = 0; localCounter < _SWITCH_NUM; localCounter++)
	{
		Switch.gpioPORT = Switches[localCounter].SwitchPORT;
		Switch.gpioPIN = Switches[localCounter].SwitchPINnumber;
		Switch.gpioMODE = Switches[localCounter].SwitchConnection;
		RETURN_ERRORSTATUS = GPIO_InitPin(&Switch);
		if (RETURN_ERRORSTATUS != OK)
		{
			Errorcheck++;
		}
	}
	if (Errorcheck == 0)
	{
		RETURN_ERRORSTATUS = OK;
	}
	else
	{
		RETURN_ERRORSTATUS = ParameterError;
	}
	return RETURN_ERRORSTATUS;
}

ErrorStatus_t SWITCH_GETSTATUS(SWITCHES_enu Switchname, uint16_t *SwitchStatus)
{
	ErrorStatus_t RETURN_ERRORSTATUS = NOK;
	if (Switchname > _SWITCH_NUM)
	{
		RETURN_ERRORSTATUS = ParameterError;
	}
	else if (SwitchStatus == NULL)
	{
		RETURN_ERRORSTATUS = NULL_PointerError;
	}
	else
	{
		RETURN_ERRORSTATUS = OK;
		*SwitchStatus = SwitchesStates[Switchname];
		if (Switches[Switchname].SwitchConnection == SWITCH_PULLUP)
		{
			*SwitchStatus = *SwitchStatus ^ 1;
		}
	}
		return RETURN_ERRORSTATUS;
}

void SWITCH_Runabble(void)
{
	static uint8_t counter[_SWITCH_NUM];
	static uint8_t current;
	static uint8_t pervious[_SWITCH_NUM];
	uint8_t localindex;
	for (localindex = 0; localindex < _SWITCH_NUM; localindex++)
	{
		GPIO_GetPinValue(Switches[localindex].SwitchPORT, Switches[localindex].SwitchPINnumber,&current);
		if (current == pervious[localindex])
		{
			counter[localindex]++;
		}
		else
		{
			counter[localindex] = 0;
		}
		if (counter[localindex] % 5 == 0)
		{
			SwitchesStates[localindex] = current;
		}
		pervious[localindex] = current;
	}
}
