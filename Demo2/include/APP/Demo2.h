#ifndef DEMO2_H
#define DEMO2_H

#include <stdint.h>
#define CMD_SET_PIN_STATE 0x01
#define CMD_GET_PIN_STATE 0x02
#define PARAM_ON 0x01
#define PARAM_OF 0x00
#define PARAM_PORTA 0x0A
#define PARAM_PORTB 0x0B


typedef enum
{
    PARAM_PIN0,
    PARAM_PIN1,
    PARAM_PIN2,
    PARAM_PIN3,
    PARAM_PIN4,
    PARAM_PIN5,
    PARAM_PIN6,
    PARAM_PIN7,
    PARAM_PIN8,
    PARAM_PIN9,
    PARAM_PIN10,
    PARAM_PIN11,
    PARAM_PIN12,
    PARAM_PIN13,
    PARAM_PIN14,
    PARAM_PIN15,
} PARAM_PIN_t;

extern uint8_t Receive_Buffer[5];
void demo2_runnable();
void parse_command();
void IO_App_init();












#endif