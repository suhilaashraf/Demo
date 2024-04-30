/*
 * RCC.c
 *
 *  Created on: Feb 20, 2024
 *      Author: Amina Tawfik
 */
#include "RCC.h"

#define RCC_BASE_ADDRESS 					0x40023800
#define RCC     ((volatile RCC_Registers_t * const)RCC_BASE_ADDRESS)


#define SW_MASK 		0x03

#define SWS_MASK 		0x0000000C

#define PLL_SRC_MASK 	0x00400000
#define PLL_HSE 		0x00400000
#define PLL_HSI 		0x00000000

#define HSIRDY_FLAG 	0x00000002
#define HSERDY_FLAG 	0x00020000
#define PLLRDY_FLAG 	0x02000000

#define HPRE_MASK       0x000000F0
#define PPRE1_MASK		0x00001C00
#define PPRE2_MASK		0x0000E000





#define P_MASK 			0x00030000
#define M_MASK			0x0000003F
#define N_MASK			0x00007FC0

#define N_SHIFT			0x00000040


typedef struct{
	uint32_t CR;
	uint32_t PLLCFGR;
	uint32_t CFGR;
	uint32_t CIR;
	uint32_t AHB1RSTR;
	uint32_t AHB2RSTR;
	uint32_t Reserved0[2];
	uint32_t APB1RSTR;
	uint32_t APB2RSTR;
	uint32_t Reserved1[2];
	uint32_t AHB1ENR;
	uint32_t AHB2ENR;
	uint32_t Reserved2[2];
	uint32_t APB1ENR;
	uint32_t APB2ENR;
	uint32_t Reserved3[2];
	uint32_t AHB1LPENR;
	uint32_t AHB2LPENR;
	uint32_t Reserved4[2];
	uint32_t APB1LPENR;
	uint32_t APB2LPENR;
	uint32_t Reserved5[2];
	uint32_t RCC_BDCR;
	uint32_t RCC_CSR;
	uint32_t Reserved6[2];
    uint32_t RCC_SSCGR;
    uint32_t PLLI2SCFGR;
    uint32_t Reserved7;
    uint32_t DCKCFGR;
}RCC_Registers_t;






ErrorStatus_t RCC_ControlClock (uint64_t CLK_clock,uint8_t RCC_STAT)
{
	ErrorStatus_t RetErrorStatus=NOK;
	uint32_t* PTR_clock=0;
	PTR_clock = (uint32_t*) &CLK_clock;
	PTR_clock++;
	if ( (CLK_clock != CLK_HSE_EXT) && (CLK_clock != CLK_HSE_OSC) && (CLK_clock != CLK_HSI) && (CLK_clock != CLK_PLL) && (CLK_clock!= CLK_PLLI2S))
	{
		RetErrorStatus=ParameterError;
	}
	else if ( (RCC_STAT!=RCC_STAT_ON) && (RCC_STAT != RCC_STAT_OFF) )
	{
		RetErrorStatus=ParameterError;
	}
	else
	{
		RetErrorStatus=OK;
		if ( RCC_STAT	==	RCC_STAT_ON	)
		{
			RCC->CR|=(uint32_t)CLK_clock;
			uint16_t timeout=1000;
			while (!(RCC->CR & *(PTR_clock)) && timeout )
			{
				timeout--;
			}
			if (!(RCC->CR & *(PTR_clock)))
					{
						RetErrorStatus=ConfigurationError;
					}
		}
		else
		{
			RCC->CR&= ~(uint32_t)(CLK_clock);
			uint16_t timeout=1000;
			while ((RCC->CR & (uint32_t)*(PTR_clock)) && timeout )
			{
				timeout--;
			}
			if (!(RCC->CR & (uint32_t)PTR_clock))
			{
				RetErrorStatus=ConfigurationError;
			}
		}

	}
return RetErrorStatus;
}


ErrorStatus_t RCC_SelectSystemClock (uint32_t SYS_CLK_clock)
{
	ErrorStatus_t RetErrorStatus=NOK;
	if( (SYS_CLK_clock !=SYS_CLK_HSE) && (SYS_CLK_clock !=SYS_CLK_HSI) && (SYS_CLK_clock !=SYS_CLK_PLL)  )
	{
		RetErrorStatus=ParameterError;
	}
	else
	{
		RetErrorStatus=ConfigurationError;
		RCC->CFGR &= ~SW_MASK;
		RCC->CFGR |= (uint32_t)SYS_CLK_clock;
		if ((RCC->CFGR & SW_MASK) == ((RCC->CFGR & SWS_MASK)>>2)) /*next two bits*/
		{
			RetErrorStatus=OK;
		}

	}
	return RetErrorStatus;
}

ErrorStatus_t RCC_EnablePeripheral (uint8_t Bus,uint64_t BUSEN_peripheral)
{
	ErrorStatus_t RetErrorStatus=NOK;
	uint32_t* Peri_ptr=0;
	Peri_ptr= (uint32_t*)&BUSEN_peripheral;
	Peri_ptr++;
	if (*Peri_ptr != Bus)
	{
		RetErrorStatus=ParameterError;
	}
	else
	{
		RetErrorStatus=OK;
		switch(Bus)
		{
		case AHB1:
			RCC->AHB1ENR |= (uint32_t) BUSEN_peripheral;
			break;
		case APB1:
			RCC->APB1ENR |= (uint32_t) BUSEN_peripheral;
			break;
		case APB2:
			RCC->APB2ENR |= (uint32_t) BUSEN_peripheral;
			break;
		case AHB2:
			RCC->AHB2ENR |= (uint32_t) BUSEN_peripheral;
			break;
		default:
			break;
		}
	}
	return RetErrorStatus;
}
ErrorStatus_t RCC_DisablePeripheral (uint8_t Bus,uint64_t BUSEN_peripheral)
{
	ErrorStatus_t RetErrorStatus=NOK;
	uint32_t* Peri_ptr=0;
	Peri_ptr= (uint32_t*)&BUSEN_peripheral;
	Peri_ptr++;
	if (*Peri_ptr != Bus)
	{
		RetErrorStatus=ParameterError;
	}
	else
	{
		RetErrorStatus=OK;
		switch(Bus)
		{
		case AHB1:
			RCC->AHB1ENR &= ~(uint32_t) BUSEN_peripheral;
			break;
		case APB1:
			RCC->APB1ENR &= ~(uint32_t) BUSEN_peripheral;
			break;
		case APB2:
			RCC->APB2ENR &= ~(uint32_t) BUSEN_peripheral;
			break;
		case AHB2:
			RCC->AHB2ENR &= ~(uint32_t) BUSEN_peripheral;
			break;
		default:
			break;
		}
	}
	return RetErrorStatus;
}


ErrorStatus_t RCC_SetAHB1Prescaler (uint64_t HPRE_value)
{
	ErrorStatus_t RetErrorStatus=NOK;
	uint32_t* Bus_ptr= 0;
	Bus_ptr=(uint32_t*) &HPRE_value;
	Bus_ptr++;
	if (*Bus_ptr!=AHB1)
	{
		RetErrorStatus=ParameterError;
	}
	else
	{
		RetErrorStatus=OK;
		RCC->CFGR &= ~HPRE_MASK;
		RCC->CFGR|=(uint32_t) HPRE_value;
	}
	return RetErrorStatus;

}
ErrorStatus_t RCC_SetAPB1Prescaler (uint64_t PPRE1_value)
{
	ErrorStatus_t RetErrorStatus=NOK;
	uint32_t* Bus_ptr= 0;
	Bus_ptr=(uint32_t*) &PPRE1_value;
	Bus_ptr++;
	if (*Bus_ptr!=APB1)
	{
		RetErrorStatus=ParameterError;
	}
	else
	{
		RetErrorStatus=OK;
		RCC->CFGR &= ~PPRE1_MASK;
		RCC->CFGR|=(uint32_t) PPRE1_value;
	}
	return RetErrorStatus;

}
ErrorStatus_t RCC_SetAPB2Prescaler (uint64_t PPRE2_value)
{
	ErrorStatus_t RetErrorStatus=NOK;
	uint32_t* Bus_ptr= 0;
	Bus_ptr=(uint32_t*) &PPRE2_value;
	Bus_ptr++;
	if (*Bus_ptr !=APB2)
	{
		RetErrorStatus=ParameterError;
	}
	else
	{
		RetErrorStatus=OK;
		RCC->CFGR &= ~PPRE2_MASK;
		RCC->CFGR|=(uint32_t) PPRE2_value;
	}
	return RetErrorStatus;
}


ErrorStatus_t RCC_ConfigurePLL(uint32_t CLK_clock,uint32_t P_value,uint16_t M_value, uint16_t N_value)
{
	ErrorStatus_t RetErrorStatus=NOK;

	if((RCC->CR&CLK_PLL)!=0)
	{
		RetErrorStatus=ConfigurationError;
	}

	else
	{
		if ((CLK_clock!=(uint32_t)CLK_HSI) && (CLK_clock!=(uint32_t)CLK_HSE_EXT) && (CLK_clock!=(uint32_t)CLK_HSE_OSC))
				{
					RetErrorStatus=ParameterError;
				}
		else if (P_value!=P_2 && P_value!=P_4 && P_value!=P_6 && P_value!=P_8)
		{
			RetErrorStatus=ParameterError;
		}
		else if (N_value<192 || N_value>432)
		{
			RetErrorStatus=ParameterError;
		}
		else if (M_value <2 || M_value >63)
		{
			RetErrorStatus=ParameterError;
		}
		else
		{
			RetErrorStatus=OK;
			if (CLK_clock == (uint32_t)CLK_HSI)
			{
				RCC->PLLCFGR &= ~PLL_SRC_MASK;
				RCC->PLLCFGR |= (uint32_t)PLL_HSI;
			}
			else
			{
				RCC->PLLCFGR &= ~PLL_SRC_MASK;
				RCC->PLLCFGR |= (uint32_t)PLL_HSE;
			}

			RCC->PLLCFGR &= ~P_MASK;
			RCC->PLLCFGR |= P_value;

			RCC->PLLCFGR &= ~M_MASK;
			RCC->PLLCFGR |= (uint32_t) M_value;

			RCC->PLLCFGR &= ~N_MASK;
			RCC->PLLCFGR |= (uint32_t) N_value * N_SHIFT;
		}

	}

	return RetErrorStatus;
}

















