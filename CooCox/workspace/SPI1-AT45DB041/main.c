
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "USART1.h"
#include "AT45DB041.h"

/* Private function prototypes -----------------------------------------------*/  
void  Delay (uint32_t nCount);

/* Private function ----------------------------------------------------------*/
int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    u8 data, k;
    u16 i;
    	 
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 	
    
   	/* LED0 -> PB0  LED1 -> PB1 */ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* LED0 ON */
    GPIO_SetBits(GPIOB , GPIO_Pin_0);

	USART1_Init();
	USART1_Print("\r\n");
	USART1_Print("*************************************************************\r\n");
    USART1_Print("*                                                           *\r\n");
	USART1_Print("*  Thank you for using MatchboxARM Development Board ! ^_^  *\r\n");
	USART1_Print("*                                                           *\r\n");
	USART1_Print("*************************************************************\r\n"); 
    USART1_Print("\r\n");
    USART1_Print("\r\n");    
    USART1_Print("Hardware SPI R/W test for AT45DB041 dataflash.\n\r");
    USART1_Print("\r\n");
    USART1_Print("Writing 264*10 bytes, 10 pages.\n\r");
    
    k=1;	
	for(i=0; i<264*10; i++)   // Dataflash size = 2048pages * 264byte/page = 540672 bytes
	{
	    to_flash(k);
		if( (k%127)==0 ) k=0;
		k++;
	}
	
	USART1_Print("Ok.\n\r");
	USART1_Print("Reading data:\n\r");

    Delay(0xfffff);

    Cont_Flash_Read_Enable();	
	
   	for(i=0; i<264*10; i++)
    {
        data = DF_SPI_RW(0x00);
		USART1_Print_Int(data);
        USART1_Print("\n\r");
        //Delay(0xfff);
	}
	
	DF_CS_high;           // Deselect DataFlash good for power consumption 
		
    while(1)
    {
    }
}   
    
void  Delay(uint32_t nCount)
{
    while(nCount) nCount--;
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
