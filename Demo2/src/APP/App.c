#include"RCC.h"
#include"gpio.h"
#include"Sched.h"
#include "Uart.h"
#include "NVIC.h"
#include "Demo2.h"

int main()
{
  
  NVIC_EnableIRQ(IRQ_USART1);
  RCC_EnablePeripheral(AHB1, AHB1EN_GPIOA);
  RCC_EnablePeripheral(AHB1, AHB1EN_GPIOB);
  RCC_EnablePeripheral(APB2, APB2EN_USART1);
  
  GPIOPIN_t uarttx;
  GPIOPIN_t uartrx;
  uartrx.gpioPORT =  GPIO_PORTB;
  uartrx.gpioPIN = GPIO_PIN7;
  uartrx.gpioMODE = GPIO_AF_PP;
  uartrx.GPIO_AF = GPIO_AF_USART1_2;
  uartrx.gpioSPEED=GPIO_HIGHSPEED;

  uarttx.gpioPORT =  GPIO_PORTB;
  uarttx.gpioPIN = GPIO_PIN6;
  uarttx.gpioMODE = GPIO_AF_PP;
  uarttx.GPIO_AF = GPIO_AF_USART1_2;
  uarttx.gpioSPEED=GPIO_HIGHSPEED;

  GPIO_InitPin(&uartrx);
  GPIO_InitPin(&uarttx);
  IO_App_init();


  Sched_init();
  Sched_Start();

  while (1)
  {
    
  }
  return 0;
}