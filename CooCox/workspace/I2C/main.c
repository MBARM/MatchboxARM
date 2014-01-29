
#include "stm32f10x.h"
#include "MPU6050.h"
#include "USART1.h"

void Delay(uint32_t nCount)
{
  while(nCount) nCount--;
}


int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    s16 rawAccelGyroBuffer[6];
    float accel_x, accel_y, accel_z;
    float gyro_x, gyro_y, gyro_z;   
    

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE );
    
     /* Reset GPIO init structure */
    GPIO_StructInit(&GPIO_InitStructure);
    
    /* Configure LED0 & LED1 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

 	USART1_Init();
	USART1_Print("\r\n");
	USART1_Print("*************************************************************\n\r");
    USART1_Print("*                                                           *\n\r");
	USART1_Print("*  Thank you for using MatchboxARM Development Board ! ^_^  *\n\r");
	USART1_Print("*                                                           *\n\r");
	USART1_Print("*************************************************************\n\r"); 
    
    MPU6050_I2C_Init();
    MPU6050_Initialize();
      
    /* Ping MPU6050 to ensure is connected */    
    if (MPU6050_TestConnection())
    {
        USART1_Print("Connection success!\n\r");
    }
    else
    {        
        USART1_Print("Connection error!\n\r");
        
        /* Stop the execution */
        while (1);
    }
      
    while(1)
    {        
        /* Read the accelerometer and gyroscope data */
        MPU6050_GetRawAccelGyro(rawAccelGyroBuffer);  
       
        /* Extract accelerometer data from the main buffer*/        
        accel_x = rawAccelGyroBuffer[0] / 16384.0;
        accel_y = rawAccelGyroBuffer[1] / 16384.0;
        accel_z = rawAccelGyroBuffer[2] / 16384.0;
        
        /* Extract gyroscope Y component from the main buffer*/ 
        /* Convert to degrees/sec  */
        gyro_x = rawAccelGyroBuffer[3] / 16.4;
        gyro_y = rawAccelGyroBuffer[4] / 16.4;
        gyro_z = rawAccelGyroBuffer[5] / 16.4;
        
        USART1_Print("Accelerometer values: ");  
        USART1_Print_Float(accel_x);     
        USART1_Print(", ");  
        USART1_Print_Float(accel_y);     
        USART1_Print(", ");  
        USART1_Print_Float(accel_z);     
        USART1_Print("\n");  
        USART1_Print("Gyroscope values: ");  
        USART1_Print_Float(gyro_x);     
        USART1_Print(", ");  
        USART1_Print_Float(gyro_y);     
        USART1_Print(", ");  
        USART1_Print_Float(gyro_z);     
        USART1_Print("\n\r");

        /* Toggle LED */  
        GPIOB->ODR ^= GPIO_Pin_0;       

        Delay(0xfffff);
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
/*void Delay(uint32_t nCount)
{
  while(nCount) nCount--;
}
*/
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
