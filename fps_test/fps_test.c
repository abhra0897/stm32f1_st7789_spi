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
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
//#include "font_microsoft_16.h"
#include "font_ubuntu_mono_24.h"
#include "st7789_stm32_spi.h"
#include <libopencm3/cm3/systick.h>

uint64_t systick_count = 0;

/**
 * Configure RCC registers for 8MHz HSE and 80MHz output.
 * I'm using STM32F103 which supports maximum 72 MHz, so I'm overclocking it
 */

// Overcloks to 80MHz.
// Possible to overclock to 128MHz but for some weird reason my MCU stops working above 80MHz
void rcc_clock_setup_in_hse_8mhz_out_custom(void);

void delay_ms(uint16_t ms_delay);

static void clock_setup(void)
{
	//rcc_clock_setup_in_hse_8mhz_out_72mhz();
	rcc_clock_setup_in_hse_8mhz_out_custom();
}

void sys_tick_handler()
{
	systick_count++;
}

int main(void)
{
	/* Hardware config starts */
	clock_setup();
	/* Hardware config ends */

	// 80MHz/8 = 10MHz
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
	systick_set_reload(9999);
	systick_interrupt_enable();
	systick_counter_enable();

	//----------------------------------------------------------
	//initializing the st7789 display driver
	st_init();	

	/*To fill a specific area multiple times superfast, first set the area then fill it.*/
	// Set an area on the display to be drawn
	st_set_address_window(0, 0, 239, 239);
	//delay_ms(500);
	uint32_t len = 240 * 240; 
	uint64_t time_start = systick_count;
	// Now fill the area many times quickly
	for (uint16_t i = 0; i < 240; i++)
	{
		st_fill_color(ST_COLOR_MAROON + i, len);
	}

	uint64_t time_end = systick_count;
	uint64_t time_taken = time_end - time_start;
	char result[24];
	
	sprintf(result, "240 frames: %ldms", time_taken);
	st_draw_string_withbg(10, 10, result, ST_COLOR_WHITE, ST_COLOR_BLACK, &font_ubuntu_mono_24);
	sprintf(result, "FPS: %d", (uint16_t)((240.0 / (double)time_taken)*1000.0));
	st_draw_string_withbg(10, 70, result, ST_COLOR_WHITE, ST_COLOR_BLACK, &font_ubuntu_mono_24);
	sprintf(result, "Clock (AHB): %dMHz", (uint16_t)(rcc_ahb_frequency / 1000000.0));
	st_draw_string_withbg(10, 100, result, ST_COLOR_WHITE, ST_COLOR_BLACK, &font_ubuntu_mono_24);
	st_draw_string_withbg(10, 130, "Optimization: -O0", ST_COLOR_WHITE, ST_COLOR_BLACK, &font_ubuntu_mono_24);

	
	return 0;
}


/**
 * Overclock stm32 to 80MHz. HSE 8MHz multiplied by PLL 10
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
	rcc_ahb_frequency = 80000000;
	rcc_apb1_frequency = 80000000;
	rcc_apb2_frequency = 80000000;
}
