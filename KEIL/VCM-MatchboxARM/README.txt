
This example shows how to use MatchboxARM as a USB to serial device.

You must install first the driver for the device(driver is in the stsw-stm32102.zip file).


After you programm the MatchboxARM with the .bin file, when you connect to PC USB
it will show as a USB serial adapter. You need to open the Termite serial interface,
configure for port (the one of the device), 8bit, stop bit 1, parity none, flow control none.
When you type something the MatchboxARM will send you back the text. 

For Linux the port will be ACME type and you don't need drivers.(use dmesg).