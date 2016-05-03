/**
  ******************************************************************************
  * @file    line_interpolation.c
  * @author  LQM
  * @version V1.0.0
  * @brief   ���Բ�ֵ�㷨
  * @log  
  * @date    2016.4.28   ��ʼ�汾
  ******************************************************************************
  */

/* Include -------------------------------------------------------------------*/
#include "line_interpolation.h"
#include "stdlib.h"

#define X 0
#define Y 1

/**
  ******************************************************************************
  * @brief  ���Բ�ֵ�㷨�����캯��
  * @param  _num ��׼������
  * @note   
  * @retval none
  ******************************************************************************
  */
LINE::LINE(uint8_t _num)
{
	base = (uint16_t (*)[2])malloc(sizeof(uint16_t (*)[2]) * _num);
	indexMax = _num < 2 ? 2:(_num-1);
}

/**
  ******************************************************************************
  * @brief  ָ���������Բ�ֵ����
  * @param  _x ������x���ꣻ _n ��Ӧ����β����������±�
  * @note   
  * @retval �����yֵ
  ******************************************************************************
  */
uint16_t LINE::_GetResult_Assign(uint16_t _x,uint8_t _n)
{
	float _tmp;

	if (base[_n-1][Y] < base[_n][Y])
	{
#ifndef USE_ROUND
        // y1 < y2
		// yn = y1+(y2-y1)*(xn-x1)/(x2-x1)
		_tmp = base[_n-1][Y] + ((base[_n][Y] - base[_n-1][Y]) * (_x - base[_n-1][X]))/(base[_n][X] - base[_n-1][X]);
#else
		_tmp = base[_n-1][Y] + ((base[_n][Y] - base[_n-1][Y]) * (_x - base[_n-1][X]) + (base[_n][X] - base[_n-1][X])/2)/(base[_n][X] - base[_n-1][X]);
#endif
	}else {
#ifndef USE_ROUND
		// y1 > y2
		// yn = y1-(y1-y2)*(xn-x1)/(x2-x1)
		_tmp = base[_n-1][Y] - ((base[_n-1][Y] - base[_n][Y]) * (_x - base[_n-1][X]))/(base[_n][X] - base[_n-1][X]);
#else
		_tmp = base[_n-1][Y] - ((base[_n-1][Y] - base[_n][Y]) * (_x - base[_n-1][X]) + (base[_n][X] - base[_n-1][X])/2)/(base[_n][X] - base[_n-1][X]);
#endif
	}
	return (uint16_t)_tmp;
}

/**
  ******************************************************************************
  * @brief  ȫ�����Բ�ֵ����
  * @param  _x:����yֵ������Ӧ��x����
  * @note   
  * @retval �����yֵ
  ******************************************************************************
  */
uint16_t LINE::_GetResult(uint16_t _x)
{
	uint8_t i;
	uint16_t _y;

	// ȷ���������������ǰx���������֪�궨��x���ֱ꣬�ӷ��ر궨��Yֵ
	for (i=0;i<=index;i++)
	{
		if (_x < base[i][X])
		{
			break;
		}else if (_x == base[i][X]){
			return base[i][Y];
		}
	}
	
	i = i>index ? index : i;
	_y = _GetResult_Assign(_x,i);
	return _y;
}

/**
  ******************************************************************************
  * @brief  ��ӻ�׼��
  * @param  _x ��֪�̵�x����,_y ��֪�˵�y����
  * @note   
  * @retval bool ture,��ӻ�׼��ɹ��� false,�ڴ�ռ�������ӻ�׼��ʧ��
  ******************************************************************************
  */
bool LINE::_addBase(uint16_t _x,uint16_t _y){
	if (index > indexMax)
	{
		return false;
	}else{
		base[index][0] = _x;
		base[index++][1] = _y;		
	}
	return true;
}

/**
  ******************************************************************************
  * @brief  �޸�ָ����׼������ֵ
  * @param  _x�޸ĵĶ˵�����x; _y �޸ĵĶ˵�����y; _n ��Ҫ�޸ĵĶ˵㣬��СΪ0
  *		���Ϊindex
  * @note   ֻ���Ѿ���������ݵĻ�׼��ſ����޸�
  * @retval bool ture,��ӻ�׼��ɹ��� false,�ڴ�ռ�������ӻ�׼��ʧ��
  ******************************************************************************
  */
bool LINE::_setBase(uint16_t _x,uint16_t _y,const uint8_t _n)
{
	if ( _n >= index)
	{
		return false;
	}
	
	base[_n][0] = _x;
	base[_n][1] = _y;
	
	return true;
}
