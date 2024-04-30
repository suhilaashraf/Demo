#include <Demo2.h>
#include <UART.h>
#include <gpio.h>

typedef struct
{
    uint8_t request_id;
    uint8_t cmd;
    uint8_t param_port;
    uint8_t param_pin;
    uint8_t param_state;
} CommandPacket;

uint8_t Receive_Buffer[5];
uint8_t Send_Buffer[2];

void demo2_runnable()
{
    Uart_RxBufferAsync(Receive_Buffer, 5, UART_1, parse_command);
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
    uint8_t GPIO_STATE;
    switch (cmd)
    {
    case CMD_GET_PIN_STATE:
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
        Send_Buffer[1] = 0x50;
        Uart_TxBufferAsync(Send_Buffer, 2, UART_1, NULL);
        break;

    case CMD_SET_PIN_STATE:
        GPIO_SetPinValue(GPIO_PORTA, param_pin, param_state);
        Send_Buffer[0] = 0;
        Send_Buffer[1] = 0x50;
        Uart_TxBufferAsync(Send_Buffer, 2, UART_1, NULL);
        break;
    default:
        break;
    }

    // Send_Buffer[0]=packet->request_id;
}
void IO_App_init()
{
	uint8_t Localcounter=0;
	GPIOPIN_t PIN;
	PIN.gpioSPEED = GPIO_HIGHSPEED;
	for(Localcounter=0;Localcounter <8;Localcounter++)
	{
		PIN.gpioMODE = GPIO_OUTPUT_PP;
		PIN.gpioPORT = GPIO_PORTA;
		PIN.gpioPIN =  Localcounter;
		GPIO_InitPin(&PIN);
	}
    GPIOPIN_t PIN2;
	PIN2.gpioSPEED = GPIO_HIGHSPEED;
    uint8_t loc_counter2=8;
    for(loc_counter2=8;loc_counter2 <14;loc_counter2++)
	{
		PIN2.gpioMODE = GPIO_INPUT_PD;
		PIN2.gpioPORT = GPIO_PORTA;
		PIN2.gpioPIN =  Localcounter;
		GPIO_InitPin(&PIN2);
	}
}

