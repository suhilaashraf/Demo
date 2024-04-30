#include "Switch.h"


SWITCHCFG_str Switches [_SWITCH_NUM ]=
{
		[Switch_one]=
		{
				.SwitchPORT = SWITCH_PORTA,
				.SwitchPINnumber = SWITCH_PIN8,
				.SwitchConnection = SWITCH_PULLDOWN,
		},					
};
