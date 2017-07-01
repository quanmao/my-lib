/******************************************************************************
�� �� ���� TM7707.CPP����
�� д �ˣ� LQM
��дʱ�䣺 2017��6��21��
���ƽ̨�� STM32F1xx, ebox, TM7707ģ�顡��
�ӿ�˵���� SPI�ӿڣ�����5v��
�޸���־�� ����
����NO.1-  ��ʼ�汾 �������������ṩ����ʾ����
******************************************************************************/
#include "TM7707.h"

/* ���üĴ���bit���� 8λ MD1,MD0,G2,G1,G0,BU,BUF,FSYNC  Ĭ��0x01 */
enum
{
	MD_NORMAL		= (0 << 6),	/* ����ģʽ */
	MD_CAL_SELF		= (1 << 6),	/* ��У׼ģʽ */
	MD_CAL_ZERO		= (2 << 6),	/* У׼0�̶�ģʽ */
	MD_CAL_FULL		= (3 << 6),	/* У׼���̶�ģʽ */
};
// ���������ⲿ���ʣ����Է��ڴ˴�

/**
 *@brief    TM7707���캯��
 *@param    Spi *spi,Gpio *DRDY,Gpio *CS,Gpio *RST
 *@retval   NONE
*/
TM7707::TM7707(Spi *spi,Gpio *DRDY,Gpio *CS,Gpio *RST){
	_spi = spi;
	_drdy = DRDY;
	_cs = CS;
	_rst = RST;
}

/**
 *@brief    TM7707����
 *@param    uint16_t prescaler spiƵ�ʣ�Ĭ��256��Ƶ,uint8_t d_num �豸�ţ�Ĭ��1
 *@retval   NONE
*/
void TM7707::begin(uint16_t prescaler,uint8_t d_num){
	SPI_CONFIG_TYPE tm7702_spi;

	tm7702_spi.bit_order = SPI_BITODER_MSB;
	tm7702_spi.dev_num = d_num;
	tm7702_spi.mode = SPI_MODE3;
	tm7702_spi.prescaler = prescaler;

	_spi->begin(&tm7702_spi);

	if (_cs != NULL) _cs->mode(OUTPUT_PP_PU);
	if (_drdy != NULL) _drdy->mode(INPUT_PU);

	_hardRest();
	delay_ms(5);
	_readReg();

// TM7705����������ʼ��
//	_writeByte(0x20);   //дͨѶ�Ĵ�������һ��д�˲����߼Ĵ�����DRDY=0�����üĴ���RS2 RS1 RS0=0 1 0����һ��дR/W=0��STBY=0��ͨ��1ʱCH1 CH0=0 0��
//	_writeByte(0x0A);
// TM7707 ����������ʼ��
	sapmlingRate(RATE500);
}

/**
 *@brief    TM7707������
 *@param    rate_t rate ������
 *@retval   NONE
*/
void TM7707::sapmlingRate(rate_t rate){
	uint8_t bst = 0;
	// ����С��4��bstλ��Ϊ0
	bst = (_config >>3) > 2 ? 0x20 : 0;

	_writeByte(0x20);   //дͨѶ�Ĵ�������һ��д�˲����߼Ĵ�����DRDY=0�����üĴ���RS2 RS1 RS0=0 1 0����һ��дR/W=0��STBY=0��ͨ��1ʱCH1 CH0=0 0��
	_writeByte(rate>>8| bst);
	_writeByte(0x50);	 //дͨѶ�Ĵ�������һ��д�˲����ͼĴ���
	_writeByte(rate);
}

/**
 *@brief    ��ȡTM7707�ļĴ���������ӡ��������Ǳ���ģ�������
 *@param    NONE
 *@retval   NONE
*/
void TM7707::_readReg()
{
	uart1.printf("/***************************/ \r\n");
	// �����üĴ���
	_writeByte(0x00 | READ );
	uart1.printf("ͨѶ�Ĵ���Ĭ��0x00,ʵ�ʣ�%x \n", _readByte());
	_writeByte(0x10 | READ );
	uart1.printf("���üĴ���Ĭ��0x01,ʵ�ʣ�%x \n", _readByte());
	// ��ʱ�ӼĴ���
	_writeByte(0x20 | READ );
	uart1.printf("�˲��߼Ĵ���,Ĭ��0x05,ʵ�ʣ�%x \n", _readByte());
//	_writeByte(0x30 | READ );
//	uart1.printf("���ݼĴ��� ,ʵ�ʣ�%x \n", _readByte(3));
//	_writeByte(0x40 | READ );
//	uart1.printf("���ԼĴ���,ʵ�ʣ�%x \n", _readByte());
//	_writeByte(0x50 | READ );
//	uart1.printf("�˲��ͼĴ���,Ĭ��0x00,ʵ�ʣ�%x \n", _readByte());
	// _writeByte(0x60 | READ );
	uart1.printf("CH1���24λ,ʵ��1f4000��%x \n",readCaliReg(CH_1,REG_ZERO));
//	_writeByte(0x70 | READ );
	uart1.printf("CH1����24λ,ʵ��5761AB��%x \n",readCaliReg(CH_1,REG_FULL));
	_writeByte(0x60 | READ|CH_2 );
	uart1.printf("CH2���24λ,ʵ��1f4000��%x \n",readCaliReg(CH_2,REG_ZERO));
	_writeByte(0x70 | READ|CH_2  );
	uart1.printf("CH2����24λ,ʵ��5761AB��%x \n",_readByte(3));
}

/**
 *@brief    ��У׼�Ĵ���
 *@param    CH_T ch Ҫ��ȡ��ͨ�� ,CALI_REG reg Ҫ��ȡ�ļĴ�����REG_ZERO REG_FULL
 *@retval   32λ�Ĵ���ֵ
*/
uint32_t TM7707:: readCaliReg(CH_T ch,CALI_REG reg){
	_writeByte(reg | READ | ch );
	return _readByte(3);
}

/**
 *@brief    дУ׼�Ĵ���
 *@param    CH_T ch Ҫд���ͨ�� ��
		   CALI_REG reg Ҫд��ļĴ�����REG_ZERO REG_FULL
		   uint32_t dat Ҫд�������
 *@retval   none
*/
void TM7707:: writeCaliReg(CH_T ch,CALI_REG reg,uint32_t dat){
	_writeByte(reg | WRITE | ch );
	_write3Byte(dat);
}

/**
 *@brief    ��У׼���¶�,��ѹ,����,����,���͵ȷ����仯����Ҫ����У׼
 *@param    NONE
 *@retval   ��ȡ�����ֽ�
*/
void TM7707::calibself(CH_T ch,CONFIG_T gain,uint8_t other){

	_config = gain | other;
	_writeByte(WRITE|REG_SETUP|ch);
	_writeByte(MD_CAL_SELF |_config );

	_waitDRDY();
	read(ch);
}

/**
 *@brief    ����ϵͳУ׼��λ. �뽫AIN+ AIN-�̽Ӻ�ִ�иú�����У׼Ӧ������������Ʋ�����У׼������
		ִ����Ϻ󡣿���ͨ�� TM7705_ReadReg(REG_ZERO_CH1) ��  TM7705_ReadReg(REG_ZERO_CH2) ��ȡУ׼������
 *@param   CH_T ch ѡ��ͨ��,ex CH_1,CH_2,CH_3,CH_4
 *@retval   none
*/
void TM7707::calibZero(CH_T ch,CONFIG_T gain,uint8_t other){
	_config = gain | other;
	
	_writeByte(WRITE | REG_SETUP | ch);
	_writeByte(MD_CAL_ZERO|gain |other);

	_waitDRDY();
	read(ch);	// ��������ת�����
}

/**
 *@brief  ����ϵͳУ׼��λ. �뽫AIN+ AIN-����������ѹԴ��ִ�иú�����У׼Ӧ������������Ʋ�����У׼������
		 ִ����Ϻ󡣿���ͨ�� TM7705_ReadReg(REG_FULL_CH1) ��  TM7705_ReadReg(REG_FULL_CH2) ��ȡУ׼������
 *@param    CH_T ch ѡ��ͨ��,ex CH_1,CH_2,CH_3,CH_4
 *@retval   none
*/
void TM7707::calibFull(CH_T ch,CONFIG_T gain,uint8_t other){
	_config = gain | other;
	
	_writeByte(WRITE | REG_SETUP | ch);
	_writeByte(MD_CAL_FULL|gain |other);

	_waitDRDY();
	read(ch);	// ��������ת�����
}

/**
 *@brief  ��ȡָ��ͨ��ת�����
 *@param    CH_T ch ѡ��ͨ��,ex CH_1,CH_2,CH_3,CH_4
 *@retval   uint32_t ת�����
*/
uint32_t TM7707::read(CH_T ch){

	// ���ȡһ�飬�л�ͨ��
	_waitDRDY();
	_writeByte(READ | REG_DATA |  ch);
	_readByte(3);
	_waitDRDY();
	_writeByte(READ | REG_DATA |  ch);

	return _readByte(3);
}

/*---------------------------------------����Ϊ�ڲ�˽�к���------------------------------*/
/**
 *@brief    ����Ƿ�У׼��ɻ��Ƿ�׼��д������
 *@param    uint16_t tOut ��ʱʱ��
 *@retval   ��ȡ�����ֽ�
*/
void TM7707::_waitDRDY(uint32_t tOut){
	uint8_t t;

	if (_drdy == NULL){
		// ͨ���Ĵ�������ædrdy���
		do{
			_writeByte(0x00 | READ);
			t = _readByte();
		}while (t & 0x80 && --tOut);
	}else{
		// ͨ��drdy pin���
		while (_drdy->read()&& --tOut);
	}

	if (tOut == 0){
		_syncclock();
		calibself(CH_1);
		calibself(CH_2);
		uart1.printf("��ʱ��λ ***********\n");
	}
}

/**
 *@brief    ��n���ֽ�
 *@param    uint8_t n Ҫ��ȡ���ֽ�����Ĭ��Ϊ1��n= 1-3
 *@retval   ��ȡ�����ֽ�
*/
uint32_t TM7707::_readByte(uint8_t n){
	uint32_t r = 0;
	_csSet(0);

	switch (n)
	{
	case 1:
		r = _spi->read();
		break;
	case 2:
		r = (_spi->read()<<8) | _spi->read();
		break;
	case 3:
		r = (_spi->read()<<16) | (_spi->read()<<8) | _spi->read();
		break;
	default:
		r = _spi->read();
		break;
	}

	_csSet(1);
	return r;
}

/**
 *@brief    д�ֽ�
 *@param    uint8_t dat1 Ҫд����ֽ�
 *@retval   NONE
*/
void TM7707::_writeByte(uint8_t dat1)
{
	_csSet(0);
	_spi->write(dat1);
	_csSet(1);
}

/**
 *@brief    ����д3�ֽ�
 *@param    uint32_t dat1 Ҫд����ֽ�
 *@retval   NONE
*/
void TM7707::_write3Byte(uint32_t dat1)
{
	_csSet(0);
	_spi->write((dat1>>16) & 0xff);
	_spi->write((dat1>>8) & 0xff);
	_spi->write(dat1 & 0xff);
	_csSet(1);
}

/**
 *@brief    TM7707ͬ��spi��������32������ʱ��������DINд1����λ���нӿڣ�����spi��ʧ
		�˴�ѡ40��ʱ�ӡ��˲�������д�Ĵ�������������д�Ĵ���
 *@param    NONE
 *@retval   NONE
*/
void TM7707::_syncclock()
{
	_csSet(0);
	_cs->reset();
	for (uint8_t i =0;i<5;i++){
		_spi->write(0xff);
	}

	_csSet(1);
}

/**
 *@brief    Ӳ����λ
 *@param    none
 *@retval   none
*/
void TM7707::_hardRest(){
	if (_rst == NULL) return;

	_rst->mode(OUTPUT_PP);

	_rst->set();
	delay_ms(1);
	_rst->reset();
	delay_ms(3);
	_rst->set();
}

/**
 *@brief    Ƭѡ����
 *@param    uint8_t set = 0 or 1
 *@retval   none
*/
void TM7707::_csSet(uint8_t set){
	if (_cs == NULL) return;
	set == 0 ? _cs->reset():_cs->set();
}
