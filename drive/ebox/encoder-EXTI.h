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

#ifndef __ENCODER_EXIT_H
#define __ENCODER_EXIT_H
#include "ebox.h"

#define KEY_NONE	0
#define KEY_A		1
#define	KEY_B		2

// 默认为单中断，够用了 需要双中断去掉下面的注释
//#define	DUAL_INTERRUPT	1

class ENCODER
{
	public:
		ENCODER(GPIO* Apin,GPIO* Bpin);
		void 		begin(void);
		uint8_t read(void);
		friend  void exProcess(void);
	private:		
	GPIO*   a_pin;
	GPIO*   b_pin; 
//	EXTIx*	pexA;
//	EXTIx* 	pexB;
	volatile uint8_t	KEY_NUM;	
};

extern ENCODER EN;

#endif
