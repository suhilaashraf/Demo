#ifndef LED_H_
#define LED_H_

#include "std_types.h"
#include "Mask.h"
#include "Errors.h"
#include "Ledcfg.h"
#include "gpio.h"


#define LED_ACTIVE_HIGH 0
#define LED_ACTIVE_LOW 1
#define LED_OFF 0
#define LED_ON 1

#define LED_PORTA  (void *)(0x40020000)
#define LED_PORTB	(void *)(0x40020400)
#define LED_PORTC  (void *)(0x40020800)

#define LED_PIN0 	0x00000000
#define LED_PIN1 	0x00000001
#define LED_PIN2 	0x00000002
#define LED_PIN3 	0x00000003
#define LED_PIN4 	0x00000004
#define LED_PIN5 	0x00000005
#define LED_PIN6 	0x00000006
#define LED_PIN7 	0x00000007
#define LED_PIN8 	0x00000008
#define LED_PIN9	0x00000009
#define LED_PIN10 	0x0000000A
#define LED_PIN11 	0x0000000B
#define LED_PIN12 	0x0000000C
#define LED_PIN13 	0x0000000D
#define LED_PIN14 	0x0000000E
#define LED_PIN15 	0x0000000F

typedef struct
{
	void* ledPORT;
	uint32_t ledPIN;
	uint32_t ledconnection; //led forward or reversed
}LedCfg_str;

Error_enumStatus_t LED_init ();
Error_enumStatus_t Led_SetStatus(uint8_t Led,uint8_t Led_state);

#endif