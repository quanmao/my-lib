/**
  ******************************************************************************
  * @file    filter.h
  * @author  LQM
  * @version V1.0.0
  * @brief   滤波算法（整数运算）
  * @log  
  * @date    2016.4.17   初始版本
  * @date    2016.4.20  增加中间值平均滤波 & 一阶滤波算法
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
  * @brief  移动平均值滤波，又称递推平均滤波法或滑动平均滤波發。
  * 将连续采集的数据放入固定长度为N的队列中，每次采集新数据放入队尾，并丢弃队
  * 首数据（先进先出），将队列中的数据进行平均运算
  * @优点 有效抑制周期性干扰，平滑度高；适用于高频震荡的系统
  * @缺点 灵敏度低，对脉冲性干扰作用差，RAM消耗大
  ******************************************************************************
  */
#ifdef	USE_GLIDEAVERAGE
class Filter_GlideAverage{
public:
	Filter_GlideAverage(uint32_t _n);
	bool _initialize(uint32_t _newData);
	uint32_t _getResult(uint32_t _newData);
private:
	// 用队列保存数据
	uint32_t *data;
	uint32_t head;
	uint32_t tail;
	uint32_t len;	//队列长度
	// 数据和
	uint32_t sum;
};
#endif

/**
  ******************************************************************************
  * @brief  中位值平均滤波法，相当于中位值滤波+算术平均滤波
  * 先连续采集N个数据，去掉最大/最小值。然后计算N-2个输的算数平均值
  * @优点 对偶然出现的脉冲干扰比较有效
  * @缺点 测量速度较慢
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
	uint32_t len;	//队列长度
	// 数据和
	uint32_t sum;

};
#endif 

/**
  ******************************************************************************
  * @brief  一阶滤波算法
  * 公式 Yn = Xn*k+Yn-1(1-K); 
  *  k=滤波系数；X(n)=本次采样值；Y(n-1)=上次滤波输出值；Y(n)=本次滤波输出值。
  *  其核心是用本次采样值与上次滤波输出值进行加权，得到有效滤波值，使得输出
  *  对输入有反馈作用
  *  滤波系数取 0-255;其值决定本次采样结果在滤波结果中占得比重。值越小，越稳
  * 定，但灵敏度越低;值越大，灵敏度越高，但稳定性越差
  * @优点 输出堆输入有反馈作用，占用ram较小
  * @缺点 多次乘除运算，耗时；有时会和实际值有一个固定偏差无法消除
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
	uint16_t	_filteOld;			// 上次滤波结果
	uint8_t		_filteCount;		// 
	uint16_t	_factor;			// 一阶滤波系数
	uint8_t	 	_dirFlag;			// 变化方向
	
	uint8_t	 	_factorMax;			// 系数最大值
	uint8_t		_factorStep;		// 系数加速步进值
	uint16_t	_filteThreshold;	// 加速阀值
	
	inline uint16_t _AdjustFactor(uint16_t _newData);
};
#endif

#endif
