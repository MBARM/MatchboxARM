
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private function ----------------------------------------------------------*/ 
int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;
    volatile int i;
    int n = 1;
    int brightness = 0;
    
    /* Enable clock for GPIOB port and for alternative functions */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE );
    /* Enable clock for timer 3 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE );
 
    /* Reset init structure */
    GPIO_StructInit(&GPIO_InitStructure);
 
    /* LED0 -> PB0     LED1 -> PB1 */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    /* Alt Function - Push Pull */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;            
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    /* Map TIM3_CH3 to PB0, TIM3_CH4 to PB1 */
    GPIO_PinRemapConfig( GPIO_PartialRemap_TIM3, ENABLE );     
 
    /* Let PWM frequency equal 100Hz */
    /* Let period equal 1000. Therefore, timer runs from zero to 1000. Gives 0.1Hz resolution */
    /* Solving for prescaler gives 240 */
    TIM_TimeBaseStructInit( &TIM_TimeBaseInitStruct );
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV4;
    TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;   /* 0..999 */
    TIM_TimeBaseInitStruct.TIM_Prescaler = 240 - 1; /* Div 240 */
    TIM_TimeBaseInit( TIM3, &TIM_TimeBaseInitStruct );
 
    TIM_OCStructInit( &TIM_OCInitStruct );
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    /* Initial duty cycle equals 0%. Value can range from zero to 1000 */
    /* 0 .. 1000 (0=Always Off, 1000=Always On) */
    TIM_OCInitStruct.TIM_Pulse = 0; 
 
    /* Channel 3 -> PB0 */ 
    TIM_OC3Init( TIM3, &TIM_OCInitStruct ); 
    /* Channel 4 -> PB1 */ 
    TIM_OC4Init( TIM3, &TIM_OCInitStruct );
 
    TIM_Cmd( TIM3, ENABLE );
 
    /* Infinite loop */ 
    while(1)
    {
        /* If  brightness maximum/maximum change direction */ 
        if (((brightness + n) >= 1000) || ((brightness + n) <= 0))
            n = -n; 
 
        brightness += n;
 
        /* Set brightness for each led */
        TIM3->CCR3 = brightness; 
        TIM3->CCR4 = 1000 - brightness;
 
        /* Delay :) */
        for(i=0;i<10000;i++);
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
