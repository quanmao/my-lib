/******************************************************************************
程 序 名： TM770X.CPP　　
编 写 人： LQM
编写时间： 2017年6月21日
相关平台： STM32F1xx, ebox, TM7707模块　　
接口说明： SPI接口，供电5v。
修改日志： 　　
　　NO.1-  初始版本 基于深圳天威提供的演示程序
******************************************************************************/
#include "TM770X.h"

/* 设置寄存器bit定义 8位 MD1,MD0,G2,G1,G0,BU,BUF,FSYNC  默认0x01 */
enum
{
	MD_NORMAL		= (0 << 6),	/* 正常模式 */
	MD_CAL_SELF		= (1 << 6),	/* 自校准模式 */
	MD_CAL_ZERO		= (2 << 6),	/* 校准0刻度模式 */
	MD_CAL_FULL		= (3 << 6),	/* 校准满刻度模式 */
};
// 以上无需外部访问，所以放在此处

/**
 *@brief    TM770X构造函数
 *@param    Spi *spi,Gpio *DRDY,Gpio *CS,Gpio *RST
 *@retval   NONE
*/
TM770X::TM770X(Spi *spi,type_T tm_id,Gpio *DRDY,Gpio *CS,Gpio *RST){
	_spi = spi;
	_drdy = DRDY;
	_cs = CS;
	_rst = RST;
	_id = tm_id;
}

/**
 *@brief    TM770X启动
 *@param    uint16_t prescaler spi频率，默认256分频,uint8_t d_num 设备号，默认1
 *@retval   NONE
*/
void TM770X::begin(uint16_t prescaler,uint8_t d_num){
	SPI_CONFIG_TYPE tm7702_spi;

	if (_cs != NULL) _cs->mode(OUTPUT_PP_PU);
	if (_drdy != NULL) _drdy->mode(INPUT_PU);
	
	tm7702_spi.bit_order = SPI_BITODER_MSB;
	tm7702_spi.dev_num = d_num;
	tm7702_spi.mode = SPI_MODE3;
	tm7702_spi.prescaler = prescaler;

	_spi->begin(&tm7702_spi);



	_hardRest();
	delay_ms(5);

	if (_id == TM05)
	{
		sapmlingRate(RATE5_200_500HZ);
	}else{
		sapmlingRate(RATE50);
	}
//	_readReg();
	
//	_writeByte(0x20);       //通讯寄存器 //通道 1，下一个写时钟寄存器 自校准 
//  _writeByte(0x00);       //时钟寄存器 //写时钟寄存器设置 2.459Mhz更新速率为20hz
  
  //  _writeByte(0x10);       //通讯寄存器 //通道 1，下一个写设置寄存器 
//  _writeByte(0x46);       //设置寄存器 //自校准,增益 1,双极 ,缓冲 
//  delay_ms(1000);             //延时  
//  _writeByte(0x21);       //通讯寄存器 //通道 2，下一个写时钟寄存器 自校准 
//  _writeByte(0x00);       //时钟寄存器 //写时钟寄存器设置 2.459Mhz更新速率为20hz
//  _writeByte(0x11);       //通讯寄存器 //通道 2，下一个写设置寄存器 
//  _writeByte(0x46);       //设置寄存器 //自校准,增益 1,双极,缓冲 
//  delay_ms(1000);   

// TM7705用下面代码初始化
//	_writeByte(0x20);   //写通讯寄存器的下一步写滤波器高寄存器；DRDY=0，设置寄存器RS2 RS1 RS0=0 1 0，下一步写R/W=0，STBY=0，通道1时CH1 CH0=0 0；
//	_writeByte(0x0A);
// TM7707 用下面代码初始化
//	sapmlingRate(RATE500);
}

/**
 *@brief    TM7707采样率
 *@param    rate_t rate 采样率
 *@retval   NONE
*/
void TM770X::sapmlingRate(rate_t rate){
	uint8_t bst = 0;
	// 增益小于4，bst位设为0
	bst = (_config >>3) > 2 ? 0x20 : 0;

	_writeByte(0x20);   //写通讯寄存器的下一步写滤波器高寄存器；DRDY=0，设置寄存器RS2 RS1 RS0=0 1 0，下一步写R/W=0，STBY=0，通道1时CH1 CH0=0 0；
	_writeByte(rate>>8| bst);
	_writeByte(0x50);	 //写通讯寄存器的下一步写滤波器低寄存器
	_writeByte(rate);
}

/**
 *@brief    TM7707采样率
 *@param    rate_t rate 采样率
 *@retval   NONE
*/
void TM770X::sapmlingRate(rate5_t rate,clock_T clk){
	_writeByte(0x20);   //写通讯寄存器的下一步写滤波器高寄存器；DRDY=0，设置寄存器RS2 RS1 RS0=0 1 0，下一步写R/W=0，STBY=0，通道1时CH1 CH0=0 0；
	_writeByte(clk|rate);
}

/**
 *@brief    读取TM770X的寄存器，并打印输出，不是必须的，调试用
 *@param    NONE
 *@retval   NONE
*/
void TM770X::_readReg()
{
	uart1.printf("/***************************/ \r\n");
	// 读设置寄存器
	_writeByte(0x00 | READ );
	uart1.printf("通讯寄存器默认0x00,实际：%x \n", _readByte());
	_writeByte(0x10 | READ );
	uart1.printf("设置寄存器默认0x01,实际：%x \n", _readByte());
	// 读时钟寄存器
	_writeByte(0x20 | READ );
	uart1.printf("滤波高寄存器,默认0x05,实际：%x \n", _readByte());
//	_writeByte(0x30 | READ );
//	uart1.printf("数据寄存器 ,实际：%x \n", _readByte(3));
//	_writeByte(0x40 | READ );
//	uart1.printf("测试寄存器,实际：%x \n", _readByte());
//	_writeByte(0x50 | READ );
//	uart1.printf("滤波低寄存器,默认0x00,实际：%x \n", _readByte());
	// _writeByte(0x60 | READ );
	uart1.printf("CH1零度24位,实际1f4000：%x \n",readCaliReg(CH_1,REG_ZERO));
//	_writeByte(0x70 | READ );
	uart1.printf("CH1满度24位,实际5761AB：%x \n",readCaliReg(CH_1,REG_FULL));
	_writeByte(0x60 | READ|CH_2 );
	uart1.printf("CH2零度24位,实际1f4000：%x \n",readCaliReg(CH_2,REG_ZERO));
	_writeByte(0x70 | READ|CH_2  );
	uart1.printf("CH2满度24位,实际5761AB：%x \n",_readByte(3));
}

/**
 *@brief    读校准寄存器
 *@param    CH_T ch 要读取的通道 ,CALI_REG reg 要读取的寄存器，REG_ZERO REG_FULL
 *@retval   32位寄存器值
*/
uint32_t TM770X:: readCaliReg(CH_T ch,CALI_REG reg){
	_writeByte(reg | READ | ch );
	return _readByte(3);
}

/**
 *@brief    写校准寄存器
 *@param    CH_T ch 要写入的通道 ；
		   CALI_REG reg 要写入的寄存器，REG_ZERO REG_FULL
		   uint32_t dat 要写入的数据
 *@retval   none
*/
void TM770X:: writeCaliReg(CH_T ch,CALI_REG reg,uint32_t dat){
	_writeByte(reg | WRITE | ch );
	_write3Byte(dat);
}

/**
 *@brief    自校准。温度,电压,增益,缓冲,极型等发生变化是需要重新校准
 *@param    NONE
 *@retval   读取到的字节
*/
void TM770X::calibself(CH_T ch,CONFIG_T gain,uint8_t other){

	_config = gain | other;
	_writeByte(WRITE|REG_SETUP|ch);
	_writeByte(MD_CAL_SELF | _config );

	_waitDRDY();
	read(ch);
}

/**
 *@brief    启动系统校准零位. 请将AIN+ AIN-短接后，执行该函数。校准应该由主程序控制并保存校准参数。
		执行完毕后。可以通过 TM7705_ReadReg(REG_ZERO_CH1) 和  TM7705_ReadReg(REG_ZERO_CH2) 读取校准参数。
 *@param   CH_T ch 选择通道,ex CH_1,CH_2,CH_3,CH_4
 *@retval   none
*/
void TM770X::calibZero(CH_T ch,CONFIG_T gain,uint8_t other){
	_config = gain | other;
	
	_writeByte(WRITE | REG_SETUP | ch);
	_writeByte(MD_CAL_ZERO|gain |other);

	_waitDRDY();
	read(ch);	// 抛弃本次转换结果
}

/**
 *@brief  启动系统校准满位. 请将AIN+ AIN-接最大输入电压源，执行该函数。校准应该由主程序控制并保存校准参数。
		 执行完毕后。可以通过 TM7705_ReadReg(REG_FULL_CH1) 和  TM7705_ReadReg(REG_FULL_CH2) 读取校准参数。
 *@param    CH_T ch 选择通道,ex CH_1,CH_2,CH_3,CH_4
 *@retval   none
*/
void TM770X::calibFull(CH_T ch,CONFIG_T gain,uint8_t other){
	_config = gain | other;
	
	_writeByte(WRITE | REG_SETUP | ch);
	_writeByte(MD_CAL_FULL|gain |other);

	_waitDRDY();
	read(ch);	// 抛弃本次转换结果
}

/**
 *@brief  读取指定通道转换结果
 *@param    CH_T ch 选择通道,ex CH_1,CH_2,CH_3,CH_4
 *@retval   uint32_t 转换结果
*/
uint32_t TM770X::read(CH_T ch){

	// 多读取一遍，切换通道
	_waitDRDY();
	_writeByte(READ | REG_DATA |  ch);
	_readByte(_id);

	_waitDRDY();
	_writeByte(READ | REG_DATA |  ch);

	return _readByte(_id);
}

/*---------------------------------------以下为内部私有函数------------------------------*/
/**
 *@brief    检测是否校准完成或是否准备写入数据
 *@param    uint16_t tOut 超时时间
 *@retval   读取到的字节
*/
void TM770X::_waitDRDY(uint32_t tOut){
	uint8_t t;

	if (_drdy == NULL){
		// 通过寄存器进行忙drdy检测
		do{
			_writeByte(0x00 | READ);
			t = _readByte();
		}while (t & 0x80 && --tOut);
	}else{
		// 通过drdy pin检测
		while (_drdy->read()&& --tOut);
	}

	if (tOut == 0){
		_syncclock();
		calibself(CH_1);
		calibself(CH_2);
		uart1.printf("超时复位 ***********\n");
	}
}

/**
 *@brief    读n个字节
 *@param    uint8_t n 要读取的字节数，默认为1；n= 1-3
 *@retval   读取到的字节
*/
uint32_t TM770X::_readByte(uint8_t n){
	uint32_t r = 0;
	_csSet(0);

	switch (n)
	{
	case 1:
		r = _spi->read();
		break;
	case 2:
//		r = (uint16_t)((uint16_t)_spi->read()<<8) | _spi->read();
		r = _spi->read()<<8;
		r |= _spi->read();
		break;
	case 3:
		r = (_spi->read()<<16) | (_spi->read()<<8) | _spi->read();
//		r = _spi->read()<<16;
//		r |= _spi->read()<<8;
//		r |= _spi->read();
		break;
	default:
		r = _spi->read();
		break;
	}

	_csSet(1);
	return r;
}

/**
 *@brief    写字节
 *@param    uint8_t dat1 要写入的字节
 *@retval   NONE
*/
void TM770X::_writeByte(uint8_t dat1)
{
	_csSet(0);
	_spi->write(dat1);
	_csSet(1);
}

/**
 *@brief    连续写3字节
 *@param    uint32_t dat1 要写入的字节
 *@retval   NONE
*/
void TM770X::_write3Byte(uint32_t dat1)
{
	_csSet(0);
	_spi->write((dat1>>16) & 0xff);
	_spi->write((dat1>>8) & 0xff);
	_spi->write(dat1 & 0xff);
	_csSet(1);
}

/**
 *@brief    TM770X同步spi。在至少32个串行时钟周期向DIN写1，复位串行接口，消除spi迷失
		此处选40个时钟。此操作不该写寄存器，但建议重写寄存器
 *@param    NONE
 *@retval   NONE
*/
void TM770X::_syncclock()
{
	_csSet(0);
	_cs->reset();
	for (uint8_t i =0;i<5;i++){
		_spi->write(0xff);
	}

	_csSet(1);
}

/**
 *@brief    硬件复位
 *@param    none
 *@retval   none
*/
void TM770X::_hardRest(){
	
	if (_rst == NULL){
			_syncclock();
			return;
	} 

	_rst->mode(OUTPUT_PP);

	_rst->set();
	delay_ms(1);
	_rst->reset();
	delay_ms(3);
	_rst->set();
}

/**
 *@brief    片选控制
 *@param    uint8_t set = 0 or 1
 *@retval   none
*/
void TM770X::_csSet(uint8_t set){
	if (_cs == NULL) return;
	set == 0 ? _cs->reset():_cs->set();
}
