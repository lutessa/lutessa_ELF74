//*****************************************************************************
//
// blinky.c - Simple example to blink the on-board LED.
//
// Copyright (c) 2013-2020 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.2.0.295 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
//#include "TM4C1294NCPDT.h""


#include <iostream>
using namespace std;
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Blinky (blinky)</h1>
//!
//! A very simple example that blinks the on-board LED using direct register
//! access.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

//uint32_t g_ui32SysClock;

volatile uint32_t g_ui32counter=0;
volatile uint8_t g_ui8button=1;



//Tratador Interrupção do Botão 
void PortJHandler(){
  GPIOIntClear(GPIO_PORTJ_AHB_BASE,GPIO_PIN_0);
  g_ui8button=0;
}

//Tratador do Timer , acrescenta na contagem
void TIMERHandler(){
  TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
  g_ui32counter++;
}

//*****************************************************************************
//
// Blink the on-board LED.
//
//*****************************************************************************
int
main(void)
{
    volatile uint32_t ui32Loop;
    uint32_t ui32SysClock;

    //Set clock
     ui32SysClock=SysCtlClockFreqSet(SYSCTL_OSC_INT | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_320,40000000);
    
    // Enable the GPIO port that is used for the on-board LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    //
    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }
    
    
    //Enable GPIO Botoes PortJ
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    //
    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))
    {
    }
    
    //TimerDisable(TIMER0_BASE,TIMER_A);
 
   //Enable do timer ,interrupção e  configurações
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerDisable(TIMER0_BASE,TIMER_A);
    TimerConfigure(TIMER0_BASE,TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC);
  
    TimerLoadSet(TIMER0_BASE,TIMER_A,ui32SysClock/1000);
    
    IntMasterEnable();
    
    TimerIntRegister(TIMER0_BASE,TIMER_A,TIMERHandler);
    TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A);
    TimerEnable(TIMER0_BASE,TIMER_A);
    
      
      //
    // Enable the GPIO pin for the LED (PN0).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    
    //Enable Botão como input e Configurações de interrupção
    GPIOPinTypeGPIOInput(GPIO_PORTJ_AHB_BASE, GPIO_PIN_0|GPIO_PIN_1);
    
    GPIOPadConfigSet(GPIO_PORTJ_AHB_BASE ,GPIO_PIN_0,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    
    GPIOIntTypeSet(GPIO_PORTJ_AHB_BASE,GPIO_PIN_0,GPIO_FALLING_EDGE);
    GPIOIntRegister(GPIO_PORTJ_AHB_BASE,PortJHandler);
    GPIOIntEnable(GPIO_PORTJ_AHB_BASE,GPIO_PIN_0);
    
    
  //Espera 1s para ligar o led
    while(1){if(g_ui32counter==1000){break;}}  
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);

    g_ui32counter=0;
    ui32Loop=0;
    g_ui8button=1;
    while(g_ui8button){
      if(g_ui32counter==3000){
        //printf("Perdeu!\n");
        break;
        }
      else{
        ui32Loop++;
      }
         
    }
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);
    //printf("Tempo : %d\n",ui32Loop);
    while(1){}
   
    
}
