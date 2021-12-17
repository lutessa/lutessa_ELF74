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
#include "driverlib/uart.h"

//#include "TM4C1294NCPDT.h"

#define DEMO_STACK_SIZE         1024
#define DEMO_BYTE_POOL_SIZE     9120


uint32_t g_ui32SysClock;

//Ultimo comando recebido
char comando[6];

//Vetores de requisições de presença de cada cabine em cada andar
//Se req[i]=0, não se deve parar, se req[i]=1 para obrigatoria
//Se =2, parada somente quando se encontra no estado subindo
//Se =3 , parada somente quando se encontra no estado descendo
char reqs1[16];
char reqs2[16];
char reqs3[16];

//Andar em que a cabine se encontra
uint16_t andarE;
uint16_t andarC;
uint16_t andarD;

uint32_t ci;
uint8_t novo;


//Se cada porta foi completamente fechada ou aberta
uint8_t abertaE;
uint8_t abertaC;
uint8_t abertaD;

uint8_t fechadaE;
uint8_t fechadaC;
uint8_t fechadaD;




//Estados dos elevadores
typedef enum 
{	
	idle,
	subindo,
        descendo,
        parado,
        abrindo,
        fechando

}estadosElevador;

estadosElevador Elift=idle;
estadosElevador Clift=idle;
estadosElevador Dlift=idle;


/* Define the ThreadX object control blocks...  */

TX_THREAD               thread_0;
TX_THREAD               thread_1;
TX_THREAD               thread_2;
TX_THREAD               thread_3;


TX_BYTE_POOL            byte_pool_0;



//Mutex para uso da Uart
TX_MUTEX                mutex_0;



/* Define byte pool memory.  */

UCHAR                   byte_pool_memory[DEMO_BYTE_POOL_SIZE];


/* Define event buffer.  */

#ifdef TX_ENABLE_EVENT_TRACE
UCHAR   trace_buffer[0x10000];
#endif





/* Define thread prototypes.  */
void    thread_0_entry(ULONG thread_input);
void    thread_1_entry(ULONG thread_input);
void    thread_2_entry(ULONG thread_input);
void    thread_3_entry(ULONG thread_input);




void
UART0_Handler(void)
{
    //Limpa interrupção
    uint32_t ui32Status;
    ui32Status = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, ui32Status);
    
    uint32_t c;

    //Recebe letras
    comando[0]=0;
    comando[1]=0;
    comando[2]=0;
    comando[3]=0;
    comando[4]=0;
    ci=0;
    c=0;
    
    while(UARTCharsAvail(UART0_BASE))
    {
   
      c= UARTCharGetNonBlocking(UART0_BASE);
      
      comando[ci]=c;
 
      ci++;

 
     
    }
    //novo comando recebido
    novo=1;
}



int main()
{

    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_240), 120000000);
 
    
    //Habilita perifericos a serem usados na uart usb
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    
    
    IntMasterEnable();

    //Configura os pinos para transmissao e recepção
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //Baud Rate 115200 e 8N1
    UARTConfigSetExpClk(UART0_BASE, g_ui32SysClock, 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    //Habilita interrupçao uart
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    //zera vetor de comandos e requisições
    ci=0;
    for(int i=0;i<6;i++){
      comando[i]=0;
    }
    for(int i=0;i<16;i++){
      reqs1[i]=0;
      reqs2[i]=0;
      reqs3[i]=0;
    }
    andarE=0;
    andarC=0;
    andarD=0;
    
    //Inicializa simulador para cada elevador
    UARTCharPutNonBlocking(UART0_BASE,'e');
    UARTCharPutNonBlocking(UART0_BASE,'r');
    UARTCharPutNonBlocking(UART0_BASE, 13);
            
     while(comando[1]!='A'){
     }
            
    UARTCharPutNonBlocking(UART0_BASE,'e');
    UARTCharPutNonBlocking(UART0_BASE,'f');
    UARTCharPutNonBlocking(UART0_BASE,13);
            
    while(comando[1]!='F'){
    }
    
    UARTCharPutNonBlocking(UART0_BASE,'c');
    UARTCharPutNonBlocking(UART0_BASE,'r');
    UARTCharPutNonBlocking(UART0_BASE, 13);
            
     while(comando[1]!='A'){
     }
            
    UARTCharPutNonBlocking(UART0_BASE,'c');
    UARTCharPutNonBlocking(UART0_BASE,'f');
    UARTCharPutNonBlocking(UART0_BASE,13);
            
    while(comando[1]!='F'){
    }
    
    UARTCharPutNonBlocking(UART0_BASE,'d');
    UARTCharPutNonBlocking(UART0_BASE,'r');
    UARTCharPutNonBlocking(UART0_BASE, 13);
            
     while(comando[1]!='A'){
     }
            
    UARTCharPutNonBlocking(UART0_BASE,'d');
    UARTCharPutNonBlocking(UART0_BASE,'f');
    UARTCharPutNonBlocking(UART0_BASE,13);
            
    while(comando[1]!='F'){
    }
    
    
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

    /* Create thread 0.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,  
            pointer, DEMO_STACK_SIZE, 
            4, 4, 5, TX_AUTO_START);
    
    /* Allocate the stack for thread 1.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create threads 1.  */
    tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, DEMO_STACK_SIZE, 
            4, 4, 5, TX_AUTO_START);

    /* Allocate the stack for thread 2.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, DEMO_STACK_SIZE, 
            4, 4, 5, TX_AUTO_START);

    /* Allocate the stack for thread 3.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create threads 3 .  */
    tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, DEMO_STACK_SIZE, 
            4, 4, 5, TX_AUTO_START);

  

     tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);



  
}



/* Define the test threads.  */

//thread que controla requisições
void    thread_0_entry(ULONG thread_input)
{

    UINT    status;
    while(1)
    {
      //Se um novo comando foi recebido
      if(novo){
        
        //Comando do Tipo Botão interno pressionado, adiciona um andar no vetor de requisições
        if(comando[1]=='I'){
          status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
          if (status != TX_SUCCESS)
            break;
          UARTCharPutNonBlocking(UART0_BASE,comando[0]);
          UARTCharPutNonBlocking(UART0_BASE,'L');
          UARTCharPutNonBlocking(UART0_BASE,comando[2]);
          UARTCharPutNonBlocking(UART0_BASE,13);  
          status = tx_mutex_put(&mutex_0);
          if (status != TX_SUCCESS)
            break;
          if(comando[0]=='e'){
            reqs1[comando[2]-97]=1;
          }
          else if(comando[0]=='c'){
            reqs2[comando[2]-97]=1;
          }
          else if(comando[0]=='d'){
            reqs3[comando[2]-97]=1;
          }
          
        }
        //Comando do tipo botão externo pressionado, adiciona uma requisição de subida ou descida
        else if(comando[1]=='E'){  
          if(comando[0]=='e'){
            if(comando[2]=='0'){
              if(comando[4]=='s'){
                reqs1[comando[3]-48]=2;
              }
              else{
                reqs1[comando[3]-48]=3;
              } 
            }
            else{
              if(comando[4]=='s'){
                reqs1[(comando[3]-48)+10]=2;
              }
              else{
                reqs1[(comando[3]-48)+10]=3;
              }
            }
          }
          else if(comando[0]=='c'){
            if(comando[2]=='0'){
              if(comando[4]=='s'){
                reqs2[comando[3]-48]=2;
              }
              else{
                reqs2[comando[3]-48]=3;
              } 
            }
            else{
              if(comando[4]=='s'){
                reqs2[(comando[3]-48)+10]=2;
              }
              else{
                reqs2[(comando[3]-48)+10]=3;
              }
            }
          }
          else if(comando[0]=='d'){
            if(comando[2]=='0'){
              if(comando[4]=='s'){
                reqs3[comando[3]-48]=2;
              }
              else{
                reqs3[comando[3]-48]=3;
              } 
            }
            else{
              if(comando[4]=='s'){
                reqs3[(comando[3]-48)+10]=2;
              }
              else{
                reqs3[(comando[3]-48)+10]=3;
              }
            }
          }
          
       }
    //Comando do tipo chegada a um andar, atualiza o andar atual do elevador em questão
     else if(comando[1]>47&&comando[1]<58){
        if (comando[0]=='e'){

          if(ci==5){
            andarE=10+(comando[2]-48);
          }
          else{
            andarE=comando[1]-48;
          }
        }
        else if (comando[0]=='c'){

          if(ci==5){
            andarC=10+(comando[2]-48);
          }
          else{
            andarC=comando[1]-48;
          }
        }
        else if (comando[0]=='d'){

          if(ci==5){
            andarD=10+(comando[2]-48);
          }
          else{
            andarD=comando[1]-48;
          }
        }
        
      }
      //comando do tipo, completamenta aberta ou fechada, informa se a porta está aberta ou fechada
     else if(comando[1]=='A'){
       if(comando[0]=='e'){
          abertaE=1;
       }
       else if(comando[0]=='c'){
          abertaC=1;
       }
       else if(comando[0]=='d'){
          abertaD=1;
       }
     }
     else if(comando[1]=='F'){
        if(comando[0]=='e'){
          fechadaE=1;
       }
       else if(comando[0]=='c'){
          fechadaC=1;
       }
       else if(comando[0]=='d'){
          fechadaD=1;
       }
     }
      novo=0;
      
      }
    }
}


//Thread de controle do elevador esquerdo
void    thread_1_entry(ULONG thread_input)
{
    UINT    status;
    uint16_t statusE;
    abertaE=0;
    fechadaE=0;
    while(1)
    {
        //Executa de acordo com o estado atual
      /*Se em Idle verifica se há alguma nova requisição e se tiver sobe ou desce de acordo
        Se Subindo ou descendo verifica se chegou em algum andar com parada prevista pelo vetor de requisição, 
              se sim para a cabine e vai para o estado parado, requisição é limpa
        Se parado, apaga os leds do andar atual,e manda abrir a porta, vai para o estado abrindo
        Se abrindo, aguarda porta estar completamente aberta para então ir para o estado fechando
        Se fechando, aguarda a porta estar completamente fechada , verifica se ainda há requisições , se sim
         continua a subida se estava previamente subindo, ou descendo se estava  descendo
              se não vai para o estado IDLE
      */
        switch(Elift){
        case  idle  :
          statusE=0;
          for(int i=0;i<16;i++){
            if(reqs1[i]!=0){
              if(i>andarE){
                status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
                UARTCharPutNonBlocking(UART0_BASE,  'e');
                UARTCharPutNonBlocking(UART0_BASE,'s');
                UARTCharPutNonBlocking(UART0_BASE,13);
                status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
                Elift=subindo;
              }
              else if(i<andarE){
                status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
                UARTCharPutNonBlocking(UART0_BASE,  'e');
                UARTCharPutNonBlocking(UART0_BASE,'d');
                UARTCharPutNonBlocking(UART0_BASE,13);
                status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
                Elift=descendo;
              }
              
            }
          }
          break;
        case  subindo :
          statusE=1;
          if(reqs1[andarE]==1||reqs1[andarE]==2){
            status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
              UARTCharPutNonBlocking(UART0_BASE,'e');
            UARTCharPutNonBlocking(UART0_BASE,'p');
            UARTCharPutNonBlocking(UART0_BASE,13);
            status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
            reqs1[andarE]=0;
            Elift=parado;
          }
          else if(reqs1[andarE]==3){
            bool flag=true;
            for(int i=andarE;i<15;i++)
            {
              if (reqs1[i+1]!=0){
                flag=false;
                break;
              }
            }
            if(flag){
              status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
              UARTCharPutNonBlocking(UART0_BASE, 'e');
              UARTCharPutNonBlocking(UART0_BASE,'p');
              UARTCharPutNonBlocking(UART0_BASE,13);
              status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
              reqs1[andarE]=0;
              Elift=parado;
            }
          }
          break;
        case  descendo :
          statusE=2;
          if(reqs1[andarE]==1||reqs1[andarE]==3){
            status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
              UARTCharPutNonBlocking(UART0_BASE, 'e');
            UARTCharPutNonBlocking(UART0_BASE,'p');
            UARTCharPutNonBlocking(UART0_BASE,13);
            status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
            reqs1[andarE]=0;
            Elift=parado;
          }
          else if(reqs1[andarE]==2){
            bool flag=true;
            for(int i=andarE;i>0;i--)
            {
              if (reqs1[i-1]!=0){
                flag=false;
                break;
              }
            }
            if(flag){
              status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
              UARTCharPutNonBlocking(UART0_BASE, 'e');
              UARTCharPutNonBlocking(UART0_BASE,'p');
              UARTCharPutNonBlocking(UART0_BASE,13);
              status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
              reqs1[andarE]=0;
              Elift=parado;
            }
          }
          
          break;
        case  parado:
          status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
          UARTCharPutNonBlocking(UART0_BASE,'e');
          UARTCharPutNonBlocking(UART0_BASE,'D');
          UARTCharPutNonBlocking(UART0_BASE,andarE+'a');
          UARTCharPutNonBlocking(UART0_BASE,13); 
          
          
          UARTCharPutNonBlocking(UART0_BASE,'e');
          UARTCharPutNonBlocking(UART0_BASE,'a');
          UARTCharPutNonBlocking(UART0_BASE,13);
          status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
          
          Elift=abrindo;
            
          break;
        case  abrindo :
          if(abertaE==1){
            status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
            UARTCharPutNonBlocking(UART0_BASE,'e');
            UARTCharPutNonBlocking(UART0_BASE,'f');
            UARTCharPutNonBlocking(UART0_BASE, 13);
            status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
            
            abertaE=0;
            Elift=fechando;
          }
          break;
        case  fechando :
          if(fechadaE==1){
            fechadaE=0;
            Elift=idle;
            if(statusE==1){
              for(int i=andarE;i<16;i++){
                if(reqs1[andarE]!=0){
                  status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                  if  (status != TX_SUCCESS)
                    break;
                  UARTCharPutNonBlocking(UART0_BASE,  'e');
                  UARTCharPutNonBlocking(UART0_BASE,'s');
                  UARTCharPutNonBlocking(UART0_BASE,13);
                  status = tx_mutex_put(&mutex_0);
                  if  (status != TX_SUCCESS)
                    break;
                  Elift=subindo;
                  break;
                }                  
              }
            }
            else if(statusE==2){
              for(int i=andarE;i>=0;i--){
                if(reqs1[andarE]!=0){
                  status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                  if  (status != TX_SUCCESS)
                    break;
                  UARTCharPutNonBlocking(UART0_BASE,  'e');
                  UARTCharPutNonBlocking(UART0_BASE,'d');
                  UARTCharPutNonBlocking(UART0_BASE,13);
                  status = tx_mutex_put(&mutex_0);
                  if  (status != TX_SUCCESS)
                    break;
                  Elift=descendo;
                  break;
                }
              }
            }
          }
            break;
        
       
       

      }
    }
}

//Thread de controle do elevador Central, análogo à thread1
void    thread_2_entry(ULONG thread_input)
{


    UINT    status;
    uint16_t statusC;
    abertaC=0;
    fechadaC=0;
    while(1)
    {
        switch(Clift){
        case  idle  :
          statusC=0;
          for(int i=0;i<16;i++){
            if(reqs2[i]!=0){
              if(i>andarC){
                status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
                UARTCharPutNonBlocking(UART0_BASE,  'c');
                UARTCharPutNonBlocking(UART0_BASE,'s');
                UARTCharPutNonBlocking(UART0_BASE,13);
                status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
                Clift=subindo;
              }
              else if(i<andarC){
                status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
                UARTCharPutNonBlocking(UART0_BASE,  'c');
                UARTCharPutNonBlocking(UART0_BASE,'d');
                UARTCharPutNonBlocking(UART0_BASE,13);
                status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
                Clift=descendo;
              }
              
            }
          }
          break;
        case  subindo :
          statusC=1;
          if(reqs2[andarC]==1||reqs2[andarC]==2){
            status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
              UARTCharPutNonBlocking(UART0_BASE,'c');
            UARTCharPutNonBlocking(UART0_BASE,'p');
            UARTCharPutNonBlocking(UART0_BASE,13);
            status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
            reqs2[andarC]=0;
            Clift=parado;
          }
          else if(reqs2[andarC]==3){
            bool flag=true;
            for(int i=andarC;i<15;i++)
            {
              if (reqs2[i+1]!=0){
                flag=false;
                break;
              }
            }
            if(flag){
              status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
              UARTCharPutNonBlocking(UART0_BASE, 'c');
              UARTCharPutNonBlocking(UART0_BASE,'p');
              UARTCharPutNonBlocking(UART0_BASE,13);
              status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
              reqs2[andarC]=0;
              Clift=parado;
            }
          }
          break;
        case  descendo :
          statusC=2;
          if(reqs2[andarC]==1||reqs2[andarC]==3){
            status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
              UARTCharPutNonBlocking(UART0_BASE, 'c');
            UARTCharPutNonBlocking(UART0_BASE,'p');
            UARTCharPutNonBlocking(UART0_BASE,13);
            status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
            reqs2[andarC]=0;
            Clift=parado;
          }
           else if(reqs2[andarC]==2){
            bool flag=true;
            for(int i=andarC;i>0;i--)
            {
              if (reqs2[i-1]!=0){
                flag=false;
                break;
              }
            }
            if(flag){
              status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
              UARTCharPutNonBlocking(UART0_BASE, 'c');
              UARTCharPutNonBlocking(UART0_BASE,'p');
              UARTCharPutNonBlocking(UART0_BASE,13);
              status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
              reqs2[andarC]=0;
              Clift=parado;
            }
          }
          break;
        case  parado:
          status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
          UARTCharPutNonBlocking(UART0_BASE,'c');
          UARTCharPutNonBlocking(UART0_BASE,'D');
          UARTCharPutNonBlocking(UART0_BASE,andarC+'a');
          UARTCharPutNonBlocking(UART0_BASE,13); 
          
          
          UARTCharPutNonBlocking(UART0_BASE,'c');
          UARTCharPutNonBlocking(UART0_BASE,'a');
          UARTCharPutNonBlocking(UART0_BASE,13);
          status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
          
          Clift=abrindo;
            
          break;
        case  abrindo :
          if(abertaC==1){
            status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
            UARTCharPutNonBlocking(UART0_BASE,'c');
            UARTCharPutNonBlocking(UART0_BASE,'f');
            UARTCharPutNonBlocking(UART0_BASE, 13);
            status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
            
            abertaC=0;
            Clift=fechando;
          }
          break;
        case  fechando :
          if(fechadaC==1){
            fechadaC=0;
            Clift=idle;
            if(statusC==1){
              for(int i=andarC;i<16;i++){
                if(reqs2[andarC]!=0){
                  status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                  if  (status != TX_SUCCESS)
                    break;
                  UARTCharPutNonBlocking(UART0_BASE,  'c');
                  UARTCharPutNonBlocking(UART0_BASE,'s');
                  UARTCharPutNonBlocking(UART0_BASE,13);
                  status = tx_mutex_put(&mutex_0);
                  if  (status != TX_SUCCESS)
                    break;
                  Clift=subindo;
                  break;
                }                  
              }
            }
            else if(statusC==2){
              for(int i=andarC;i>=0;i--){
                if(reqs2[andarC]!=0){
                  status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                  if  (status != TX_SUCCESS)
                    break;
                  UARTCharPutNonBlocking(UART0_BASE,  'c');
                  UARTCharPutNonBlocking(UART0_BASE,'d');
                  UARTCharPutNonBlocking(UART0_BASE,13);
                  status = tx_mutex_put(&mutex_0);
                  if  (status != TX_SUCCESS)
                    break;
                  Clift=descendo;
                  break;
                }
              }
            }
          }
            break;
        
       
       

      }
    }
}

//Thread de controle do elevador Direito, análoga à thread1
void    thread_3_entry(ULONG thread_input)
{

    UINT    status;
    uint16_t statusD;
    abertaD=0;
    fechadaD=0;
    while(1)
    {
        switch(Dlift){
        case  idle  :
          statusD=0;
          for(int i=0;i<16;i++){
            if(reqs3[i]!=0){
              if(i>andarD){
                status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
                UARTCharPutNonBlocking(UART0_BASE,  'd');
                UARTCharPutNonBlocking(UART0_BASE,'s');
                UARTCharPutNonBlocking(UART0_BASE,13);
                status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
                Dlift=subindo;
              }
              else if(i<andarD){
                status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
                UARTCharPutNonBlocking(UART0_BASE,  'd');
                UARTCharPutNonBlocking(UART0_BASE,'d');
                UARTCharPutNonBlocking(UART0_BASE,13);
                status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
                Dlift=descendo;
              }
              
            }
          }
          break;
        case  subindo :
          statusD=1;
          if(reqs3[andarD]==1||reqs3[andarD]==2){
            status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
              UARTCharPutNonBlocking(UART0_BASE,'d');
            UARTCharPutNonBlocking(UART0_BASE,'p');
            UARTCharPutNonBlocking(UART0_BASE,13);
            status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
            reqs3[andarD]=0;
            Dlift=parado;
          }
          else if(reqs3[andarD]==3){
            bool flag=true;
            for(int i=andarD;i<15;i++)
            {
              if (reqs3[i+1]!=0){
                flag=false;
                break;
              }
            }
            if(flag){
              status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
              UARTCharPutNonBlocking(UART0_BASE, 'd');
              UARTCharPutNonBlocking(UART0_BASE,'p');
              UARTCharPutNonBlocking(UART0_BASE,13);
              status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
              reqs3[andarD]=0;
              Dlift=parado;
            }
          }
          break;
        case  descendo :
          statusD=2;
          if(reqs3[andarD]==1||reqs3[andarD]==3){
            status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
              UARTCharPutNonBlocking(UART0_BASE, 'd');
            UARTCharPutNonBlocking(UART0_BASE,'p');
            UARTCharPutNonBlocking(UART0_BASE,13);
            status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
            reqs3[andarD]=0;
            Dlift=parado;
          }
           else if(reqs3[andarD]==2){
            bool flag=true;
            for(int i=andarD;i>0;i--)
            {
              if (reqs3[i-1]!=0){
                flag=false;
                break;
              }
            }
            if(flag){
              status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
              UARTCharPutNonBlocking(UART0_BASE, 'd');
              UARTCharPutNonBlocking(UART0_BASE,'p');
              UARTCharPutNonBlocking(UART0_BASE,13);
              status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
              reqs3[andarD]=0;
              Dlift=parado;
            }
          }
          break;
        case  parado:
          status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
          UARTCharPutNonBlocking(UART0_BASE,'d');
          UARTCharPutNonBlocking(UART0_BASE,'D');
          UARTCharPutNonBlocking(UART0_BASE,andarD+'a');
          UARTCharPutNonBlocking(UART0_BASE,13); 
          
          
          UARTCharPutNonBlocking(UART0_BASE,'d');
          UARTCharPutNonBlocking(UART0_BASE,'a');
          UARTCharPutNonBlocking(UART0_BASE,13);
          status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
          
          Dlift=abrindo;
            
          break;
        case  abrindo :
          if(abertaD==1){
            status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                if  (status != TX_SUCCESS)
                  break;
            UARTCharPutNonBlocking(UART0_BASE,'d');
            UARTCharPutNonBlocking(UART0_BASE,'f');
            UARTCharPutNonBlocking(UART0_BASE, 13);
            status = tx_mutex_put(&mutex_0);
                if  (status != TX_SUCCESS)
                  break;
            
            abertaD=0;
            Dlift=fechando;
          }
          break;
        case  fechando :
          if(fechadaD==1){
            fechadaD=0;
            Dlift=idle;
            if(statusD==1){
              for(int i=andarD;i<16;i++){
                if(reqs3[andarD]!=0){
                  status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                  if  (status != TX_SUCCESS)
                    break;
                  UARTCharPutNonBlocking(UART0_BASE,  'd');
                  UARTCharPutNonBlocking(UART0_BASE,'s');
                  UARTCharPutNonBlocking(UART0_BASE,13);
                  status = tx_mutex_put(&mutex_0);
                  if  (status != TX_SUCCESS)
                    break;
                  Dlift=subindo;
                  break;
                }                  
              }
            }
            else if(statusD==2){
              for(int i=andarD;i>=0;i--){
                if(reqs3[andarD]!=0){
                  status = tx_mutex_get(&mutex_0,TX_WAIT_FOREVER);
                  if  (status != TX_SUCCESS)
                    break;
                  UARTCharPutNonBlocking(UART0_BASE,  'd');
                  UARTCharPutNonBlocking(UART0_BASE,'d');
                  UARTCharPutNonBlocking(UART0_BASE,13);
                  status = tx_mutex_put(&mutex_0);
                  if  (status != TX_SUCCESS)
                    break;
                  Dlift=descendo;
                  break;
                }
              }
            }
          }
            break;
        
       
       

      }
    }
}
