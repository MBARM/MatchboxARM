
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private variables ---------------------------------------------------------*/
static volatile uint32_t TimingDelay;

/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nTime);

/* Private functions ---------------------------------------------------------*/
int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Set the Vector Table base adress at 0x8004000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
    
	 
    /* Setup SysTick Timer for 1 msec interrupts.
       ------------------------------------------
    1. The SysTick_Config() function is a CMSIS function which configure:
       - The SysTick Reload register with value passed as function parameter.
       - Configure the SysTick IRQ priority to the lowest value (0x0F).
       - Reset the SysTick Counter register.
       - Configure the SysTick Counter clock source to be Core Clock Source (HCLK).
       - Enable the SysTick Interrupt.
       - Start the SysTick Counter.
    
    2. You can change the SysTick Clock source to be HCLK_Div8 by calling the
       SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8) just after the
       SysTick_Config() function call. The SysTick_CLKSourceConfig() is defined
       inside the misc.c file.

    3. You can change the SysTick IRQ priority by calling the
       NVIC_SetPriority(SysTick_IRQn,...) just after the SysTick_Config() function 
       call. The NVIC_SetPriority() is defined inside the core_cm3.h file.

    4. To adjust the SysTick time base, use the following formula:
                            
         Reload Value = SysTick Counter Clock (Hz) x  Desired Time base (s)
    
       - Reload Value is the parameter to be passed for SysTick_Config() function
       - Reload Value should not exceed 0xFFFFFF
    */
    if(SysTick_Config(SystemCoreClock / 1000))
    { 
        /* Capture error */ 
        while (1);
    }    
    
    /* Enable clock for GPIOB port */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 	

 	/* LED0 -> PB0     LED1 -> PB1  */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Infinite loop */
    while (1)
	{
		/* LED0-ON LED1-OFF */
		GPIO_SetBits(GPIOB , GPIO_Pin_0);
        GPIO_ResetBits(GPIOB , GPIO_Pin_1);
        /* Insert 100 ms delay */
        Delay(100);

		/* LED0-OFF LED1-ON */
		GPIO_ResetBits(GPIOB , GPIO_Pin_0);
        GPIO_SetBits(GPIOB , GPIO_Pin_1);
        /* Insert 400 ms delay */
        Delay(400);			
    }
}


/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(volatile uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
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
