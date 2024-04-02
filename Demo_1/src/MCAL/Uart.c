#include "Uart.h"
#include "Uartcfg.h"

/**************************Uart define************************************/
/*SR BITS*/
#define Uart_ORE    BIT_3_MASK  // 0: No Overrun error 1: Overrun error is detected
#define Uart_RXNE   BIT_5_MASK // 0: Data is not received 1: Received data is ready to be read.
#define Uart_TC     BIT_6_MASK
#define Uart_TXE    BIT_7_MASK
/*CR1 BITS*/
#define Uart_RE BIT_2_MASK
#define UART_TE BIT_3_MASK
#define Uart_RXNEIE BIT_5_MASK // RXNE interrupt enable
#define Uart_TXEIE BIT_7_MASK  // TXE interrupt enable
#define UART_Mbit BIT_12_MASK
#define UART_UE BIT_13_MASK // Uart Enable

#define ONESTOP_BITMASK 0x00003000
#define BAUDRATE_MASK 4
/**************************Uart Registers************************************/
typedef struct Uart
{
    uint32_t SR;
    uint32_t DR;
    uint32_t BRR;
    uint32_t CR1;
    uint32_t CR2;
    uint32_t CR3;
    uint32_t GTPR;
} Uart_t;


/***********************UART VARIABLES**************************************/

TxReq TxUserRequest;
RxReq RxUserRequest;

/**************************Uart APIs PROTOTYPES******************************/
static void Uart_Cfg(void);
ErrorStatus_t Uart_SendByte(uint8_t Byte);
void Uart_ReceiveByte(void);
/***********************UART APIS IMPLEMENTATION*****************************/
static void Uart_Cfg(void)
{
    uint32_t localmask;
    /*set baudrate*/
    localmask = (CLK_FRQ / (8 * (2 - OVERSAMPLING) * BAUDRATE));
    ((Uart_t *)TxUserRequest.UART_X)->BRR |= localmask << BAUDRATE_MASK;

    ((Uart_t *)TxUserRequest.UART_X)->CR1 |= UART_UE;

    /*Config data bits*/
    ((Uart_t *)TxUserRequest.UART_X)->CR1 &= ~UART_Mbit;

    /*config stop bits*/
    localmask = ((Uart_t *)TxUserRequest.UART_X)->CR2;
    localmask &= ~ONESTOP_BITMASK;
    ((Uart_t *)TxUserRequest.UART_X)->CR2 = localmask;

    ((Uart_t *)TxUserRequest.UART_X)->CR1 |= UART_TE;
}

ErrorStatus_t Uart_TxBufferAsync(uint8_t *buffer, uint32_t len, void *UART_x)
{
    ErrorStatus_t Returnstatus = OK;
    if (TxUserRequest.state == Ready)
    {
        if ( !(buffer && len) )
        {
            Returnstatus =  NULL_PointerError;
        }
        else
        {
            TxUserRequest.state = Busy;
            TxUserRequest.buffer.size = len;
            TxUserRequest.UART_X = UART_x;
            TxUserRequest.buffer.data = buffer;
            Uart_Cfg();
 //           ((Uart_t *)TxUserRequest.UART_X)->CR1 |= Uart_TXEIE;
//            Uart_SendByte(TxUserRequest.buffer.data[TxUserRequest.buffer.pos]);
            Returnstatus = OK;
        }
    }
    return Returnstatus;
}

ErrorStatus_t Uart_SendByte(uint8_t Byte)
{
    ErrorStatus_t Returnstatus = NOK;
    if (((Uart_t *)TxUserRequest.UART_X)->SR & Uart_TXE)
    {
        ((Uart_t *)TxUserRequest.UART_X)->CR1 |= UART_TE;
        ((Uart_t *)TxUserRequest.UART_X)->DR = Byte;
        TxUserRequest.buffer.pos++;

        Returnstatus = OK;
    }
    return Returnstatus;
}

ErrorStatus_t Uart_RxBufferAsync(uint8_t *buffer, uint32_t len, void *UART_x)
{
    ErrorStatus_t Returnstatus = NOK;
    if (RxUserRequest.state == Ready)
    {

        if ( !(buffer && len) )
        {
            Returnstatus = NULL_PointerError;
        }
        else if (!((UART_x == UART_1) && (UART_x == UART_2) && (UART_x == UART_6)))
        {
            Returnstatus = ParameterError;
        }
        else
        {
            RxUserRequest.buffer.size = len;
            RxUserRequest.state = Busy;
            RxUserRequest.UART_X = UART_x;
            RxUserRequest.buffer.data = buffer;
            Uart_Cfg();
#if UART_BY == UART_INT
            ((Uart_t *)TxUserRequest.UART_X)->CR1 |= Uart_RXNEIE;
#endif

            Returnstatus =OK;
        }
    }
    return Returnstatus;
}

void Uart_ReceiveByte(void)
{
    if (((Uart_t *)RxUserRequest.UART_X)->SR & Uart_RXNE)
    {
        RxUserRequest.buffer.data[RxUserRequest.buffer.pos]= ((Uart_t *)TxUserRequest.UART_X)->DR ;
        RxUserRequest.buffer.pos++;
    }
}

void Uart_Hnadler(void)
{
    if (TxUserRequest.state == Busy)
    {
        if (((Uart_t *)TxUserRequest.UART_X)->SR & Uart_TXE)
        {
            if (TxUserRequest.buffer.pos < TxUserRequest.buffer.size)
            {
                ((Uart_t *)TxUserRequest.UART_X)->DR = TxUserRequest.buffer.data[TxUserRequest.buffer.pos];
                TxUserRequest.buffer.pos++;
            }
            else
            {
                TxUserRequest.state = Ready;
                ((Uart_t *)TxUserRequest.UART_X)->CR1 &= ~Uart_TXEIE;
            }
        }
    }
    if (RxUserRequest.state == Busy)
    {
        if (((Uart_t *)RxUserRequest.UART_X)->SR & Uart_RXNE)
        {
            if (RxUserRequest.buffer.pos < RxUserRequest.buffer.size)
            {

                RxUserRequest.buffer.data[TxUserRequest.buffer.pos] = ((Uart_t *)RxUserRequest.UART_X)->DR;
                RxUserRequest.buffer.pos++;
            }
            else
            {
                RxUserRequest.state = Ready;
                ((Uart_t *)TxUserRequest.UART_X)->CR1 &= ~ Uart_RXNEIE;
            }
        }
    }
}

void USART1_IRQHandler(void)
{
    Uart_Hnadler();
}

void USART2_IRQHandler(void)
{
    Uart_Hnadler();
}

void USART6_IRQHandler(void)
{
    Uart_Hnadler();
}
