MatchboxARM
===========

CD content for MatchboxARM STM32F103C8T6 board and MatchboxARM robotic platform



To compile examplex for CooCox:
    * download CooCox
	* copy all "workspace" content from CD to "C:\CooCox\CoIDE\workspace"
	* install gcc-arm-none-eabi-4_6-2012q2-20120614.exe
Open CoIDE. In the menu -> project -> select toolchain path
set the pat to armgcc( C:\Program Files (x86)\GNU Tools ARM Embedded\4.6 2012q2\bin ).


To compile examples for KEIL, download KEIL evaluation version with 32kb limit.

The bootloader sources are in KEIL directory.

All the examples need NEED USB-TTL adapter to get the feedback.
You need to connect the adapter like this:

USB-TTL ADAPTER            MatchboxARM
    TX           ->         RX (PA10)
    RX           ->         TX (PA9)
    GND          ->         GND 

The setting on the PC side for the serial port are:
	   BAUD RATE : 38400 
	   DATA BITS : 8
	   STOP BITS : 1
	      PARITY : NONE
	FLOW CONTROL : NONE

The termite is a free serial terminal for Windows.

The work is still in progress for Linux.


