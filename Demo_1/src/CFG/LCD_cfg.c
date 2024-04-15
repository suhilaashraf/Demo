#include "LCDinterface.h"
#include "gpio.h"

const LCD_cfg_t LCD_pins [_LCD_ARR_SIZE]=
{
	[_RS]=
	{
		.LCD_Port = GPIO_PORTA,
		.LCD_Pin = GPIO_PIN8    
	},
	[_RW]=
	{
		.LCD_Port = GPIO_PORTA,
		.LCD_Pin = GPIO_PIN9
	},
	[_EN]=
	{
		.LCD_Port = GPIO_PORTA,
		.LCD_Pin = GPIO_PIN10
	},		
	[_D0]=
	{
		.LCD_Port = GPIO_PORTA,
		.LCD_Pin = GPIO_PIN0
	},	
	[_D1]=
	{
		.LCD_Port = GPIO_PORTA,
		.LCD_Pin =GPIO_PIN1
	},	
	[_D2]=
	{
		.LCD_Port = GPIO_PORTA,
		.LCD_Pin = GPIO_PIN2
	},
	[_D3]=
	{
		.LCD_Port = GPIO_PORTA,
		.LCD_Pin = GPIO_PIN3
	},		
	[_D4]=
	{
		.LCD_Port = GPIO_PORTA,
		.LCD_Pin = GPIO_PIN4
	},	
	[_D5]=
	{
		.LCD_Port = GPIO_PORTA,
		.LCD_Pin = GPIO_PIN5
	},	
	[_D6]=
	{
		.LCD_Port = GPIO_PORTA,
		.LCD_Pin = GPIO_PIN6
	},
	[_D7]=
	{
		.LCD_Port = GPIO_PORTA,
		.LCD_Pin = GPIO_PIN7
	},	 
	
};
