/*
file   : encoder-EXTI.cpp
author : cat_li
version: V1.1
date   : 2015/11/10
         2016/04/10 修复输出键值不稳定，加入双中断模式，添加中断读值说明

Copyright 2015 cat_li. All Rights Reserved.

Copyright Notice
No part of this software may be used for any commercial activities by any form or means, without the prior written consent of shentqlf@163.com.

Disclaimer
This specification is preliminary and is subject to change at any time without notice. shentqlf@163.com assumes no responsibility for any errors contained herein.
*/

#include "encoder-EXTI.h"

ENCODER::ENCODER(GPIO* Apin,GPIO* Bpin)
{
	a_pin = Apin;
	b_pin = Bpin;

	b_pin->mode(INPUT_PU);
	a_pin->mode(INPUT_PU);		
}

//                           _______         _______       
//               Pin1 ______|       |_______|       |______ Pin1
// negative <---         _______         _______         __      --> positive
//               Pin2 __|       |_______|       |_______|   Pin2

		//	old	    old     new	    new
		//	pin2	pin1	pin2	pin1	DEC		Result
		//	----	----	----	----	---		------
		//	0		0		0		0		0		无效
		//	0		0		0		1		1		+1	Pin2上升沿触发-正
		//	0		0		1		0		2		-1  Pin1上升沿触发-反
		//	0		0		1		1		3		+2  (仅Pin1中断时，上升沿-正)
		//	0		1		0		0		4		-1	Pin2下降沿触发-反
		//	0		1		0		1		5		无效
		//	0		1		1		0		6		-2  (仅Pin1中断时，上升沿-反)
		//	0		1		1		1		7		+1	Pin1上升沿触发-正
		//	1		0		0		0		8		+1	Pin1下降沿触发-正
		//	1		0		0		1		9		-2  (仅Pin1中断时，下降沿-反)
		//	1		0		1		0		10		无效
		//	1		0		1		1		11		-1	Pin2上升沿触发-反
		//	1		1		0		0		12		+2  (仅Pin1中断时，下降沿-正)
		//	1		1		0		1		13		-1	Pin1下降沿触发-反
		//	1		1		1		0		14		+1  Pin2下降沿触发-正
		//	1		1		1		1		15		no movement

void exProcess(void)
{
	static uint8_t lastCode = 0xff;
// 读Pin值
	uint8_t A = EN.a_pin->read()<<1|EN.b_pin->read();
	lastCode = lastCode<<2 | A;

	switch (lastCode)
	{
#ifdef DUAL_INTERRUPT	// A,B中断
		// 双中断时会跳双，所以屏蔽一组，如果转动2次才输出1次，请去掉屏蔽
	case 1: /*case 7: case 8:*/	case 14:
		EN.KEY_NUM = KEY_B;		//
		break;
	case 2:/* case 4: case 11:*/case 13:
		EN.KEY_NUM = KEY_A;		//
		break;
#else                 // A中断
	case 3:	case 12: 
	case 7: case 8:	  // 反转过程中换成正转会出现该值
		EN.KEY_NUM = KEY_B;		//
		break;
	case 6:	case 9:	
	case 2:	case 13:  // 正转过程中换成反转会出现该值
		EN.KEY_NUM = KEY_A;		//
		break;
#endif
	default:
		EN.KEY_NUM = KEY_NONE;
		break;
	}
	lastCode = A;
}

void ENCODER::begin(void)
{
	EXTIx exA(a_pin,EXTI_Trigger_Rising_Falling);	
	exA.begin();
	exA.attach_interrupt(exProcess);
	exA.interrupt(ENABLE);
#ifdef	DUAL_INTERRUPT	
	EXTIx exB(b_pin,EXTI_Trigger_Rising_Falling);	
	exB.begin();
	exB.attach_interrupt(exProcess);
	exB.interrupt(ENABLE);
#endif
}

uint8_t ENCODER::read(void)
{
	uint8_t _key_temp = KEY_NUM;
	KEY_NUM = KEY_NONE;
	return _key_temp;
}

