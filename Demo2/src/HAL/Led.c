#include "Led.h"

extern LedCfg_str Ledcfg [_LEDNUM];



Error_enumStatus_t LED_init ()
{
	Error_enumStatus_t RETURN_ERRORSTATUS=Status_enumNotOk;
	uint8_t Localcounter=0;
	GPIOPIN_t LED;
	uint8_t errorcounter=0;
	LED.gpioSPEED = GPIO_HIGHSPEED;
	for(Localcounter=0;Localcounter <_LEDNUM;Localcounter++)
	{
		LED.gpioMODE = GPIO_OUTPUT_PP;
		LED.gpioPORT = Ledcfg [Localcounter].ledPORT;
		LED.gpioPIN =  Ledcfg [Localcounter].ledPIN;
		RETURN_ERRORSTATUS = GPIO_InitPin(&LED);
		if (RETURN_ERRORSTATUS != Status_enumOk )
		{
			errorcounter++;
		}
	}
	if (errorcounter == 0)
	{
		RETURN_ERRORSTATUS = Status_enumOk;
	}
	else
	{
		RETURN_ERRORSTATUS = Status_enumParameterError;
	}
	return RETURN_ERRORSTATUS;
}



Error_enumStatus_t Led_SetStatus(uint8_t Led,uint8_t Led_state)
{
	Error_enumStatus_t RETURN_ERRORSTATUS=Status_enumNotOk;
	if (Led >= _LEDNUM)
	{
		RETURN_ERRORSTATUS=Status_enumParameterError;
	}
	else if ( !((Ledcfg [Led].ledconnection==LED_ACTIVE_HIGH)||(Ledcfg [Led].ledconnection==LED_ACTIVE_LOW)) )
	{
		RETURN_ERRORSTATUS=Status_enumParameterError;
	}
	else if ( !((Led_state == LED_ON)||(Led_state ==LED_OFF)) )
	{
		RETURN_ERRORSTATUS=Status_enumParameterError;
	}
	else
	{
		RETURN_ERRORSTATUS=Status_enumOk;
		GPIO_SetPinValue(Ledcfg [Led].ledPORT,Ledcfg [Led].ledPIN,(Led_state^Ledcfg [Led].ledconnection));
	}

	return RETURN_ERRORSTATUS;
}



