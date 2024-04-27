/*
 * NVIC.c
 *
 *  Created on: Mar 4, 2024
 *      Author: adel
 */
#include <NVIC.h>

#define NVIC_ISER_BASE_ADDRESS 		0xE000E100
#define NVIC_ISER ((volatile uint32_t* const)NVIC_ISER_BASE_ADDRESS)

#define NVIC_ICER_BASE_ADDRESS 		0xE000E180
#define NVIC_ICER ((volatile uint32_t* const )NVIC_ICER_BASE_ADDRESS)

#define NVIC_ISPR_BASE_ADDRESS 		0xE000E200
#define NVIC_ISPR ((volatile uint32_t* const)NVIC_ISPR_BASE_ADDRESS)

#define NVIC_IABR_BASE_ADDRESS 		0xE000E300
#define NVIC_IABR ((volatile uint32_t* const) NVIC_IABR_BASE_ADDRESS)

#define NVIC_IPR_BASE_ADDRESS 		0xE000E400
#define NVIC_IPR ((volatile uint32_t* const) NVIC_IPR_BASE_ADDRESS)

#define CBR_AIRCR_BASE_ADDRESS		 0xE000ED0C
#define CBR_AIRCR *((volatile uint32_t* const) CBR_AIRCR_BASE_ADDRESS)

#define PRI_GROUP_MASK 				0xFFFF0700
#define PRI_OFFSET 			4

#define PRI_WRITE_MASK      0x05FA0000

ErrorStatus_t NVIC_EnableIRQ(IRQ_t IRQn)
{
	ErrorStatus_t RetErrorStatus=NOK;
	if (IRQn>IRQ_SPI4)
	{
		RetErrorStatus=ParameterError;
	}
	else
	{
		RetErrorStatus=OK;
		NVIC_ISER[IRQn/32] = 1<<(IRQn%32);
	}

	return RetErrorStatus;
}

ErrorStatus_t NVIC_DisableIRQ(IRQ_t IRQn)
{
	ErrorStatus_t RetErrorStatus=NOK;
	if (IRQn>IRQ_SPI4)
	{
		RetErrorStatus=ParameterError;
	}
	else
	{
		RetErrorStatus=OK;
		NVIC_ICER [IRQn/32] = 1<<(IRQn%32);
	}
	return RetErrorStatus;
}

ErrorStatus_t NVIC_SetPendingIRQ(IRQ_t IRQn)
{
	ErrorStatus_t RetErrorStatus=NOK;
	if (IRQn>IRQ_SPI4)
		{
			RetErrorStatus=ParameterError;
		}
		else
		{
			RetErrorStatus=OK;
			NVIC_ISPR[IRQn/32] = 1<<(IRQn%32);
		}

	return RetErrorStatus;
}

ErrorStatus_t NVIC_GetActive(IRQ_t IRQn,uint8_t* add_retvalue)
{
	ErrorStatus_t RetErrorStatus=NOK;
	*add_retvalue = NVIC_IABR[IRQn/32] & 1<<(IRQn%32);
	return RetErrorStatus;
}

ErrorStatus_t NVIC_GetPendingIRQ (IRQ_t IRQn, uint8_t* add_retvalue )
{
	ErrorStatus_t RetErrorStatus=NOK;
	*add_retvalue = NVIC_ISPR[IRQn/32] & 1<<(IRQn%32);
	return RetErrorStatus;
}
ErrorStatus_t NVIC_SetPriorityGrouping (uint32_t group)
{
	ErrorStatus_t RetErrorStatus=NOK;
	uint32_t loc_register = CBR_AIRCR;
	loc_register &= ~(PRI_GROUP_MASK);
	loc_register |= group;
	CBR_AIRCR=loc_register;
	return RetErrorStatus;
}

ErrorStatus_t NVIC_SetPriority(IRQ_t IRQn, uint16_t priority)
{
	ErrorStatus_t RetErrorStatus=NOK;
	uint32_t loc_register=NVIC_IPR[IRQn/4];
	loc_register &= ~(0xF<<(8*(IRQn%4))<<PRI_OFFSET);
	loc_register|= priority<<(8*(IRQn%4)+4);
	NVIC_IPR[IRQn/4]=loc_register;
	return RetErrorStatus;
}



