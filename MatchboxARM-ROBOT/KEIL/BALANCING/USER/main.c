
/****************************************************************************
* Balancing robot program for MatchboxARM and MatchboxARM robotic platform   *
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

/* MPU-6050 is connected to I2C1 on PORTB pins SCL -> GPIO_Pin_6, SDA -> GPIO_Pin_7 */

#include "stm32f10x.h"
#include "MPU6050.h"
#include "KalmanY.h"
#include "math.h"
#include "main.h"

#define RADIANS_TO_DEGREES   180/3.14159

/* Systick counter for useconds */
static volatile u32 systickTimerCounter = 0;

/* GPIOB, PWM is on TIM2 CH3 and CH4 Remaped */
#define MotorLeft_Direction_Pin    GPIO_Pin_9
#define MotorLeft_PWM_Pin          GPIO_Pin_10

#define MotorRight_Direction_Pin   GPIO_Pin_8
#define MotorRight_PWM_Pin         GPIO_Pin_11

#define FORWARD     1
#define BACKWARD   -1

/* PD tunning constants */
//float Kp, Kd;
/* PD control variable */
float control;
/* Previous error */
long ePrev  = 0;   
/* Speed values for left and right motors */
int speedLeft, speedRight;
/* Maximmum alowed speed */
int MAXSPEED;
/* Command character sent by PC */
char ch;

/* USART functions */
void USART1_Configuration(void);
void USART1_Putch(u8 ch);
void USART1_Print(char s[]);
void USART1_Print_Int(int number);
void USART1_Print_Float(float number);
u32 Micros(void);

/* Motors functions */
void Pins_Configuration(void);
void MotorLeft(int direction, int speed);    // As seen from the back
void MotorRight(int direction, int speed);   // As seen from the back
void TIM2_Configuration(void);

/* Get robot tilt angle */
int Get_Tilt_Angle(u32 lastTimeStamp);


void Delay(uint32_t nCount)
{
  while(nCount) nCount--;
}


int main(void)
{
    unsigned long timer = 0;  
    float tilt_angle;
  
    // Proportional gain
    float Kp = 12.5;
    // Diferential gain
    float Kd = 0.5;
    // Integral gain
    float Ki = 12.5;
    
    // Type C PID controller
    float error = 0;
    float Aangle = 0;
    float Lastangle = 0;
    float LLastangle = 0;
    // Angle adjustment for Forward Backward control Is through the Desired angle
    // Desired angle
    float Dangle = -9;
    // PID controller command
    float COcommand = 0;    
    
    
    
    /* Set the Vector Table base adress at 0x8004000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
    
    
    /* Setup SysTick Timer for 1 usec interrupts.
     ------------------------------------------
    1. The SysTick_Config() function is a CMSIS function which configure:
       - The SysTick Reload register with value passed as function parameter.
       - Configure the SysTick IRQ priority to the lowest value (0x0F).
       - Reset the SysTick Counter register.
       - Configure the SysTick Counter clock source to be Core Clock Source (HCLK).
       - Enable the SysTick Interrupt.
       - Start the SysTick Counter.
    
    2. You can change the SysTick Clock source to be HCLK_Div8 by calling the
       SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8) just after the
       SysTick_Config() function call. The SysTick_CLKSourceConfig() is defined
       inside the misc.c file.

    3. You can change the SysTick IRQ priority by calling the
       NVIC_SetPriority(SysTick_IRQn,...) just after the SysTick_Config() function 
       call. The NVIC_SetPriority() is defined inside the core_cm3.h file.

    4. To adjust the SysTick time base, use the following formula:
                            
         Reload Value = SysTick Counter Clock (Hz) x  Desired Time base (s)
    
       - Reload Value is the parameter to be passed for SysTick_Config() function
       - Reload Value should not exceed 0xFFFFFF
    */    
    if (SysTick_Config(SystemCoreClock / 1000000))
    { 
        /* Capture error */        
        while (1);
    }
         
    Pins_Configuration();
    MotorLeft(FORWARD, 0);
    MotorRight(FORWARD, 0);
    USART1_Configuration();
    TIM2_Configuration();
        
    USART1_Print("\n\r");
    USART1_Print("Balancing robot demo.\n\r");
    USART1_Print("Initializing MPU-6050.\n\r");
    
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
        /* Mix accelerometer and gyroscope angles using Kalman filter*/ 
        tilt_angle = Get_Tilt_Angle(timer);

        /* Get the time stamp for further gyroscope integration */
        timer = Micros();
       
        
        // PID type C controller
        LLastangle = Lastangle;
        Lastangle = Aangle;
        Aangle = tilt_angle;
        // error is the Desired angle - actual angle
        error = Dangle - Aangle;        
        
        // PID controller
        COcommand = COcommand - Kp * (Aangle - Lastangle) + (Ki / 50) * error - (Kd * 50) * (Aangle - 2 * Lastangle+LLastangle);
                
        // constrain the Control low command to a valid range
        if ( COcommand > 255) COcommand = 255;
        if ( COcommand < -255) COcommand = -255;

        if ( COcommand >= 0) 
        {
            if ( COcommand < 1) 
            {
                MotorLeft(FORWARD, 0); 
                MotorRight(FORWARD, 0);
            }    
            else
            {
                MotorLeft(FORWARD, COcommand + 50); 
                MotorRight(FORWARD, COcommand + 50);
            }    
        }
        else
        {
            if ( COcommand > -5) 
            {            
                MotorLeft(BACKWARD, 0); 
                MotorRight(BACKWARD, 0);
            }
            else
            {            
                MotorLeft(BACKWARD, -COcommand + 50); 
                MotorRight(BACKWARD, -COcommand + 50);
            }
        }
     
        
        /* Toggle LED */  
        GPIOB->ODR ^= GPIO_Pin_0;                     
        Delay(0xfff2);
    }
}


int Get_Tilt_Angle(u32 lastTimeStamp)
{
    s16 rawAccelGyroBuffer[6];
    float accel_angle_y;
    float accel_x, accel_y, accel_z;
    float gyro_y;

    /* Read the accelerometer and gyroscope data */
    MPU6050_GetRawAccelGyro(rawAccelGyroBuffer);  
       
    /* Extract accelerometer data from the main buffer*/        
    accel_x = rawAccelGyroBuffer[0];
    accel_y = rawAccelGyroBuffer[1];
    accel_z = rawAccelGyroBuffer[2];
        
    /* Extract gyroscope Y component from the main buffer*/ 
    /* Convert to degrees/sec  */
    gyro_y = rawAccelGyroBuffer[4] / 131;
       
    /* Compute accelerometer Y angle */
    accel_angle_y = atan(-1 * accel_z / sqrt((accel_y * accel_y) + (accel_x * accel_x))) * RADIANS_TO_DEGREES;
        
    /* Mix accelerometer and gyroscope angles using Kalman filter*/ 
    return kalmanY(accel_angle_y, gyro_y, (double)(Micros() - lastTimeStamp));
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

    /* Reset GPIO init structure */
    GPIO_StructInit(&GPIO_InitStructure);
    
    /* Configure LED0 & LED1 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
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


/* Timer2 is used to generate PWM for motors speed */
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


void USART1_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // USART1_TX -> PA9 , USART1_RX ->	PA10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);		   

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 19200;
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

	// Wait until the end of transmision
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
    
    /* We only print 2 decimals after quota */
    if (float_side < 0) float_side *= -1;
    USART1_Print_Int(float_side * 100);
}


u32 Micros(void)
{
    u32 microsCounterValue = systickTimerCounter;
    
    return microsCounterValue;
}


/* This function is defined in main.h */
void SYSTICK_Counter_Increment_And_Delay(void)
{
    /* Used for micros function */
    systickTimerCounter++;
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
