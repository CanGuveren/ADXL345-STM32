/*
 * ADXL345.h
 *
 *  Created on: 4 Kas 2021
 *      Author: Ümit Can Güveren
 */

#ifndef INC_ADXL345_H_
#define INC_ADXL345_H_

#include "stdint.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f4xx.h"

#define ADXL345_DEVICE_ADDRESS	  0x53
#define ADXL345_I2C_SLAVE_ADDRESS (ADXL345_DEVICE_ADDRESS << 1)

/*
 * Register Map
 */
#define DEVID_ID			0x00
#define THRESH_TAP			0x1D
#define OFSX 			    0x1E
#define OFSY 				0x0F
#define OFSZ 				0x20
#define DUR 				0x21
#define Latent 				0x22
#define Window 				0x23
#define THRESH_ACT			0x24
#define THRESH_INACT		0x25
#define TIME_INACT			0x26
#define ACT_INACT_CTL		0x27
#define THRESH_FF			0x28
#define TIME_FF 			0x29
#define TAP_AXES			0x2A
#define ACT_TAP_STATUS		0x2B
#define BW_RATE 			0x2C
#define POWER_CTL			0x2D
#define INT_ENABLE			0x2E
#define INT_MAP 			0x2F
#define INT_SOURCE			0x30
#define DATA_FORMAT			0x31
#define DATAX0   			0x32
#define DATAX1   			0x33
#define DATAY0   			0x34
#define DATAY1   			0x35
#define DATAZ0   			0x36
#define DATAZ1   			0x37
#define FIFO_CTL  			0x38
#define FIFO_STATUS  		0x39

/*
 * @def_group Axis_Select
 */
#define Xaxis   			0x32
#define Yaxis   			0x34
#define Zaxis   			0x36

/*
 * @def_group Rate_Select
 */

#define BWRATE_0_10 	0x00
#define BWRATE_0_20 	0x01
#define BWRATE_0_39 	0x02
#define BWRATE_0_78 	0x03
#define BWRATE_1_56 	0x04
#define BWRATE_3_13 	0x05
#define BWRATE_6_25 	0x06
#define BWRATE_12_5 	0x07
#define BWRATE_25 		0x08
#define BWRATE_50 		0x09
#define BWRATE_100		0x0A
#define BWRATE_200		0x0B
#define BWRATE_400		0x0C
#define BWRATE_800		0x0D
#define BWRATE_1600   	0x0E
#define BWRATE_3200   	0x0F

/*
 * @def_group Power_Modes
 */

#define LowPower 	    0x10
#define NormalPower		0x00

/*
 * @def_group AutoSleep_ON/OFF
 */
#define AutoSleepON 	0x30
#define AutoSleepOFF	0x00

/*
 * @def_group WakeUp_Select
 */
#define WakeUpRate_8	0x00
#define WakeUpRate_4	0x01
#define WakeUpRate_2	0x02
#define WakeUpRate_1	0x03

/*
 * @def_group Resolution_Select
 */

#define RESFULL 		0x08
#define RES10BIT		0x00

/*
 * @def_group Range_Select
 */

#define RANGE_2G		0x00
#define RANGE_4G		0x01
#define RANGE_8G		0x02
#define RANGE_16G		0x03

/*
 * @def_group Active_HIGH/LOW
 */

#define ACTIVE_HIGH 	0x00
#define ACTIVE_LOW  	0x20

/*
 * @def_group TAP_AXIS_ENABLE
 */

#define X				0x04
#define Y				0x02
#define Z				0x01

/*
 * @def_group Interrupts
 */

#define DATA_READY 		0x80
#define SINGLE_TAP 		0x40
#define DOUBLE_TAP	    0x20
#define Activity   		0x10
#define Inactivity 		0x08
#define FREE_FALL		0x04
#define Watermark		0x02
#define Overrun			0x01

typedef enum
{
	ADXL_ERR = 0,
	ADXL_OK = !ADXL_ERR

}ADXL_Status;

typedef enum
{
	OFF = 0,
	ON = !OFF

}Function_State;

typedef enum
{
	INT1 = 0,
	INT2

}Mapping_State;

typedef enum
{
	DC = 0,
	AC

}Coupled_State;

typedef enum
{
	FlagLow = 0,
	FlagHigh

}Flag_State;

typedef struct
{
	uint8_t AutoSleep;		/*  AutoSleep ON/OFF Select   @def_group AutoSleep_ON/OFF	   				 		   	 */
	uint8_t ThreshInact; 	/*	This value must be between 1-256. The scale factor is 62.5 mg/LSB. 					 */
	uint8_t TimeInact; 		/*	This value must be between 1-256. The scale factor is 1 sec/LSB. 					 */

}ADXL_ConfigAutoSleep_t;

typedef struct
{
	uint8_t Resolution;		/*  Select Resolution   			@def_group Resolution_Select		   				 */
	uint8_t Range; 			/*	Select Range					@def_group Range_Select								 */
	uint8_t IntInvert; 		/*	Select Interrupt Active Status 	@def_group Active_HIGH/LOW			 				 */

}ADXL_ConfigFormat_t;


typedef struct
{
	uint8_t PowerMode;   	/* Power Mode Select	   @def_group Power_Modes 										 */
	uint8_t BWRate;			/* Output Data Rate Select @def_group Rate_Select	    								 */
	uint8_t WakeUpRate;		/* Wake Up Rate Select	   @def_group WakeUp_Select	   									 */
	ADXL_ConfigAutoSleep_t AutoSleepConfig;
	ADXL_ConfigFormat_t Format;
}ADXL_ConfigTypeDef_t;


/* Register Write and Read Function Prototype */
void regWrite(uint8_t Reg, uint8_t Value);
void regRead(uint8_t Reg, uint8_t *Value, uint16_t ByteSize);

ADXL_Status ADXL345_Init(ADXL_ConfigTypeDef_t *ADXL, I2C_HandleTypeDef I2CHandle);
int16_t ADXL345_GetValue(uint8_t Axis);
float ADXL345_GetGValue(uint8_t Axis);
void ADXL345_MeasureON();
void ADXL345_StandbyON();
void ADXL345_SleepON();
void ADXL345_Offset(uint8_t X_Offset, uint8_t Y_Offset, uint8_t Z_Offset );

/* Tap Function Prototype */
void ADXL345_TapStatus(Function_State Status, uint8_t Axis);
void ADXL345_SingleTapConfig(uint8_t DURValue, uint8_t ThreshTapValue);
void ADXL345_DoubleTapConfig(uint8_t DURValue, uint8_t ThreshTapValue, uint8_t LatentValue, uint8_t WindowValue);

/* Activity and Inactivity Function Prototype */
void ADXL345_ActInactStatus(Function_State Status, uint8_t ActInact, uint8_t Axis);
void ADXL345_ActivityConfig(Coupled_State Coupled, uint8_t Thresh);
void ADXL345_InactivityConfig(Coupled_State Coupled, uint8_t Thresh, uint8_t Time);

/* Free-Fall Function Prototype */
void ADXL345_FreeFallConfig(uint8_t Thresh, uint8_t Time);

/* Interrupt Function Prototype*/
void ADXL345_INTEnable(Function_State Status, uint8_t Int_Type);
void ADXL345_INTMapping(Mapping_State Pin, uint8_t Int_Type);
Flag_State ADXL345_INTRead(uint8_t Int_Type);

/* Self-Test Function Prototype*/
void ADXL345_SelfTestStatus(Function_State Status);

/* Others Function Prototype */
uint8_t I2C_ModulAdress_Scan();

#endif /* INC_ADXL345_H_ */
