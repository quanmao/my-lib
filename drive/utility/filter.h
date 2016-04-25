/**
  ******************************************************************************
  * @file    filter.h
  * @author  LQM
  * @version V1.0.0
  * @brief   �˲��㷨���������㣩
  * @log  
  * @date    2016.4.17   ��ʼ�汾
  * @date    2016.4.20  �����м�ֵƽ���˲� & һ���˲��㷨
  ******************************************************************************
  */
#ifndef __FILTER_H
#define __FILTER_H
/* Include -------------------------------------------------------------------*/
#include "stdint.h"

/* config -------------------------------------------------------------------*/
#define USE_GLIDEAVERAGE
#define USE_MIDDLEAVERAGE
#define USE_FIRSTORDER

/**
  ******************************************************************************
  * @brief  �ƶ�ƽ��ֵ�˲����ֳƵ���ƽ���˲����򻬶�ƽ���˲��l��
  * �������ɼ������ݷ���̶�����ΪN�Ķ����У�ÿ�βɼ������ݷ����β����������
  * �����ݣ��Ƚ��ȳ������������е����ݽ���ƽ������
  * @�ŵ� ��Ч���������Ը��ţ�ƽ���ȸߣ������ڸ�Ƶ�𵴵�ϵͳ
  * @ȱ�� �����ȵͣ��������Ը������òRAM���Ĵ�
  ******************************************************************************
  */
#ifdef	USE_GLIDEAVERAGE
class Filter_GlideAverage{
public:
	Filter_GlideAverage(uint32_t _n);
	bool _initialize(uint32_t _newData);
	uint32_t _getResult(uint32_t _newData);
private:
	// �ö��б�������
	uint32_t *data;
	uint32_t head;
	uint32_t tail;
	uint32_t len;	//���г���
	// ���ݺ�
	uint32_t sum;
};
#endif

/**
  ******************************************************************************
  * @brief  ��λֵƽ���˲������൱����λֵ�˲�+����ƽ���˲�
  * �������ɼ�N�����ݣ�ȥ�����/��Сֵ��Ȼ�����N-2���������ƽ��ֵ
  * @�ŵ� ��żȻ���ֵ�������űȽ���Ч
  * @ȱ�� �����ٶȽ���
  ******************************************************************************
  */
#ifdef USE_MIDDLEAVERAGE
class Filter_MiddleAverage{
public:
	Filter_MiddleAverage(uint32_t _n);
	bool _update(uint32_t _data);
	uint32_t _getResult(void);
private:
	uint16_t max;
	uint16_t min;
	uint32_t len;	//���г���
	// ���ݺ�
	uint32_t sum;

};
#endif 

/**
  ******************************************************************************
  * @brief  һ���˲��㷨
  * ��ʽ Yn = Xn*k+Yn-1(1-K); 
  *  k=�˲�ϵ����X(n)=���β���ֵ��Y(n-1)=�ϴ��˲����ֵ��Y(n)=�����˲����ֵ��
  *  ��������ñ��β���ֵ���ϴ��˲����ֵ���м�Ȩ���õ���Ч�˲�ֵ��ʹ�����
  *  �������з�������
  *  �˲�ϵ��ȡ 0-255;��ֵ�������β���������˲������ռ�ñ��ء�ֵԽС��Խ��
  * ������������Խ��;ֵԽ��������Խ�ߣ����ȶ���Խ��
  * @�ŵ� ����������з������ã�ռ��ram��С
  * @ȱ�� ��γ˳����㣬��ʱ����ʱ���ʵ��ֵ��һ���̶�ƫ���޷�����
  ******************************************************************************
  */
#ifdef USE_FIRSTORDER

class FILTER_FIRSTORDER{
public:
	FILTER_FIRSTORDER(uint8_t _filteFactor);
	FILTER_FIRSTORDER(uint16_t _threshold,uint8_t _fMAX);
	uint16_t _GetResult(uint16_t _newData);
 	void _SetParameter(uint16_t _threshold,uint8_t _fMAX,uint8_t _fStep);
	uint16_t _getFactor();
private:
	uint16_t	_filteOld;			// �ϴ��˲����
	uint8_t		_filteCount;		// 
	uint16_t	_factor;			// һ���˲�ϵ��
	uint8_t	 	_dirFlag;			// �仯����
	
	uint8_t	 	_factorMax;			// ϵ�����ֵ
	uint8_t		_factorStep;		// ϵ�����ٲ���ֵ
	uint16_t	_filteThreshold;	// ���ٷ�ֵ
	
	inline uint16_t _AdjustFactor(uint16_t _newData);
};
#endif

#endif
