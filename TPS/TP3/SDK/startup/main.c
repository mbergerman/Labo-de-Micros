/***************************************************************************//**
  @file     main.c
  @brief    FW main
  @author   Grupo 1
 ******************************************************************************/

#include "hardware.h"

void App_Init (void);
void App_Run (void);

int main (void){


	hw_Init();
    hw_DisableInterrupts();
    App_Init(); /* Program-specific setup */
    hw_EnableInterrupts();

    __FOREVER__
        App_Run(); /* Program-specific loop  */
}
