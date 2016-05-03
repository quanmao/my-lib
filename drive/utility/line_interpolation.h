/**
  ******************************************************************************
  * @file    line_interpolation.h
  * @author  LQM
  * @version V1.0.0
  * @brief   分段线性插值算法。
  *	1 将一条曲线分成若干首尾相连的线段，并确定每个端点（标定点）坐标（x,y;保
  *   存在二维数组中） 
  * 2 所有标定点的x坐标按增序排列：x1<x2<x3<.....  
  * 3 已知该曲线上一点的x坐标Xn 求该点的Y坐标Yn(近似)
  * @log  
  * @date    2016.4.28   初始版本
  ******************************************************************************
  */
#ifndef __LINE_H
#define __LINE_H
/* Include -------------------------------------------------------------------*/
#include "stdint.h"

/* config -------------------------------------------------------------------*/

//#define USE_ROUND	//计算过程进行四舍五入

/* example ---------------------------
1 创建对象
  LINE linetest(n);  其中n为需要保存的标定点数，最小为2
2 添加基准点
  linetest._addBase(x,y);  x,y为标定点的坐标。返回值为bool型，false说明标定点
						  存储空间已满，请确认是否需要修正n值
3 根据已知点的x坐标求该点的y坐标
  linetest._getResult(x);  
----------------------------------------*/

class LINE{
public:
	LINE(uint8_t _num);	
	// 根据x获取对应y点
	uint16_t _GetResult(uint16_t _x);
	// 根据x以及其所在的直线段（_n为线段尾下标）获取对应y点
	uint16_t _GetResult_Assign(uint16_t _x,uint8_t _n);
	// 添加修改标定点
	bool _addBase(uint16_t _x,uint16_t _y);
	bool _setBase(uint16_t _x,uint16_t _y, const uint8_t _n);
private:
	uint16_t  (* base)[2];	// 二维数组，用来保存已知标定坐标点
	uint8_t	  index;		// 数组下标，用来保存目前有多少组标定点
	uint8_t	  indexMax;		// 最多可存放的标定点数量，为二维数组长度
};

#endif
