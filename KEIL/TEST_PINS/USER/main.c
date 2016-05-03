
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private function prototypes -----------------------------------------------*/
void Delay(uint32_t nCount);

void pause(void)
{
    Delay(0xffff);
}

/* Private function ----------------------------------------------------------*/
int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Set the Vector Table base adress at 0x8004000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
    	
    /* Enable clock for GPIOB port */    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); 	
  
    /* So we must disable the JTAG and the SWD interface */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);  	
       

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_1);	
    GPIO_ResetBits(GPIOB, GPIO_Pin_5);
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);	
    GPIO_ResetBits(GPIOB, GPIO_Pin_7);
    GPIO_ResetBits(GPIOB, GPIO_Pin_8);	
    GPIO_ResetBits(GPIOB, GPIO_Pin_9);
    GPIO_ResetBits(GPIOB, GPIO_Pin_10);	
    GPIO_ResetBits(GPIOB, GPIO_Pin_11);
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);	
    GPIO_ResetBits(GPIOB, GPIO_Pin_13);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);	
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);	
    GPIO_ResetBits(GPIOA, GPIO_Pin_2);
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);	
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);	
    GPIO_ResetBits(GPIOA, GPIO_Pin_6);
    GPIO_ResetBits(GPIOA, GPIO_Pin_7);	
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    GPIO_ResetBits(GPIOA, GPIO_Pin_9);	
    GPIO_ResetBits(GPIOA, GPIO_Pin_10);
    GPIO_ResetBits(GPIOA, GPIO_Pin_13);	
    GPIO_ResetBits(GPIOA, GPIO_Pin_14);	
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);	
	
    /* Infinite loop, just toggle leds */
    while (1)
	{
        GPIO_SetBits(GPIOB, GPIO_Pin_0);    pause();
        GPIO_ResetBits(GPIOB , GPIO_Pin_0);
        
        GPIO_SetBits(GPIOB, GPIO_Pin_1);    pause();
        GPIO_ResetBits(GPIOB , GPIO_Pin_1);
        
        GPIO_SetBits(GPIOB, GPIO_Pin_15);   pause();
        GPIO_ResetBits(GPIOB , GPIO_Pin_15);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_8);    pause();
        GPIO_ResetBits(GPIOA , GPIO_Pin_8);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_9);    pause();
        GPIO_ResetBits(GPIOA , GPIO_Pin_9);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_10);   pause();
        GPIO_ResetBits(GPIOA , GPIO_Pin_10);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_13);   pause();
        GPIO_ResetBits(GPIOA , GPIO_Pin_13);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_14);   pause();
        GPIO_ResetBits(GPIOA , GPIO_Pin_14);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_15);   pause();
        GPIO_ResetBits(GPIOA , GPIO_Pin_15);
        
        GPIO_SetBits(GPIOB, GPIO_Pin_5);    pause();
        GPIO_ResetBits(GPIOB , GPIO_Pin_5);
        
        GPIO_SetBits(GPIOB, GPIO_Pin_6);    pause();
        GPIO_ResetBits(GPIOB , GPIO_Pin_6);
        
        GPIO_SetBits(GPIOB, GPIO_Pin_7);    pause();
        GPIO_ResetBits(GPIOB , GPIO_Pin_7);
        
        GPIO_SetBits(GPIOB, GPIO_Pin_8);    pause();
        GPIO_ResetBits(GPIOB , GPIO_Pin_8);
        
        GPIO_SetBits(GPIOB, GPIO_Pin_9);    pause();
        GPIO_ResetBits(GPIOB , GPIO_Pin_9);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_0);    pause();
        GPIO_ResetBits(GPIOA , GPIO_Pin_0);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_1);    pause();
        GPIO_ResetBits(GPIOA , GPIO_Pin_1);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_2);    pause();
        GPIO_ResetBits(GPIOA , GPIO_Pin_2);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_3);    pause();
        GPIO_ResetBits(GPIOA , GPIO_Pin_3);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_4);    pause();
        GPIO_ResetBits(GPIOA , GPIO_Pin_4);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_5);    pause();
        GPIO_ResetBits(GPIOA , GPIO_Pin_5);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_6);    pause();
        GPIO_ResetBits(GPIOA , GPIO_Pin_6);
        
        GPIO_SetBits(GPIOA, GPIO_Pin_7);    pause();
        GPIO_ResetBits(GPIOA , GPIO_Pin_7);
        
        GPIO_SetBits(GPIOB, GPIO_Pin_10);   pause();
        GPIO_ResetBits(GPIOB , GPIO_Pin_10);
        
        GPIO_SetBits(GPIOB, GPIO_Pin_11);   pause();
        GPIO_ResetBits(GPIOB , GPIO_Pin_11);
        
        GPIO_SetBits(GPIOB, GPIO_Pin_12);   pause();
        GPIO_ResetBits(GPIOB , GPIO_Pin_12);
        
        GPIO_SetBits(GPIOB, GPIO_Pin_13);   pause();
        GPIO_ResetBits(GPIOB , GPIO_Pin_13);
        
        GPIO_SetBits(GPIOB, GPIO_Pin_14);   pause();
        GPIO_ResetBits(GPIOB , GPIO_Pin_14);        
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
