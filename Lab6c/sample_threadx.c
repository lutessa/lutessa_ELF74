/* This is a small demo of the high-performance ThreadX kernel.  It includes examples of eight
   threads of different priorities, using a message queue, semaphore, mutex, event flags group, 
   byte pool, and block pool. Please refer to Chapter 6 of the ThreadX User Guide for a complete
   description of this demonstration.  */


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "tx_api.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

//#include "TM4C1294NCPDT.h"

#define DEMO_STACK_SIZE         1024
#define DEMO_BYTE_POOL_SIZE     9120



/* Define the ThreadX object control blocks...  */


TX_THREAD               thread_1;
TX_THREAD               thread_2;
TX_THREAD               thread_3;


TX_BYTE_POOL            byte_pool_0;



/* Define byte pool memory.  */

UCHAR                   byte_pool_memory[DEMO_BYTE_POOL_SIZE];


/* Define event buffer.  */

#ifdef TX_ENABLE_EVENT_TRACE
UCHAR   trace_buffer[0x10000];
#endif


/* Define the counters used in the demo application...  */


ULONG                   thread_1_counter;
ULONG                   thread_1_messages_sent;
ULONG                   thread_2_counter;
ULONG                   thread_2_messages_received;
ULONG                   thread_3_counter;


/* Define thread prototypes.  */

void    thread_1_entry(ULONG thread_input);
void    thread_2_entry(ULONG thread_input);
void    thread_3_entry(ULONG thread_input);



void piscaLeds(uint32_t loops, uint8_t led){

  uint32_t i=0;
  
  while(i<loops){
    if(led==1){
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_0);
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x00);

      }
    else if(led==2){
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1|GPIO_PIN_0, GPIO_PIN_1);
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x00);
   
    }
    else{
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0|GPIO_PIN_1, 0x00);
     
    }
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x00);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0|GPIO_PIN_1, 0x00);
    i++;
  }


}

int main()
{
     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }
    
    
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0|GPIO_PIN_1);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }
    
    
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0|GPIO_PIN_1, 0x00);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x00);
    

    
    //piscaLeds(10000000,1);
    tx_kernel_enter();
}


/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{

CHAR    *pointer = TX_NULL;


#ifdef TX_ENABLE_EVENT_TRACE
    tx_trace_enable(trace_buffer, sizeof(trace_buffer), 32);
#endif

    /* Create a byte memory pool from which to allocate the thread stacks.  */
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", byte_pool_memory, DEMO_BYTE_POOL_SIZE);

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

  

    /* Allocate the stack for thread 1.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create threads 1.  */
    tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, DEMO_STACK_SIZE, 
            1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Allocate the stack for thread 2.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, DEMO_STACK_SIZE, 
            2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Allocate the stack for thread 3.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create threads 3.*/
    tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, DEMO_STACK_SIZE, 
            3, 3, TX_NO_TIME_SLICE, TX_AUTO_START);

  

 


  
}



/* Define the test threads.  */




void    thread_1_entry(ULONG thread_input)
{




   
    while(1)
    {

        piscaLeds(40000,1);
       
        tx_thread_sleep(500);
  
    }
}


void    thread_2_entry(ULONG thread_input)
{


    while(1)
    {
        
     

         piscaLeds(66666,2);
         tx_thread_sleep(750);
     
    }
}


void    thread_3_entry(ULONG thread_input)
{


    while(1)
    {

     
        piscaLeds(106666,3);
        tx_thread_sleep(2000);
        
    }
}
