
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private function prototypes -----------------------------------------------*/
void Delay(uint32_t nCount);

/* Private function ----------------------------------------------------------*/
int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable clock for GPIOB port */    
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 	
       
 	/* LED0 -> PB0   LED1 -> PB1 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Infinite loop, just toggle leds */
    while (1)
	{
		/* LED0-ON LED1-OFF */
		GPIO_SetBits(GPIOB , GPIO_Pin_0);
		GPIO_ResetBits(GPIOB , GPIO_Pin_1);
		Delay(0xfffff);
        
		/* LED0-OFF LED1-ON */
		GPIO_ResetBits(GPIOB , GPIO_Pin_0);
		GPIO_SetBits(GPIOB , GPIO_Pin_1);
		Delay(0xfffff);
		
    }			  
}


void  Delay (uint32_t nCount)
{
    for(; nCount != 0; nCount--);
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
