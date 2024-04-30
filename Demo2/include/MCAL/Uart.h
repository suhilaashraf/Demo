#ifndef UART_H_
#define UART_H_
#include "Errors.h"
#include "std_types.h"
#include "Mask.h"

/**************************Uart define************************************/
#define UART_INT            0
#define UART_TASK           1
#define UART_BY          UART_INT 
#define UART_1          (void *)0x40011000
#define UART_2          (void *)0x40004400
#define UART_6          (void *)0x40011400

/**************************Uart Registers************************************/

typedef void (*Txcb_t)(void);
typedef void (*Rxcb_t)(void);
/**************************Uart APIs PROTOTYPES************************************/

void Uart_TxBufferAsync(uint8_t *buffer, uint32_t len, void *UART_x,Txcb_t cb);
void Uart_RxBufferAsync(uint8_t *buffer, uint32_t len, void *UART_x,Rxcb_t cb);

#endif