/*
file   : encoder-EXTI.cpp
author : cat_li
version: V1.1
date   : 2015/11/10
         2016/04/10 �޸������ֵ���ȶ�������˫�ж�ģʽ������ж϶�ֵ˵��

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
		//	0		0		0		0		0		��Ч
		//	0		0		0		1		1		+1	Pin2�����ش���-��
		//	0		0		1		0		2		-1  Pin1�����ش���-��
		//	0		0		1		1		3		+2  (��Pin1�ж�ʱ��������-��)
		//	0		1		0		0		4		-1	Pin2�½��ش���-��
		//	0		1		0		1		5		��Ч
		//	0		1		1		0		6		-2  (��Pin1�ж�ʱ��������-��)
		//	0		1		1		1		7		+1	Pin1�����ش���-��
		//	1		0		0		0		8		+1	Pin1�½��ش���-��
		//	1		0		0		1		9		-2  (��Pin1�ж�ʱ���½���-��)
		//	1		0		1		0		10		��Ч
		//	1		0		1		1		11		-1	Pin2�����ش���-��
		//	1		1		0		0		12		+2  (��Pin1�ж�ʱ���½���-��)
		//	1		1		0		1		13		-1	Pin1�½��ش���-��
		//	1		1		1		0		14		+1  Pin2�½��ش���-��
		//	1		1		1		1		15		no movement

void exProcess(void)
{
	static uint8_t lastCode = 0xff;
// ��Pinֵ
	uint8_t A = EN.a_pin->read()<<1|EN.b_pin->read();
	lastCode = lastCode<<2 | A;

	switch (lastCode)
	{
#ifdef DUAL_INTERRUPT	// A,B�ж�
		// ˫�ж�ʱ����˫����������һ�飬���ת��2�β����1�Σ���ȥ������
	case 1: /*case 7: case 8:*/	case 14:
		EN.KEY_NUM = KEY_B;		//
		break;
	case 2:/* case 4: case 11:*/case 13:
		EN.KEY_NUM = KEY_A;		//
		break;
#else                 // A�ж�
	case 3:	case 12: 
	case 7: case 8:	  // ��ת�����л�����ת����ָ�ֵ
		EN.KEY_NUM = KEY_B;		//
		break;
	case 6:	case 9:	
	case 2:	case 13:  // ��ת�����л��ɷ�ת����ָ�ֵ
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

