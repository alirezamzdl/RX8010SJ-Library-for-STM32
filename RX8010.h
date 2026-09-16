
#ifndef RX8010_H
#define RX8010_H

#include "i2c.h"

#define RX8010_ADDR    (0x32 << 1)    // HAL uses 8-bit address

typedef struct RTC_TimeTypeDef RTC_TimeTypeDef;

struct RTC_TimeTypeDef{
	I2C_HandleTypeDef i2c;
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t week;
    uint8_t day;
    uint8_t month;
    uint8_t year;
};

void RX8010_Init(RTC_TimeTypeDef *dev , I2C_HandleTypeDef I2C_CH);

uint8_t BCD2DEC(uint8_t bcd);

void RX8010_GetTime(RTC_TimeTypeDef *t);

void RX8010_WriteReg(RTC_TimeTypeDef *dev , uint8_t reg, uint8_t value);

uint8_t RX8010_ReadReg(RTC_TimeTypeDef *dev , uint8_t reg);

uint8_t DEC2BCD(uint8_t dec);

void RX8010_SetTime(uint8_t hour,
                    uint8_t min,
                    uint8_t sec,
                    uint8_t day,
                    uint8_t month,
                    uint8_t year,
					RTC_TimeTypeDef *t);

#endif
