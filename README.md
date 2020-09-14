## ST7789 SPI Display Driver for STM32F1
This is a display driver for interfacing ST7789 LCD display with STM32F1 microcontroller over SPI bus. It's mainly written for my personal usage.

This driver is based on my other driver for ili9341: https://github.com/abhra0897/stm32f1_ili9341_parallel

This driver needs **[libopencm3](https://github.com/libopencm3/libopencm3.git)** library. The library is provided with this repository. You may get the latest version of libopencm3 from [here](https://github.com/libopencm3/libopencm3.git), but that may or may not work depending on the changes made in libopencm3 latest version.

In order to be able to program smt32 devices [st-flash](https://github.com/stlink-org/stlink/blob/master/doc/man/st-flash.md) is needed.

### Download
Download this repository using [git](https://git-scm.com/):

```
git clone https://github.com/abhra0897/stm32f1_st7789_spi.git
```

### Wiring
Connections between STM32F1 and ST7789 parallel display. **The display has no CS (Chip Select) pin**.
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
    <tr>
        <td>CS</td>
        <td>PA6 (unused)</td>
    </tr>
</table>

### Configuration
All configuration options are in the [header file](st7789_stm32_spi.h). 
- If display has no RESET pin, comment out `#define HAS_RST`.
- To use CS pin, uncomment `#define HAS_CS`
- If using CS and there's no other device connected in the same SPI bus, uncomment `#define RELEASE_WHEN_IDLE` to get a bit extra speed.
- Here `SPI1` is used, but can be changed from the header.
- Compiling with `-O1` flag gives almost twice as speed. But may reduce stability slightly. `-O0` flag is the most stable one and other optimization levels don't work.

#### RCC Peripheral Clock Settings
- If you change GPIOs, change inside `#define ST_CONFIG_GPIO_CLOCK()` too.
- Same is applicable for changing SPI bus.

#### DMA Settings
- uncomment `#define ST_USE_SPI_DMA` to disable DMA based SPI write
- If you change SPI bus, also change DMA number (`#define ST_DMA`) and DMA channel number (`#define ST_DMA_CHANNEL`)

### Example
Example code (**[main.c](example/main.c)**) is in **[example](example)** directory. To compile using the provided [Makefile](example/Makefile), keep the directory structure as it is. If you change the directory structure, edit the SRCS, INCLS, and LIBS in the Makefile accordingly.
Example is compiled and tested on STM32F103 (overclocked to 80MHz).

![Output of example code](example/photo/example_output.gif)

### Speed Test
Code for testing fps is [here](fps_test/fps_test.c). Compiling with different optimization flag and/or overclocking the MCU may give different FPS.

### Making Fonts
To know how to make more fonts as per your need, check my [fonts_embedded](https://github.com/abhra0897/fonts_embedded.git) repository.

### Important API Methods

```C
/**
 * Initialize the display driver
 */
void st_init();

/**
 * Set an area for drawing on the display with start row,col and end row,col.
 * User don't need to call it usually, call it only before some functions who don't call it by default.
 * @param x1 start column address.
 * @param y1 start row address.
 * @param x2 end column address.
 * @param y2 end row address.
 */
void st_set_address_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/**
 * Fills `len` number of pixels with `color`.
 * Call st_set_address_window() before calling this function.
 * @param color 16-bit RGB565 color value
 * @param len 32-bit number of pixels
 */
void st_fill_color(uint16_t color, uint32_t len);

/**
 * Draw a line from (x0,y0) to (x1,y1) with `width` and `color`.
 * @param x0 start column address.
 * @param y0 start row address.
 * @param x1 end column address.
 * @param y1 end row address.
 * @param width width or thickness of the line
 * @param color 16-bit RGB565 color of the line
 */
void st_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t width, uint16_t color);

/**
 * Rotate the display clockwise or anti-clockwie set by `rotation`
 * @param rotation Type of rotation. Supported values 0, 1, 2, 3
 */
void st_rotate_display(uint8_t rotation);

/**
 * Fills a rectangular area with `color`.
 * Before filling, performs area bound checking
 * @param x Start col address
 * @param y Start row address
 * @param w Width of rectangle
 * @param h Height of rectangle
 * @param color 16-bit RGB565 color
 */
void st_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/*
 * Same as `st_fill_rect()` but does not do bound checking, so it's slightly faster
 */
void st_fill_rect_fast(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t color);

/**
 * Fill the entire display (screen) with `color`
 * @param color 16-bit RGB565 color
 */
void st_fill_screen(uint16_t color);

/**
 * Experimental
 * Draw a rectangle without filling it
 * @param x start column address.
 * @param y start row address
 * @param w Width of rectangle
 * @param h height of rectangle
 */
void st_draw_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/**
 * Draws a character at a given position, fore color, back color.
 * @param x Start col address
 * @param y Start row address
 * @param character the ASCII character to be drawn
 * @param fore_color foreground color
 * @param back_color background color
 * @param font Pointer to the font of the character
 * @param is_bg Defines if character has background or not (transparent)
 */
void st_draw_char(uint16_t x, uint16_t y, char character, uint16_t fore_color, uint16_t back_color, const tFont *font, uint8_t is_bg)

/**
 * Draws a string on the display with `font` and `color` at given position.
 * Background of this string is transparent
 * @param x Start col address
 * @param y Start y address
 * @param str pointer to the string to be drawn
 * @param color 16-bit RGB565 color of the string
 * @param font Pointer to the font of the string
 */
void st_draw_string(uint16_t x, uint16_t y, char *str, uint16_t color, tFont *font);

/**
 * Draws a string on the display with `font`, `fore_color`, and `back_color` at given position.
 * The string has background color
 * @param x Start col address
 * @param y Start y address
 * @param str pointer to the string to be drawn
 * @param foe_color 16-bit RGB565 color of the string
 * @param back_color 16-bit RGB565 color of the string's background
 * @param font Pointer to the font of the string
 */
void st_draw_string_withbg(uint16_t x, uint16_t y, char *str, uint16_t fore_color, uint16_t back_color, tFont *font);

/**
 * Draw a bitmap image on the display
 * @param x Start col address
 * @param y Start row address
 * @param bitmap Pointer to the image data to be drawn
 */
void st_draw_bitmap(uint16_t x, uint16_t y, const tImage *bitmap);

/**
 * Draw a pixel at a given position with `color`
 * @param x Start col address
 * @param y Start row address
 */
void st_draw_pixel(uint16_t x, uint16_t y, uint16_t color);

```
### TO DO

 - [x] Add example code(s) and write some docs
 - [x] Write better comments
 - [x] Explain how to create fonts
 - [x] Add DMA transfer capability
 - [ ] Add a few more DMA based functions
 - [ ] Explain how to create bitmap image

### License
**[libopencm3](libopencm3)** and any derivative of the same are licensed under the terms of the GNU Lesser General Public License (LGPL), version 3 or later. The binaries generated after compilation will also be licensed under the same. See [this](libopencm3/COPYING.LGPL3) and [this](libopencm3/COPYING.GPL3) for the LGPL3 and GPL3 licenses.

All other source codes of the root directory and example directory are licensed under MIT License, unless the source file has no other license asigned for it. See [MIT License](LICENSE).
