/*
 * FreeModbus Libary: ATMega168 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *   - Initial version and ATmega168 support
 * Modfications Copyright (C) 2006 Tran Minh Hoang:
 *   - ATmega8, ATmega16, ATmega32 support
 *   - RS485 support for DS75176
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.6 2006/09/17 16:45:53 wolti Exp $
 */

//#include "stm32f10x_conf.h"

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if( xRxEnable )
    {
        USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );
    }
    else
    {
        USART_ITConfig( USART1, USART_IT_RXNE, DISABLE );
    }

    if ( xTxEnable )
    {
        USART_ITConfig( USART1, USART_IT_TXE, ENABLE );

#ifdef RTS_ENABLE
        RTS_HIGH;
#endif
    }
    else
    {
        USART_ITConfig( USART1, USART_IT_TXE, DISABLE );
    }
}

BOOL xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
        eMBParity eParity )
{
    NVIC_InitTypeDef        NVIC_InitStructure;
	  GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	
    // Suppress compiler warning about an unused variable
    (void) ucPORT;
   
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);

    // USART1_TX -> PA9 , USART1_RX ->	PA10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);		   

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = (uint32_t)ulBaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure); 	

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;           // channel
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   // Priority
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          // Priority subgroup
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             // include * Channel
    NVIC_Init(&NVIC_InitStructure);                             // initialize

    USART_Cmd(USART1, ENABLE);

    vMBPortSerialEnable( TRUE, TRUE );

#ifdef RTS_ENABLE
    RTS_INIT;
#endif
    return TRUE;
}

BOOL xMBPortSerialPutByte( CHAR ucByte )
{
    USART_SendData( USART1, (uint16_t) ucByte );
    return TRUE;
}

BOOL xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = (CHAR) USART_ReceiveData( USART1 );
    return TRUE;
}

void USART1_IRQHandler( void )
{
    if ( USART_GetITStatus( USART1, USART_IT_RXNE ) != RESET )
    {
        USART_ClearITPendingBit( USART1, USART_IT_RXNE );
        pxMBFrameCBByteReceived();

    }
    if ( USART_GetITStatus( USART1, USART_IT_TXE ) != RESET )
    {
        USART_ClearITPendingBit( USART1, USART_IT_TXE );
        pxMBFrameCBTransmitterEmpty();
    }
}

//SIGNAL( SIG_USART_DATA )
//{
//    pxMBFrameCBTransmitterEmpty(  );
//}
//
//SIGNAL( SIG_USART_RECV )
//{
//
//}

#ifdef RTS_ENABLE
SIGNAL( SIG_UART_TRANS )
{
    RTS_LOW;
}
#endif

