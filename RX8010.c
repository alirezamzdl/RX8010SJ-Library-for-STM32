
#include "RX8010.h"


void RX8010_Init(RTC_TimeTypeDef *dev , I2C_HandleTypeDef I2C_CH){
	// setting the i2c channel
	dev->i2c = I2C_CH;

    uint8_t reg;
    // Clear VLF flag (Extension Register)
    reg = RX8010_ReadReg(&dev->i2c , 0x1D);
    reg &= ~(1 << 1);
    RX8010_WriteReg(&dev->i2c , 0x1D, reg);

    // Stop all interrupts
    RX8010_WriteReg(&dev->i2c , 0x1E, 0x00);

    // Clear all status flags
    RX8010_WriteReg(&dev->i2c , 0x1F, 0x00);
}



uint8_t BCD2DEC(uint8_t bcd){
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void RX8010_GetTime(RTC_TimeTypeDef *t){
    uint8_t buf[7];

    HAL_I2C_Mem_Read(&t->i2c,
                     RX8010_ADDR,
                     0x10,
                     I2C_MEMADD_SIZE_8BIT,
                     buf,
                     7,
                     100);

    t->sec   = BCD2DEC(buf[0] & 0x7F);
    t->min   = BCD2DEC(buf[1] & 0x7F);
    t->hour  = BCD2DEC(buf[2] & 0x3F);
    t->week  = buf[3];
    t->day   = BCD2DEC(buf[4] & 0x3F);
    t->month = BCD2DEC(buf[5] & 0x1F);
    t->year  = BCD2DEC(buf[6]);
}

void RX8010_WriteReg(RTC_TimeTypeDef *dev , uint8_t reg, uint8_t value){
    HAL_I2C_Mem_Write(&dev->i2c,
                      RX8010_ADDR,
                      reg,
                      I2C_MEMADD_SIZE_8BIT,
                      &value,
                      1,
                      100);
}

uint8_t RX8010_ReadReg(RTC_TimeTypeDef *dev , uint8_t reg){
    uint8_t value;

    HAL_I2C_Mem_Read(&dev->i2c,
                     RX8010_ADDR,
                     reg,
                     I2C_MEMADD_SIZE_8BIT,
                     &value,
                     1,
                     100);
    return value;
}



uint8_t DEC2BCD(uint8_t dec){
    return ((dec / 10) << 4) | (dec % 10);
}

void RX8010_SetTime(uint8_t hour,
                    uint8_t min,
                    uint8_t sec,
                    uint8_t day,
                    uint8_t month,
                    uint8_t year ,
					RTC_TimeTypeDef *t){
    uint8_t buf[7];

    buf[0] = DEC2BCD(sec);
    buf[1] = DEC2BCD(min);
    buf[2] = DEC2BCD(hour);
    buf[3] = 0x01;              // Monday
    buf[4] = DEC2BCD(day);
    buf[5] = DEC2BCD(month);
    buf[6] = DEC2BCD(year);

    HAL_I2C_Mem_Write(&t->i2c,
                      RX8010_ADDR,
                      0x10,
                      I2C_MEMADD_SIZE_8BIT,
                      buf,
                      7,
                      100);
}

