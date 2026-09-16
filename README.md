# RX8010SJ-Library-for-STM32
RX8010SJ Library for STM32

A simple, lightweight driver for the **RX8010SJ** Real-Time Clock (RTC) chip, built on top of **STM32 HAL I2C**. Suitable for embedded projects that need real-time clock keeping.

---

## 🔹 About the RX8010SJ Chip

The **RX8010SJ** is a Real-Time Clock IC manufactured by **Epson**, which communicates over the **I2C** interface. It is widely used in embedded systems, data loggers, industrial equipment, and IoT products.

Key features:

- ✅ **I2C** communication interface with default address `0x32`
- ✅ Very low power consumption (suitable for battery-powered devices)
- ✅ Keeps track of seconds, minutes, hours, weekday, day, month, and year
- ✅ Built-in **VLF** (Voltage Low Flag) to detect backup battery voltage drop
- ✅ Configurable interrupt registers and status registers
- ✅ High accuracy and stable operation across industrial temperature ranges

This library implements a simple driver layer on top of this chip, allowing you to easily read or set the time using STM32 HAL.

---

## 📁 Project Structure

```
RX8010.h    // Header file, function and struct declarations
RX8010.c    // Driver function implementations
```

---

## ⚙️ Requirements

- An STM32 microcontroller (any family that supports HAL I2C)
- One of the I2C peripherals configured and enabled in **CubeMX**
- Add `RX8010.h` and `RX8010.c` to your project

---

## 🧩 Main Data Structure

```c
struct RTC_TimeTypeDef {
    I2C_HandleTypeDef i2c;
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t week;
    uint8_t day;
    uint8_t month;
    uint8_t year;
};
```

This struct holds both the I2C handle used for communication and the time values read from the chip.

---

## 🚀 Usage

### 1) Initialization

```c
#include "RX8010.h"

RTC_TimeTypeDef rtc;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_I2C1_Init();

    // Initialize the RTC with I2C1 channel
    RX8010_Init(&rtc, hi2c1);

    while (1)
    {
        // ...
    }
}
```

The `RX8010_Init` function:
- Stores the I2C channel inside the struct
- Clears the VLF (voltage-low) flag
- Disables all interrupts
- Resets the status flags

---

### 2) Setting the Time

```c
// 14:30:00 - Day 16 - Month 9 - Year 26 (2026)
RX8010_SetTime(14, 30, 0, 16, 9, 26, &rtc);
```

Parameter order:

```c
RX8010_SetTime(hour, min, sec, day, month, year, &rtc);
```

> ⚠️ Note: In the current version, the weekday value is hardcoded to **Monday (0x01)** and there is no separate input parameter for it. If you need a configurable weekday, you'll need to modify the function.

---

### 3) Reading the Time

```c
RX8010_GetTime(&rtc);

printf("%02d:%02d:%02d  %02d/%02d/%02d\r\n",
       rtc.hour, rtc.min, rtc.sec,
       rtc.day, rtc.month, rtc.year);
```

After calling this function, the following struct fields are updated:

| Field   | Description        |
|---------|---------------------|
| `sec`   | Seconds (0-59)       |
| `min`   | Minutes (0-59)       |
| `hour`  | Hours (0-23)         |
| `week`  | Weekday (raw value)  |
| `day`   | Day of month         |
| `month` | Month                |
| `year`  | Year (last 2 digit)        |

---

### 4) Direct Register Read/Write (Optional)

```c
uint8_t val = RX8010_ReadReg(&rtc, 0x1D);   // Read Extension register
RX8010_WriteReg(&rtc, 0x1E, 0x00);          // Disable interrupts
```

---

## 🔧 Format Conversion Helpers

| Function | Description |
|----------|-------------|
| `BCD2DEC(uint8_t bcd)` | Converts a BCD value read from the chip into a decimal number |
| `DEC2BCD(uint8_t dec)` | Converts a decimal number into BCD format for writing to the chip |

---

## 📌 Important Notes

- The chip's I2C address is defined as `0x32 << 1` (8-bit format required by HAL).
- Always call `RX8010_Init` before using `RX8010_GetTime` or `RX8010_SetTime`.
- If you're using a backup battery, it's a good idea to check the VLF flag after the initial connection to confirm the stored time is still valid.

---


## 🤝 Contributing

Pull requests and issues to improve this driver are always welcome 🌟
