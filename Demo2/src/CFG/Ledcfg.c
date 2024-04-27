#include "Ledcfg.h"
#include "Led.h"

LedCfg_str Ledcfg[_LEDNUM] =
	{
		[Ledone] = {
			.ledPORT = LED_PORTA,
			.ledPIN = LED_PIN0,
			.ledconnection = LED_ACTIVE_HIGH,
		},
};