
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "USART1.h"

/* Private define ------------------------------------------------------------*/
#define  FLASH_ADR   0x0800F000  		/* Flash memory page 62 on 64Kb chip STM32F103C8T6 */
#define	 FLASH_DATA	 0x5a5a5a5a		    /* Word data to be written */

/* Private functions ---------------------------------------------------------*/
void GPIO_Configuration(void);

/* Private function prototypes -----------------------------------------------*/
void Delay(uint32_t nCount);

/* Private function ----------------------------------------------------------*/
int main(void)
{
    uint32_t FlashData;

	GPIO_Configuration();
	USART1_Init();
	USART1_Print("\r\n");
	USART1_Print("*************************************************************\r\n");
    USART1_Print("*                                                           *\r\n");
	USART1_Print("*  Thank you for using MatchboxARM Development Board ! ^_^  *\r\n");
	USART1_Print("*                                                           *\r\n");
	USART1_Print("*************************************************************\r\n");

	/* Read current value from the flash memory */
	FlashData=*(vu32*)(FLASH_ADR);	 
    
    /* If data is empty (flash was erased) then programm new data */
	if(FlashData==0xffffffff)
	{
		FLASH_Unlock();		
		FLASH_ProgramWord(FLASH_ADR,FLASH_DATA);   
		FLASH_Lock();							  
		USART1_Print("Word data was programmed in the flash memory.\r\n");
	}
    /* If data is the same */
	else if(FlashData==FLASH_DATA)
	{
		USART1_Print("Word data already programmed in flash memory.\r\n");
	}
    /* If data something, that means the flash location already been programmed with different data */
	else
	{
		USART1_Print("Flash memory page 62 needs an erase.\r\n");
	    FLASH_Unlock();
		FLASH_ErasePage(FLASH_ADR);		  
		FLASH_Lock();
		USART1_Print("Flash memory page 62 erased.\r\n");
	}

    /* Infinite loop, just toggle leds */
    while (1)
	{
		/* LED0-ON LED1-OFF */
		GPIO_SetBits(GPIOB , GPIO_Pin_0);
		GPIO_ResetBits(GPIOB , GPIO_Pin_1);
		Delay(0xfffff);
		Delay(0xfffff);
		Delay(0x5ffff);	

		/* LED0-OFF LED1-ON */
		GPIO_ResetBits(GPIOB , GPIO_Pin_0);
		GPIO_SetBits(GPIOB , GPIO_Pin_1);
		Delay(0xfffff);
		Delay(0xfffff);
		Delay(0x5ffff);				
    }
}


void Delay(uint32_t nCount)
{
   for(; nCount != 0; nCount--);
}


void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 						 
  /* LED0 -> PB0   LED1 -> PB1 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
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

