/****************************************************************************
* Line follower program for MatchboxARM and MatchboxARM robotic platform    *
* Copyright 2013-2014 Nedelcu Bogdan Sebastian                              *
*                                                                           *
* This program is free software: you can redistribute it and/or modify      *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
****************************************************************************/

#include "stm32f10x.h"


/* GPIOB, PWM is on TIM2 CH3 and CH4 Remaped */
#define MotorLeft_Direction_Pin    GPIO_Pin_9
#define MotorLeft_PWM_Pin          GPIO_Pin_10

#define MotorRight_Direction_Pin   GPIO_Pin_8
#define MotorRight_PWM_Pin         GPIO_Pin_11

#define FORWARD     1
#define BACKWARD   -1

#define SENSOR_BLACK_THRESHOLD     3000

/* PD tunning constants */
float Kp, Kd;
/* PD control variable */
float control;
/* Previous error */
long ePrev  = 0;   
/* Current sensors reading */
float s;
/* Previous sensors reading */
float sprev;
/* Speed values for left and right motors */
int speedLeft, speedRight;


float PD(float current_value, float requested_value);
void Pins_Configuration(void);
void MotorLeft(int direction, int speed);    // As seen from the back
void MotorRight(int direction, int speed);   // As seen from the back
float LinePositionUnderSensors(void);

void TIM2_Configuration(void);
void ADC1_Configuration(void);
u16  ADC1_Read(u8 channel);
void USART1_Configuration(void);
void USART1_Putch(u8 ch);
void USART1_Print(char s[]);
void USART1_Print_Int(int number);
void USART1_Print_Float(float number);


void  Delay (uint32_t nCount)
{
  while(nCount) nCount--;
}

int main(void)
{
    /* Set the Vector Table base adress at 0x8004000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
    
    Pins_Configuration();
    MotorLeft(FORWARD, 0);
    MotorRight(FORWARD, 0);
    USART1_Configuration();
    TIM2_Configuration();
    ADC1_Configuration();    

    /* Wait until the user press the button */
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0);
    
    Kp =  225;
    Kd =  225;
    
    while(1)
    {
        s = LinePositionUnderSensors();

        /* If line is not found beneath any sensor, use last sensor value */
        if (s == 0xFF) s=sprev;

        control = PD(s, 3.5);

        /* Limit the control */
        if (control > 500) control = 500;
        if (control < -500) control = -500;

        /* Line under left sensors, so we must turn right */
        if (control > 0.0) 
        {
            if (control > 250) MotorLeft(BACKWARD, control - 250);
            else MotorLeft(FORWARD, 250 - control);			
            MotorRight(FORWARD, 250);
        }	
    
        /* Line under right sensors, so we must turn left */
        if (control <= 0.0) 
        {
            if (control < -250) MotorRight(BACKWARD, -(control + 250));
            else MotorRight(FORWARD, 250 + control);         
            MotorLeft(FORWARD, 250);
        }    
    
        Delay(5000);
        sprev = s;  
    }
}


float PD(float current_value, float requested_value)
{
    float pd, error;

    /* Compute the new error */
    error = requested_value - current_value;
    /* Compute PD control value */
    pd = (Kp * error) + (Kd * (error - ePrev));
    /* Save previous error for derivative */
    ePrev = error;

    return pd;
}


void Pins_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE );
 
    /* Reset GPIO init structure */
    GPIO_StructInit(&GPIO_InitStructure); 
 
    /* Motors direction pins configuration as output push-pull */
    GPIO_InitStructure.GPIO_Pin =  MotorLeft_Direction_Pin | MotorRight_Direction_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    /* Reset GPIO init structure */
    GPIO_StructInit(&GPIO_InitStructure);  
 
    /* Motors PWM pins configuration as alternative functions push-pull */
    GPIO_InitStructure.GPIO_Pin =  MotorLeft_PWM_Pin | MotorRight_PWM_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIOB, &GPIO_InitStructure );    
    
    /* The user button is on GPIOB Pin 4, where NRST from JTAG is */
    /* So we must disable the JTAG, but not the SWD interface */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  

    /* Reset GPIO init structure */
    GPIO_StructInit(&GPIO_InitStructure);
    
    /* Configure the user button as input floatingt */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


/* As seen from the back */
void MotorLeft(int direction, int speed)
{
    if (direction < 0) GPIO_ResetBits(GPIOB , MotorLeft_Direction_Pin);
    else GPIO_SetBits(GPIOB , MotorLeft_Direction_Pin);
    TIM2->CCR3 = speed;     
}


/* As seen from the back */
void MotorRight(int direction, int speed)
{
    if (direction < 0) GPIO_ResetBits(GPIOB , MotorRight_Direction_Pin);
    else GPIO_SetBits(GPIOB , MotorRight_Direction_Pin);
    TIM2->CCR4 = speed; 
}


float LinePositionUnderSensors(void)
{
    u8 Sensor1, Sensor2, Sensor3, Sensor4, Sensor5, Sensor6;
    
    if (ADC1_Read(ADC_Channel_6) > SENSOR_BLACK_THRESHOLD) Sensor1 = 1;
    else Sensor1 = 0;
    if (ADC1_Read(ADC_Channel_5) > SENSOR_BLACK_THRESHOLD) Sensor2 = 1;
    else Sensor2 = 0;
    if (ADC1_Read(ADC_Channel_4) > SENSOR_BLACK_THRESHOLD) Sensor3 = 1;
    else Sensor3 = 0;
    if (ADC1_Read(ADC_Channel_3) > SENSOR_BLACK_THRESHOLD) Sensor4 = 1;
    else Sensor4 = 0;
    if (ADC1_Read(ADC_Channel_2) > SENSOR_BLACK_THRESHOLD) Sensor5 = 1;
    else Sensor5 = 0;
    if (ADC1_Read(ADC_Channel_1) > SENSOR_BLACK_THRESHOLD) Sensor6 = 1;
    else Sensor6 = 0;

    /* Return error code when line is not under any oif the six sensors */
    if ((Sensor1 == 0) && (Sensor2 == 0) && (Sensor3 == 0) && (Sensor4 == 0) && (Sensor5 == 0) && (Sensor6 == 0)) return 0xFF;

    /* Calculate weighted mean and return an average position of line under sensors */
    return (float) (Sensor1*1 + Sensor2*2 + Sensor3*3 + Sensor4*4 + Sensor5*5 + Sensor6*6) / (float) (Sensor1 + Sensor2 + Sensor3 + Sensor4 + Sensor5 + Sensor6);
}


void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;
    
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );
    
    /* Map TIM2_CH3 to GPIOB.Pin10, TIM2_CH4 to GPIOB.Pin11 */
    GPIO_PinRemapConfig( GPIO_FullRemap_TIM2, ENABLE );
 
    /* Let PWM frequency equal 100Hz */
    /* Let period equal 1000. Therefore, timer runs from zero to 1000. Gives 0.1Hz resolution */
    /* Solving for prescaler gives 240 */
    TIM_TimeBaseStructInit( &TIM_TimeBaseInitStruct );
    
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV4;
	/* 0..999 */
    TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;
	/* Div 240 */
    TIM_TimeBaseInitStruct.TIM_Prescaler = 240 - 1; 
    TIM_TimeBaseInit( TIM2, &TIM_TimeBaseInitStruct );
 
    TIM_OCStructInit( &TIM_OCInitStruct );
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    /* Initial duty cycle equals 0%. Value can range from 0 to 1000 (0=Always Off, 1000=Always On)*/
    TIM_OCInitStruct.TIM_Pulse = 0; 
 
    /* Left  motor PWM */
    TIM_OC3Init( TIM2, &TIM_OCInitStruct ); 
	/* Right motor PWM */
    TIM_OC4Init( TIM2, &TIM_OCInitStruct ); 
 
    TIM_Cmd( TIM2, ENABLE );
}


void ADC1_Configuration(void) 
{ 
    ADC_InitTypeDef ADC_InitStructure; 
    
    /* PCLK2 is the APB2 clock */ 
    /* ADCCLK = PCLK2/6 = 72/6 = 12MHz*/ 
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
    
    /* Enable ADC1 clock so that we can talk to it */ 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
    
    /* Put everything back to power-on defaults */ 
    ADC_DeInit(ADC1); 
    
    /* ADC1 Configuration */
    
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


u16 ADC1_Read(u8 channel) 
{  
    /* Set conversion sampling time */
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_28Cycles5);   
    
    /* Start the conversion */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); 
    
    /* Wait until conversion completion */
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);  
    
    /* Get the conversion value */
    return ADC_GetConversionValue(ADC1);  
}


void USART1_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    /* USART1_TX -> PA9 , USART1_RX ->	PA10 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);		   

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 38400;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure); 
    USART_Cmd(USART1, ENABLE);    
}


void USART1_Putch(unsigned char ch)
{
	USART_SendData( USART1, ch);

	/* Wait until the end of transmision */
	while( USART_GetFlagStatus( USART1, USART_FLAG_TC) == RESET){}
}


void USART1_Print(char s[])
{
    int i=0;
    
    while( i < 64)
	{
	    if( s[i] == '\0') break;
        USART1_Putch( s[i++]);
    }	
}


void USART1_Print_Int(int number)
{
	unsigned char s[5], i=1, j=0;

    if( number < 0)
    { 
    	USART1_Putch( '-');
		number = -number;
	}	
 
    while( number >= 10)
    {
	    s[i++] = number % 10;
	    number /= 10;
    }
    s[i] = number;
    j = i;
    for( i=j; i>0; i--) USART1_Putch( '0' + s[i]);
}

void USART1_Print_Float(float number)
{
    float float_side;
    int int_side;
    
    int_side = number;
    float_side = number - int_side; 
    USART1_Print_Int(int_side);
    USART1_Print(".");
    /* We only print 1 decimals after quota, multiply with 100 for 2 decimals, 1000 for 3 decimals... */
    USART1_Print_Int(float_side * 10);
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
