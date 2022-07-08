/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#ifndef	__I2C_H
#define	__I2C_H

#include "RegisterDefine.h"
#include <stdint.h>


#define	I2C_Mode_Master			1
#define	I2C_Mode_Slave			0

#define	I2C_P14_P15					(0<<4)
#define	I2C_P24_P25					(1<<4)
#define	I2C_P33_P32					(3<<4)

typedef struct
{
	uint8_t	I2C_Speed;				//总线速度=Fosc/2/(Speed*2+4),      0~63
	uint8_t	I2C_Enable;				//I2C功能使能,   ENABLE, DISABLE
	uint8_t	I2C_Mode;					//主从模式选择,  I2C_Mode_Master,I2C_Mode_Slave
	uint8_t	I2C_MS_Interrupt;	//使能主机模式中断,  ENABLE, DISABLE
	uint8_t	I2C_MS_WDTA;				//主机使能自动发送,  ENABLE, DISABLE

	uint8_t	I2C_SL_ESTAI;			//从机接收START信号中断使能,  ENABLE, DISABLE
	uint8_t	I2C_SL_ERXI;			//从机接收1字节数据中断使能,  ENABLE, DISABLE
	uint8_t	I2C_SL_ETXI;			//从机发送1字节数据中断使能,  ENABLE, DISABLE
	uint8_t	I2C_SL_ESTOI;			//从机接收STOP信号中断使能,  ENABLE, DISABLE

	uint8_t	I2C_SL_ADR;				//从机设备地址,  0~127
	uint8_t	I2C_SL_MA;				//从机设备地址比较使能,  ENABLE, DISABLE

	uint8_t	I2C_IoUse;				//I2C_P14_P15, I2C_P24_P25, I2C_P33_P32
} I2C_InitTypeDef;



void	I2C_Init(I2C_InitTypeDef *I2Cx);
void	WriteNbyte(uint8_t addr, uint8_t *p, uint8_t number);
void	ReadNbyte( uint8_t addr, uint8_t *p, uint8_t number);

#endif

