/*
 * ADXL345.c
 *
 *  Created on: 4 Kas 2021
 *      Author: Ümit Can Güveren
 */
#include "ADXL345.h"

static I2C_HandleTypeDef ADXL345_I2C;


float ScaleFactor = 1/256.0f;


/*
 * @brief regWrite, Writes to ADXL Register using I2C
 * @param Reg = ADXL345 Register Address
 *
 * @param Value = This value is written to the ADXL345 Register
 *
 * @retval Void
 */
void regWrite(uint8_t Reg, uint8_t Value)
{
	uint8_t WriteData[2] = {0};
	WriteData[0] = Reg;
	WriteData[1] = Value;

	HAL_I2C_Master_Transmit(&ADXL345_I2C, ADXL345_I2C_SLAVE_ADDRESS, WriteData, 2, 100);

	//HAL_I2C_Mem_Write(&hi2c1, ADXL345_I2C_SLAVE_ADDRESS, Reg, 1, Value, 1, 10);  or using this.
}

/*
 * @brief regWrite, Writes to ADXL Register using I2C
 * @param Reg = ADXL345 Register Address
 *
 * @param Value = The value read from the register is assigned to this variable.
 *
 * @param ByteSize = Byte length to be read
 *
 * @retval Void
 */
void regRead(uint8_t Reg, uint8_t *Value, uint16_t ByteSize)
{
	HAL_I2C_Mem_Read(&ADXL345_I2C, ADXL345_I2C_SLAVE_ADDRESS, Reg, 1, Value, ByteSize, 100);


	//HAL_I2C_Master_Transmit(&hi2c1, ADXL345_I2C_SLAVE_ADDRESS, &Reg, 1, 100);
	//HAL_I2C_Master_Receive(&hi2c1, ADXL345_I2C_SLAVE_ADDRESS, Value, 1, 100);	or using these.

}

/*
 * @brief ADXL345_Init, configures ADXL345
 * @param ADXL = Configuration Structers
 *
 * @param I2CHandle = STM32 I2C Handle
 *
 * @retval ADXL_Status
 */
ADXL_Status ADXL345_Init(ADXL_ConfigTypeDef_t *ADXL, I2C_HandleTypeDef I2CHandle)
{
	memcpy(&ADXL345_I2C, &I2CHandle, sizeof(I2CHandle));

	uint8_t testDEVID;
	regRead(DEVID_ID, &testDEVID,1);

	if(testDEVID != 0xE5){return ADXL_ERR;}

	/*********** BW_Rate Config ***********/

	uint8_t regBWRate = 0;

	regWrite(BW_RATE, 0);

	if(ADXL->PowerMode == LowPower)
	{
		if(ADXL->BWRate < BWRATE_12_5)
		{
			ADXL->BWRate = BWRATE_12_5;
		}
		if(ADXL->BWRate > BWRATE_400)
		{
			ADXL->BWRate = BWRATE_400;
		}
	}

	regBWRate = (ADXL->PowerMode | ADXL->BWRate);

	regWrite(BW_RATE, regBWRate);

	/********* AutoSleepMode Config and WakeUpRate Config ********/

	uint8_t regValue = 0;

	regWrite(POWER_CTL, regValue);

	regValue |= ADXL->AutoSleepConfig.AutoSleep | ADXL->WakeUpRate;

	regWrite(POWER_CTL, regValue);

	regWrite(THRESH_INACT,0x00);
	regWrite(TIME_INACT,0x00);

	regWrite(THRESH_INACT,ADXL->AutoSleepConfig.ThreshInact);
	regWrite(TIME_INACT,ADXL->AutoSleepConfig.TimeInact);

	/********* Data Format Config ********/

	regValue = 0;

	regWrite(DATA_FORMAT, regValue);

	regValue = (ADXL->Format.Resolution | ADXL->Format.Range | ADXL->Format.IntInvert);

	if(ADXL->Format.Resolution == RESFULL || ADXL->Format.Range == RANGE_2G)
	{
		ScaleFactor = (float)1/256.0;
	}
	else
	{
		switch(ADXL->Format.Range)
		{
			case (RANGE_4G):
			{
				ScaleFactor = (float)1/128.0;
				break;
			}
			case (RANGE_8G):
			{
				ScaleFactor = (float)1/64.0;
				break;
			}
			case (RANGE_16G):
			{
				ScaleFactor = (float)1/32.0;
				break;
			}
		}
	}

	return ADXL_OK;
}

/*
 * @brief ADXL345_GetValue, get axis value
 *
 * @param Axis = X, Y and Z axis select
 *
 * @retval int16_t
 */
int16_t ADXL345_GetValue(uint8_t Axis)
{
	uint8_t Data[2] = { 0 };
	int16_t OutputData;

	regRead(Axis, Data, 2);

	OutputData = ((Data[1] << 8) | Data[0]);

	return OutputData;
}

/*
 * @brief ADXL345_GetGValue, get axis "G" value
 *
 * @param Axis = X, Y and Z axis select @def_group Axis_Select
 *
 * @retval float
 */
float ADXL345_GetGValue(uint8_t Axis)
{
	float OutputData = 0;

	OutputData = ADXL345_GetValue(Axis);

	OutputData = (float)(OutputData * ScaleFactor);

	return OutputData;
}

/*
 * @brief ADXL345_MeasureON, Measure Mode On, Sleep Mode and Standby Mode Off
 *
 * @retval void
 */
void ADXL345_MeasureON()
{
	uint8_t regValue = 0;

	regRead(POWER_CTL, &regValue, 1);

	regValue |= ( 1 << 3 );
	regValue &= ~( 1 << 2 );

	regWrite(POWER_CTL, regValue);
}

/*
 * @brief ADXL345_StandbyON, Standby Mode On, Sleep Mode and Measure Mode Off
 *
 * @retval void
 */
void ADXL345_StandbyON()
{
	uint8_t regValue = 0;

	regRead(POWER_CTL, &regValue, 1);

	regValue &= ~( 1 << 3 );
	regValue &= ~( 1 << 2 );

	regWrite(POWER_CTL, regValue);
}

/*
 * @brief ADXL345_StandbyON, Sleep Mode On, Standby Mode and Measure Mode Off
 *
 * @retval void
 */
void ADXL345_SleepON()
{
	uint8_t regValue = 0;

	regRead(POWER_CTL, &regValue, 1);

	regValue |= ( 1 << 2 );
	regValue &= ~( 1 << 3 );

	regWrite(POWER_CTL, regValue);
}

/*
 * @brief ADXL345_Offset, Offset settings
 *
 * @param X_Offset, X offset value
 *
 * @param Y_Offset, Y offset value
 *
 * @param Z_Offset, Z offset value
 *
 * @retval void
 */
void ADXL345_Offset(uint8_t X_Offset, uint8_t Y_Offset, uint8_t Z_Offset )
{
	// Standby Mode ON !!!!!!!!!!!!!!!!!
	regWrite(OFSX, X_Offset);
	regWrite(OFSY, Y_Offset);
	regWrite(OFSZ, Z_Offset);

}

/*
 * @brief ADXL345_TapStatus, Top function on/off
 *
 * @param Status, ON/OFF Select
 *
 * @param Axis, Axis Select @def_group TAP_AXIS_ENABLE
 *
 * @retval void
 */
void ADXL345_TapStatus(Function_State Status, uint8_t Axis)
{
	ADXL345_StandbyON();

	uint8_t regValue = 0;

	switch(Status)
	{
		case ON:
		{
			regRead(TAP_AXES, &regValue, 1);

			regValue |= Axis;

			regWrite(TAP_AXES, regValue);
			break;
		}

		case OFF:
		{
			regRead(TAP_AXES, &regValue, 1);

			regValue &= ~(Axis);

			regWrite(TAP_AXES, regValue);
			break;
		}
	}
}

/*
 * @brief ADXL345_SingleTapConfig, Single tap configuration
 *
 * @param DURValue, This value must be between 1-256. The scale factor is 625 μs/LSB.
 *
 * @param ThreshTapValue, This value must be between 1-256. The scale factor is 62.5 mg/LSB.
 *
 * @retval void
 */
void ADXL345_SingleTapConfig(uint8_t DURValue, uint8_t ThreshTapValue)
{
	regWrite(DUR, 0); 			//Reset DUR Register
	regWrite(THRESH_TAP, 0); 	//Reset THRESH_TAP Register

	regWrite(DUR, DURValue);
	regWrite(THRESH_TAP, ThreshTapValue);
}

/*
 * @brief ADXL345_DoubleTapConfig, Double tap configuration
 *
 * @param DURValue, This value must be between 1-256. The scale factor is 625 μs/LSB.
 *
 * @param ThreshTapValue, This value must be between 1-256. The scale factor is 62.5 mg/LSB.
 *
 * @param LatentValue, This value must be between 1-256. The scale factor is 1.25 ms/LSB.
 *
 * @param WindowValue, This value must be between 1-256. The scale factor is 1.25 ms/LSB.
 *
 * @retval void
 */
void ADXL345_DoubleTapConfig(uint8_t DURValue, uint8_t ThreshTapValue, uint8_t LatentValue, uint8_t WindowValue)
{
	regWrite(DUR, 0); 			//Reset DUR Register
	regWrite(THRESH_TAP, 0); 	//Reset THRESH_TAP Register
	regWrite(Latent, 0);		//Reset Latent Register
	regWrite(Window, 0);		//Reset Window Register

	regWrite(DUR, DURValue);
	regWrite(THRESH_TAP, ThreshTapValue);
	regWrite(Latent, LatentValue);
	regWrite(Window, WindowValue);
}

/*
 * @brief ADXL345_ActInactStatus, Activity and Inactivity features enable/disable and axis select
 *
 * @param Status, ON/OFF Select
 *
 * @param ActInact, Activity/Inactivity Select
 *
 * @param Axis, Axis Select @def_group TAP_AXIS_ENABLE
 *
 * @retval void
 */
void ADXL345_ActInactStatus(Function_State Status, uint8_t ActInact, uint8_t Axis)
{
	uint8_t regValue = 0;

	regRead(ACT_INACT_CTL, &regValue, 1);

	switch(Status)
	{

		case ON:
		{
			if(ActInact == Activity)
			{
				regValue |= (Axis << 4);
			}
			else
			{
				regValue |= (Axis);
			}
			break;
		}
		case OFF:
		{
			if(ActInact == Activity)
			{
				regValue &= ~(Axis << 4);
			}
			else
			{
				regValue &= ~(Axis);

			}
			break;
		}
	}
}

/*
 * @brief ADXL345_ActivityConfig, Activity and Inactivity features enable/disable and axis select
 *
 * @param Coupled, AC/DC Select
 *
 * @param Thresh, This value must be between 1-256. The scale factor is 62.5 mg/LSB.
 *
 * @retval void
 */
void ADXL345_ActivityConfig(Coupled_State Coupled, uint8_t Thresh)
{
	uint8_t regValue = 0;

	regRead(ACT_INACT_CTL, &regValue, 1);

	regValue |= (Coupled << 7);

	regWrite(THRESH_ACT, 0);
	regWrite(THRESH_ACT, Thresh);


}

/*
 * @brief ADXL345_InactivityConfig, Activity and Inactivity features enable/disable and axis select
 *
 * @param Coupled, AC/DC Select
 *
 * @param Thresh, This value must be between 1-256. The scale factor is 62.5 mg/LSB.
 *
 * @param Time, This value must be between 1-256. The scale factor is 1 sec/LSB.
 *
 * @retval void
 */
void ADXL345_InactivityConfig(Coupled_State Coupled, uint8_t Thresh, uint8_t Time)
{
	uint8_t regValue = 0;

	regRead(ACT_INACT_CTL, &regValue, 1);

	regValue |= (Coupled << 3);

	regWrite(THRESH_INACT, 0);
	regWrite(TIME_INACT, 0);

	regWrite(THRESH_INACT, Thresh);
	regWrite(TIME_INACT, Time);

}

/*
 * @brief ADXL345_FreeFallConfig, Free-Fall feature configuration
 *
 * @param Thresh, This value must be between 1-256. The scale factor is 62.5 mg/LSB.
 *
 * @param Time, This value must be between 1-256. The scale factor is 5 ms/LSB.
 *
 * @retval void
 */
void ADXL345_FreeFallConfig(uint8_t Thresh, uint8_t Time)
{
	regWrite(THRESH_FF, 0); 	//Reset THRESH_FF Register
	regWrite(TIME_FF, 0); 		//Reset TIME_FF Register

	regWrite(THRESH_FF, Thresh);
	regWrite(TIME_FF, Time);
}


/*
 * @brief ADXL345_INTEnable, Interrupt Enable/Disable
 *
 * @param Status, ON/OFF Select
 *
 * @param Int_Type, Interrupt Select @def_group Interrupts
 *
 * @retval void
 */
void ADXL345_INTEnable(Function_State Status, uint8_t Int_Type)
{

	uint8_t regValue = 0;

	switch(Status)
	{
		case ON:
		{
			regRead(INT_ENABLE, &regValue, 1);

			regValue |= (Int_Type);

			regWrite(INT_ENABLE, regValue);
			break;
		}

		case OFF:
		{
			regRead(INT_ENABLE, &regValue, 1);

			regValue &= ~(Int_Type);

			regWrite(INT_ENABLE, regValue);
			break;
		}
	}
}

/*
 * @brief ADXL345_INTMapping, Interrupt pin select
 *
 * @param Pin, INT1 or INT2
 *
 * @param Int_Type, Interrupt Select @def_group Interrupts
 *
 * @retval void
 */
void ADXL345_INTMapping(Mapping_State Pin, uint8_t Int_Type)
{
	uint8_t regValue = 0;
	uint8_t INTENABLEreg = 0;

	regRead(INT_ENABLE, &INTENABLEreg, 1);

	INTENABLEreg &= ~(Int_Type);

	regWrite(INT_ENABLE, INTENABLEreg);

	switch(Pin)
	{
		case INT1:
		{
			regRead(INT_MAP, &regValue, 1);

			regValue &= ~(Int_Type);

			regWrite(INT_MAP, regValue);
			break;
		}

		case INT2:
		{
			regRead(INT_MAP, &regValue, 1);

			regValue |= (Int_Type);

			regWrite(INT_MAP, regValue);
			break;
		}
	}

	INTENABLEreg |= (Int_Type);

	regWrite(INT_ENABLE, INTENABLEreg);
}


Flag_State ADXL345_INTRead(uint8_t Int_Type)
{
	uint8_t regValue = 0;

	regRead(INT_SOURCE, &regValue, 1);

	if( (Int_Type & regValue) != 0 )
	{
		return FlagHigh;
	}
	else
	{
		return FlagLow;
	}
}

/*
 * @brief ADXL345_SelfTestStatus, Self Test ON/OFF
 *
 * @param Status, ON/OFF Select
 *
 * @retval void
 */
void ADXL345_SelfTestStatus(Function_State Status)
{
	uint8_t regValue = 0;

	regRead(DATA_FORMAT, &regValue, 1);

	switch(Status)
	{
		case ON:
		{
			regValue |= (1 << 7);
			break;
		}
		case OFF:
		{
			regValue &= ~(1 << 7);
			break;
		}
	}

	regWrite(DATA_FORMAT, regValue);
}

uint8_t I2C_ModulAdress_Scan()
{
	uint8_t address;
	for(address = 0; address <= 255; address++)
	{
		if(HAL_I2C_IsDeviceReady(&ADXL345_I2C, address, 1, 10) == HAL_OK)
		{
			break;
		}
	}

	return address;
}
