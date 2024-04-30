#include "Uart.h"
#include "Uartcfg.h"

/**************************Uart define************************************/
/*SR BITS*/
#define Uart_ORE    BIT_3_MASK  // 0: No Overrun error 1: Overrun error is detected
#define Uart_RXNE   BIT_5_MASK // 0: Data is not received 1: Received data is ready to be read.
#define Uart_TC     BIT_6_MASK
#define Uart_TXE    BIT_7_MASK
/*CR1 BITS*/
#define Uart_RE     BIT_2_MASK
#define UART_TE     BIT_3_MASK
#define Uart_RXNEIE BIT_5_MASK // RXNE interrupt enable
#define Uart_TXEIE  BIT_7_MASK  // TXE interrupt enable
#define UART_Mbit   BIT_12_MASK
#define UART_UE     BIT_13_MASK // Uart Enable

#define ONESTOP_BITMASK 0x00003000
#define CLK_FRQ         16000000
#define BAUDRATE_MASK   4

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

typedef struct
{
    uint8_t* data;
    uint32_t pos;
    uint32_t size;
} buffer_t;

typedef enum
{
    Ready,
    Busy
} ReqState;

typedef struct
{
    buffer_t buffer;
    uint8_t state;
    void* UART_X;
    Txcb_t cbf;
} TxReq;

typedef struct
{
    buffer_t buffer;
    uint8_t state;
    void* UART_X;
    Rxcb_t cbf;
} RxReq;
/***********************UART VARIABLES**************************************/

TxReq TxUserRequest;
RxReq RxUserRequest;

/**************************Uart APIs PROTOTYPES******************************/
ErrorStatus_t UartTx_Cfg(void);
ErrorStatus_t UartRx_Cfg(void);
/***********************UART APIS IMPLEMENTATION*****************************/
ErrorStatus_t UartTx_Cfg(void)
{
    uint32_t localmask;
    ErrorStatus_t Returnstatus = NOK;
    if (!((TxUserRequest.UART_X == UART_1) || (TxUserRequest.UART_X == UART_2) || (TxUserRequest.UART_X == UART_6)))
    {
        Returnstatus = ParameterError;
    }
    else
    {
        Returnstatus = OK;

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
    return Returnstatus;
}
ErrorStatus_t UartRx_Cfg(void)
{
    uint32_t localmask;
    ErrorStatus_t Returnstatus = NOK;
    if (!((RxUserRequest.UART_X == UART_1) || (RxUserRequest.UART_X == UART_2) || (RxUserRequest.UART_X == UART_6)))
    {
        Returnstatus = ParameterError;
    }
    else
    {
        localmask = (CLK_FRQ / (8 * (2 - OVERSAMPLING) * BAUDRATE));
        ((Uart_t *)RxUserRequest.UART_X)->BRR |= localmask << BAUDRATE_MASK;
        ((Uart_t *)RxUserRequest.UART_X)->CR1 |=  UART_UE;
        ((Uart_t *)RxUserRequest.UART_X)->CR1 &= ~UART_Mbit;
        localmask = ((Uart_t *)RxUserRequest.UART_X)->CR2;
        localmask &= ~ONESTOP_BITMASK;
        ((Uart_t *)RxUserRequest.UART_X)->CR2 = localmask;
        ((Uart_t *)RxUserRequest.UART_X)->CR1 |= Uart_RE ;
    }
    return Returnstatus;
}



void Uart_TxBufferAsync(uint8_t *buffer, uint32_t len, void *UART_x,Txcb_t cb)
{
    if (TxUserRequest.state == Ready)
    {
            TxUserRequest.state = Busy;
            TxUserRequest.buffer.pos = 0;
            TxUserRequest.buffer.size = len;
            TxUserRequest.UART_X = UART_x;
            TxUserRequest.cbf=cb;
            TxUserRequest.buffer.data = buffer;
            UartTx_Cfg();
            ((Uart_t *)TxUserRequest.UART_X)->CR1 |= Uart_TXEIE;
    }
}

void Uart_RxBufferAsync(uint8_t *buffer, uint32_t len, void *UART_x, Rxcb_t cb)
{
    if (RxUserRequest.state == Ready)
    {
        RxUserRequest.buffer.size = len;
        RxUserRequest.buffer.pos = 0;
        RxUserRequest.state = Busy;
        RxUserRequest.UART_X = UART_x;
        RxUserRequest.cbf=cb;
        RxUserRequest.buffer.data = buffer;
        UartRx_Cfg();
        ((Uart_t *)RxUserRequest.UART_X)->CR1 |= Uart_RXNEIE;
    }
}

void Uart_TxRunnable(void)
{
    if (TxUserRequest.state == Busy)
    {
        if (((Uart_t *)TxUserRequest.UART_X)->SR & Uart_TXE)
        {
            if (TxUserRequest.buffer.pos < TxUserRequest.buffer.size)
            {
                ((Uart_t *)TxUserRequest.UART_X)->DR = TxUserRequest.buffer.data[TxUserRequest.buffer.pos];
                TxUserRequest.buffer.pos++;
                if (TxUserRequest.buffer.pos == TxUserRequest.buffer.size)
                {
                    TxUserRequest.state = Ready;
                    if (TxUserRequest.cbf)
                    {
                        TxUserRequest.cbf();
                    }
                }
            }
            else
            {
                TxUserRequest.state = Ready;
            }
        }
    }
}

void Uart_RxRunnable(void)
{
    if (RxUserRequest.state == Busy)
    {
        if (RxUserRequest.buffer.pos < RxUserRequest.buffer.size)
        {

            RxUserRequest.buffer.data[RxUserRequest.buffer.pos] = ((Uart_t *)RxUserRequest.UART_X)->DR;
            RxUserRequest.buffer.pos++;
            if (RxUserRequest.buffer.pos == RxUserRequest.buffer.size)
            {
                RxUserRequest.state = Ready;
            }
        }
        else
        {
            if (RxUserRequest.cbf)
            {
                RxUserRequest.cbf();
            }
            RxUserRequest.state = Ready;
        }
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
                if (TxUserRequest.buffer.pos == TxUserRequest.buffer.size)
                {
                    TxUserRequest.state = Ready;
                    if (TxUserRequest.cbf)
                    {
                        TxUserRequest.cbf();
                    }
                    ((Uart_t *)TxUserRequest.UART_X)->CR1 &= ~Uart_TXEIE;
                }
            }
        }
    }
    else if (RxUserRequest.state == Busy)
    {
        if (RxUserRequest.buffer.pos < RxUserRequest.buffer.size)
        {
            if (((Uart_t *)RxUserRequest.UART_X)->SR & Uart_RXNE)
            {
                RxUserRequest.buffer.data[RxUserRequest.buffer.pos] = ((Uart_t *)RxUserRequest.UART_X)->DR;
                RxUserRequest.buffer.pos++;
                if (RxUserRequest.buffer.pos == RxUserRequest.buffer.size)
                {
                    RxUserRequest.state = Ready;
                    if (RxUserRequest.cbf)
                    {
                        RxUserRequest.cbf();
                    }
                    ((Uart_t *)RxUserRequest.UART_X)->CR1 &= ~Uart_RXNEIE;
                }
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
