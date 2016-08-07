
MODBUS stack in MatchboxARM

Connected as follows:


MBARM -> 485
------------
Matchboxarm      485 from ELECROW
---------------------------------
    +5V      |         VCC
	A9       |         TXD
    A10      |         RXD
    GND      |         GND


485 -> TCP
----------
485 from ELECROW     tGW-718
--------------------------------
    A+            |  TXD1+/D1+  
    B-            |  TXD1-/D1- 


// Read holdingregisters from 10 to 99
-t 3 holding
-t 2 input
-t 0 coils

modpoll -b 38400 -p none -m rtu -a 1 -t 3 - r 10 -c 99 COM6


// Ca sa citesc atunci cand folosesc adaptorul 485/TCP
modpoll -m tcp -t 0 -r 1 -c 60 192.168.1.150







Usage

Usage: modpoll [options] serialport|host
Arguments:
serialport    Serial port when using Modbus ASCII or Modbus RTU protocol
              COM1, COM2 ...                on Windows
              /dev/ttyS0, /dev/ttyS1 ...    on Linux
              /dev/ser1, /dev/ser2 ...      on QNX
host          Host name or dotted ip address when using MODBUS/TCP protocol
General options:
-m ascii      Modbus ASCII protocol
-m rtu        Modbus RTU protocol (default)
-m tcp        MODBUS/TCP protocol
-m enc        Encapsulated Modbus RTU over TCP
-a #          Slave address (1-255, 1 is default)
-r #          Start reference (1-65536, 100 is default)
-c #          Number of values to poll (1-100, 1 is default)
-t 0          Discrete output (coil) data type
-t 1          Discrete input data type
-t 3          16-bit input register data type
-t 3:hex      16-bit input register data type with hex display
-t 3:int      32-bit integer data type in input register table
-t 3:mod      32-bit module 10000 data type in input register table
-t 3:float    32-bit float data type in input register table
-t 4          16-bit output (holding) register data type (default)
-t 4:hex      16-bit output (holding) register data type with hex display
-t 4:int      32-bit integer data type in output (holding) register table
-t 4:mod      32-bit module 10000 type in output (holding) register table
-t 4:float    32-bit float data type in output (holding) register table
-i            Slave operates on big-endian 32-bit integers
-f            Slave operates on big-endian 32-bit floats
-1            Poll only once, otherwise poll every second
-e            Use Daniel/Enron single register 32-bit mode
-0            First reference is 0 (PDU addressing) instead 1
Options for MODBUS/TCP:
-p #          TCP port number (502 is default)
Options for Modbus ASCII and Modbus RTU:
-b #          Baudrate (e.g. 9600, 19200, ...) (9600 is default)
-d #          Databits (7 or 8 for ASCII protocol, 8 for RTU)
-s #          Stopbits (1 or 2, 1 is default)
-p none       No parity
-p even       Even parity (default)
-p odd        Odd parity
-4 #          RS-485 mode, RTS on while transmitting and another # ms after
-o #          Time-out in seconds (0.01 - 10.0, 1.0 s is default)
Usage Examples

To get help on usage run the following command:
modpoll -h
To retrieve continuously 10 Modbus holding registers starting from reference 500 of slave ID number 5 with Modbus RTU at 19200 baud, no parity on COM1 run:
modpoll -b 19200 -p none -m rtu -a 3 -r 500 -c 10 COM1
To retrieve once 5 floating point values starting from reference 100 with Modbus/TCP from slave device with IP 10.0.0.100:
modpoll -m tcp -t4:float -r 100 -c 5 -1 10.0.0.100