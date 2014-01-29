
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "USART1.h"

/* Private variables ---------------------------------------------------------*/
EXTI_InitTypeDef   EXTI_InitStructure;
GPIO_InitTypeDef   GPIO_InitStructure;
NVIC_InitTypeDef   NVIC_InitStructure;

/* Private function prototypes -----------------------------------------------*/
void Delay(uint32_t nCount);
void EXTI0_Config(void);
void EXTI9_5_Config(void);

/* Private function ----------------------------------------------------------*/
int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 	

 	/* LED0 -> PB0 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	USART1_Init();
	USART1_Print("\r\n");
	USART1_Print("*************************************************************\r\n");
    USART1_Print("*                                                           *\r\n");
	USART1_Print("*  Thank you for using MatchboxARM Development Board ! ^_^  *\r\n");
	USART1_Print("*                                                           *\r\n");
	USART1_Print("*************************************************************\r\n"); 

    /* Configure PA0 in interrupt mode */
    EXTI0_Config();

    /* Generate software interrupt on EXTI0 line */
    EXTI_GenerateSWInterrupt(EXTI_Line0);    
    
    /* Infinite loop */
    while(1)
	{		
    }
}


void Delay(uint32_t nCount)
{
   for(; nCount != 0; nCount--);
}


void EXTI0_Config(void)
{
   /* Enable GPIOA clock */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
   /* Configure PA0 pin as input floating */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   /* Enable alternative function(AFIO) clock */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

   /* Connect EXTI0 Line to PA0 pin */
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

   /* Configure EXTI0 line */
   EXTI_InitStructure.EXTI_Line = EXTI_Line0;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);

   /* Enable and set EXTI0 Interrupt to the lowest priority */
   /* (check "misc.h" priority table on line 79)*/
   NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 15;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
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
