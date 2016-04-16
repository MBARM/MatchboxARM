/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               systick.c
** Descriptions:            Use SysTick Normal counting mode for delay management
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-10-30
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "systick.h"
#include "stm32f10x_systick.h"

/* Private variables ---------------------------------------------------------*/	 
static uint8_t  delay_fac_us = 0;  /* us Delay times multiplier */
static uint16_t delay_fac_ms = 0;  /* ms Delay times multiplier */
static FlagStatus  Status;


/*******************************************************************************
* Function Name  : delay_init
* Description    : Initialization delay function 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : SYSTICK The clock is fixed to HCLK clock 1/8
*******************************************************************************/
void delay_init(void)
{
    RCC_ClocksTypeDef RCC_ClocksStatus;

    RCC_GetClocksFreq(&RCC_ClocksStatus);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);  /*External clock  HCLK/8 */
	SysTick_ITConfig(DISABLE);
    delay_fac_us = RCC_ClocksStatus.HCLK_Frequency / 8000000;
    delay_fac_ms = RCC_ClocksStatus.HCLK_Frequency / 8000;      
}
					
/*******************************************************************************
* Function Name  : delay_us
* Description    : Initialization delay function 
* Input          : - Nus: Delay us
* Output         : None
* Return         : None
* Attention		 : The maximum value for the parameter 0xffffff / (HCLK / 8000000)
*******************************************************************************/            
void delay_us(u32 Nus)
{ 
    SysTick_SetReload(delay_fac_us * Nus);          /* Loading time */
    SysTick_CounterCmd(SysTick_Counter_Clear);		/* Empty counter */
    SysTick_CounterCmd(SysTick_Counter_Enable);		/* Start the countdown */ 	
    do
    {
        Status = SysTick_GetFlagStatus(SysTick_FLAG_COUNT);
    }while (Status != SET);							/* Wait time is reached */
    SysTick_CounterCmd(SysTick_Counter_Disable);    /* Close Counter */
	SysTick_CounterCmd(SysTick_Counter_Clear);	    /* Empty counter */   
}


/*******************************************************************************
* Function Name  : delay_ms
* Description    : Initialization delay function 
* Input          : - nms: Delay ms
* Output         : None
* Return         : None
* Attention		 : Parameter max. nms<=0xffffff*8*1000/SYSCLK Correct 72M Conditions ,nms<=1864 
*******************************************************************************/  
void delay_ms(uint16_t nms)
{    
    uint32_t temp = delay_fac_ms * nms;

    if (temp > 0x00ffffff)
    {
        temp = 0x00ffffff;
    }
    SysTick_SetReload(temp);		             /* Loading time */
    SysTick_CounterCmd(SysTick_Counter_Clear);	 /* Empty counter */
    SysTick_CounterCmd(SysTick_Counter_Enable);	 /* Start the countdown */ 
    do
    {
        Status = SysTick_GetFlagStatus(SysTick_FLAG_COUNT);
    }while (Status != SET);				         /* Wait time is reached */
    SysTick_CounterCmd(SysTick_Counter_Disable); /* Close Counter */
	SysTick_CounterCmd(SysTick_Counter_Clear);	 /* Empty counter */
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
















