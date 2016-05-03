
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "mb.h"

/* Private function prototypes -----------------------------------------------*/  
void  Delay (uint32_t nCount);

volatile u32 Timer1;

volatile uint8_t Modbus_Request_Flag;

/* Private function ----------------------------------------------------------*/
int main(void)
{
	
/*
	In fisierul mb.c am adaugat la linia 61:
	    >> extern volatile u8 Modbus_Request_Flag;
  ca sa faca referire la definitia din acest fisier a variabilei flag pentru cererile modbus
  
  RMMS settings
  Timeout (ms) 1000
  Delay between pools (ms) 200
*/	
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Set the Vector Table base adress at 0x8004000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
    
	  Modbus_Request_Flag = 0;
	
	  /* Enable clock for GPIOB port */ 
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 	
    
    /* LED0 -> PB0     LED1 -> PB1  */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	
    // Initialize protocol stack in RTU mode for a slave with address 10 = 0x0A
	  // MB_RTU, Device ID: 1, USART portL: 1 (este configurat in portserial.h, Baud rate: 38400, Parity: NONE)
    eMBInit(MB_RTU, 1, 1, 38400, MB_PAR_NONE);
	  // Enable the Modbus Protocol Stack.
    eMBEnable();								

    while(1)
    {
			  // Reset the flag. It will only be set if the modbus pooling has a request
			  Modbus_Request_Flag = 0;


  			// Call the main polling loop of the Modbus protocol stack.
        eMBPoll();				

  			if (Modbus_Request_Flag)
				{
	    	    GPIO_SetBits(GPIOB , GPIO_Pin_0);
				}

        GPIO_ResetBits(GPIOB , GPIO_Pin_1);
				Delay(0xfffff);
				

			  if (Modbus_Request_Flag)
				{
	        	GPIO_ResetBits(GPIOB , GPIO_Pin_0);
				}

        GPIO_SetBits(GPIOB , GPIO_Pin_1);
        Delay(0xfffff);		


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
