/******************************************************************************
�� �� ���� TM7707.h����
�� д �ˣ� LQM
��дʱ�䣺 2017��6��21��
���ƽ̨�� STM32F1xx, ebox,TM7707ģ�飬��׼2.5V,�����ѹ��Χ0-5V������
		   ��ѹ�ӵ�оƬ����ˣ�ģ��Ĭ��Ϊ�������롡��
�ӿ�˵���� SPI�ӿڣ�����5v��
�޸���־�� ����
����NO.1-  ��ʼ�汾
*******************************************************************************
��ע��
	1 TM7705���԰�������ʼ��������ʱ����7707�ķ�ʽ����ȡ24bit�������ȶ�
	2 TM7707���ܰ���7705�ķ�ʽ����.�����ȡ24bit����
Ӧ��Ҫ�㣺
	1 DRDY�߼���Ҫ�㷴���ߵ�ƽʱ�ȴ�ת��״̬���͵�ƽΪ���Զ�ȡת������
	2 һ���Ӧ��ֻҪ�ڲ�У׼������ܼ򵥣�ֻҪ��ʼ���Ϳ��ԡ����������Χ
����оƬ�ķ�Χ������ͨ��ϵͳУ׼��ʵ�֣�������У׼��ѹҪ�������̵�4/5
	3 У׼ʱҪ�ȴ�һ����ʱ�䣬��ȡ����Ҳһ�������ܳ���ת������
	4 �ǻ���ģʽ������ǰ�ε�����ݶԽ��Ӱ��ܴ���˽���ʹ�û���ģʽ
	5 2.4576MHZ�Ǳ�׼Ƶ�ʣ������4.9152MHZ��ʱ��(TM7705)Ҫ��CLKDIVλ��λ
	6 ����ת����Χ��
		������0--Vref/Gain,��Ӧ0--0xffffff(16777216)@24bit ��0-0xffff(65535)@16bit
		˫����-Vref/Gain--0��Ӧ0--0x8000(32768)0-Vref/Gain��Ӧ0x8000(32768)--0xffff(65535)
	7 5vʱ��׼Ӧ����2.5v�� 3vʱ��׼Ӧ����1.225
	8 5vʱ���rst��Ϊ3.3v,оƬ���ܿɿ���λ
******************************************************************************/
#ifndef __TM7707_H_
#define __TM7707_H_

#include "ebox.h"

/* ͨ�żĴ���bit���� 8λ DRDY,RS2,RS1,RS0,R/W,STBY,CH1,CH0 */
enum
{
	/* �Ĵ���ѡ��  RS2 RS1 RS0  */
	REG_COMM	= 0x00,	/* ͨ�żĴ��� */
	REG_SETUP	= 0x10,	/* ���üĴ��� */
	REG_FILTERH	= 0x20,	/* �˲����߼Ĵ��� */
	REG_FILTERL	= 0x50,	/* �˲����ͼĴ��� */
	REG_DATA	= 0x30,	/* ���ݼĴ��� 24λֻ���Ĵ���*/
	REG_TEST	= 0x40, /* ���ԼĴ��� 24λֻ���Ĵ���*/
	/* ��д���� */
	WRITE 		= 0x00,	/* д���� */
	READ 		= 0x08,	/* ������ */

	/* �ȴ�ģʽ */
	STBY		= 0x04, /* �������ƣ�1 ����������У׼�Ϳ����֡�*/
	NSTBY		= 0x00, /* �������ƣ�0 ����ģʽ */
};

/* У׼�Ĵ���ѡ��  RS2 RS1 RS0  */
typedef enum cali
{
	REG_ZERO	= 0x60, /* ��̶�У׼�Ĵ��� 24λ��д�Ĵ�����Ĭ��1F4000 Hex */
	REG_FULL    = 0x70, /* ���̶�У׼�Ĵ��� 24λ��д�Ĵ�����Ĭ��5761AB Hex */
}CALI_REG;

/* ͨ������ */
typedef enum CH
{
	CH_1		= 0,	/* AIN1+  AIN1- У׼�Ĵ�����0 */
	CH_2		= 1,	/* AIN2+  AIN2- У׼�Ĵ�����1 */
	CH_3		= 2,	/* AIN1-  AIN1- У׼�Ĵ�����0 */
	CH_4		= 3		/* AIN1-  AIN2- У׼�Ĵ�����3 */
}CH_T;

/* ���üĴ���bit���� 8λ MD1,MD0,G2,G1,G0,BU,BUF,FSYNC  Ĭ��0x01 */
typedef enum CONFIG
{
	GAIN_1			= (0 << 3),	/* ���� */
	GAIN_2			= (1 << 3),	/* ���� */
	GAIN_4			= (2 << 3),	/* ���� */
	GAIN_8			= (3 << 3),	/* ���� */
	GAIN_16			= (4 << 3),	/* ���� */
	GAIN_32			= (5 << 3),	/* ���� */
	GAIN_64			= (6 << 3),	/* ���� */
	GAIN_128		= (7 << 3),	/* ���� */

	/* ����˫���Ի��ǵ����Զ����ı��κ������źŵ�״̬����ֻ�ı�������ݵĴ����ת�������ϵ�У׼�� */
	BIPOLAR			= (0 << 2),	/* ˫�������� */
	UNIPOLAR		= (1 << 2),	/* ���������� */
	/* �˴�������ָӲ������������������Ƹ���,�������ݻ��� */
	BUF_NO			= (0 << 1),	/* �����޻��壨�ڲ�������������) */
	BUF_EN			= (1 << 1),	/* �����л��� (�����ڲ�������) */

	FSYNC_0			= 0,
	FSYNC_1			= 1		/* ������ */
}CONFIG_T;

/* �˲����߼Ĵ���bit���� 8λ ZERO,ZERO,BST,CLKDIS,FS11,FS10,FS9,FS8 Ĭ��0x05 */
/* �˲����ͼĴ���bit���� 8λ FS7,FS6,FS5,FS4,FS3,FS2,FS1,FS0 Ĭ��0x00 */
enum
{
	CLK_EN	= 0x00,		/* ʱ�����ʹ�� ������Ӿ���ʱ������ʹ�ܲ����񵴣� */
	CLK_DIS	= 0x10,		/* ʱ�ӽ�ֹ �����ⲿ�ṩʱ��ʱ�����ø�λ���Խ�ֹMCK_OUT�������ʱ����ʡ�� */
};

typedef enum rate{
	RATE5  	= (0x0f <<8)|0x00,
	RATE10	= (0x07 <<8)|0x80,
	RATE20 	= (0x03 <<8)|0xc0,
	RATE30 	= (0x02 <<8)|0x80,
	RATE50 	= (0x01 <<8)|0x80,
	RATE60 	= (0x01 <<8)|0x40,
	RATE100 = (0x00 <<8)|0xc0,
	RATE250 = (0x00 <<8)|0x4c,
	RATE500 = (0x00 <<8)|0x26,
	RATE1000 = (0x00 <<8)|0x13,
}rate_t;

typedef enum type
{
	TM05 = 2,
	TM07
}type_T;

class TM7707{
public:
	TM7707(Spi *spi,Gpio *DRDY = NULL,Gpio *CS = NULL,Gpio *RST = NULL);

	void begin(uint16_t prescaler = SPI_CLOCK_DIV256,uint8_t d_num = 1);
	/**
	 *@brief    TM7707������
	 *@param    rate_t rate ������
	 *@retval   NONE
	*/
	void sapmlingRate(rate_t rate = RATE5);
	/**
	 *@brief    ��У׼���¶�,��ѹ,����,����,���͵ȷ����仯����Ҫ����У׼
	 *@param    NONE
	 *@retval   ��ȡ�����ֽ�
	*/
	void calibself(CH_T ch,CONFIG_T gain = GAIN_1,uint8_t other = UNIPOLAR|BUF_EN|FSYNC_0);
	/**
	*@brief    ����ϵͳУ׼��λ. �뽫AIN+ AIN-�̽Ӻ�ִ�иú�����У׼Ӧ������������Ʋ�����У׼������
		ִ����Ϻ󡣿���ͨ�� TM7705_ReadReg(REG_ZERO_CH1) ��  TM7705_ReadReg(REG_ZERO_CH2) ��ȡУ׼������
	*@param   CH_T ch ѡ��ͨ��,ex CH_1,CH_2,CH_3,CH_4
	*@retval   none
	*/
	void calibZero(CH_T ch,CONFIG_T gain = GAIN_1,uint8_t other = UNIPOLAR|BUF_EN|FSYNC_0);
	/**
	*@brief  ����ϵͳУ׼��λ. �뽫AIN+ AIN-����������ѹԴ��ִ�иú�����У׼Ӧ������������Ʋ�����У׼������
		 ִ����Ϻ󡣿���ͨ�� TM7705_ReadReg(REG_FULL_CH1) ��  TM7705_ReadReg(REG_FULL_CH2) ��ȡУ׼������
	*@param    CH_T ch ѡ��ͨ��,ex CH_1,CH_2,CH_3,CH_4
	*@retval   none
	*/
	void calibFull(CH_T ch,CONFIG_T gain = GAIN_1,uint8_t other = UNIPOLAR|BUF_EN|FSYNC_0);
	/**
	*@brief    ��У׼�Ĵ���
	*@param    CH_T ch Ҫ��ȡ��ͨ�� ,CALI_REG reg Ҫ��ȡ�ļĴ�����REG_ZERO REG_FULL
	*@retval   32λ�Ĵ���ֵ
	*/
	uint32_t readCaliReg(CH_T ch,CALI_REG reg);
	/**
	*@brief    дУ׼�Ĵ���
	*@param    CH_T ch Ҫд���ͨ�� ��
		   CALI_REG reg Ҫд��ļĴ�����REG_ZERO REG_FULL
		   uint32_t dat Ҫд�������
	*@retval   none
	*/
	void writeCaliReg(CH_T ch,CALI_REG reg,uint32_t dat);

	/**
	*@brief  ��ȡָ��ͨ��ת�����
	*@param    CH_T ch ѡ��ͨ��,ex CH_1,CH_2,CH_3,CH_4
	*@retval   uint32_t ת�����
	*/
	uint32_t read(CH_T ch);

	// �����ã�ͨ������һ���Ĵ�����ֵ���
	void _readReg();

private:
	void _writeByte(uint8_t dat1);
	void _write3Byte(uint32_t dat1);
	uint32_t _readByte(uint8_t n = 1);
	void _hardRest();
	void _syncclock();
	void _waitDRDY(uint32_t tOut = 8000000);
	void _csSet(uint8_t set);

	Gpio * _cs;
	Gpio * _drdy;
	Gpio * _rst;
	Spi  * _spi;

	uint8_t _config;
};

#endif


