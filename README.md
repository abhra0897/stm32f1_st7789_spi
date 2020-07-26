
# -- INCOMPLETE (work in progress) --

## ST7789 SPI Display Driver for STM32F1
This is a fast display driver for interfacing ST7789 LCD display with STM32F1 microcontroller over SPI bus. It's mainly written for my personal usage.

This driver needs **[libopencm3](https://github.com/libopencm3/libopencm3.git)** library. The library is provided with this repository. You may get the latest version of libopencm3 from [here](https://github.com/libopencm3/libopencm3.git), but that may or may not work depending on the changes made in libopencm3 latest version.

### Download
Download this repository using [git](https://git-scm.com/):

```
git clone https://github.com/abhra0897/stm32f1_st7789_spi.git
```

### Wiring
Connections between STM32F1 and ILI9341 parallel display. **The display has no CS (Chip Select) pin**.
<table border=1>
    <tr>
        <th colspan=1>ST7789</th>
        <th rowspan=1>STM32F1</th>
    </tr>
    <tr>
        <td>SDA (SO)</td>
        <td>PA7</td>
    </tr>
    <tr>
        <td>SCL (CLK)</td>
        <td>PA5</td>
    </tr>
    <tr>
        <td>RES (RESETn)</td>
        <td>PA4</td>
    </tr>
    <tr>
        <td>DC (DATA/CMDn)</td>
        <td>PA2</td>
    </tr>
    <tr>
        <td>BLK (BACKLIGHT)</td>
        <td>PA3</td>
    </tr>
</table>

### Benchmarks
TBD

### Example
Example is not ready yet

### Important API Methods
Not ready yet

```

### License
**[libopencm3](libopencm3)** and any derivative of the same are licensed under the terms of the GNU Lesser General Public License (LGPL), version 3 or later. The binaries generated after compilation will also be licensed under the same. See [this](libopencm3/COPYING.LGPL3) and [this](libopencm3/COPYING.GPL3) for the LGPL3 and GPL3 licenses.

All other source codes of the root directory and example directory are licensed under MIT License, unless the source file has no other license asigned for it. See [MIT License](LICENSE).
