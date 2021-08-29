/***************************************************************************//**
  @file     main.c
  @brief    FW main
  @author   Nicolás Magliola
 ******************************************************************************/

#include "hardware.h"

void App_Init_Baliza_IRQ (void);
void App_Run_Baliza_IRQ (void);

void App_Init_IRQ (void);
void App_Run_IRQ (void);

int main (void)
{
    hw_Init();
    hw_DisableInterrupts();
    App_Init_Baliza_IRQ(); /* Program-specific setup */
    hw_EnableInterrupts();

    __FOREVER__
        App_Run_Baliza_IRQ(); /* Program-specific loop  */
}
