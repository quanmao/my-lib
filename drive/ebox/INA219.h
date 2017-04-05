/******************************************************************************
�� �� ���� INA219.h����
�� д �ˣ� 
��дʱ�䣺 2017��3��21��
���ƽ̨�� STM32F0xx, ebox,cjmcu INA219ģ�顡��
�ӿ�˵���� I2C�ӿڣ�����3-5v��
�޸���־������
����NO.1-  2017��3��21�� ��ֲ��eboxƽ̨
******************************************************************************/
#ifndef __INA219_H
#define __INA219_H

#include "ebox.h"

/**************************************************************************/
/*! 
  �������������Ϊ������INA219��ز�����
  ����������
  // VBUS_MAX = 32V             (Assumes 32V, can also be set to 16V)
  // VSHUNT_MAX = 0.32          (Assumes Gain 8, 320mV, can also be 160mv, 80mv, 40mv)
  // RSHUNT = 0.1               (Resistor value in ohms)
  
  // 1. Determine max possible current  ȷ�����ɲ����
  // MaxPossible_I = VSHUNT_MAX / RSHUNT
  // MaxPossible_I = 3.2A
  
  // 2. Determine max expected current  ���Ԥ�ڵ���
  // MaxExpected_I = 2.0A
  
  // 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit����ӦPGA:1/8,1/4,1/2,1) ������ܵĲ�������
  // MinimumLSB = MaxExpected_I/32767
  // MinimumLSB = 0.000061              (61uA per bit)
  // MaximumLSB = MaxExpected_I/4096
  // MaximumLSB = 0,000488              (488uA per bit)
  
  // 4. Choose an LSB between the min and max values ѡ��������ȣ�����ѡ������
  //    (Preferrably a roundish number close to MinLSB)
  // CurrentLSB = 0.0001 (100uA per bit)  ������������
  
  // 5. Compute the calibration register ����У׼�Ĵ������У׼ֵ
  // Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
  // Cal = 4096 (0x1000)
  
  // 6. Calculate the power LSB ���㹦�ʾ���
  // PowerLSB = 20 * CurrentLSB
  // PowerLSB = 0.002 (2mW per bit)
  
  // 7. Compute the maximum current and shunt voltage values before overflow �����������ͷ�����ѹ
  //
  // Max_Current = Current_LSB * 32767
  // Max_Current = 3.2767A before overflow
  //
  // If Max_Current > Max_Possible_I then
  //    Max_Current_Before_Overflow = MaxPossible_I
  // Else
  //    Max_Current_Before_Overflow = Max_Current
  // End If
  //
  // Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
  // Max_ShuntVoltage = 0.32V
  //
  // If Max_ShuntVoltage >= VSHUNT_MAX
  //    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
  // Else
  //    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
  // End If
  
  // 8. Compute the Maximum Power ���������
  // MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
  // MaximumPower = 3.2 * 32V
  // MaximumPower = 102.4W
 
  // 9. Set Calibration register to 'Cal' calculated above	 ����У׼�Ĵ���
  WriteRegister(REG_CALIBRATION, _calValue);
  
  // 10.Set Config register to take into account the settings above �������üĴ���
  uint16_t config = CONFIG_BVOLTAGERANGE_32V |
                    CONFIG_GAIN_8_320MV |
                    CONFIG_BADCRES_12BIT |
                    CONFIG_SADCRES_12BIT_1S_532US |
                    CONFIG_MODE_SANDBVOLT_CONTINUOUS;
  WriteRegister(REG_CONFIG, config);
}
*/
/**************************************************************************/
/*=========================================================================
    I2C Address Options
    -----------------------------------------------------------------------*/
	#define INA219_ADDRESS_0               (uint8_t)(0x40 << 1)     // A0 = GND, A1 = GND
	#define INA219_ADDRESS_1               (uint8_t)(0x41 << 1)     // A0 = VS+, A1 = GND
	#define INA219_ADDRESS_2               (uint8_t)(0x42 << 1)     // A0 = SDA, A1 = GND
	#define INA219_ADDRESS_3               (uint8_t)(0x43 << 1)     // A0 = SCL, A1 = GND
	#define INA219_ADDRESS_4               (uint8_t)(0x44 << 1)     // A0 = GND, A1 = VS+
	#define INA219_ADDRESS_5               (uint8_t)(0x45 << 1)     // A0 = VS+, A1 = VS+
	#define INA219_ADDRESS_6               (uint8_t)(0x46 << 1)     // A0 = SDA, A1 = VS+
	#define INA219_ADDRESS_7               (uint8_t)(0x47 << 1)     // A0 = SCL, A1 = VS+
	#define INA219_ADDRESS_8               (uint8_t)(0x48 << 1)     // A0 = GND, A1 = SDA
	#define INA219_ADDRESS_9               (uint8_t)(0x49 << 1)     // A0 = VS+, A1 = SDA
	#define INA219_ADDRESS_A               (uint8_t)(0x4A << 1)     // A0 = SDA, A1 = SDA
	#define INA219_ADDRESS_B               (uint8_t)(0x4B << 1)     // A0 = SCL, A1 = SDA
	#define INA219_ADDRESS_C               (uint8_t)(0x4C << 1)     // A0 = GND, A1 = SCL
	#define INA219_ADDRESS_D               (uint8_t)(0x4D << 1)     // A0 = VS+, A1 = SCL
	#define INA219_ADDRESS_E               (uint8_t)(0x4E << 1)     // A0 = SDA, A1 = SCL
	#define INA219_ADDRESS_F               (uint8_t)(0x4F << 1)     // A0 = SCL, A1 = SCL
	#define INA219_I2C_ADDRESS                      INA219_ADDRESS_0	

/*=========================================================================
    CONFIG REGISTER (R/W)
    -----------------------------------------------------------------------*/
    #define REG_CONFIG                      (0x00)
    /*---------------------------------------------------------------------*/
    #define CONFIG_RESET                    (0x8000)  // Reset Bit
	
    #define CONFIG_BVOLTAGERANGE_MASK       (0x2000)  // Bus Voltage Range Mask
    #define CONFIG_BVOLTAGERANGE_16V        (0x0000)  // 0-16V Range
    #define CONFIG_BVOLTAGERANGE_32V        (0x2000)  // 0-32V Range
	
    #define CONFIG_GAIN_MASK                (0x1800)  // Gain Mask
    #define CONFIG_GAIN_1_40MV              (0x0000)  // Gain 1, 40mV Range
    #define CONFIG_GAIN_2_80MV              (0x0800)  // Gain 2, 80mV Range
    #define CONFIG_GAIN_4_160MV             (0x1000)  // Gain 4, 160mV Range
    #define CONFIG_GAIN_8_320MV             (0x1800)  // Gain 8, 320mV Range
	
    #define CONFIG_BADCRES_MASK             (0x0780)  // Bus ADC Resolution Mask
    #define CONFIG_BADCRES_9BIT             (0x0080)  // 9-bit bus res = 0..511
    #define CONFIG_BADCRES_10BIT            (0x0100)  // 10-bit bus res = 0..1023
    #define CONFIG_BADCRES_11BIT            (0x0200)  // 11-bit bus res = 0..2047
    #define CONFIG_BADCRES_12BIT            (0x0400)  // 12-bit bus res = 0..4097
    
    #define CONFIG_SADCRES_MASK             (0x0078)  // Shunt ADC Resolution and Averaging Mask
    #define CONFIG_SADCRES_9BIT_1S_84US     (0x0000)  // 1 x 9-bit shunt sample
    #define CONFIG_SADCRES_10BIT_1S_148US   (0x0008)  // 1 x 10-bit shunt sample
    #define CONFIG_SADCRES_11BIT_1S_276US   (0x0010)  // 1 x 11-bit shunt sample
    #define CONFIG_SADCRES_12BIT_1S_532US   (0x0018)  // 1 x 12-bit shunt sample
    #define CONFIG_SADCRES_12BIT_2S_1060US  (0x0048)	 // 2 x 12-bit shunt samples averaged together
    #define CONFIG_SADCRES_12BIT_4S_2130US  (0x0050)  // 4 x 12-bit shunt samples averaged together
    #define CONFIG_SADCRES_12BIT_8S_4260US  (0x0058)  // 8 x 12-bit shunt samples averaged together
    #define CONFIG_SADCRES_12BIT_16S_8510US (0x0060)  // 16 x 12-bit shunt samples averaged together
    #define CONFIG_SADCRES_12BIT_32S_17MS   (0x0068)  // 32 x 12-bit shunt samples averaged together
    #define CONFIG_SADCRES_12BIT_64S_34MS   (0x0070)  // 64 x 12-bit shunt samples averaged together
    #define CONFIG_SADCRES_12BIT_128S_69MS  (0x0078)  // 128 x 12-bit shunt samples averaged together
	
    #define CONFIG_MODE_MASK                (0x0007)  // Operating Mode Mask
    #define CONFIG_MODE_POWERDOWN           (0x0000)
    #define CONFIG_MODE_SVOLT_TRIGGERED     (0x0001)
    #define CONFIG_MODE_BVOLT_TRIGGERED     (0x0002)
    #define CONFIG_MODE_SANDBVOLT_TRIGGERED (0x0003)
    #define CONFIG_MODE_ADCOFF              (0x0004)
    #define CONFIG_MODE_SVOLT_CONTINUOUS    (0x0005)
    #define CONFIG_MODE_BVOLT_CONTINUOUS    (0x0006)
    #define CONFIG_MODE_SANDBVOLT_CONTINUOUS (0x0007)	
/*=========================================================================*/

/*=========================================================================
    SHUNT VOLTAGE REGISTER (R) ��������ѹ��
    -----------------------------------------------------------------------*/
    #define REG_SHUNTVOLTAGE                (0x01)
/*=========================================================================*/

/*=========================================================================
    BUS VOLTAGE REGISTER (R) ���ߵ�ѹ��V- - GND��
    -----------------------------------------------------------------------*/
    #define REG_BUSVOLTAGE                  (0x02)
/*=========================================================================*/

/*=========================================================================
    POWER REGISTER (R) ����
    -----------------------------------------------------------------------*/
    #define REG_POWER                       (0x03)
/*=========================================================================*/

/*=========================================================================
    CURRENT REGISTER (R) ����
    -----------------------------------------------------------------------*/
    #define REG_CURRENT                     (0x04)
/*=========================================================================*/

/*=========================================================================
    CALIBRATION REGISTER (R/W) У׼
    -----------------------------------------------------------------------*/
    #define REG_CALIBRATION                 (0x05)
/*=========================================================================*/

#define OVER 	-1
#define TIMEOUT 0
#define VALID	1

	
class INA219{
public:
	INA219(E_I2c *i2c,uint8_t addr = INA219_ADDRESS_0);
	void begin(void);
	void reset(void);

	float getBusVoltage_V(void);
	int32_t getShuntVoltage_uV(void);
	int32_t getCurrent_uA(void);
	int32_t getPower_uw(void);
	
	
	int8_t  getState(void);
	void calibration(int calibration,int curLSB);
	uint8_t autoCalibration(int iMax_uA, int rShunt_mR);
	void config(uint16_t config);

	/**
		******************************************************************************
		* @brief  ina219 ����
		* @param  range (��ѹ��Χ)Ĭ��0-32v
		* @param  gain  (���棬��ѹ�����ѹֵ��Χ)Ĭ�� 1/8 gain  320mv range
		* @param  bus adc (gnd->bus- ��ѹadc)Ĭ��  3 12bit, ��������532usת��ʱ��
		* @param  shunt adc (�������� ��ѹadc)Ĭ��  3 12bit, ��������532usת��ʱ��
		* @param  mode (ģʽ) Ĭ�� 7 ������ת��
		* @note   
		* @retval none
		******************************************************************************
		*/
	void config( uint16_t range, uint16_t gain, 
				   uint16_t bus_adc=CONFIG_BADCRES_12BIT, 
				   uint16_t shunt_adc=CONFIG_SADCRES_12BIT_1S_532US, 
				   uint16_t mode = CONFIG_MODE_SANDBVOLT_CONTINUOUS);
	
	

private:
	E_I2c 	*_i2c;			// i2c�˿�
	uint8_t  _addr; 		// �豸��ַ
	uint32_t _calValue;		// У׼ֵ
	uint8_t  _state; 		// ת��״̬
	
	int _currentLSB;	// �����ֱ���
	int _powerLSB;		// ���ʷֱ���

	void WriteRegister(uint8_t reg, uint16_t value);
	void ReadRegister(uint8_t reg, uint16_t *value);
	int16_t getBusVoltage_raw(void);
	int16_t getShuntVoltage_raw(void);
	int16_t getCurrent_raw(void);
	int16_t getPower_raw(void);
};

#endif

