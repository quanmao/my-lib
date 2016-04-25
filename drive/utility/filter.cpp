/**
  ******************************************************************************
  * @file    filter.h
  * @author  LQM
  * @version V1.0.0
  * @brief   �˲��㷨
  * @log  
  * @date    2016.4.17  ��ʼ�汾
	* @date    2016.4.20  �����м�ֵƽ���˲� & һ���˲��㷨
  ******************************************************************************
  */

/* Include -------------------------------------------------------------------*/
#include "filter.h"
#include "stdlib.h"

/**
  ******************************************************************************
  * @brief  �ƶ�ƽ��ֵ�˲������캯��
  * @param  _n �˲�����
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
  * @brief  ��ʼ��������������������
  * @param  _newData ��Ҫ�˲������ݡ�ex adc��ȡֵ
  * @note   
  * @retval bool true - �����Ѿ�����; false - ����δ��������Ҫ����
  ******************************************************************************
  */
bool Filter_GlideAverage::_initialize(uint32_t _newData){
	data[tail] = _newData;
	sum = sum + _newData;
	tail = (tail+1)%len;  // �ƶ�����һ��
	if (tail==0)
	{
		return true;
	}else{
		return false;
	}
}

/**
  ******************************************************************************
  * @brief  ��ȡ�˲����
  * @param  _newData ��Ҫ�˲������ݡ�ex adc��ȡֵ
  * @note   
  * @retval �˲���Ľ��
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
  * @brief  ��λֵƽ��ֵ�˲������캯��
  * @param  _n �˲�����
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
  * @brief  ���ԭʼ���ݣ�ȥ�����/��Сֵ�����
  * @param  _data ��Ҫ�˲������ݡ�ex adc��ȡֵ
  * @note   
  * @retval bool true - �����Ѿ�����; false - ����δ��������Ҫ����
  ******************************************************************************
  */
bool Filter_MiddleAverage::_update(uint32_t _data){
	static uint32_t i = len;

	sum += _data;		// �ۼ����

	if (_data < min)	// ���������Сֵ
	{
		min = _data;
	}else if ( _data > max)
	{
		max = _data;
	}

	if (--i == 0)
	{
		i = len;
		sum = sum -max -min; // ȥ�������Сֵ����������ֵ
		max = 0;
		min = 0xffff;
		return true;
	}
	return false;
}

/**
  ******************************************************************************
  * @brief  ��ȡ�˲����
  * @param  void
  * @note   
  * @retval �˲���Ľ��
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
  * @brief  ���캯�����̶�ϵ����
  * @param  _filteFactor �˲�ϵ��
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
  * @brief  ���캯������̬����ϵ����
  * @param  _threshold ���ٷ�ֵ
  *			_fMAX �˲�ϵ������
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
  * @brief  ��̬�����˲�ϵ��
  * @param  _newData �²ɼ�������
  * @note   �������ݱ仯��������һ�β�һ�£���Ϊ���ݲ������˲�ϵ����0������
  *	�������ݡ�
  *    ���ݳ�����һ������仯���������˲�ϵ������߱��β���ֵȨ��
  *    ͬ���ұ仯�Ͽ죨��ֵ>��ֵ����������˲�ϵ��    
  * @retval ���������ϴ��˲�����ľ���ֵ
  ******************************************************************************
  */
inline uint16_t FILTER_FIRSTORDER::_AdjustFactor(uint16_t _newData){
	uint8_t _tDirFlag = NONE;
	uint16_t _abs = 0;
	// �жϱ仯���򣬼������ֵ
	if (_newData > _filteOld)
	{
		_abs = _newData - _filteOld;
		_tDirFlag = UP;
	}else if (_newData < _filteOld)
	{
		_abs = _filteOld - _newData;
		_tDirFlag = DOWN;
	}
	
	if (_tDirFlag == _dirFlag)    // ���α仯����һ��  
	{
		// ���ڷ�ֵ�����ٵ���ϵ��������������Ӧ
		if (_abs > _filteThreshold)
		{
      //_filteCount = 5;
			_factor += _factorStep;
		}else{      	
        _factor ++;
    }
		// �����ϵ���������ֵ����ʹ�����ֵ
		if (_factor > _factorMax){
			_factor = _factorMax;
		}
	}else{						  // ����������������Ч���˲�ϵ����0�����ϴ��˲����
			_factor = 0;
	}

	_dirFlag = _tDirFlag;		// ���棬�´�ʹ��
	return _abs;
}

/**
  ******************************************************************************
  * @brief  ��ȡһ���˲����
  * @param  _newData �²ɼ�������
  * @note   Ϊ�������������Թ�ʽ������
  *  ���²���ֵ<�ϴ��˲����ʱ��  
  *  �˲����=�ϴ��˲���������ϴ��˲����-�²���ֵ�� ��һ���˲�ϵ����256 
  *  ���²���ֵ>�ϴ��˲����ʱ��  
  *  �˲����=�ϴ��˲���������²���ֵ-�ϴ��˲������ ��һ���˲�ϵ����256
  * @retval �˲����
  ******************************************************************************
  */
uint16_t FILTER_FIRSTORDER::_GetResult(uint16_t _newData){
	volatile uint32_t _result = 0;

	if (_filteThreshold)
	{
		// ����ϵ��
		_newData = _AdjustFactor(_newData);
		// ����仯��
		_result = _newData*_factor;
		_result = _result + 128;	// ��������
		_result = _result >> 8;		// ����256
		// ������
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
		if (_newData > _filteOld)  		// ���β��� > �ϴ��˲����
		{
			_result = (_newData - _filteOld)*_factor;
			_result = _result + 128;	// ��������
			_result = _result >> 8;
			_result = _filteOld + _result;
		}else if (_newData < _filteOld)	// ���β��� < �ϴ��˲����
		{
			_result = ( _filteOld -_newData)*_factor;
			_result = _result + 128;	// ��������
			_result = _result >> 8;
			_result = _filteOld - _result;

		}else{							// ���β��� = �ϴ��˲����
			_result = _filteOld;
		}
	}

	_filteOld = (uint16_t)_result;		// ���棬�´�ʹ��

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
