
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "mb.h"


volatile u32 Timer1;

/* Private function ----------------------------------------------------------*/
int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Set the Vector Table base adress at 0x8004000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
    
    // Initialize protocol stack in RTU mode for a slave with address 10 = 0x0A
	  // MB_RTU, Device ID: 1, USART portL: 1 (este configurat in portserial.h, Baud rate: 38400, Parity: NONE)
    eMBInit(MB_RTU, 1, 1, 38400, MB_PAR_NONE);
	  // Enable the Modbus Protocol Stack.
    eMBEnable();								

    while(1)
    {
			  // Call the main polling loop of the Modbus protocol stack.
        eMBPoll();								   
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
