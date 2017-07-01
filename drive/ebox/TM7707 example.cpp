/**
 ******************************************************************************
 * @file   : *.cpp
 * @author : shentq
 * @version: V1.2
 * @date   : 2016/08/14
 *
 * @brief   ebox application example .
 *
 * Copyright 2016 shentq. All Rights Reserved.
 ******************************************************************************
 */

//STM32 RUN IN eBox
#include "ebox.h"
#include "TM7707.h"

// DIN  PA7(MOSI)
// DOUT PA6(MISO)
// CLK  PA5(SCLK)
// DRDY PA1(�͵�ƽ���Զ�ȡ����,�ߵ�ƽ����ת��)
// CS   PB1(Ƭѡ)
// RST  PB2(�͵�ƽ��λ)

TM7707 tm7707(&spi1,&PA1,&PB1,&PB2);
//TM7707 tm7707(&spi1,NULL,&PB1);


void uart_init(){
	uart1.begin(115200);
	uart2.begin(115200);
	uart3.begin(115200);
	uart4.begin(115200);	//��Ӧdata2/TX,data3/Rx; PC10,PC11
	uart5.begin(115200);  //��Ӧclk/tx,cmd/rx;PC12,PD2
}

void uart_test(){
	uart1.printf("uart1 test\n");
	uart2.printf("uart2 test\n");
	uart3.printf("uart3 test\n");
	uart4.printf("uart4 test\n");
	uart5.printf("uart5 test\n");
}


void setup()
{
	ebox_init();
	uart_init();
	
	tm7707.begin();	

}

int main(void)
{
	uint32_t uiADCValue;
	float uiVoltage;
	setup();
	uart_test();
	
	tm7707.calibself(CH_1);
	tm7707.calibself(CH_2);
	uart1.printf("\r\n ��У׼����");
	tm7707._readReg();
//	tm7707.calibZero(CH_1);
//	tm7707.calibZero(CH_2);
//	tm7707.calibFull(CH_1);
//	uart1.printf("\r\n 0��׼����");
//	tm7707._readReg();

	while (1)
	{

		uiADCValue = tm7707.read(CH_1);
		uiVoltage =  (2.5*uiADCValue/16777216)*2;
	  //uiVoltage = (2.5*uiADCValue/(65536))*2;
		uart1.printf("CH1:%6x, Voltage:%1.9f | ", uiADCValue, uiVoltage);
		

		uiADCValue = tm7707.read(CH_2);
		uiVoltage = 5.0*uiADCValue/16777216;
		//uiVoltage = (2.5*uiADCValue/65536)*2;
		uart1.printf("CH4:%6x, Voltage:%1.9f \r\n", uiADCValue, uiVoltage);
		delay_ms(5000);
	}
}
