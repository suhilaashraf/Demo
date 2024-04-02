#ifndef GPIO_H_
#define GPIO_H_

/********************Includes ********************/
#include "Mask.h"
#include "Errors.h"
#include "std_types.h"
/********************GPIO ports********************/
#define GPIO_PORTA  (void *)(0x40020000)
#define GPIO_PORTB	(void *)(0x40020400)
#define GPIO_PORTC  (void *)(0x40020800)
/*
#define GPIO_PORTD 	(void *)(0x40020C00)
#define GPIO_PORTE 	(void *)(0x40021000)
#define GPIO_PORTH	(void *)(0x40021C00)*/

/********************GPIO pins********************/
#define GPIO_PIN0 	0x00000000
#define GPIO_PIN1 	0x00000001
#define GPIO_PIN2 	0x00000002
#define GPIO_PIN3 	0x00000003
#define GPIO_PIN4 	0x00000004
#define GPIO_PIN5 	0x00000005
#define GPIO_PIN6 	0x00000006
#define GPIO_PIN7 	0x00000007
#define GPIO_PIN8 	0x00000008
#define GPIO_PIN9	0x00000009
#define GPIO_PIN10 	0x0000000A
#define GPIO_PIN11 	0x0000000B
#define GPIO_PIN12 	0x0000000C
#define GPIO_PIN13 	0x0000000D
#define GPIO_PIN14 	0x0000000E
#define GPIO_PIN15 	0x0000000F

/********************GPIO pins Mode********************/

/**
 * Mask divided to 4 bytes:
 * first byte: ID for parameter check
 * 2nd byte : GPIOMODE 
 * 3rd byte : GPIOPUPD
 * 4th byte :GPIO OTYP
 * 
*/
#define GPIO_INPUT_FL		(uint32_t)0x00000000
#define GPIO_INPUT_PU		(uint32_t)0x00010001
#define GPIO_INPUT_PD		(uint32_t)0x00020002

#define GPIO_OUTPUT_PP 		(uint64_t)0x00000103
#define GPIO_OUTPUT_PP_PU	(uint32_t)0x00010104
#define GPIO_OUTPUT_PP_PD	(uint32_t)0x00020105
#define GPIO_OUTPUT_OD		(uint32_t)0x01000106
#define GPIO_OUTPUT_OD_PU	(uint32_t)0x01010107
#define GPIO_OUTPUT_OD_PD	(uint32_t)0x01020108

#define GPIO_AF_PP 			(uint32_t)0x0000020A
#define GPIO_AF_PP_PU		(uint32_t)0x0001020B
#define GPIO_AF_PP_PD		(uint32_t)0x0002020C
#define GPIO_AF_OD			(uint32_t)0x0100020D
#define GPIO_AF_OD_PU		(uint32_t)0x0101020E
#define GPIO_AF_OD_PD		(uint32_t)0x0102020F


#define GPIO_ANALOG 		(uint32_t)0x00000309



/************** GPIO Alternate Functions **************/

#define GPIO_AF_SYSTEM          0x00000000
#define GPIO_AF_TIM1_2          0x00000001
#define GPIO_AF_TIM3_5         	0x00000002
#define GPIO_AF_TIME9_11       	0x00000003
#define GPIO_AF_I2C1_3         	0x00000004
#define GPIO_AF_SPI1_4         	0x00000005
#define GPIO_AF_SPI3            0x00000006
#define GPIO_AF_USART1_2        0x00000007
#define GPIO_AF_USART6          0x00000008
#define GPIO_AF_I2C2_3          0x00000009
#define GPIO_AF_OTG_FS          0x0000000A
#define GPIO_AF_SDIO            0x0000000C
#define GPIO_AF_EVENTOUT        0x0000000F

/********************GPIO SPEED ********************/

#define GPIO_LOWSPEED       (uint32_t)0x00000010
#define GPIO_MEDIUMSPEED    (uint32_t)0x00000111
#define GPIO_HIGHSPEED      (uint32_t)0x00000212
#define GPIO_VERYHIGHSPEED  (uint32_t)0x00000313

#define MAX_SPEED  			(uint32_t)0x00030013

#define MIN_SPEED


/********************GPIO pinStatus********************/

#define LOGIC_HIGH        (1u)
#define LOGIC_LOW         (0u)

#define BSRR_SHIFTMASK     16
#define SHIFTMASK 			2

/********************GPIO pins CONFIGURATIONS ********************/
typedef struct
{
	void * gpioPORT;
	uint32_t gpioPIN;
	uint32_t gpioMODE;
	uint32_t gpioSPEED;
	uint8_t GPIO_AF;
}GPIOPIN_t;
/********************GPIO APIS Prototypes ********************/

ErrorStatus_t GPIO_InitPin(GPIOPIN_t* gpiopin);

ErrorStatus_t GPIO_SetPinValue(void* PORT , uint32_t gpio_pinNumber , uint8_t gpiopinStatus);

ErrorStatus_t GPIO_GetPinValue(void* PORT , uint32_t gpio_pinNumber , uint8_t* gpiopinStatus);

ErrorStatus_t GPIO_TogglePin (void* PORT , uint32_t gpio_pinNumber);


#endif
