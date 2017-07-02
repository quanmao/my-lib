/******************************************************************************
程 序 名： TM770X.h　　
编 写 人： LQM
编写时间： 2017年6月21日
相关平台： STM32F1xx, ebox,TM7707模块，基准2.5V,输入电压范围0-5V经电阻
		   分压加到芯片输入端，模块默认为单端输入　　
接口说明： SPI接口，供电5v。
修改日志： 　　
　　NO.1-  初始版本
		NO.2-  可以驱动TM7707,TM7705,修改文件名为TM770x		2017年7月1日
*******************************************************************************
备注：
	1 TM7705可以按正常初始化，读数时按照7707的方式，读取24bit。读数稳定,低8位值为0或0xff
不影响转换结果
	2 TM7707不能按照7705的方式操作.必须读取24bit数据
	3 TM7705如果启用buf，一定要确保供电为5v,否则数据偏差很大
应用要点：
	1 DRDY逻辑不要搞反，高电平时等待转换状态，低电平为可以读取转换数据
	2 一般的应用只要内部校准，这个很简单，只要初始化就可以。如果测量范围
不是芯片的范围，可以通过系统校准来实现，满量程校准电压要大于量程的4/5
	3 校准时要等待一定的时间，读取数据也一样，不能超过转换速率
	4 非缓冲模式，输入前段电阻电容对结果影响很大，因此建议使用缓冲模式
	5 2.4576MHZ是标准频率，如果用4.9152MHZ的时候，(TM7705)要将CLKDIV位置位
	6 数据转换范围：
		单极性0--Vref/Gain,对应0--0xffffff(16777216)@24bit 或0-0xffff(65535)@16bit
		双极性-Vref/Gain--0对应0--0x8000(32768)0-Vref/Gain对应0x8000(32768)--0xffff(65535)
	7 5v时基准应该用2.5v， 3v时基准应该用1.225
	8 5v时如果rst端为3.3v,芯片不能可靠复位
******************************************************************************/
#ifndef __TM770X_H_
#define __TM770X_H_

#include "ebox.h"

/* 通信寄存器bit定义 8位 DRDY,RS2,RS1,RS0,R/W,STBY,CH1,CH0 */
enum
{
	/* 寄存器选择  RS2 RS1 RS0  */
	REG_COMM	= 0x00,	/* 通信寄存器 */
	REG_SETUP	= 0x10,	/* 设置寄存器 */
	REG_FILTERH	= 0x20,	/* 滤波器高寄存器 */
	REG_FILTERL	= 0x50,	/* 滤波器低寄存器 */
	REG_DATA	= 0x30,	/* 数据寄存器 24位只读寄存器*/
	REG_TEST	= 0x40, /* 测试寄存器 24位只读寄存器*/
	/* 读写操作 */
	WRITE 		= 0x00,	/* 写操作 */
	READ 		= 0x08,	/* 读操作 */

	/* 等待模式 */
	STBY		= 0x04, /* 待机控制，1 待机，保持校准和控制字。*/
	NSTBY		= 0x00, /* 待机控制，0 工作模式 */
};

/* 校准寄存器选择  RS2 RS1 RS0  */
typedef enum cali
{
	REG_ZERO	= 0x60, /* 零刻度校准寄存器 24位读写寄存器，默认1F4000 Hex */
	REG_FULL    = 0x70, /* 满刻度校准寄存器 24位读写寄存器，默认5761AB Hex */
}CALI_REG;

/* 通道定义 */
typedef enum CH
{
	CH_1		= 0,	/* AIN1+  AIN1- 校准寄存器对0 */
	CH_2		= 1,	/* AIN2+  AIN2- 校准寄存器对1 */
	CH_3		= 2,	/* AIN1-  AIN1- 校准寄存器对0 */
	CH_4		= 3		/* AIN1-  AIN2- 校准寄存器对3 */
}CH_T;

/* 设置寄存器bit定义 8位 MD1,MD0,G2,G1,G0,BU,BUF,FSYNC  默认0x01 */
typedef enum CONFIG
{
	GAIN_1			= (0 << 3),	/* 增益 */
	GAIN_2			= (1 << 3),	/* 增益 */
	GAIN_4			= (2 << 3),	/* 增益 */
	GAIN_8			= (3 << 3),	/* 增益 */
	GAIN_16			= (4 << 3),	/* 增益 */
	GAIN_32			= (5 << 3),	/* 增益 */
	GAIN_64			= (6 << 3),	/* 增益 */
	GAIN_128		= (7 << 3),	/* 增益 */

	/* 无论双极性还是单极性都不改变任何输入信号的状态，它只改变输出数据的代码和转换函数上的校准点 */
	BIPOLAR			= (0 << 2),	/* 双极性输入 */
	UNIPOLAR		= (1 << 2),	/* 单极性输入 */
	/* 此处换出器指硬件缓冲器，类似与设计跟随,而非数据缓冲 */
	BUF_NO			= (0 << 1),	/* 输入无缓冲（内部缓冲器不启用) */
	BUF_EN			= (1 << 1),	/* 输入有缓冲 (启用内部缓冲器) */

	FSYNC_0			= 0,
	FSYNC_1			= 1		/* 不启用 */
}CONFIG_T;

/* 滤波器高寄存器bit定义 8位 ZERO,ZERO,BST,CLKDIS,FS11,FS10,FS9,FS8 默认0x05 */
/* 滤波器低寄存器bit定义 8位 FS7,FS6,FS5,FS4,FS3,FS2,FS1,FS0 默认0x00 */
enum
{
	CLK_EN	= 0x00,		/* 时钟输出使能 （当外接晶振时，必须使能才能振荡） */
	CLK_DIS	= 0x10,		/* 时钟禁止 （当外部提供时钟时，设置该位可以禁止MCK_OUT引脚输出时钟以省电 */
};

typedef enum rate{
	RATE5  	= (0x0f <<8)|0x00,
	RATE10	= (0x07 <<8)|0x80,
	RATE20 	= (0x03 <<8)|0xc0,
	RATE30 	= (0x02 <<8)|0x80,
	RATE50 	= (0x01 <<8)|0x80,
	RATE60 	= (0x01 <<8)|0x40,
	RATE100 = (0x00 <<8)|0xc0,
	RATE250 = (0x00 <<8)|0x4c,
	RATE500 = (0x00 <<8)|0x26,
	RATE1000 = (0x00 <<8)|0x13,
}rate_t;

// TM7705 采样率，陷波频率
typedef enum rate_5{
	RATE5_20_50HZ		= 0x00,		//截止频率 5.24hz,13.1hz
	RATE5_25_60HZ		= 0x01,		//截止频率 6.55hz,15.7hz
	RATE5_100_250HZ		= 0x02,		//截止频率 16.2hz,65.5hz
	RATE5_200_500HZ		= 0x04,		//截止频率 52.4hz,131hz
}rate5_t;

// TM7705时钟频率
typedef enum clock{
	/*
		2.4576MHz（CLKDIV=0 ）或为 4.9152MHz （CLKDIV=1 ），CLK 应置 “0”。
		1MHz （CLKDIV=0 ）或 2MHz   （CLKDIV=1 ），CLK 该位应置  “1”
	*/
	CLK_4_9152M = 0x08,
	CLK_2_4576M = 0x00,
	CLK_1M 		= 0x04,
	CLK_2M 		= 0x0C,
}clock_T;

typedef enum type
{
	TM05 = 2,
	TM07
}type_T;

class TM770X{
public:
	TM770X(Spi *spi,type_T tm_id,Gpio *DRDY = NULL,Gpio *CS = NULL,Gpio *RST = NULL);

	void begin(uint16_t prescaler = SPI_CLOCK_DIV256,uint8_t d_num = 1);
	/**
	 *@brief    TM7707采样率
	 *@param    rate_t rate 采样率
	 *@retval   NONE
	*/
	void sapmlingRate(rate_t rate = RATE5);
	void sapmlingRate(rate5_t rate,clock_T clk = CLK_4_9152M);
	/**
	 *@brief    自校准。温度,电压,增益,缓冲,极型等发生变化是需要重新校准
	 *@param    NONE
	 *@retval   读取到的字节
	*/
	void calibself(CH_T ch,CONFIG_T gain = GAIN_1,uint8_t other = UNIPOLAR|BUF_EN|FSYNC_0);
	/**
	*@brief    启动系统校准零位. 请将AIN+ AIN-短接后，执行该函数。校准应该由主程序控制并保存校准参数。
		执行完毕后。可以通过 TM7705_ReadReg(REG_ZERO_CH1) 和  TM7705_ReadReg(REG_ZERO_CH2) 读取校准参数。
	*@param   CH_T ch 选择通道,ex CH_1,CH_2,CH_3,CH_4
	*@retval   none
	*/
	void calibZero(CH_T ch,CONFIG_T gain = GAIN_1,uint8_t other = UNIPOLAR|BUF_EN|FSYNC_0);
	/**
	*@brief  启动系统校准满位. 请将AIN+ AIN-接最大输入电压源，执行该函数。校准应该由主程序控制并保存校准参数。
		 执行完毕后。可以通过 TM7705_ReadReg(REG_FULL_CH1) 和  TM7705_ReadReg(REG_FULL_CH2) 读取校准参数。
	*@param    CH_T ch 选择通道,ex CH_1,CH_2,CH_3,CH_4
	*@retval   none
	*/
	void calibFull(CH_T ch,CONFIG_T gain = GAIN_1,uint8_t other = UNIPOLAR|BUF_EN|FSYNC_0);
	/**
	*@brief    读校准寄存器
	*@param    CH_T ch 要读取的通道 ,CALI_REG reg 要读取的寄存器，REG_ZERO REG_FULL
	*@retval   32位寄存器值
	*/
	uint32_t readCaliReg(CH_T ch,CALI_REG reg);
	/**
	*@brief    写校准寄存器
	*@param    CH_T ch 要写入的通道 ；
		   CALI_REG reg 要写入的寄存器，REG_ZERO REG_FULL
		   uint32_t dat 要写入的数据
	*@retval   none
	*/
	void writeCaliReg(CH_T ch,CALI_REG reg,uint32_t dat);

	/**
	*@brief  读取指定通道转换结果
	*@param    CH_T ch 选择通道,ex CH_1,CH_2,CH_3,CH_4
	*@retval   uint32_t 转换结果
	*/
	uint32_t read(CH_T ch);

	// 测试用，通过串口一将寄存器数值输出
	void _readReg();

private:
	void _writeByte(uint8_t dat1);
	void _write3Byte(uint32_t dat1);
	uint32_t _readByte(uint8_t n = 1);
	void _hardRest();
	void _syncclock();
	void _waitDRDY(uint32_t tOut = 13000000);
	void _csSet(uint8_t set);

	Gpio * _cs;
	Gpio * _drdy;
	Gpio * _rst;
	Spi  * _spi;
	
	type_T _id;
	uint8_t _config;
};

#endif


