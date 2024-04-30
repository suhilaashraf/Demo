#include "gpio.h"

/********************GPIO MASKS********************/
#define GPIO_GETMODE		3
#define GPIO_GETAF			0x0000000F
#define AF_SHIFTMASK 		4
#define ShiftFactor 		8
/********************GPIO Registers********************/
typedef struct
{
	uint32_t MODER     ;
	uint32_t OTYPER    ;
	uint32_t OSPEEDR   ;
	uint32_t PUPDR     ;
	uint32_t IDR       ;
	uint32_t ODR       ;
	uint32_t BSRR      ;
	uint32_t LCKR      ;
	uint32_t AFRL      ;
	uint32_t AFRH      ;
}GPIO_t;



/********************GPIO APIS Implementations********************/
ErrorStatus_t GPIO_InitPin(GPIOPIN_t* gpiopin)
{
	ErrorStatus_t RETURN_ERRORSTATUS=NOK;
	uint8_t* gpioptr =NULL;
	uint32_t localmask;
	gpioptr = (uint8_t*)&gpiopin->gpioMODE;
	if (gpiopin == NULL)
	{
		RETURN_ERRORSTATUS= NULL_PointerError ;
	}
	else if( !((gpiopin->gpioPORT == GPIO_PORTA)||(gpiopin->gpioPORT == GPIO_PORTB)||(gpiopin->gpioPORT == GPIO_PORTC)))
	{
		RETURN_ERRORSTATUS=ParameterError;
	}
	else if(gpiopin->gpioPIN > 15)
	{
		RETURN_ERRORSTATUS=ParameterError;
	}
	//check on masks of modes
	else if (*gpioptr > 15)
	{
		RETURN_ERRORSTATUS=ParameterError;
	}
	else
	{
		gpioptr = (uint8_t*)&gpiopin->gpioSPEED;
	}
	if((*gpioptr < 16)&&(*gpioptr >19))
	{
		RETURN_ERRORSTATUS=ParameterError;
	}
	else
	{
		RETURN_ERRORSTATUS=OK;
		gpioptr++;

		localmask = ((GPIO_t*)gpiopin->gpioPORT)->OSPEEDR;
		localmask &= ~ (GPIO_GETMODE << (gpiopin->gpioPIN *SHIFTMASK ) );
		localmask |= ( (uint32_t)(*gpioptr) << (gpiopin->gpioPIN *SHIFTMASK ) );
		((GPIO_t*)gpiopin->gpioPORT)->OSPEEDR = localmask;

		gpioptr = (uint8_t*)&gpiopin->gpioMODE ;
		gpioptr++;
		localmask = ((GPIO_t*)gpiopin->gpioPORT)->MODER;
		localmask &=~ (GPIO_GETMODE << (gpiopin->gpioPIN *SHIFTMASK ) );
		localmask |= ( (uint32_t)(*gpioptr) << (gpiopin->gpioPIN *SHIFTMASK ) );
		((GPIO_t*)gpiopin->gpioPORT)->MODER = localmask;

		gpioptr++;
		localmask = ((GPIO_t*)gpiopin->gpioPORT)->PUPDR;
		localmask &=~ ( GPIO_GETMODE << (gpiopin->gpioPIN *SHIFTMASK ) );
		localmask |= ((uint32_t)(*gpioptr) <<(gpiopin->gpioPIN *SHIFTMASK) );
		((GPIO_t*)gpiopin->gpioPORT)->PUPDR = localmask;

		gpioptr++;
		if (BIT_0_MASK&(uint32_t)(*gpioptr))
		{
			((GPIO_t*)gpiopin->gpioPORT)->OTYPER |=( (BIT_0_MASK&(uint32_t)(*gpioptr))<<(gpiopin->gpioPIN ) );
		}
		else
		{
			((GPIO_t*)gpiopin->gpioPORT)->OTYPER &= ~( (BIT_0_MASK&(uint32_t)(*gpioptr))<<(gpiopin->gpioPIN ) );
		}

		
		if (gpiopin->GPIO_AF < 8)
		{
			localmask= ((GPIO_t*)gpiopin->gpioPORT)->AFRL;
			localmask &= ~ (GPIO_GETAF << (gpiopin->gpioPIN *AF_SHIFTMASK));
			localmask |= gpiopin->GPIO_AF <<(gpiopin->gpioPIN *AF_SHIFTMASK) ;
			((GPIO_t*)gpiopin->gpioPORT)->AFRL = localmask;
		}
		else
		{
			localmask= ((GPIO_t*)gpiopin->gpioPORT)->AFRH;
			localmask &= ~ (GPIO_GETAF << ((gpiopin->gpioPIN - ShiftFactor)*AF_SHIFTMASK));
			localmask|= gpiopin->GPIO_AF<<((gpiopin->gpioPIN - ShiftFactor)*AF_SHIFTMASK) ;
			((GPIO_t*)gpiopin->gpioPORT)->AFRH = localmask;
		}
	}
	return RETURN_ERRORSTATUS;
}


ErrorStatus_t GPIO_SetPinValue(void* PORT , uint32_t gpio_pinNumber , uint8_t gpiopinStatus)
{
	ErrorStatus_t RETURN_ERRORSTATUS=NOK;
	if (PORT == NULL)
	{
		RETURN_ERRORSTATUS= NULL_PointerError ;
	}
	else if( !((PORT == GPIO_PORTA)||(PORT == GPIO_PORTB)||(PORT == GPIO_PORTC)))
	{
		RETURN_ERRORSTATUS=ParameterError;
	}
	else if(gpio_pinNumber > 15)
	{
		RETURN_ERRORSTATUS=ParameterError;
	}
	else if(gpiopinStatus > LOGIC_HIGH)
	{
		RETURN_ERRORSTATUS=ParameterError;
	}
	else
	{
		RETURN_ERRORSTATUS=OK;
		if (BIT_0_MASK&gpiopinStatus)
		{
			((GPIO_t*)PORT)->BSRR |= (BIT_0_MASK << gpio_pinNumber) ;
		}
		else
		{
			((GPIO_t*)PORT)->BSRR |= (BIT_0_MASK << (gpio_pinNumber+BSRR_SHIFTMASK) ) ;
		}
	}
	return RETURN_ERRORSTATUS;
}

ErrorStatus_t GPIO_GetPinValue(void* PORT , uint32_t gpio_pinNumber , uint8_t* gpiopinStatus)
{
	ErrorStatus_t RETURN_ERRORSTATUS=NOK;
	if (PORT == NULL)
	{
		RETURN_ERRORSTATUS= NULL_PointerError ;
	}
	else if( !((PORT == GPIO_PORTA)||(PORT == GPIO_PORTB)||(PORT == GPIO_PORTC)))
	{
		RETURN_ERRORSTATUS=ParameterError;
	}
	else if(gpio_pinNumber > 15)
	{
		RETURN_ERRORSTATUS=ParameterError;
	}
	else if(gpiopinStatus == NULL )
	{
		RETURN_ERRORSTATUS=ParameterError;
	}
	else
	{
		RETURN_ERRORSTATUS=OK;
		if ( ( (BIT_0_MASK << (gpio_pinNumber*2) ) & ( ( (GPIO_t*)PORT )->MODER) )&& (!((BIT_1_MASK << (gpio_pinNumber*2) ) & ( ( (GPIO_t*)PORT )->MODER))) ) //check if the pin is configuered input or output
		{
			* gpiopinStatus = ( ( ( ((GPIO_t*)PORT)->ODR) >> gpio_pinNumber ) & (1) );
		}
		else
		{
			* gpiopinStatus = ( ( ( ((GPIO_t*)PORT)->IDR ) >> gpio_pinNumber ) & (1) );
		}
	}
	return RETURN_ERRORSTATUS;
}


ErrorStatus_t GPIO_TogglePin (void* PORT , uint32_t gpio_pinNumber)
{
	ErrorStatus_t ReturnStatus = NOK;
	if( !((PORT == GPIO_PORTA)||(PORT == GPIO_PORTB)||(PORT == GPIO_PORTC)))
	{
		ReturnStatus=ParameterError;
	}
	else if(gpio_pinNumber > 15)
	{
		ReturnStatus=ParameterError;
	}	
	else
	{
		ReturnStatus=OK;
		if (((GPIO_t*)PORT)->ODR & (1<<gpio_pinNumber))
		{
			((GPIO_t*)PORT)->BSRR |= (BIT_0_MASK << (gpio_pinNumber+BSRR_SHIFTMASK) ) ;
		}
		else
		{
			((GPIO_t*)PORT)->BSRR |= (BIT_0_MASK << gpio_pinNumber) ;
		}
	}
	return ReturnStatus;
}










