#include <Demo2.h>
#include <UART.h>
#include <gpio.h>
#include "Led.h"
#include "Switch.h"


typedef struct {
    uint8_t request_id;
    uint8_t cmd;
    uint8_t param_port;
    uint8_t param_pin;
    uint8_t param_state;
    uint8_t param_idx;
} CommandPacket;



uint8_t Receive_Buffer[6];
uint8_t Send_Buffer[2];

void demo2_runnable()
{
    Uart_RxBufferAsync(Receive_Buffer, 6, UART_1, parse_command);
}

void parse_command()
{

    CommandPacket *packet = (CommandPacket *)Receive_Buffer;

    // Access fields of the packet
    uint8_t request_id = packet->request_id;
    uint8_t cmd = packet->cmd;
    uint8_t param_port = packet->param_port;
    uint8_t param_pin = packet->param_pin;
    uint8_t param_state = packet->param_state;
    uint8_t param_idx = packet->param_idx;
    uint8_t GPIO_STATE;
    uint8_t SWITCH_STATE;
    LED_init();
    SWITCH_init();
    switch (cmd)
    {
    case CMD_GET_LED_STATE:
        switch (param_port)
        {
        case PARAM_PORTA:
            GPIO_GetPinValue(GPIO_PORTA, param_pin, &GPIO_STATE);
            break;
        case PARAM_PORTB:
            GPIO_GetPinValue(GPIO_PORTB, param_pin, &GPIO_STATE);
            break;
        default:
            break; 
        }
        Send_Buffer[0] = GPIO_STATE;
        Send_Buffer[1] = 0xFF;
        Uart_TxBufferAsync(Send_Buffer, 2, UART_1, NULL);
        break;

    case CMD_SET_LED_STATE:
        GPIO_SetPinValue(GPIO_PORTA,0,param_state);
        Send_Buffer[0] = GPIO_STATE;
        Send_Buffer[1] = 0xFF;
        Uart_TxBufferAsync(Send_Buffer, 2, UART_1, NULL);
        break;
    case CMD_GET_SWITCH_STATE:
        SWITCH_GETSTATUS(param_idx,&SWITCH_STATE);
        Send_Buffer[0] = SWITCH_STATE;
        Send_Buffer[1] = 0xFF;
        Uart_TxBufferAsync(Send_Buffer, 2, UART_1, NULL);

    default:
        break;
    }
    
    // Send_Buffer[0]=packet->request_id;
}
