/***************************************************************************//**
  @file     main.c
  @brief    FW main
  @author   Nicolás Magliola
 ******************************************************************************/

#include "hardware.h"

void App_Init (void);
void App_Run (void);

void App_Init6 (void);
void App_Run6 (void);

int main (void)
{
    hw_Init();
    hw_DisableInterrupts();
    App_Init6(); /* Program-specific setup */
    hw_EnableInterrupts();

    __FOREVER__
        App_Run6(); /* Program-specific loop  */
}
