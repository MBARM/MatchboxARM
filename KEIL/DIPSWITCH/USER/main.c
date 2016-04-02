
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "USART1.h"

/* Private function prototypes -----------------------------------------------*/  
void Delay (uint32_t nCount);
void ADC_Configuration(void);
u16  readADC1(u8 channel);


uint8_t readDIPSwitch(void)
{
/*
	Voltage step for each ADC division (4095 divisions) = 0.00080586 V
	VOLTAGE = ADC*(3.3/4095.0)
	ADC resolution is 12bit => 4096 values

  Dipswitch with 3 contact
	000 = 0 = 3.3 V    => readADC = 4095
	001 = 1 = 2.27 V   => readADC = 2816
	010 = 2 = 1.96 V   => readADC = 2432
	011 = 3 = 1.54 V   => readADC = 1911
	100 = 4 = 1.32 V   => readADC = 1638
	101 = 5 = 1.12 V   => readADC = 1389
	110 = 6 = 1.04 V   => readADC = 1290
	111 = 7 = 0.9075 V => readADC = 1126	
*/	
    uint32_t k=0;
	  uint8_t i;
    // Average 30 readings
    for (i=0; i<30; i++)
        k += readADC1(ADC_Channel_0);
    k /= 30;
	
	  if (k>3500) return 0;
    if ((k>2750)&&(k<3500)) return 1;
    if ((k>2200)&&(k<2750)) return 2;
    if ((k>1800)&&(k<2200)) return 3;
    if ((k>1550)&&(k<1800)) return 4;
    if ((k>1350)&&(k<1550)) return 5;
    if ((k>1250)&&(k<1350)) return 6;
    if (k<1250) return 7;
	  // Else return value 8, ERROR 
	  return 8;
}

/* Private function ----------------------------------------------------------*/
/* The core is running at 72MHz */
int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Set the Vector Table base adress at 0x8004000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA , ENABLE); 
	
    /* Full SWJ Disabled (JTAG-DP + SW-DP) */
    GPIO_PinRemapConfig (GPIO_Remap_SWJ_Disable, ENABLE);  	
	
	
   	//LED0 -> PB0, LED1 -> PB1 
	  // Reset GPIO init structure
    GPIO_StructInit(&GPIO_InitStructure);	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	
		// Configure PA1 as input floating
    // Reset GPIO init structure
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


	
 	  USART1_Init();

    ADC_Configuration();

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
			
        /* Print the ADC PA1 DIP Switch value to USART */
        USART1_Print_Int(readDIPSwitch());
			
			  if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 1) USART1_Print(", ON\n");
			  else USART1_Print(", OFF\n");
    }			
}


void  Delay (uint32_t nCount)
{
    while(nCount) nCount--;
}


void ADC_Configuration(void) 
{ 
    ADC_InitTypeDef ADC_InitStructure; 
    
    /* PCLK2 is the APB2 clock */ 
    /* ADCCLK = PCLK2/6 = 72/6 = 12MHz*/ 
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
    
    /* Enable ADC1 clock so that we can talk to it */ 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
    
    /* Put everything back to power-on defaults */ 
    ADC_DeInit(ADC1); 
    
    /* ADC1 Configuration ------------------------------------------------------*/ 
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                    /* ADC1 and ADC2 operate independently */
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                         /* Disable the scan conversion so we do one at a time */ 
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                   /* Don't do contimuous conversions - do them on demand */
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   /* Start conversin by software, not an external trigger */ 
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                /* Conversions are 12 bit - put them in the lower 12 bits of the result */ 
    ADC_InitStructure.ADC_NbrOfChannel = 1;                               /* Say how many channels would be used by the sequencer */ 
    ADC_Init(ADC1, &ADC_InitStructure);                                   /* Now do the setup */ 
    ADC_Cmd(ADC1, ENABLE);                                                /* Enable ADC1 */ 
    
    /* Enable ADC1 reset calibaration register */ 
    ADC_ResetCalibration(ADC1); 
    
    /* Check the end of ADC1 reset calibration register */ 
    while(ADC_GetResetCalibrationStatus(ADC1)); 
    
    /* Start ADC1 calibaration */ 
    ADC_StartCalibration(ADC1); 
    
    /* Check the end of ADC1 calibration */ 
    while(ADC_GetCalibrationStatus(ADC1)); 
}


u16 readADC1(u8 channel) 
{ 
    /* Start the conversion */
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_1Cycles5);
    /* Wait until conversion completion */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    /* Get the conversion value */
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); 
    return ADC_GetConversionValue(ADC1); 
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
