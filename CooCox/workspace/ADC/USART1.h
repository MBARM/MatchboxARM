
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_usart.h"

/* Function prototypes -------------------------------------------------------*/
void USART1_Putch(unsigned char ch);
void USART1_Print(char s[]);
void USART1_Print_Int(int number);
void USART1_Init(void);

/* Function definitions ------------------------------------------------------*/
void USART1_Putch(unsigned char ch)
{
    USART_SendData( USART1, ch);
    // Wait until the end of transmision
    while( USART_GetFlagStatus( USART1, USART_FLAG_TC) == RESET){}
}


void USART1_Print(char s[])
{
    int i=0;
    
    while( i < 64)
    {
        if( s[i] == '\0') break;
        USART1_Putch( s[i++]);
    }	
}


void USART1_Print_Int(int number)
{
    unsigned char s[5], i=1, j=0;

    if( number < 0)
    { 
    	USART1_Putch( '-');
    	number = -number;
    }	
 
    while( number >= 10)
    {
        s[i++] = number % 10;
	number /= 10;
    }
    s[i] = number;
    j = i;
    for( i=j; i>0; i--) USART1_Putch( '0' + s[i]);
}


void USART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);

    // PA9 is TX , PA10 is RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);		   

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 38400;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure); 
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}
