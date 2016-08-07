
#include "stm32f10x.h"
#include "main.h"
#include "mbutils.h"
#include "mb.h"

void ENTER_CRITICAL_SECTION(void)// Off into the supercritical total interruption
{
	__set_PRIMASK(1);
}

void EXIT_CRITICAL_SECTION(void)// Exit supercritical total break open
{
	__set_PRIMASK(0);
}


u8 REG_INPUT_START=0, REG_HOLDING_START=0, REG_COIL_START=0;
u8 REG_INPUT_NREGS=100, REG_HOLDING_NREGS=100, REG_COIL_NREGS=64;
u8 usRegInputStart=0, usRegHoldingStart=0, usRegCoilsStart=0;


// Reading function code word register 0x04
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )&& ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}


// Register read and write commands to function supports read and write 0x06 0x03
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
	// u16 *PRT=(u16*)pucRegBuffer;

    if( ( usAddress >= REG_HOLDING_START ) && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
            case MB_REG_READ:
                while( usNRegs > 0 )
                {
                    // *PRT++ = __REV16(usRegHoldingBuf[iRegIndex++]); // Sequence data transfer REV16.W
                    *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] >> 8 );
                    *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] & 0xFF );
                    iRegIndex++;
                    usNRegs--;
                }
                break;

            case MB_REG_WRITE:
                while( usNRegs > 0 )
                {
                    // usRegHoldingBuf[iRegIndex++] = __REV16(*PRT++); // Sequence data transfer REV16.W
                    usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                    usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                    iRegIndex++;
                    usNRegs--;
                }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


// Read / write register switch 0x01  x05
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    short           iNCoils = ( short )usNCoils;
    unsigned short  usBitOffset;

    /* Check if we have registers mapped at this block. */
    if( ( usAddress >= REG_COIL_START ) && ( usAddress + usNCoils <= REG_COIL_START + REG_COIL_NREGS ) )
    {
        usBitOffset = ( unsigned short )( usAddress - REG_COIL_START );
        switch ( eMode )
        {
            /* Read current values and pass to protocol stack. */
            case MB_REG_READ:
                while( iNCoils > 0 )
                {
                    *pucRegBuffer++ =
                        xMBUtilGetBits( usRegCoilBuf, usBitOffset,
                                        ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ) );
                    iNCoils -= 8;
                    usBitOffset += 8;
                }
                break;

            /* Update current register values. */
            case MB_REG_WRITE:
                while( iNCoils > 0 )
                {
                    xMBUtilSetBits( usRegCoilBuf, usBitOffset,
                                    ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ),
                                    *pucRegBuffer++ );
                    iNCoils -= 8;
                }
                break;
        }

    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


// Read switch register 0x02
eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNDiscrete;
    return MB_ENOREG;
}
