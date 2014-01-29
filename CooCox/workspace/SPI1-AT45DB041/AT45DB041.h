
#include "stm32f10x_spi.h"

// AT45DB041 SPI DATAFLASH DRIVER

//Dataflash opcodes
#define StatusReg					0xD7	// Status register
#define ContArrayRead				0x03	// Continuous Array Read (Low Frequency) Up to 33MHz
#define Buf1ToFlashWE   			0x83	// Buffer 1 to main memory page program with built-in erase
#define Buf1Write					0x84	// Buffer 1 write
#define Buf2ToFlashWE				0x86	// Buffer 2 to main memory page program with built-in erase
#define Buf2Write					0x87	// Buffer 2 write


// WP & RESET has 10K pull-up resistors

// SPI1 on PORTA
#define SS     GPIO_Pin_4
#define SCK    GPIO_Pin_5
#define MISO   GPIO_Pin_6
#define MOSI   GPIO_Pin_7


//Dataflash macro definitions
#define DF_CS_low	 GPIO_ResetBits(GPIOA , SS);
#define DF_CS_high   GPIO_SetBits(GPIOA , SS);

 
unsigned int page_counter=0, buffer_counter=0;


/* Function prototypes -------------------------------------------------------*/
void DF_SPI_init(void);
unsigned char DF_SPI_RW(unsigned char output);
unsigned char Read_DF_Status(void);
void Buffer_Write_Byte(unsigned char BufferNo, unsigned int IntPageAdr, unsigned char Data);
void Buffer_To_Page(unsigned char BufferNo, unsigned int PageAdr);
void Cont_Flash_Read_Enable(void);
void to_flash(unsigned char data);




/* Function definitions ------------------------------------------------------*/
void DF_SPI_init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 

    GPIO_InitStructure.GPIO_Pin = SCK | MISO | MOSI;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
								  
    GPIO_InitStructure.GPIO_Pin = SS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    DF_CS_high;
    /* SPI1 Config -------------------------------------------------------------*/ 								  
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; 
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; 
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);
    /* Enable SPI1 */ 
    SPI_Cmd(SPI1, ENABLE); 
}


unsigned char DF_SPI_RW(unsigned char data)
{
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    /* Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(SPI1, data);
    
    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);      
}


unsigned char Read_DF_Status(void)
{
	unsigned char result;
	
	DF_CS_low;							//to reset dataflash command decoder
	
	DF_SPI_RW(StatusReg);			        //send status register read op-code
	result = DF_SPI_RW(0x00);				//dummy write to get result
	
	DF_CS_high;	                        // Deselect DataFlash good for power consumption 
	
	return result;							//return the read status register value
}


void Buffer_Write_Byte(unsigned char BufferNo, unsigned int IntPageAdr, unsigned char Data)
{
	DF_CS_low;								      //to reset dataflash command decoder
	
	if (1 == BufferNo)							      //write byte to buffer 1
	{
		DF_SPI_RW(Buf1Write);					      //buffer 1 write op-code
		DF_SPI_RW(0x00);						      //don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));   //upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	  //lower part of internal buffer address
		DF_SPI_RW(Data);						      //write data byte
	}
	else
	if (2 == BufferNo)							      //write byte to buffer 2
	{  
		DF_SPI_RW(Buf2Write);					      //buffer 2 write op-code
		DF_SPI_RW(0x00);						      //don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));   //upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	  //lower part of internal buffer address
		DF_SPI_RW(Data);						      //write data byte
	}		

	DF_CS_high;								      //make sure to toggle CS signal in order	
}


void Buffer_To_Page(unsigned char BufferNo, unsigned int PageAdr)
{
	DF_CS_low;									 //to reset dataflash command decoder

	if (1 == BufferNo)								 //program flash page from buffer 1
	{
		DF_SPI_RW(Buf1ToFlashWE);					 //buffer 1 to flash with erase op-code
		DF_SPI_RW((unsigned char)(PageAdr >> 7));   //upper part of page address
		DF_SPI_RW((unsigned char)(PageAdr << 1));	 //lower part of page address
		DF_SPI_RW(0x00);										//don't cares
	}
	else	
	if (2 == BufferNo)								 //program flash page from buffer 2
	{
		DF_SPI_RW(Buf2ToFlashWE);					 //buffer 2 to flash with erase op-code
		DF_SPI_RW((unsigned char)(PageAdr >> 7));	 //upper part of page address
		DF_SPI_RW((unsigned char)(PageAdr << 1));	 //lower part of page address
		DF_SPI_RW(0x00);										//don't cares
	}
	
	DF_CS_high;									 //initiate flash page programming ( low-to-high transition )
	
	while(!(Read_DF_Status() & 0x80));				 //monitor the status register, wait until busy-flag is high
}


void Cont_Flash_Read_Enable(void)
{
	DF_CS_low;																//to reset dataflash command decoder
	
	DF_SPI_RW(ContArrayRead);	//Continuous Array Read op-code
	DF_SPI_RW(0x00);
	DF_SPI_RW(0x00);
	DF_SPI_RW(0x00);
}


void to_flash(unsigned char data)
{
    Buffer_Write_Byte(1, buffer_counter, data);
	buffer_counter++; 
	
	if(buffer_counter == 264)
	{
	    buffer_counter = 0;
		
	    if(page_counter < 4096)
		{
            Buffer_To_Page(1, page_counter);
	        page_counter++;
		}
	}
}

/*
7.1 Buffer Write
=================
Data can be clocked in from the input pin (SI) into either buffer 1 or buffer 2. To load data into the
standard Atmel� DataFlash� buffer (264-bytes), a 1-byte opcode, 84H for buffer 1 or 87H for buffer
2, must be clocked into the device, followed by three address bytes comprised of 15 don�t
care bits and nine buffer address bits (BFA8 - BFA0). The nine buffer address bits specify the
first byte in the buffer to be written. 

After the last address byte has been clocked into the device, data can then be clocked in on subsequent clock
cycles. If the end of the data buffer is reached, the device will wrap around back to the beginning
of the buffer. Data will continue to be loaded into the buffer until a low-to-high transition is
detected on the CS pin.


7.2 Buffer to Main Memory Page Program with Built-in Erase
===========================================================
Data written into either buffer 1 or buffer 2 can be programmed into the main memory. A 1-byte
opcode, 83H for buffer 1 or 86H for buffer 2, must be clocked into the device. For the DataFlash
standard page size (264-bytes), the opcode must be followed by three address bytes consist of
three don�t care bits, 12 page address bits (PA11 - PA0) that specify the page in the main memory
to be written and nine don�t care bits. 

When a low-to-high transition occurs on the CS pin, the part will first
erase the selected page in main memory (the erased state is a logic 1) and then program the
data stored in the buffer into the specified page in main memory. Both the erase and the programming
of the page are internally self-timed and should take place in a maximum time of tEP.
During this time, the status register will indicate that the part is busy.


6.3 Continuous Array Read (Low Frequency Mode: 03H): Up to 33MHz
=================================================================
This command can be used with the serial interface to read the main memory array sequentially
without a dummy byte up to maximum frequencies specified by fCAR2. To perform a continuous
read array with the page size set to 264-bytes, the CS must first be asserted then an opcode,
03H, must be clocked into the device followed by three address bytes (which comprise the 24-bit
page and byte address sequence). The first 12 bits (PA11 - PA0) of the 21-bit address sequence
specify which page of the main memory array to read, and the last nine bits (BA8 - BA0) of the
21-bit address sequence specify the starting byte address within the page.

Following the address bytes, additional clock pulses
on the SCK pin will result in data being output on the SO (serial output) pin.
The CS pin must remain low during the loading of the opcode, the address bytes, and the reading
of data. When the end of a page in the main memory is reached during a Continuous Array
Read, the device will continue reading at the beginning of the next page with no delays incurred
during the page boundary crossover (the crossover from the end of one page to the beginning of
the next page). When the last bit in the main memory array has been read, the device will continue
reading back at the beginning of the first page of memory. As with crossing over page
boundaries, no delays will be incurred when wrapping around from the end of the array to the
beginning of the array. A low-to-high transition on the CS pin will terminate the read operation
and tri-state the output pin (SO). The Continuous Array Read bypasses both data buffers and
leaves the contents of the buffers unchanged.
*/
