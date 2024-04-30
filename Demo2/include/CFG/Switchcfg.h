#ifndef SWITCHCFG_H_
#define SWITCHCFG_H_

typedef enum
{
	Switch_one,
	
	_SWITCH_NUM  /*THE USER SHOULDNOT CHANGE IT */
}SWITCHES_enu;

typedef struct
{
	void* SwitchPORT;
	uint32_t SwitchPINnumber;
	uint32_t SwitchConnection;
}SWITCHCFG_str;


#endif  /*SWITCHCFG_H_*/

