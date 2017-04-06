/**
  ******************************************************************************
  * @file    line_interpolation.c
  * @author  LQM
  * @version V1.0.0
  * @brief   线性插值算法
  * @log  
  * @date    2016.4.28   初始版本
  ******************************************************************************
  */

/* Include -------------------------------------------------------------------*/
#include "line_interpolation.h"
#include "stdlib.h"

#define X 0
#define Y 1

/**
  ******************************************************************************
  * @brief  线性插值算法，构造函数
  * @param  _num 基准点数量
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
  * @brief  指定区间线性差值计算
  * @param  _x 被求点的x坐标； _n 对应区间尾坐标的数组下标
  * @note   
  * @retval 待求的y值
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
  * @brief  全程线性插值计算
  * @param  _x:待求y值坐标点对应的x坐标
  * @note   
  * @retval 待求的y值
  ******************************************************************************
  */
uint16_t LINE::_GetResult(uint16_t _x)
{
	uint8_t i;
	uint16_t _y;

	// 确定坐标区域，如果当前x坐标等于已知标定点x坐标，直接返回标定点Y值
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
  * @brief  添加基准点
  * @param  _x 以知短点x坐标,_y 以知端点y坐标
  * @note   
  * @retval bool ture,添加基准点成功； false,内存空间满，添加基准点失败
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
  * @brief  修改指定基准点坐标值
  * @param  _x修改的端点坐标x; _y 修改的端点坐标y; _n 需要修改的端点，最小为0
  *		最大为index
  * @note   只有已经添加了数据的基准点才可以修改
  * @retval bool ture,添加基准点成功； false,内存空间满，添加基准点失败
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
