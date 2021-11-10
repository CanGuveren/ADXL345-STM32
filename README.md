# ADXL345-STM32
ADXL345 Library for STM32-HAL

This library is not complete so this library may run unstable.

How to use this library
-----------------------

1. Change the header file according to the microcontroller that you are using. This library is being run on STM32F407.
2. Add a variable of type ADXL_ConfigTypeDef_t to your main.c.
3. You can configure this variable.
4. After you make the your configuration, add ADXL345_Init() function to your main.c.
5. Add ADXL345_MeasureON() to start measuring.

Example
-----------
ADXL_ConfigTypeDef_t ADXL

ADXL345_StandbyON();  -----> It is recommended to configure the device in standby mode and then to enable measurement mode in Datasheet.

ADXL.BWRate = BWRATE_1600;

ADXL.PowerMode = NormalPower;

ADXL.Format.Range = RANGE_8G;

ADXL345_Init(&ADXL, hi2c1);   -----> Write according to the I2C peripheral that you are using.

ADXL345_MeasureON();

X_Data = ADXL345_GetValue(Xaxis);  -----> ADXL345_GetValue returns a variable of type int16
