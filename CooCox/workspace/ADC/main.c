
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "USART1.h"

/* Private function prototypes -----------------------------------------------*/  
void  Delay (uint32_t nCount);
void ADC_Configuration(void);
u16 readADC1(u8 channel);

/* Private function ----------------------------------------------------------*/
int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    uint32_t k;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 	
    
    //LED0 -> PB0, LED1 -> PB1 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
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

    ADC_Configuration();

    k = 0;
    while (1)
    {
        /* LED0-ON LED1-OFF */
	GPIO_SetBits(GPIOB , GPIO_Pin_0);
	GPIO_ResetBits(GPIOB , GPIO_Pin_1);
	Delay(0xfffff);
        Delay(k);
        
	/* LED0-OFF LED1-ON */
	GPIO_ResetBits(GPIOB , GPIO_Pin_0);
	GPIO_SetBits(GPIOB , GPIO_Pin_1);
	Delay(0xfffff);
        Delay(k);
             
	/* Read the pot value */
        k = readADC1(ADC_Channel_1);
        
        /* Print the ADC value from the pot, to USART */
        USART1_Print_Int(k);
        USART1_Print("\n\r");
        
        /* The ADC value is on 12 bits(4095), so we need to multiply to get a bigger delay */
        /* The core is running at 72MHz */
        k *= 500;
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
    /* ADC1 and ADC2 operate independently */ 
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 
    
    /* Disable the scan conversion so we do one at a time */ 
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; 
    
    /* Don't do contimuous conversions - do them on demand */
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
    
    /* Start conversin by software, not an external trigger */ 
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 
    
    /* Conversions are 12 bit - put them in the lower 12 bits of the result */ 
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 
    
    /* Say how many channels would be used by the sequencer */ 
    ADC_InitStructure.ADC_NbrOfChannel = 1; 
    
    /* Now do the setup */ 
    ADC_Init(ADC1, &ADC_InitStructure); 
    
    /* Enable ADC1 */ 
    ADC_Cmd(ADC1, ENABLE); 
    
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
