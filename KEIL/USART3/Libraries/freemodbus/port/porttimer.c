/*
 * FreeModbus Libary: ATMega168 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
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
 * File: $Id: porttimer.c,v 1.4 2006/09/03 11:53:10 wolti Exp $
 */
//#include "stm32f10x.h"
//#include "stm32f10x_conf.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef base_timer;
	
    // разрешение тактирования таймера 2
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );

    TIM_TimeBaseStructInit( &base_timer );

    // 72000 kHz / 3600 = 20 kHz ( 50 ms )
    base_timer.TIM_Prescaler = 3600 - 1;
    base_timer.TIM_Period = ( (uint32_t) usTim1Timerout50us ) - 1;
    base_timer.TIM_ClockDivision = 0;
    base_timer.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM2, &base_timer );

    TIM_ClearITPendingBit( TIM2, TIM_IT_Update );

    // Enable interrupt to update (in this case - overflow) counter-timer TIM2.
    TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE );

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );

    TIM_Cmd( TIM2, ENABLE );

    return TRUE;
}


void vMBPortTimersEnable()
{
    TIM_SetCounter( TIM2, 0 );
    TIM_Cmd( TIM2, ENABLE );
}

void vMBPortTimersDisable()
{
    TIM_Cmd( TIM2, DISABLE );
}


void TIM2_IRQHandler( void )
{
    if ( TIM_GetITStatus( TIM2, TIM_IT_Update ) != RESET )
    {
        TIM_ClearITPendingBit( TIM2, TIM_IT_Update );

        (void) pxMBPortCBTimerExpired();
    }
}
