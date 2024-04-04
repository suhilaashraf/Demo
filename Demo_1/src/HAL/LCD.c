/*
 * LCD.c
 *
 *  Created on: Mar 20, 2024
 *      Author: Amina Ismail
 */
#include "LCDinterface.h"
#include "GPIO.h"
#include "sched.h"
/******************************LCD states***********************************/
#define OFF 		0
#define INIT		1
#define OPERATION	2

/******************************LCD init states***********************************/
#define POWER_ON		0
#define FUNCTIONAL_SET	0x38
#define DISPLAY_ON_ON	0x0E
#define DISPLAY_ON_OFF	0x0C
#define DISPLAY_CLEAR	0x01
#define	ENTRY_MODE		0x06
#define END				0x10

/*********************************user request types********************************************/
#define REQ_WRITE	1
#define REQ_SET_CURSOR	2
#define REQ_CLEAR_SCREEN	3
#define REQ_GET_STATE	4
#define REQ_INIT		5
#define NO_REQ	100

/***********************************user request states****************************************/
#define BUSY	1
#define	READY 	0

uint8_t EN_status;
uint8_t LCD_State;
uint8_t init_State;
uint8_t flag[4]={0};

struct{
	uint8_t type;
	uint8_t state;
	char* string;
	uint8_t len;
	uint8_t positionX;
	uint8_t positionY;
}user_request;


extern const LCD_cfg_t LCD_pins[_LCD_ARR_SIZE];

void LCD_task();
static void initSM();
static void statLCD_Write_Command(uint8_t command);
static ErrorStatus_t stat_writeString();
static void statLCD_setCursorPos();
static void statLCD_clearScreen();

ErrorStatus_t LCD_initAsyn()
{
	ErrorStatus_t RetErrorStatus=NOK;
	if (user_request.state==READY)
	{
		RetErrorStatus=OK;
		user_request.state=BUSY;
		user_request.type=REQ_INIT;
	}
	else
		RetErrorStatus=ConfigurationError;

	return RetErrorStatus;
}
ErrorStatus_t LCD_writeStringAsyn(char*string,uint8_t length)
{
	ErrorStatus_t RetErrorStatus=NOK;
	if (string==0)
	{
		RetErrorStatus=NULL_PointerError;
	}
	else if (user_request.state==READY)
	{
		RetErrorStatus=OK;
		user_request.state=BUSY;
		user_request.type=REQ_WRITE;
		user_request.string=string;
		user_request.len=length;
	}
	else
		RetErrorStatus=ConfigurationError;
	return RetErrorStatus;
}
ErrorStatus_t LCD_setCursorPosAsyn(uint8_t posX,uint8_t posY)
{
	ErrorStatus_t RetErrorStatus=NOK;
	if (posX>SECOND_LINE)
	{
		RetErrorStatus=ParameterError;
	}
	else if (user_request.state==READY)
	{
		RetErrorStatus=OK;
		user_request.state=BUSY;
		user_request.type=REQ_SET_CURSOR;
		user_request.positionX=posX;
		user_request.positionY=posY;
	}
	else
		RetErrorStatus=ConfigurationError;
	return RetErrorStatus;
}
ErrorStatus_t LCD_clearScreenAsyn()
{
	ErrorStatus_t RetErrorStatus=NOK;
	if (user_request.state==READY)
	{
		RetErrorStatus=OK;
		user_request.state=BUSY;
		user_request.type=REQ_CLEAR_SCREEN;
	}
	else
		RetErrorStatus=ConfigurationError;
	return RetErrorStatus;
}

ErrorStatus_t LCD_getState(uint8_t* state)
{
	ErrorStatus_t RetErrorStatus=NOK;
	if (state==0)
	{
		RetErrorStatus=NULL_PointerError;
	}
	else
	{
		RetErrorStatus=OK;
		*state=LCD_State;
	}
	return RetErrorStatus;

}



static void statLCD_Write_Command(uint8_t command)
{
	GPIO_SetPinValue(LCD_pins[_RS].LCD_Port,LCD_pins[_RS].LCD_Pin,LOGIC_LOW);				/*	switch to instruction mode	*/
	GPIO_SetPinValue(LCD_pins[_RW].LCD_Port,LCD_pins[_RW].LCD_Pin,LOGIC_LOW);				/*	switch to write mode	*/

	for(uint8_t idx=_D0;idx<=_D7;idx++)
	{
		GPIO_SetPinValue(LCD_pins[idx].LCD_Port,LCD_pins[idx].LCD_Pin,(command>>(idx-_D0) & 1));	/*	assign commands to data pins bit by bit	*/
	}
	GPIO_GetPinValue(LCD_pins[_EN].LCD_Port,LCD_pins[_EN].LCD_Pin,&EN_status);
	EN_status=EN_status^1;
	GPIO_SetPinValue(LCD_pins[_EN].LCD_Port,LCD_pins[_EN].LCD_Pin,EN_status);

}

static void initSM()
{
	flag[init_done]=0;
	GPIOPIN_t LCD_initPin;
	switch(init_State)
	{
	case POWER_ON:
		LCD_initPin.gpioSPEED = GPIO_HIGHSPEED;
		LCD_initPin.gpioMODE = GPIO_OUTPUT_PP;
		uint8_t idx=0;
		for (idx = 0; idx <_LCD_ARR_SIZE; idx++)
		{
			LCD_initPin.gpioPORT = LCD_pins[idx].LCD_Port;
			LCD_initPin.gpioPIN = LCD_pins[idx].LCD_Pin;
			GPIO_InitPin(&LCD_initPin);
		}
		init_State=FUNCTIONAL_SET;
		break;
	case FUNCTIONAL_SET:
		statLCD_Write_Command(FUNCTIONAL_SET);
		if (EN_status==0)
			init_State=DISPLAY_ON_OFF;
		break;
	case DISPLAY_ON_OFF:
		statLCD_Write_Command(DISPLAY_ON_OFF);
		if (EN_status==0)
			init_State=DISPLAY_CLEAR;;
		break;
	case DISPLAY_CLEAR:
		statLCD_Write_Command(DISPLAY_CLEAR);
		if (EN_status==0)
			init_State=ENTRY_MODE;
		break;
	case ENTRY_MODE:
		statLCD_Write_Command(ENTRY_MODE);
		if (EN_status==0)
		{
			init_State=END;
			user_request.state=READY;
		    flag[init_done]=1;

		}
		break;
	case END:
		break;
	default:
		break;

	}
}


static void statLCD_setCursorPos()
{
	flag[setcursor_done]=0;
	uint8_t Loc_location=0;
	/*	case: first row	*/
	if (user_request.positionX==0)
	{
		Loc_location=user_request.positionY;
	}
	/*	case: second row	*/
	else if (user_request.positionX==1)
	{
		Loc_location=0x40+user_request.positionY;				/* 0x40 is the address of the first bit in the second row	*/
	}
	statLCD_Write_Command(128+Loc_location);
	if (EN_status==0)
	{
		user_request.state=READY;
		flag[setcursor_done]=1;
	}
		
}
static ErrorStatus_t stat_writeString()
{
	flag[write_done]=0;
	ErrorStatus_t RetErrorStatus=NOK;
	GPIO_SetPinValue(LCD_pins[_RS].LCD_Port,LCD_pins[_RS].LCD_Pin,LOGIC_HIGH);				/*	switch to instruction mode	*/
	GPIO_SetPinValue(LCD_pins[_RW].LCD_Port,LCD_pins[_RW].LCD_Pin,LOGIC_LOW);				/*	switch to write mode	*/
	static uint8_t charIDX;
	if (charIDX<user_request.len) //&& user_request.string[charIDX]!= '\0')
	{
		for(uint8_t idx=_D0;(idx<=_D7);idx++)
		{
			GPIO_SetPinValue(LCD_pins[idx].LCD_Port,LCD_pins[idx].LCD_Pin,(user_request.string[charIDX]>>(idx-_D0)) & 1);	/*	assign commands to data pins bit by bit	*/
		}
		GPIO_GetPinValue(LCD_pins[_EN].LCD_Port,LCD_pins[_EN].LCD_Pin,&EN_status);
		EN_status=EN_status^1;
		GPIO_SetPinValue(LCD_pins[_EN].LCD_Port,LCD_pins[_EN].LCD_Pin,EN_status);
		if (EN_status==0)
			charIDX++;
	}
	else
	{
		charIDX=0;
		user_request.state=READY;
		flag[write_done]=1;
	}
	return RetErrorStatus;
}
static void statLCD_clearScreen()
{
	flag[clear_done]=0;
	statLCD_Write_Command(DISPLAY_CLEAR);
	if (EN_status==0)
	{
		user_request.state=READY;
		flag[clear_done]=1;
	}
}

void LCD_task()
{
	switch (LCD_State)
	{
	case INIT:
		initSM();
		if (init_State==END)
		{
			LCD_State=OPERATION;
		}
		break;
	case OPERATION:
		if(user_request.state==BUSY)
		{
			switch(user_request.type)
			{
			case REQ_WRITE:
				stat_writeString();
				break;
			case REQ_SET_CURSOR:
				statLCD_setCursorPos();
				break;
			case REQ_CLEAR_SCREEN:
				statLCD_clearScreen();
				break;
			default:
				break;
			}
		}
		break;
	case OFF:
		if(user_request.type==REQ_INIT)
			LCD_State=INIT;
		break;
	default:
		break;
	}
}






















