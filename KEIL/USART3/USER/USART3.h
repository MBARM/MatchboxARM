
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_usart.h"


/* Function prototypes -------------------------------------------------------*/
void USART3_Putch(unsigned char ch);
void USART3_Print(char s[]);
void USART3_Print_Int(int number);
void USART3_Init(void);


/* Function definitions ------------------------------------------------------*/
void USART3_Putch(unsigned char ch)
{
	USART_SendData( USART3, ch);

	// Wait until the end of transmision
	while( USART_GetFlagStatus( USART3, USART_FLAG_TC) == RESET){}
}


void USART3_Print(char s[])
{
    int i=0;
    
    while( i < 64)
	{
	    if( s[i] == '\0') break;
        USART3_Putch( s[i++]);
    }	
}


void USART3_Print_Int(int number)
{
	unsigned char s[5], i=1, j=0;

    if( number < 0)
    { 
    	USART3_Putch( '-');
		number = -number;
	}	
 
    while( number >= 10)
    {
	    s[i++] = number % 10;
	    number /= 10;
    }
    s[i] = number;
    j = i;
    for( i=j; i>0; i--) USART3_Putch( '0' + s[i]);
}


void USART3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    // Configure USART3 Tx (PB.10) as alternate function push-pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
		
	  // Configure USART3 Rx (PB.11) as input floating
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	  // USART3 configuration
    USART_InitStructure.USART_BaudRate = 19200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure); 
		
    //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    USART_Cmd(USART3, ENABLE);
}
