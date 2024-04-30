#ifndef NVIC_H
#define NVIC_H

#include "std_types.h"
#include "stm32f401cc.h"
#include <Errors.h>


#define PRI_ZERO_GROUPS	    0x05FA0000
#define PRI_ONE_GROUP		0x05FA0400
#define PRI_TWO_GROUP		0x05FA0500
#define PRI_THREE_GROUP		0x05FA0600
#define PRI_FOUR_GROUP		0x05FA0700

ErrorStatus_t NVIC_EnableIRQ(IRQ_t IRQn);
ErrorStatus_t NVIC_DisableIRQ(IRQ_t IRQn);
ErrorStatus_t NVIC_SetPriority(IRQ_t IRQn, uint16_t priority);
ErrorStatus_t NVIC_GetActive(IRQ_t IRQn,uint8_t* add_retvalue);
ErrorStatus_t NVIC_SetPendingIRQ(IRQ_t IRQn);
ErrorStatus_t NVIC_GetPendingIRQ(IRQ_t IRQn, uint8_t* add_retvalue );
ErrorStatus_t NVIC_SetPriorityGrouping (uint32_t group);
#endif
