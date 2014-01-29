/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Virtual Com Port Demo main file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"

/* Private function prototypes -----------------------------------------------*/
void Delay(uint32_t nCount);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
__IO uint8_t TestBuffer[27];

extern __IO uint8_t Receive_Buffer[64];
extern __IO  uint32_t Receive_length ;
extern __IO  uint32_t length ;
uint8_t Send_Buffer[64];
uint32_t packet_sent=1;
uint32_t packet_receive=1;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : main.
* Descriptioan    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{
    int i;

    /* Set the Vector Table base adress at 0x8004000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
    
    // Disable JTAG not the SWD  
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO , ENABLE);      
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  
  
    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
    
    for(i = 0; i < 27; i++) TestBuffer[i] = i % 9;  
    
    while (1)
    {
        if (bDeviceState == CONFIGURED)
        {
            //CDC_Send_DATA ((unsigned char*)TestBuffer,27);
            //Delay(0xFFF);         
            CDC_Receive_DATA();
            // Check to see if we have data yet
            if (Receive_length  != 0)
            {
                if (packet_sent == 1)
                CDC_Send_DATA ((unsigned char*)Receive_Buffer,Receive_length);
                Receive_length = 0;
            }
        }
    }
} 



void  Delay (uint32_t nCount)
{
    for(; nCount != 0; nCount--);
}


#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
