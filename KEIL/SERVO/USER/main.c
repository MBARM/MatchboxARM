
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"


void  Delay(uint32_t nCount)
{
  while(nCount) nCount--;
}



/* Private function ----------------------------------------------------------*/ 
int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    volatile int i;

    
    /* Set the Vector Table base adress at 0x8004000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
    

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;            
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;  // 36 MHz / 36 = 1 MHz
    TIM_TimeBaseStructure.TIM_Period = 20000 - 1; // 1 MHz / 20000 = 50 Hz (20 ms)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 1500; // 1500 us - Servo Top Centre
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);    // Channel 2 configuration = PA1

    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE); 
   
    while(1)
    {       
        TIM2->CCR2 = 1000;
        TIM2->CCR3 = 2000;
        for(i = 0; i < 10; i++) Delay(0xfffff);

        TIM2->CCR2 = 1500;
        TIM2->CCR3 = 1500;
        for(i = 0; i < 10; i++) Delay(0xfffff);

        TIM2->CCR2 = 2000;
        TIM2->CCR3 = 1000;
        for(i = 0; i < 10; i++) Delay(0xfffff);

        TIM2->CCR2 = 1500;
        TIM2->CCR3 = 1500;
        for(i = 0; i < 10; i++) Delay(0xfffff);
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
