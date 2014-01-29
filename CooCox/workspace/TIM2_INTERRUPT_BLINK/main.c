
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private function ----------------------------------------------------------*/
int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBase_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* Enable clock for GPIOB port */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 	

 	/* LED0 -> PB0 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);    
    
    /* Enable clock for timer 2 on APB1 at 36MHz */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);

    TIM_DeInit(TIM2);
    
    /* Set timer 2 clock at full speed, 36MHz */
    TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    /* Timmer 2 counting up */
    TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    /* Set timer 2 period to 0..999 */
    TIM_TimeBase_InitStructure.TIM_Period = 1000 - 1;
    /* Set timer 2 prescaler to 36000 => TIMER2 FREQ = 1kHz */
    TIM_TimeBase_InitStructure.TIM_Prescaler = 36000 - 1;
    //TIM_TimeBaseInit(TIM2, &TIM_TimeBase_InitStructure);

    /* Clear timer 2 interrupt update flag */
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    /* Enable timer 2 interrupt */
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
 
    /* Set timer 2 interrupt on lowest priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); 
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* Lowest priority level = 15 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_Init(&NVIC_InitStructure);
 
    /* Enable timer 2 */
    TIM_Cmd(TIM2, ENABLE);        
    
    /* Infinite loop */
    while(1){
    }
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

