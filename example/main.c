/*
MIT License

Copyright (c) 2020 Avra Mitra

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include "font_microsoft_16.h"
#include "st7789_stm32_spi.h"

/**
 * Configure RCC registers for 8MHz HSE and 128MHz output.
 * I'm using STM32F103 which supports maximum 72 MHz, so I'm overclocking it
 */
void rcc_clock_setup_in_hse_8mhz_out_custom(void);


/* Set STM32 to 80 MHz. (overclocking for faster performance) */
static void clock_setup(void)
{
	rcc_clock_setup_in_hse_8mhz_out_custom();
}

int main(void)
{
	/* Hardware config starts */
	clock_setup();
	/* Hardware config ends */

	//----------------------------------------------------------
	//initializing the ili9341 display driver
	st_init();
	//rotating display to default mode
	//st_rotate_display(0);

	// Fiiling the entire screen with cyan color
	st_fill_screen(COLOR_CYAN);

	// Fill a rectangle with green yellow
	st_fill_rect(20, 0, 80, 80, COLOR_PINK);

	// Draw a rectangle
	st_draw_rectangle(5, 160, 50, 50, COLOR_BLACK);

	// Write a string with transparent background at (0, 0) position with font `font_microsoft_16`
	st_draw_string(0, 0, "Hello world", 0, &font_microsoft_16);

	// Write a string with solid background. Text color white, background color dark green
	st_draw_string_withbg(10, 40, "Hello Solid World", COLOR_WHITE, COLOR_DARKGREEN, &font_microsoft_16);

	// Draw a line between any two points
	st_draw_line(4, 18, 200, 150, 1, COLOR_RED);

	// Draw a "thicc" line
	st_draw_line(180, 2, 20, 220, 5, COLOR_BLUE);
	
	// Draw some individual pixels in random points
	for (int i = 0; i < 2000; i++)
		st_draw_pixel(rand() % 100 + 120, rand() % 100 + 60, COLOR_WHITE + rand() % 100 + 60);

	/*To fill a specific area multiple times superfast, first set the area then fill it.*/
	// Set an area on the display to be drawn
	st_set_address_window(50, 150, 150, 230);
	// Now fill the area many times quickly
	for (uint16_t i = 0; i < 300; i++)
		st_fill_color(COLOR_MAROON + i, (100+1) * (150+1));



	return 0;
}


/**
 * Overclock stm32 to 128MHz. HSE 8MHz multiplied by PLL 16
 * Performance may become unstable
 */
void rcc_clock_setup_in_hse_8mhz_out_custom(void)
{
	/* Enable internal high-speed oscillator. */
	rcc_osc_on(RCC_HSI);
	rcc_wait_for_osc_ready(RCC_HSI);

	/* Select HSI as SYSCLK source. */
	rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_HSICLK);

	/* Enable external high-speed oscillator 8MHz. */
	rcc_osc_on(RCC_HSE);
	rcc_wait_for_osc_ready(RCC_HSE);
	rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_HSECLK);

	/*
	 * Set prescalers for AHB, ADC, APB1, APB2.
	 * Do this before touching the PLL (TODO: why?).
	 */
	rcc_set_hpre(RCC_CFGR_HPRE_SYSCLK_NODIV);    /* Set. 72MHz Max. 128MHz */
	rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV8);  /* Set.  9MHz Max. 16MHz */
	rcc_set_ppre1(RCC_CFGR_PPRE1_HCLK_NODIV);     /* Set. 36MHz Max. 128MHz */
	rcc_set_ppre2(RCC_CFGR_PPRE2_HCLK_NODIV);    /* Set. 72MHz Max. 128MHz */

	/*
	 * Sysclk runs with 72MHz -> 2 waitstates.
	 * 0WS from 0-24MHz
	 * 1WS from 24-48MHz
	 * 2WS from 48-72MHz
	 */
	flash_set_ws(2);

	/*
	 * Set the PLL multiplication factor to 128.
	 * 8MHz (external) * 16 (multiplier) = 128MHz
	 */
	rcc_set_pll_multiplication_factor(RCC_CFGR_PLLMUL_PLL_CLK_MUL10);

	/* Select HSE as PLL source. */
	rcc_set_pll_source(RCC_CFGR_PLLSRC_HSE_CLK);

	/*
	 * External frequency undivided before entering PLL
	 * (only valid/needed for HSE).
	 */
	rcc_set_pllxtpre(RCC_CFGR_PLLXTPRE_HSE_CLK);

	/* Enable PLL oscillator and wait for it to stabilize. */
	rcc_osc_on(RCC_PLL);
	rcc_wait_for_osc_ready(RCC_PLL);

	/* Select PLL as SYSCLK source. */
	rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_PLLCLK);

	/* Set the peripheral clock frequencies used */
	rcc_ahb_frequency = 128000000;
	rcc_apb1_frequency = 128000000;
	rcc_apb2_frequency = 128000000;
}
