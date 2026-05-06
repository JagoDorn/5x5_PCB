/*
 * main.c
 * WS2812 Test Application — MSPM0G3507 (M0G3507QDGS28)
 *
 * Output: PA15 (TIMA1 CCP0, IOMUX_PINCM37) → level shifter → WS2812 DIN
 * Build : Code Composer Studio → SWD flash → MSPM0G3507
 */

#include "ti_msp_dl_config.h"
#include "ti/driverlib/driverlib.h"
#include "ti/driverlib/m0p/dl_core.h"
#include "ws2812.h"

/* ── Millisecond delay (80 MHz = 80,000 cycles per ms) ─────────────── */
static void delay_ms(uint32_t ms)
{
    delay_cycles(ms * 80000u);
}

/* ── DMA IRQ — delegate to driver ──────────────────────────────────── */
void DMA_IRQHandler(void)
{
    WS2812_DMA_IRQHandler();
}

/* ── Main ───────────────────────────────────────────────────────────── */
int main(void)
{
    /*
     * SYSCFG_DL_init() configures everything SysConfig set up:
     *   • 80 MHz MCLK (SYSOSC → SYSPLL → HSCLK → MCLK)
     *   • PA15 pinmux → TIMA1_CCP0
     *   • TIMA1 PWM edge-aligned, ARR=99, CC0=0, stopped
     *   • DMA Ch0 initialised with gTIMA1_CCP0Config, IRQ enabled
     */
    SYSCFG_DL_init();

    /* Initialise driver (sets DMA dest addr, clears buffers) */
    WS2812_Init();

    /* ── TEST 1: Individual LED colour control ──────────────────────── */
    WS2812_SetNumLEDs(8);
    WS2812_SetBrightness(128);          /* 50% brightness               */

    WS2812_SetLED(0, 255,   0,   0);   /* Red                          */
    WS2812_SetLED(1,   0, 255,   0);   /* Green                        */
    WS2812_SetLED(2,   0,   0, 255);   /* Blue                         */
    WS2812_SetLED(3, 255, 255,   0);   /* Yellow                       */
    WS2812_SetLED(4,   0, 255, 255);   /* Cyan                         */
    WS2812_SetLED(5, 255,   0, 255);   /* Magenta                      */
    WS2812_SetLED(6, 255, 128,   0);   /* Orange                       */
    WS2812_SetLED(7, 255, 255, 255);   /* White                        */

    WS2812_Send();
    delay_ms(2000);

    /* ── TEST 2: Brightness sweep (3 cycles) ───────────────────────── */
    WS2812_SetLED(0, 255,   0,   0);
    WS2812_SetLED(1,   0, 255,   0);
    WS2812_SetLED(2,   0,   0, 255);
    WS2812_SetLED(3, 255, 255,   0);
    WS2812_SetLED(4,   0, 255, 255);
    WS2812_SetLED(5, 255,   0, 255);
    WS2812_SetLED(6, 255, 128,   0);
    WS2812_SetLED(7, 255, 255, 255);

    for (int cycle = 0; cycle < 3; cycle++)
    {
        for (int b = 0; b <= 255; b += 5)
        {
            WS2812_SetBrightness((uint8_t)b);
            WS2812_Send();
            delay_ms(10);
        }
        for (int b = 255; b >= 0; b -= 5)
        {
            WS2812_SetBrightness((uint8_t)b);
            WS2812_Send();
            delay_ms(10);
        }
    }

    /* ── TEST 3: Variable LED count (light up one by one) ───────────── */
    WS2812_SetBrightness(200);
    for (uint8_t n = 1; n <= 8; n++)
    {
        WS2812_SetNumLEDs(n);
        for (uint8_t i = 0; i < n; i++)
            WS2812_SetLED(i, 255, 255, 255);   /* All white            */
        WS2812_Send();
        delay_ms(500);
    }

    /* ── Main loop: colour-chase animation ─────────────────────────── */
    WS2812_SetNumLEDs(8);
    WS2812_SetBrightness(180);
    uint8_t chase = 0;

    while (1)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            WS2812_SetLED(i,
                (i ==  chase          % 8) ? 255 : 0,   /* Red dot   */
                (i == (chase + 2)     % 8) ? 255 : 0,   /* Green dot */
                (i == (chase + 4)     % 8) ? 255 : 0);  /* Blue dot  */
        }
        WS2812_Send();
        chase = (chase + 1) % 8;
        delay_ms(80);
    }
}