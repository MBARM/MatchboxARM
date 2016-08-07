
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
#include "mb.h"


void  Delay (uint32_t nCount);

volatile u32 Timer1;

volatile uint8_t Modbus_Request_Flag;

// If we need to store signed values we use 2's complement
// int16_t val = -100;
// uint16_t number = (uint16_t) val 
u16 usRegInputBuf[100+1];   // 99 uint16_t registers
u16 usRegHoldingBuf[100+1]; // 99 uint16_t registers
u8  usRegCoilBuf[64/8+1]; // 60 efective coils, don't know why???!!!


void writeCoil(uint8_t coil_index, uint8_t state)
{
    uint8_t coil_offset=coil_index/8;
    if (state == 1)
        usRegCoilBuf[coil_offset] |= (1<<(coil_index%8));
    else usRegCoilBuf[coil_offset] &= ~(1<<(coil_index%8));
}

uint8_t getCoil(uint8_t coil_index)
{
    uint8_t coil_byte=usRegCoilBuf[coil_index/8];
    if (coil_byte && (1<<(coil_index%8))) return 1;
    else return 0;
}

void writeInputRegister(uint8_t reg_index, uint16_t reg_val)
{
    usRegInputBuf[reg_index] = reg_val;
}
                        
void writeHoldingRegister(uint8_t reg_index, uint16_t reg_val)
{
    usRegHoldingBuf[reg_index] = reg_val;    
}


/* Private function ----------------------------------------------------------*/
int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    uint8_t cnt=0;
    
    /* Set the Vector Table base adress at 0x8004000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);    
    
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
  		if (Modbus_Request_Flag) GPIO_SetBits(GPIOB , GPIO_Pin_0);
		Delay(0xffff);
		if (Modbus_Request_Flag) GPIO_ResetBits(GPIOB , GPIO_Pin_0);
        
        
        cnt++;

        if (cnt == 4) \
        {            
            writeInputRegister(1, 111);
            writeInputRegister(2, 222);
            writeInputRegister(98, 111);
            writeInputRegister(99, 222);
                        
            writeHoldingRegister(1, 333);
            writeHoldingRegister(2, 444);
            writeHoldingRegister(98, 333);
            writeHoldingRegister(99, 444);
            
            writeCoil(1, 0);           
            writeCoil(2, 1);                       
            writeCoil(58, 1);
            writeCoil(59, 0);
        }
        
        if (cnt == 8) 
        {                      
            writeInputRegister(1, 222);
            writeInputRegister(2, 111);
            writeInputRegister(98, 222);
            writeInputRegister(99, 111);
                        
            writeHoldingRegister(1, 444);
            writeHoldingRegister(2, 333);
            writeHoldingRegister(98, 444);
            writeHoldingRegister(99, 333);
            
            writeCoil(1, 1);           
            writeCoil(2, 0);                       
            writeCoil(58, 0);
            writeCoil(59, 1);            
            
            cnt = 0;
        }
        
        
								   
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
