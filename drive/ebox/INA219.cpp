/******************************************************************************
程 序 名： INA219.cpp　　
编 写 人： 
编写时间： 2017年3月21日
相关平台： STM32F0xx, ebox,cjmcu INA219模块　　
接口说明： I2C接口，供电3-5v。
修改日志：　　
　　NO.1-  初始版本
******************************************************************************/

#include "INA219.h"


/**
 *@brief    通过I2C写寄存器
 *@param    reg 寄存器, value,写入值
 *@retval   NONE
*/
void INA219::WriteRegister (uint8_t reg, uint16_t value)
{
	uint8_t buf[3] = {reg,(value >> 8) & 0xFF,value & 0xFF};	
	_i2c->writeBuf(_addr,buf,3);
}

/**
 *@brief    通过I2C读寄存器
 *@param    reg 寄存器, *value,读取到的值
 *@retval   NONE
*/
void INA219::ReadRegister(uint8_t reg, uint16_t *value)
{
	uint8_t buf[2];
	_i2c->writeChar(_addr,reg);
	//delay(1); // Max 12-bit conversion time is 586us per sample
	delay_ms(1);

	_i2c->readBuf(_addr,buf,2);// (ina219_i2caddr,reg,*value,2);
	*value = buf[0]<<8|buf[1];
}

/**
 *@brief    配置INA219
 *@param    config
 *@retval   NONE
*/
void INA219::config(uint16_t config){
	uint16_t _config = config & 0xffff;
	WriteRegister(REG_CONFIG, _config);
}
/**
	******************************************************************************
	* @brief  ina219 配置
	* @param  range (电压范围)根据需要测量的电压范围选择 0-16 or 0-32
	* @param  gain  (增益，分压电阻电压值范围)结合采样电阻上的压降范围选择40,80,160,320mv
	* @param  bus adc (gnd->bus- 电压adc)默认  3 12bit, 单采样，532us转换时间
	* @param  shunt adc (采样电阻 电压adc)默认  3 12bit, 单采样，532us转换时间
	* @param  mode (模式) 默认 7 ，连续转换
	* @note   
	* @retval none
	******************************************************************************
	*/
void INA219::config(uint16_t range, uint16_t gain, uint16_t bus_adc, uint16_t shunt_adc, uint16_t mode){
	uint16_t _config = 0;
	_config |= (range | gain | bus_adc  | shunt_adc | mode);
	WriteRegister(REG_CONFIG, _config);
}
/**
 *@brief    校准
 *@param    calibration 校准值
 *@param    curLSB 电流分辨率
 *@retval   NONE
*/
void INA219::calibration(int calibration,int curLSB)
{
	_calValue = calibration;
	_currentLSB = curLSB;
	_powerLSB = curLSB*20; //参考Power_LSB_Calc_INA219.PDF

	WriteRegister(REG_CALIBRATION, _calValue);
}

/**
 *@brief    根据要检测的最大电流，采样电阻，自动校准
 *@param    iMax_uA 负载最大电流 uA
 *@param    rShunt_mR 采样电阻 毫欧
 *@retval   1 ok, 0 NG
*/
uint8_t INA219::autoCalibration(int iMax_uA, int rShunt_mR){
	//Adaptation of the calibration formulae as per INA219 datasheet.
	int min_lsb = iMax_uA / 32767;
	int max_lsb = iMax_uA / 4096;
	int current_lsb = (min_lsb & 1) ? min_lsb + 1 : min_lsb + 2;
	int cal = 0;

	if (current_lsb > min_lsb && current_lsb < max_lsb) {
		cal = 40960000 / (current_lsb * rShunt_mR);
		calibration(cal,current_lsb);
		return 1;
	}
	return 0;
}

/**************************************************************************/
/*! 
    @brief  Instantiates a new INA219 class
*/
/**************************************************************************/
INA219::INA219(E_I2c *i2c,uint8_t addr) {
  _i2c = i2c;
  _addr = addr;
}

/**
 *@brief    初始化I2C通讯，并设置采样量程为最大32v2A
 *@retval   NONE
*/
void INA219::begin(void) {

  _i2c->begin(10);
  // Set chip to large range config values to start
  //setCalibration_32V_2A();
}

/**
 *@brief    重新设置I2C,并复位INA219
 *@retval   NONE
*/
void INA219::reset(void){
	_i2c->begin(10);
	WriteRegister(REG_CONFIG,CONFIG_RESET);
}


/**
 *@brief    读取RAW格式BUS电压（int16_t , +- 32767）
 *@retval   电压（+- 32767）
 *@retval   _state 状态，		
*/
int16_t INA219::getBusVoltage_raw() {
	uint16_t value,time = 5000;
	
	// 等待转换结束，并保存状态
	do{
		ReadRegister(REG_BUSVOLTAGE, &value);
		_state = value & 0x03;
	}while (_state != 2 && --time != 0);

	// Shift to the right 3 to drop CNVR and OVF

	return (int16_t)((value >> 3));
}

/**
 *@brief    获取INA219转换状态
 *@retval   -1 溢出, 0 超时, 1 有效
*/
int8_t INA219::getState(void){
	
	getBusVoltage_raw();	
	
	if ((_state & 0x01) == 0x01)	// 溢出
	{
		return OVER;
	}else if (_state == 0x02)		// 转换结束
	{
		return VALID;
	}else 							// 超时
	{
		return TIMEOUT;
	}
}

/**
 *@brief    读取RAW格式Shunt电压（int16_t , +- 32767）
 *@retval   电压（+- 32767）
*/
int16_t INA219::getShuntVoltage_raw() {
  uint16_t value;
  ReadRegister(REG_SHUNTVOLTAGE, &value);
  return (int16_t)value;
}

/**
 *@brief    读取RAW格式电流（int16_t , +- 32767）
 *@retval   电压（+- 32767）
*/
int16_t INA219::getCurrent_raw() {
  uint16_t value;

  // Sometimes a sharp load will reset the INA219, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  WriteRegister(REG_CALIBRATION, _calValue);

  // Now we can safely read the CURRENT register!
  ReadRegister(REG_CURRENT, &value);
  
  return (int16_t)value;
}

/**
 *@brief    读取RAW格式功率（int16_t , +- 32767）
 *@retval   电压（+- 32767）
*/
int16_t INA219::getPower_raw() {
  uint16_t value;

  // Sometimes a sharp load will reset the INA219, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  WriteRegister(REG_CALIBRATION, _calValue);

  // Now we can safely read the CURRENT register!
  ReadRegister(REG_POWER, &value);
  
  return (int16_t)value;
}

/**
 *@brief    读取Shunt电压 uV（int16_t , +- 327670）
 *@retval   电压（+- 327670）
*/
int32_t INA219::getShuntVoltage_uV() {
	
	int32_t value = getShuntVoltage_raw();

	return value*10;
}

/**
 *@brief    读取Bus电压 V
 *@retval   电压 V
*/
float INA219::getBusVoltage_V() {
  int16_t value = getBusVoltage_raw();
  // multiply by LSB LSB is 4mv,reference datasheet
  return value * 0.004;
}

/**
 *@brief    读取电流uA
 *@retval   电流uA
*/
int32_t INA219::getCurrent_uA() {
	int valueDec;
	
	int32_t data = getCurrent_raw();

	valueDec = data* _currentLSB;
	return valueDec;
}

/**
 *@brief    读取功率uW
 *@retval   电流uW
*/
int32_t INA219::getPower_uw(void) {

	int16_t valueDec = getPower_raw();
	valueDec *= _powerLSB;
	return valueDec;
}
