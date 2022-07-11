#include "RegisterDefine.h"
#include <stdint.h>
#include "usb.h"

__code char DEVICEDESC[18];
__code char CONFIGDESC[41];
__code char HIDREPORTDESC[27];
__code char LANGIDDESC[4];
__code char MANUFACTDESC[8];
__code char PRODUCTDESC[30];

SETUP Setup;
EP0STAGE Ep0Stage;
__xdata BYTE  HidFreature[64];
__xdata BYTE  HidInput[64];
__xdata BYTE  HidOutput[64];

// void main()
// {
//     P0M0 = 0x00;
//     P0M1 = 0x00;
//     P1M0 = 0x00;
//     P1M1 = 0x00;
//     P2M0 = 0x00;
//     P2M1 = 0x00;
//     P3M0 = 0x00;
//     P3M1 = 0x00;
//     P4M0 = 0x00;
//     P4M1 = 0x00;
//     P5M0 = 0x00;
//     P5M1 = 0x00;

//     UsbInit();

//     IE2 = 0x80;
//     EA = 1;

//     while (1);
// }

BYTE ReadReg(BYTE addr)
{
    BYTE dat;

    while (USBADR & 0x80)
        ;
    USBADR = addr | 0x80;
    while (USBADR & 0x80)
        ;
    dat = USBDAT;

    return dat;
}

void WriteReg(BYTE addr, BYTE dat)
{
    while (USBADR & 0x80)
        ;
    USBADR = addr & 0x7f;
    USBDAT = dat;
}

BYTE ReadFifo(BYTE fifo, BYTE *pdat)
{
    BYTE cnt;
    BYTE ret;

    ret = cnt = ReadReg(COUNT0);
    while (cnt--)
    {
        *pdat++ = ReadReg(fifo);
    }

    return ret;
}

void WriteFifo(BYTE fifo, BYTE *pdat, BYTE cnt)
{
    while (cnt--)
    {
        WriteReg(fifo, *pdat++);
    }
}

void UsbInit()
{
    P3M0 = 0x00;
    P3M1 = 0x03;

    P_SW2 |= 0x80;
    IRC48MCR = 0x80;
    while (!(IRC48MCR & 0x01))
        ;
    P_SW2 &= ~0x80;
    USBCLK = 0x00;
    USBCON = 0x90;

    WriteReg(FADDR, 0x00);
    WriteReg(POWER, 0x08);
    WriteReg(INTRIN1E, 0x3f);
    WriteReg(INTROUT1E, 0x3f);
    WriteReg(INTRUSBE, 0x00);
    WriteReg(POWER, 0x01);

    Ep0Stage.bStage = EPIDLE;
}

void usb_isr()  __interrupt(USB_VECTOR)
{
    BYTE intrusb;
    BYTE intrin;
    BYTE introut;
    BYTE csr;
    BYTE cnt;
    WORD len;

    intrusb = ReadReg(INTRUSB);
    intrin = ReadReg(INTRIN1);
    introut = ReadReg(INTROUT1);

    if (intrusb & RSTIF)
    {
        WriteReg(INDEX, 1);
        WriteReg(INCSR1, INCLRDT);
        WriteReg(INDEX, 1);
        WriteReg(OUTCSR1, OUTCLRDT);
        Ep0Stage.bStage = EPIDLE;
    }

    if (intrin & EP0IF)
    {
        WriteReg(INDEX, 0);
        csr = ReadReg(CSR0);
        if (csr & STSTL)
        {
            WriteReg(CSR0, csr & ~STSTL);
            Ep0Stage.bStage = EPIDLE;
        }
        if (csr & SUEND)
        {
            WriteReg(CSR0, csr | SSUEND);
        }

        switch (Ep0Stage.bStage)
        {
        case EPIDLE:
            if (csr & OPRDY)
            {
                Ep0Stage.bStage = EPSTATUS;
                ReadFifo(FIFO0, (BYTE *)&Setup);
                ((BYTE *)&Ep0Stage.wResidue)[0] = Setup.wLengthH;
                ((BYTE *)&Ep0Stage.wResidue)[1] = Setup.wLengthL;
                switch (Setup.bmRequestType & REQUEST_MASK)
                {
                case STANDARD_REQUEST:
                    switch (Setup.bRequest)
                    {
                    case SET_ADDRESS:
                        WriteReg(FADDR, Setup.wValueL);
                        break;
                    case SET_CONFIG:
                        WriteReg(INDEX, 1);
                        WriteReg(INCSR2, INMODEIN);
                        WriteReg(INMAXP, 8);
                        WriteReg(INDEX, 1);
                        WriteReg(INCSR2, INMODEOUT);
                        WriteReg(OUTMAXP, 8);
                        WriteReg(INDEX, 0);
                        break;
                    case GET_DESCRIPTOR:
                        Ep0Stage.bStage = EPDATAIN;
                        switch (Setup.wValueH)
                        {
                        case DESC_DEVICE:
                            Ep0Stage.pData = (BYTE*)DEVICEDESC;
                            len = sizeof(DEVICEDESC);
                            break;
                        case DESC_CONFIG:
                            Ep0Stage.pData = CONFIGDESC;
                            len = sizeof(CONFIGDESC);
                            break;
                        case DESC_STRING:
                            switch (Setup.wValueL)
                            {
                            case 0:
                                Ep0Stage.pData = (BYTE*)LANGIDDESC;
                                len = sizeof(LANGIDDESC);
                                break;
                            case 1:
                                Ep0Stage.pData = MANUFACTDESC;
                                len = sizeof(MANUFACTDESC);
                                break;
                            case 2:
                                Ep0Stage.pData = PRODUCTDESC;
                                len = sizeof(PRODUCTDESC);
                                break;
                            default:
                                Ep0Stage.bStage = EPSTALL;
                                break;
                            }
                            break;
                        case DESC_HIDREPORT:
                            Ep0Stage.pData = HIDREPORTDESC;
                            len = sizeof(HIDREPORTDESC);
                            break;
                        default:
                            Ep0Stage.bStage = EPSTALL;
                            break;
                        }
                        if (len < Ep0Stage.wResidue)
                        {
                            Ep0Stage.wResidue = len;
                        }
                        break;
                    default:
                        Ep0Stage.bStage = EPSTALL;
                        break;
                    }
                    break;
                case CLASS_REQUEST:
                    switch (Setup.bRequest)
                    {
                    case GET_REPORT:
                        Ep0Stage.pData = HidFreature;
                        Ep0Stage.bStage = EPDATAIN;
                        break;
                    case SET_REPORT:
                        Ep0Stage.pData = HidFreature;
                        Ep0Stage.bStage = EPDATAOUT;
                        break;
                    case SET_IDLE:
                        break;
                    case GET_IDLE:
                    case GET_PROTOCOL:
                    case SET_PROTOCOL:
                    default:
                        Ep0Stage.bStage = EPSTALL;
                        break;
                    }
                    break;
                default:
                    Ep0Stage.bStage = EPSTALL;
                    break;
                }

                switch (Ep0Stage.bStage)
                {
                case EPDATAIN:
                    WriteReg(CSR0, SOPRDY);
                    goto L_Ep0SendData;
                    break;
                case EPDATAOUT:
                    WriteReg(CSR0, SOPRDY);
                    break;
                case EPSTATUS:
                    WriteReg(CSR0, SOPRDY | DATEND);
                    Ep0Stage.bStage = EPIDLE;
                    break;
                case EPSTALL:
                    WriteReg(CSR0, SOPRDY | SDSTL);
                    Ep0Stage.bStage = EPIDLE;
                    break;
                }
            }
            break;
        case EPDATAIN:
            if (!(csr & IPRDY))
            {
            L_Ep0SendData:
                cnt = Ep0Stage.wResidue > 64 ? 64 : Ep0Stage.wResidue;
                WriteFifo(FIFO0, Ep0Stage.pData, cnt);
                Ep0Stage.wResidue -= cnt;
                Ep0Stage.pData += cnt;
                if (Ep0Stage.wResidue == 0)
                {
                    WriteReg(CSR0, IPRDY | DATEND);
                    Ep0Stage.bStage = EPIDLE;
                }
                else
                {
                    WriteReg(CSR0, IPRDY);
                }
            }
            break;
        case EPDATAOUT:
            if (csr & OPRDY)
            {
                cnt = ReadFifo(FIFO0, Ep0Stage.pData);
                Ep0Stage.wResidue -= cnt;
                Ep0Stage.pData += cnt;
                if (Ep0Stage.wResidue == 0)
                {
                    WriteReg(CSR0, SOPRDY | DATEND);
                    Ep0Stage.bStage = EPIDLE;
                }
                else
                {
                    WriteReg(CSR0, SOPRDY);
                }
            }
            break;
        }
    }

    if (intrin & EP1INIF)
    {
        WriteReg(INDEX, 1);
        csr = ReadReg(INCSR1);
        if (csr & INSTSTL)
        {
            WriteReg(INCSR1, INCLRDT);
        }
        if (csr & INUNDRUN)
        {
            WriteReg(INCSR1, 0);
        }
    }

    if (introut & EP1OUTIF)
    {
        WriteReg(INDEX, 1);
        csr = ReadReg(OUTCSR1);
        if (csr & OUTSTSTL)
        {
            WriteReg(OUTCSR1, OUTCLRDT);
        }
        if (csr & OUTOPRDY)
        {
            ReadFifo(FIFO1, HidOutput);
            WriteReg(OUTCSR1, 0);

            WriteReg(INDEX, 1);
            WriteFifo(FIFO1, HidOutput, 64);
            WriteReg(INCSR1, INIPRDY);
        }
    }
}

__code char DEVICEDESC[18] =
    {
        0x12,       // bLength(18);
        0x01,       // bDescriptorType(Device);
        0x00, 0x02, // bcdUSB(2.00);
        0x00,       // bDeviceClass(0);
        0x00,       // bDeviceSubClass0);
        0x00,       // bDeviceProtocol(0);
        0x40,       // bMaxPacketSize0(64);
        0x54, 0x53, // idVendor(5354);
        0x80, 0x43, // idProduct(4380);
        0x00, 0x01, // bcdDevice(1.00);
        0x01,       // iManufacturer(1);
        0x02,       // iProduct(2);
        0x00,       // iSerialNumber(0);
        0x01,       // bNumConfigurations(1);
};

__code char CONFIGDESC[41] =
    {
        0x09,       // bLength(9);
        0x02,       // bDescriptorType(Configuration);
        0x29, 0x00, // wTotalLength(41);
        0x01,       // bNumInterfaces(1);
        0x01,       // bConfigurationValue(1);
        0x00,       // iConfiguration(0);
        0x80,       // bmAttributes(BUSPower);
        0x32,       // MaxPower(100mA);

        0x09, // bLength(9);
        0x04, // bDescriptorType(Interface);
        0x00, // bInterfaceNumber(0);
        0x00, // bAlternateSetting(0);
        0x02, // bNumEndpoints(2);
        0x03, // bInterfaceClass(HID);
        0x00, // bInterfaceSubClass(0);
        0x00, // bInterfaceProtocol(0);
        0x00, // iInterface(0);

        0x09,       // bLength(9);
        0x21,       // bDescriptorType(HID);
        0x01, 0x01, // bcdHID(1.01);
        0x00,       // bCountryCode(0);
        0x01,       // bNumDescriptors(1);
        0x22,       // bDescriptorType(HID Report);
        0x1b, 0x00, // wDescriptorLength(27);

        0x07,       // bLength(7);
        0x05,       // bDescriptorType(Endpoint);
        0x81,       // bEndpointAddress(EndPoint1 as IN);
        0x03,       // bmAttributes(Interrupt);
        0x40, 0x00, // wMaxPacketSize(64);
        0x01,       // bInterval(10ms);

        0x07,       // bLength(7);
        0x05,       // bDescriptorType(Endpoint);
        0x01,       // bEndpointAddress(EndPoint1 as OUT);
        0x03,       // bmAttributes(Interrupt);
        0x40, 0x00, // wMaxPacketSize(64);
        0x01,       // bInterval(10ms);
};

__code char HIDREPORTDESC[27] =
    {
        0x05, 0x0c, // USAGE_PAGE(Consumer);
        0x09, 0x01, // USAGE(Consumer Control);
        0xa1, 0x01, // COLLECTION(Application);
        0x15, 0x00, //  LOGICAL_MINIMUM(0);
        0x25, 0xff, //  LOGICAL_MAXIMUM(255);
        0x75, 0x08, //  REPORT_SIZE(8);
        0x95, 0x40, //  REPORT_COUNT(64);
        0x09, 0x01, //  USAGE(Consumer Control);
        0xb1, 0x02, //  FEATURE(Data,Variable);
        0x09, 0x01, //  USAGE(Consumer Control);
        0x81, 0x02, //  INPUT(Data,Variable);
        0x09, 0x01, //  USAGE(Consumer Control);
        0x91, 0x02, //  OUTPUT(Data,Variable);
        0xc0,       // END_COLLECTION;
};

__code char LANGIDDESC[4] =
    {
        0x04,
        0x03,
        0x09,
        0x04,
};

__code char MANUFACTDESC[8] =
    {
        0x08,
        0x03,
        'S',
        0,
        'T',
        0,
        'C',
        0,
};

__code char PRODUCTDESC[30] =
    {
        0x1e,
        0x03,
        'S',
        0,
        'T',
        0,
        'C',
        0,
        ' ',
        0,
        'U',
        0,
        'S',
        0,
        'B',
        0,
        ' ',
        0,
        'D',
        0,
        'e',
        0,
        'v',
        0,
        'i',
        0,
        'c',
        0,
        'e',
        0,
};
