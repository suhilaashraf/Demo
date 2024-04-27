#include "Switch.h"


SWITCHCFG_str Switches [_SWITCH_NUM ]=
{
		[Switch_up]=
		{
				.SwitchPORT = SWITCH_PORTA,
				.SwitchPINnumber = SWITCH_PIN1,
				.SwitchConnection = SWITCH_PULLDOWN,
		},					
};
