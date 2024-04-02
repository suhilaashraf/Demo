#include"RCC.h"
#include"gpio.h"
#include"Sched.h"
#include "Switch.h"
#include"Demo.h"


int main()
{
  RCC_EnablePeripheral(AHB1, AHB1EN_GPIOA);
  RCC_EnablePeripheral(AHB1, AHB1EN_GPIOB);

  WatchInit();
  Sched_init();
  Sched_Start();

  while (1)
  {
  }
  return 0;
}