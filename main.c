// Copyright 2021 IOsetting <iosetting(at)outlook.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "fw_hal.h"
#include <string.h>
#include "st7032.h"
#include "stc.h"
#include "usb.h"

void sys_init();
void main()
{
    sys_init();
    //GPIO_P3_SetMode(GPIO_Pin_0|GPIO_Pin_1, GPIO_Mode_Input_HIP);
    usb_init();
    EXTI_Global_SetIntState(HAL_State_ON);

    // memset(HidOutput, 0x00, sizeof(HidOutput));
    // HidOutput[0] = 0xaa;
    // HidOutput[1] = 0x55;
    // HidOutput[2] = 0x02;
    // HidOutput[3] = 0x01;
    // HidOutput[4] = 0x02;
    // HidOutput[5] = 0x01;
    // HidOutput[6] = 0x02;
    // HidOutput[7] = CalCheckSum(HidOutput,7);

    // SYS_SetClock();
    //  UART1, baud 115200, baud source Timer1, 1T mode, no interrupt
    // UART1_Config8bitUart(UART1_BaudSource_Timer1, HAL_State_ON, 115200);

    
    // st7032_init(ST7032_I2C_DEFAULT_ADDR);
    // begin(16, 2, 0);
    // setCursor(0, 0);
    // write('A');

    while (1)
    {
    //     UART1_TxString("ms ticks:0x");
    //     // UART1_TxHex(ticks_ms >> 8);
    //     // UART1_TxHex(ticks_ms & 0xFF);
    //     UART1_TxString(" us ticks:0x");
    //     // UART1_TxHex(ticks_us);
    //     UART1_TxString(" clock div:0x");
    //     // UART1_TxHex(__CONF_CLKDIV);
    //     UART1_TxString(" string\r\n");
    //     SYS_Delay(1000);
    // write('A');
    };
}
void sys_init()
{
	P_SW2 |= 0x80;  //��չ�Ĵ���(XFR)����ʹ��

    P3M0 &= ~0x03;
    P3M1 |= 0x03;
    
    IRC48MCR = 0x80;
    while (!(IRC48MCR & 0x01));
    
    USBCLK = 0x00;
    USBCON = 0x90;
}