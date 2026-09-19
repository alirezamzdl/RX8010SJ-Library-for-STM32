# G030Clock_RX8010

A simple digital clock built on an **STM32G030C8T6** (Cortex‑M0+) MCU that reads the time and date from an **Epson RX8010** I²C real‑time clock (RTC) chip and displays them on a graphic LCD driven by a **SPLC501/KS0713**‑type controller (parallel 8080-ish interface).

The project is an STM32CubeIDE project (generated from a `.ioc` file with STM32CubeMX) using the STM32G0 HAL and CMSIS libraries.

## Features

- Reads current time (HH:MM:SS) and date (DD/MM/YY) from the RX8010 RTC over I²C.
- Displays time and date as text on the LCD using a built‑in 5×7-ish bitmap font.
- Simple polling loop: reads the RTC and refreshes the display once per second.
- RTC is initialized and seeded with a starting date/time on boot.

## Hardware

| Item | Details |
|---|---|
| MCU | STM32G030C8Tx (LQFP48), HSI @ 16 MHz |
| RTC | Epson RX8010 (I²C RTC, 7-bit address `0x32`) |
| Display | Graphic LCD with SPLC501/KS0713-compatible controller, 8-bit parallel interface |
| Clock source | Internal HSI (no external crystal required for the MCU; RX8010 has its own 32.768 kHz crystal) |

### Pin Mapping (from `G030Clock_RX8010.ioc`)

**RTC (I²C1)**

| Signal | Pin |
|---|---|
| I2C1_SCL | PB8 |
| I2C1_SDA | PB9 |

**LCD (8-bit parallel bus on GPIOB + control lines)**

| Signal | Pin | Purpose |
|---|---|---|
| LCD_D0–D7 | PB0–PB7 | 8-bit data bus |
| LCD_CS | PB10 | Chip select |
| LCD_RESET | PB11 | Reset |
| LCD_A0 | PB12 | Data/Command select |
| LCD_WR | PB13 | Write strobe |
| LCD_RD | PB14 | Read strobe |
| LCD_BACKLIGHT | PD1 | Backlight enable |

**Debug**

| Signal | Pin |
|---|---|
| SWDIO | PA13 |
| SWCLK | PA14 |

## Project Structure

```
G030Clock_RX8010/
├── Core/
│   ├── Inc/
│   │   ├── main.h
│   │   ├── gpio.h
│   │   ├── i2c.h
│   │   ├── RX8010.h          # RX8010 RTC driver header
│   │   ├── SPLC501.h         # LCD driver header
│   │   └── stm32g0xx_*.h
│   └── Src/
│       ├── main.c            # App entry point / main loop
│       ├── gpio.c
│       ├── i2c.c
│       ├── RX8010.c          # RX8010 RTC driver
│       ├── SPLC501.c         # LCD driver + font table
│       └── stm32g0xx_*.c
├── Drivers/
│   ├── CMSIS/                 # ARM CMSIS core + device headers
│   └── STM32G0xx_HAL_Driver/  # ST HAL/LL drivers
├── Debug/                     # STM32CubeIDE build output (objects, .elf, .map)
├── G030Clock_RX8010.ioc       # STM32CubeMX device configuration
└── STM32G030C8TX_FLASH.ld     # Linker script
```

## How It Works

### 1. RX8010 RTC driver (`RX8010.c` / `RX8010.h`)

- `RX8010_Init()` — stores the I²C handle, clears the VLF (voltage-low flag) in the extension register, disables all RTC interrupts, and clears status flags.
- `RX8010_SetTime(hour, min, sec, day, month, year, &rtc)` — writes time/date registers (BCD-encoded) starting at register `0x10`.
- `RX8010_GetTime(&rtc)` — reads 7 bytes starting at register `0x10` (seconds, minutes, hours, weekday, day, month, year) and converts them from BCD to decimal.
- Register access uses `HAL_I2C_Mem_Read` / `HAL_I2C_Mem_Write` against the RX8010's 8-bit I²C address `0x32 << 1`.

### 2. LCD driver (`SPLC501.c` / `SPLC501.h`)

- Bit-bangs an 8-bit parallel interface directly on `GPIOB` (data lines D0–D7) plus CS/RESET/A0/WR/RD control lines using `GPIOB->BSRR` for fast, glitch-free pin writes.
- `lcd_init()` resets the controller and sends the SPLC501/KS0713 initialization sequence (ADC direction, SHL direction, voltage reference/converter/regulator/follower, contrast, display on).
- `lcd_cls()` clears the display RAM.
- Includes a bitmap font table (`font[]`) covering printable ASCII characters, plus helpers (`LcdCharacter`, `LcdString`, `LCD_write_string_Medium`, `draw_progbar`, `LCD_icon`, etc.) for drawing text/graphics.

### 3. Application (`main.c`)

On boot, the firmware:

1. Calls `HAL_Init()`, configures the system clock (HSI @ 16 MHz), and initializes GPIO + I²C1 peripherals.
2. Turns on the LCD backlight, initializes and clears the LCD.
3. Initializes the RX8010 (`RX8010_Init`) and seeds it with a starting date/time (`RX8010_SetTime`) — this line should be **commented out or removed after the first programming**, otherwise the clock resets to the same time on every power cycle/reset.
4. Enters the main loop, which every second:
   - Reads the current time/date from the RX8010 (`RX8010_GetTime`).
   - Formats it into `HH:MM:SS` and `DD/MM/YYYY` strings.
   - Writes both strings to the LCD (`LCD_write_string_Medium`).

```c
RX8010_Init(&rtc, hi2c1);
RX8010_SetTime(1, 56, 0, 7, 7, 26, &rtc);   // hour, min, sec, day, month, year

while (1)
{
    RX8010_GetTime(&rtc);
    sprintf(clock_display, "%02d:%02d:%02d", rtc.hour, rtc.min, rtc.sec);
    sprintf(date_display, "%02d/%02d/20%02d", rtc.day, rtc.month, rtc.year);
    LCD_write_string_Medium(15, 1, clock_display, 0);
    LCD_write_string_Medium(5, 20, date_display, 0);
    HAL_Delay(1000);
}
```

> ⚠️ **Note:** `RX8010_SetTime` is currently called unconditionally on every boot, which will overwrite the RTC's time with the hardcoded value each time the board resets. For normal use, set the time once (e.g., via a button, UART command, or a one-time flash) and remove/guard this call afterward so the RX8010's own timekeeping (backed by its crystal, and optionally a backup battery/cap) is preserved across resets and power loss.

## Building & Flashing

This is an **STM32CubeIDE** project.

1. Open STM32CubeIDE.
2. `File → Open Projects from File System...` and select the `G030Clock_RX8010` folder (containing `.project` / `.cproject`).
3. Build the project (`Project → Build Project`), or edit `G030Clock_RX8010.ioc` in CubeMX first if you need to change pin assignments/clocks, then regenerate code.
4. Connect an ST-Link (or compatible SWD debugger) to the SWDIO/SWCLK pins.
5. Flash/debug via `Run → Debug` or `Run → Run` (uses the included `G030Clock_RX8010 Debug.launch` configuration).

Alternatively, build with the included Eclipse-style `Debug/makefile` from the command line using `arm-none-eabi-gcc` and flash the resulting `Debug/G030Clock_RX8010.elf` with `st-flash`, OpenOCD, or STM32CubeProgrammer.

## Customization Ideas

- Remove the fixed `RX8010_SetTime()` call after the first flash so the RTC keeps time across resets, or add a way to set the time (buttons/UART) at runtime.
- Use the RX8010's alarm/interrupt output for periodic wake-ups instead of polling every second with `HAL_Delay(1000)`.
- Use `rtc.week` (weekday register) to also display the day of the week.
- Add battery-backup handling using the RX8010's VLF (voltage-low flag) to detect power loss and prompt a time re-sync.