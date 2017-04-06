/**
  ******************************************************************************
  * @file    filter.h
  * @author  LQM
  * @version V1.0.0
  * @brief   滤波算法
  * @log  
  * @date    2016.4.17  初始版本
	* @date    2016.4.20  增加中间值平均滤波 & 一阶滤波算法
  ******************************************************************************
  */

/* Include -------------------------------------------------------------------*/
#include "filter.h"
#include "stdlib.h"

/**
  ******************************************************************************
  * @brief  移动平均值滤波，构造函数
  * @param  _n 滤波次数
  * @note   
  * @retval none
  ******************************************************************************
  */
#ifdef	USE_GLIDEAVERAGE
Filter_GlideAverage::Filter_GlideAverage(uint32_t _n){
	data = (uint32_t*)malloc(sizeof(uint32_t)*_n);
	sum = 0;
	head = 0;
	tail = 0;
	len = _n;
}

/**
  ******************************************************************************
  * @brief  初始化，将数据区填满数据
  * @param  _newData 需要滤波的数据。ex adc读取值
  * @note   
  * @retval bool true - 数据已经填满; false - 数据未填满，需要继续
  ******************************************************************************
  */
bool Filter_GlideAverage::_initialize(uint32_t _newData){
	data[tail] = _newData;
	sum = sum + _newData;
	tail = (tail+1)%len;  // 移动到下一个
	if (tail==0)
	{
		return true;
	}else{
		return false;
	}
}

/**
  ******************************************************************************
  * @brief  获取滤波结果
  * @param  _newData 需要滤波的数据。ex adc读取值
  * @note   
  * @retval 滤波后的结果
  ******************************************************************************
  */
uint32_t Filter_GlideAverage::_getResult(uint32_t _newData){
	sum = sum - data[head] + _newData;
	head = (head+1)%len;
	data[tail] = _newData;
	tail = (tail+1)%len;
	return (sum+len/2)/len;
}
#endif

/**
  ******************************************************************************
  * @brief  中位值平均值滤波，构造函数
  * @param  _n 滤波次数
  * @note   
  * @retval none
  ******************************************************************************
  */
#ifdef USE_MIDDLEAVERAGE
Filter_MiddleAverage::Filter_MiddleAverage(uint32_t _n){
	max = 0;
	min = 0xffff;
	sum = 0;
	len = _n;
}

/**
  ******************************************************************************
  * @brief  填充原始数据，去掉最大/最小值后求和
  * @param  _data 需要滤波的数据。ex adc读取值
  * @note   
  * @retval bool true - 数据已经填满; false - 数据未填满，需要继续
  ******************************************************************************
  */
bool Filter_MiddleAverage::_update(uint32_t _data){
	static uint32_t i = len;

	sum += _data;		// 累加求和

	if (_data < min)	// 更新最大，最小值
	{
		min = _data;
	}else if ( _data > max)
	{
		max = _data;
	}

	if (--i == 0)
	{
		i = len;
		sum = sum -max -min; // 去除最大最小值，仅保留中值
		max = 0;
		min = 0xffff;
		return true;
	}
	return false;
}

/**
  ******************************************************************************
  * @brief  获取滤波结果
  * @param  void
  * @note   
  * @retval 滤波后的结果
  ******************************************************************************
  */
uint32_t Filter_MiddleAverage::_getResult(void){
	uint32_t _t;
	_t = (sum/(len-2));
	sum = 0;
	return _t;
}
#endif

/**
  ******************************************************************************
  * @brief  构造函数（固定系数）
  * @param  _filteFactor 滤波系数
  * @note   
  * @retval none
  ******************************************************************************
  */
#ifdef USE_FIRSTORDER
FILTER_FIRSTORDER::FILTER_FIRSTORDER(uint8_t _filteFactor){
	_factor = _filteFactor;
	_filteOld = 0;
	_filteThreshold = 0;
}

#define UP 		1
#define DOWN 	2
#define NONE	0
/**
  ******************************************************************************
  * @brief  构造函数（动态调整系数）
  * @param  _threshold 加速阀值
  *			_fMAX 滤波系数上限
  * @note      
  * @retval NONE
  ******************************************************************************
  */
FILTER_FIRSTORDER::FILTER_FIRSTORDER(uint16_t _threshold,uint8_t _fMAX){
	_dirFlag = NONE;
	_factor = 0;
	_factorMax = _fMAX;
	_factorStep = 20;
	_filteThreshold = _threshold;
}

/**
  ******************************************************************************
  * @brief  动态调整滤波系数
  * @param  _newData 新采集的数据
  * @note   本次数据变化方向与上一次不一致，认为数据波动，滤波系数清0，放弃
  *	本次数据。
  *    数据持续向一个方向变化，逐渐增大滤波系数，提高本次采样值权重
  *    同向且变化较快（差值>阀值）加速提高滤波系数    
  * @retval 新数据与上次滤波结果的绝对值
  ******************************************************************************
  */
inline uint16_t FILTER_FIRSTORDER::_AdjustFactor(uint16_t _newData){
	uint8_t _tDirFlag = NONE;
	uint16_t _abs = 0;
	// 判断变化方向，计算绝对值
	if (_newData > _filteOld)
	{
		_abs = _newData - _filteOld;
		_tDirFlag = UP;
	}else if (_newData < _filteOld)
	{
		_abs = _filteOld - _newData;
		_tDirFlag = DOWN;
	}
	
	if (_tDirFlag == _dirFlag)    // 两次变化方向一致  
	{
		// 大于阀值，加速调整系数，改善数据响应
		if (_abs > _filteThreshold)
		{
      //_filteCount = 5;
			_factor += _factorStep;
		}else{      	
        _factor ++;
    }
		// 如果是系数大于最大值，则使用最大值
		if (_factor > _factorMax){
			_factor = _factorMax;
		}
	}else{						  // 波动，本次数据无效。滤波系数置0采用上次滤波结果
			_factor = 0;
	}

	_dirFlag = _tDirFlag;		// 保存，下次使用
	return _abs;
}

/**
  ******************************************************************************
  * @brief  获取一阶滤波结果
  * @param  _newData 新采集的数据
  * @note   为减少运算量，对公式做变行
  *  当新采样值<上次滤波结果时：  
  *  滤波结果=上次滤波结果－（上次滤波结果-新采样值） ×一阶滤波系数÷256 
  *  当新采样值>上次滤波结果时：  
  *  滤波结果=上次滤波结果＋（新采样值-上次滤波结果） ×一阶滤波系数÷256
  * @retval 滤波结果
  ******************************************************************************
  */
uint16_t FILTER_FIRSTORDER::_GetResult(uint16_t _newData){
	volatile uint32_t _result = 0;

	if (_filteThreshold)
	{
		// 调整系数
		_newData = _AdjustFactor(_newData);
		// 计算变化量
		_result = _newData*_factor;
		_result = _result + 128;	// 四舍五入
		_result = _result >> 8;		// 除以256
		// 计算结果
		switch (_dirFlag)
		{
		case UP:
			_result = _filteOld + _result;
			break;
		case DOWN:
			_result = _filteOld - _result;
			break;
		default:
			_result = _filteOld;
			break;
		}
	}else	{
		if (_newData > _filteOld)  		// 本次采样 > 上次滤波结果
		{
			_result = (_newData - _filteOld)*_factor;
			_result = _result + 128;	// 四舍五入
			_result = _result >> 8;
			_result = _filteOld + _result;
		}else if (_newData < _filteOld)	// 本次采样 < 上次滤波结果
		{
			_result = ( _filteOld -_newData)*_factor;
			_result = _result + 128;	// 四舍五入
			_result = _result >> 8;
			_result = _filteOld - _result;

		}else{							// 本次采样 = 上次滤波结果
			_result = _filteOld;
		}
	}

	_filteOld = (uint16_t)_result;		// 保存，下次使用

	return _filteOld;
}

void FILTER_FIRSTORDER::_SetParameter(uint16_t _threshold,uint8_t _fMAX,uint8_t _fStep)
{
    _factorMax = _fMAX;
    _factorStep = _fStep;
    _filteThreshold = _threshold;
}

uint16_t FILTER_FIRSTORDER::_getFactor(){
	return _factor;
}
#endif
