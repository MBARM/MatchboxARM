

This test show how to interface MatchboxARM with the AT45DB041 dataflash.
It will write some values in the dataflash chip, and read back the values to USART.

You must connect:

 DATAFLASH         MatchboxARM (GPIOA, SPI1)
    CS                 PA4
   SCK                 PA5
  MISO                 PA6
  MOSI                 PA7
   GND                 GND 
   VCC                 3V3
   
Also connect one resistor from dataflash RESET line to 3V3, and one resistor
from dataflash CS line to 3V3. This are pull-up resistors.   

This example need one AT45DB041 dataflash chip, 2x 10Kohm resistors and USB-TTL adapter.