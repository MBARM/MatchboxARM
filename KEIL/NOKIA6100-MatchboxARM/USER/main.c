
/****************************************************************************
* 3D Cube demo program for MatchboxARM                                      *
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


/*
 *
 * Interface LCD Nokia 6100 by GPIO to MatchboxARM (Soft SPI driver)
 *
 * Compiler: Keil-MDK 4.50.0.0
 *
 * MatchboxARM    LCD Nokia-6100
 *    PB13               CS
 *    PB10              SCLK
 *    PB11              SDATA
 *    PB12              RESET
 *    VCC               VCC
 *    GND               GND
 *                      VLCD NEEDS EXTERNALL 6-7V.                   
 */
#include "stm32f10x.h"
#include "math.h"

#include "font.h"			// Font Table Code
#include "bug.h"			// Bitmap Graphic File
#include "s1d15g00.h"		// LCD-NOKIA6610 Epson:S1D15G00 Controller





/******************************************
 *
 *
 ******************************************/
 
const int cub [8][3] =
{
    {-30, 30,-30},
    { 30, 30,-30},
    { 30,-30,-30},
    {-30,-30,-30},
	
    {-30, 30, 30},
    { 30, 30, 30},
    { 30,-30, 30},
    {-30,-30, 30}   
};

int cub_rotated[8][3];
int cub_projected[8][2];
int last_cube[8][2];
short cosa[256],sina[256];
int i;

void precalculate_cos_sin(void)
{
    for( i=0; i<256; i++)
    {
        cosa[i] = (int)( cos(i/40.585707465)*128 );    // round 
        sina[i] = (int)( sin(i/40.585707465)*128 );     
    }
}

void rotate( u8 Alpha, u8 Beta, u8 Gamma)
{
    int X,Y,Z, Y1,Z1, X2;

	for( i=0; i<8; i++)
	{
    	X = cub[i][0];
		Y = cub[i][1];
		Z = cub[i][2];
        Y1 =( cosa[Alpha]*Y   -  sina[Alpha]*Z ) / 128;
        Z1 =( sina[Alpha]*Y   +  cosa[Alpha]*Z ) / 128;
        X2 =( cosa[Beta] *X   +  sina[Beta]*Z1 ) / 128;
        Z  =( cosa[Beta] *Z1  -  sina[Beta]*X  ) / 128;
        X  =( cosa[Gamma]*X2  -  sina[Gamma]*Y1) / 128;
        Y  =( sina[Gamma]*X2  +  cosa[Gamma]*Y1) / 128;	
		cub_rotated[i][0] = X;
		cub_rotated[i][1] = Y; 
		cub_rotated[i][2] = Z;
	}
}

void perspective_projection(void)
{
    int distance;

    for( i=0; i<8; i++)
	{
        distance = 256 - cub_rotated[i][2];
        cub_projected[i][0] = 65+ (int)( (cub_rotated[i][0]*256) / distance ); // round
        cub_projected[i][1] = 65- (int)( (cub_rotated[i][1]*256) / distance );		
    }
}




/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(vu32 nCount)
{
    for(; nCount != 0; nCount--);
}

int main(void)
{
    unsigned long j;
    int TempColor[11] = { WHITE, BLACK, RED, GREEN, BLUE, CYAN,
                          MAGENTA, YELLOW, BROWN, ORANGE, PINK };

    char *TempChar[11] = { "White  ", "Black  ", "Red    ", "Green  ", "Blue   ", "Cyan   ",
                           "Magenta", "Yellow ", "Brown  ", "Orange ", "Pink   " };
    GPIO_InitTypeDef GPIO_InitStructure;
                         
    unsigned char Alpha=0, Beta=0, Gamma=0;
                         

    /* Set the Vector Table base adress at 0x8004000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);



    /* Configure IO connected to LCD-NOKIA6610 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CS, ENABLE);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_CS_PIN;
    GPIO_Init(LCD_CS_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_SCLK, ENABLE);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_SCLK_PIN;
    GPIO_Init(LCD_SCLK_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_SDATA, ENABLE);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_SDATA_PIN;
    GPIO_Init(LCD_SDATA_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_RESET, ENABLE);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_RESET_PIN;
    GPIO_Init(LCD_RESET_PORT, &GPIO_InitStructure);
  
  
    /* Config GPIO Control LCD */    
    LCD_SCLK_LO();   											// Standby SCLK
    LCD_CS_HI();												// Disable CS
    LCD_SDATA_HI();												// Standby SDATA

    // Init LCD
    InitLcd(); 													// Initial LCD

    LCDClearScreen();
    
    precalculate_cos_sin();
    
    
    // To test the 3D cube display
	for(;;)
	{
        Alpha = (Alpha+2)% 256;
		Beta  = (Beta+255)% 256;
		Gamma = (Gamma+1)% 256;		
     
	    rotate(Alpha, Beta, Gamma);
		
	    perspective_projection();
        
		Delay(10);				
		
        LCDSetLine( last_cube[0][0],last_cube[0][1], last_cube[1][0],last_cube[1][1], BLACK);
        LCDSetLine( last_cube[1][0],last_cube[1][1], last_cube[2][0],last_cube[2][1], BLACK);
        LCDSetLine( last_cube[2][0],last_cube[2][1], last_cube[3][0],last_cube[3][1], BLACK);
        LCDSetLine( last_cube[3][0],last_cube[3][1], last_cube[0][0],last_cube[0][1], BLACK);
       
        LCDSetLine( last_cube[0][0],last_cube[0][1], last_cube[4][0],last_cube[4][1], BLACK);
        LCDSetLine( last_cube[1][0],last_cube[1][1], last_cube[5][0],last_cube[5][1], BLACK);
        LCDSetLine( last_cube[2][0],last_cube[2][1], last_cube[6][0],last_cube[6][1], BLACK);
        LCDSetLine( last_cube[3][0],last_cube[3][1], last_cube[7][0],last_cube[7][1], BLACK);
      
        LCDSetLine( last_cube[4][0],last_cube[4][1], last_cube[5][0],last_cube[5][1], BLACK);
        LCDSetLine( last_cube[5][0],last_cube[5][1], last_cube[6][0],last_cube[6][1], BLACK);
        LCDSetLine( last_cube[6][0],last_cube[6][1], last_cube[7][0],last_cube[7][1], BLACK);
        LCDSetLine( last_cube[7][0],last_cube[7][1], last_cube[4][0],last_cube[4][1], BLACK);
       
/////////////////////////////////////////////////		
        LCDSetLine( cub_projected[0][0],cub_projected[0][1], cub_projected[1][0],cub_projected[1][1], WHITE);
        LCDSetLine( cub_projected[1][0],cub_projected[1][1], cub_projected[2][0],cub_projected[2][1], WHITE);
        LCDSetLine( cub_projected[2][0],cub_projected[2][1], cub_projected[3][0],cub_projected[3][1], WHITE);
        LCDSetLine( cub_projected[3][0],cub_projected[3][1], cub_projected[0][0],cub_projected[0][1], WHITE);
       
        LCDSetLine( cub_projected[0][0],cub_projected[0][1], cub_projected[4][0],cub_projected[4][1], WHITE);
        LCDSetLine( cub_projected[1][0],cub_projected[1][1], cub_projected[5][0],cub_projected[5][1], WHITE);
        LCDSetLine( cub_projected[2][0],cub_projected[2][1], cub_projected[6][0],cub_projected[6][1], WHITE);
        LCDSetLine( cub_projected[3][0],cub_projected[3][1], cub_projected[7][0],cub_projected[7][1], WHITE);
      
        LCDSetLine( cub_projected[4][0],cub_projected[4][1], cub_projected[5][0],cub_projected[5][1], WHITE);
        LCDSetLine( cub_projected[5][0],cub_projected[5][1], cub_projected[6][0],cub_projected[6][1], WHITE);
        LCDSetLine( cub_projected[6][0],cub_projected[6][1], cub_projected[7][0],cub_projected[7][1], WHITE);
        LCDSetLine( cub_projected[7][0],cub_projected[7][1], cub_projected[4][0],cub_projected[4][1], WHITE);
     
		for( i=0; i<8; i++)
		{
		    last_cube[i][0] = cub_projected[i][0];
		    last_cube[i][1] = cub_projected[i][1];
		}
    }

    /*
    // To test graphic functions      
    while(1)
    {      
        LCDClearScreen();											// Clear Screen	
        for (j = 0; j < 11; j++) 
        {    
            LCDSetRect(120, 10, 25, 120, FILL, TempColor[j]);  		// Draw Fill Box with Color
            LCDPutStr(TempChar[j], 5, 40, LARGE, YELLOW, BLACK);		// Label Name of Color    
            Delay(5000000); 											// Delay Page Display
        }
      
        LCDClearScreen();											// Clear Screen		    
        LCDPutStr("Hello World", 60, 5, SMALL, WHITE, BLACK);  		// Draw String
        LCDPutStr("Hello World", 40, 5, MEDIUM, ORANGE, BLACK);
        LCDPutStr("Hello World", 20, 5, LARGE, PINK, BLACK);     
        LCDSetLine(120, 10, 120, 50, YELLOW); 						// Draw Line Create Rectangle
        LCDSetLine(120, 50, 80, 50, YELLOW);
        LCDSetLine(80, 50, 80, 10, YELLOW);
        LCDSetLine(80, 10, 120, 10, YELLOW);    
        LCDSetLine(120, 85, 80, 105, YELLOW);						// Draw Line Create X
        LCDSetLine(80, 85, 120, 105, YELLOW);    
        LCDSetCircle(65, 100, 10, RED);								// Draw Circle    
        LCDPutChar('E', 60, 98, SMALL, WHITE, BLACK);	 			// Draw Characters
        Delay(5000000); 											// Delay Page Display
    }

    
    // To test bitmap display
    while(1)
    {      
        //LCDClearScreen();	 										// Clear Screen
        LCDWrite130x130bug();		   								// Display Graphic Bitmap 130x130
        //LCDPutStr("This is Picture ", 115, 2, LARGE, BLACK, CYAN);
	    Delay(5000000);Delay(5000000);Delay(5000000); 
    }
    */
}
  
  
  
  
  

