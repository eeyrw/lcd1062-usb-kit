#ifndef __USB_H__
#define __USB_H__

#include <stdint.h>

typedef unsigned char BYTE;
typedef unsigned int WORD;
typedef unsigned long DWORD;

#define IRC48MCR (*(__xdata unsigned char volatile *)0xfe07)

#define FADDR 0
#define POWER 1
#define INTRIN1 2
#define EP5INIF 0x20
#define EP4INIF 0x10
#define EP3INIF 0x08
#define EP2INIF 0x04
#define EP1INIF 0x02
#define EP0IF 0x01
#define INTROUT1 4
#define EP5OUTIF 0x20
#define EP4OUTIF 0x10
#define EP3OUTIF 0x08
#define EP2OUTIF 0x04
#define EP1OUTIF 0x02
#define INTRUSB 6
#define SOFIF 0x08
#define RSTIF 0x04
#define RSUIF 0x02
#define SUSIF 0x01
#define INTRIN1E 7
#define EP5INIE 0x20
#define EP4INIE 0x10
#define EP3INIE 0x08
#define EP2INIE 0x04
#define EP1INIE 0x02
#define EP0IE 0x01
#define INTROUT1E 9
#define EP5OUTIE 0x20
#define EP4OUTIE 0x10
#define EP3OUTIE 0x08
#define EP2OUTIE 0x04
#define EP1OUTIE 0x02
#define INTRUSBE 11
#define SOFIE 0x08
#define RSTIE 0x04
#define RSUIE 0x02
#define SUSIE 0x01
#define FRAME1 12
#define FRAME2 13
#define INDEX 14
#define INMAXP 16
#define CSR0 17
#define SSUEND 0x80
#define SOPRDY 0x40
#define SDSTL 0x20
#define SUEND 0x10
#define DATEND 0x08
#define STSTL 0x04
#define IPRDY 0x02
#define OPRDY 0x01
#define INCSR1 17
#define INCLRDT 0x40
#define INSTSTL 0x20
#define INSDSTL 0x10
#define INFLUSH 0x08
#define INUNDRUN 0x04
#define INFIFONE 0x02
#define INIPRDY 0x01
#define INCSR2 18
#define INAUTOSET 0x80
#define INISO 0x40
#define INMODEIN 0x20
#define INMODEOUT 0x00
#define INENDMA 0x10
#define INFCDT 0x08
#define OUTMAXP 19
#define OUTCSR1 20
#define OUTCLRDT 0x80
#define OUTSTSTL 0x40
#define OUTSDSTL 0x20
#define OUTFLUSH 0x10
#define OUTDATERR 0x08
#define OUTOVRRUN 0x04
#define OUTFIFOFUL 0x02
#define OUTOPRDY 0x01
#define OUTCSR2 21
#define OUTAUTOCLR 0x80
#define OUTISO 0x40
#define OUTENDMA 0x20
#define OUTDMAMD 0x10
#define COUNT0 22
#define OUTCOUNT1 22
#define OUTCOUNT2 23
#define FIFO0 32
#define FIFO1 33
#define FIFO2 34
#define FIFO3 35
#define FIFO4 36
#define FIFO5 37
#define UTRKCTL 48
#define UTRKSTS 49

#define EPIDLE 0
#define EPSTATUS 1
#define EPDATAIN 2
#define EPDATAOUT 3
#define EPSTALL -1

#define GET_STATUS 0x00
#define CLEAR_FEATURE 0x01
#define SET_FEATURE 0x03
#define SET_ADDRESS 0x05
#define GET_DESCRIPTOR 0x06
#define SET_DESCRIPTOR 0x07
#define GET_CONFIG 0x08
#define SET_CONFIG 0x09
#define GET_INTERFACE 0x0A
#define SET_INTERFACE 0x0B
#define SYNCH_FRAME 0x0C

#define GET_REPORT 0x01
#define GET_IDLE 0x02
#define GET_PROTOCOL 0x03
#define SET_REPORT 0x09
#define SET_IDLE 0x0A
#define SET_PROTOCOL 0x0B

#define DESC_DEVICE 0x01
#define DESC_CONFIG 0x02
#define DESC_STRING 0x03
#define DESC_HIDREPORT 0x22

#define STANDARD_REQUEST 0x00
#define CLASS_REQUEST 0x20
#define VENDOR_REQUEST 0x40
#define REQUEST_MASK 0x60

typedef struct
{
    BYTE bmRequestType;
    BYTE bRequest;
    BYTE wValueL;
    BYTE wValueH;
    BYTE wIndexL;
    BYTE wIndexH;
    BYTE wLengthL;
    BYTE wLengthH;
} SETUP;

typedef struct
{
    signed char bStage;
    WORD wResidue;
    BYTE *pData;
} EP0STAGE;

void UsbInit();
BYTE ReadReg(BYTE addr);
void WriteReg(BYTE addr, BYTE dat);
BYTE ReadFifo(BYTE fifo, BYTE *pdat);
void WriteFifo(BYTE fifo, BYTE *pdat, BYTE cnt);



#endif