/*
 * ws2812.h
 * WS2812 NeoPixel Driver for MSPM0G3507 (M0G3507QDGS28)
 * Output Pin : PA15 (TIMA1 CCP0 — IOMUX_PINCM37)
 * Pipeline   : Code Composer Studio → SWD → MSPM0G3507
 *
 * Uses: TIMA1 (PWM, 800 kHz, ARR=99) + DMA Channel 0
 * Clock: 80 MHz MCLK via SYSPLL (configured in SysConfig)
 *
 * WS2812 Protocol:
 *   GRB order, MSB first, 800 kHz bit rate
 *   Logic 1 : CCR = 66  (~66% duty, ~0.825 µs HIGH)
 *   Logic 0 : CCR = 33  (~33% duty, ~0.413 µs HIGH)
 *   Reset   : >50 µs LOW (50 zero-CCR slots appended)
 */

#ifndef WS2812_H_
#define WS2812_H_

#include <stdint.h>
#include <stdbool.h>
#include "ti_msp_dl_config.h"          /* SysConfig-generated defines  */
#include "ti/driverlib/driverlib.h"

/* ── User Configuration ────────────────────────────────────────────── */
#define WS2812_MAX_LEDS        (16u)   /* Maximum LEDs in chain        */
#define WS2812_USE_BRIGHTNESS  (1u)    /* 1 = enable brightness scale  */

/* ── PWM Timing (80 MHz MCLK, ARR=99 → 800 kHz) ───────────────────── */
#define WS2812_ARR             (99u)
#define WS2812_CCR_HI          (66u)   /* Logic 1 ~66% duty            */
#define WS2812_CCR_LO          (33u)   /* Logic 0 ~33% duty            */
#define WS2812_RESET_SLOTS     (50u)   /* 50 × 1.25 µs = 62.5 µs LOW  */

/* ── Buffer Size ────────────────────────────────────────────────────── */
#define WS2812_BUF_SIZE  ((24u * WS2812_MAX_LEDS) + WS2812_RESET_SLOTS)

/* ── Public API ─────────────────────────────────────────────────────── */

/**
 * @brief Initialise DMA destination address and clear LED buffer.
 *        Call ONCE after SYSCFG_DL_init() in main().
 */
void WS2812_Init(void);

/**
 * @brief Set RGB colour of one LED (zero-based index).
 */
void WS2812_SetLED(uint8_t ledIndex, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Set global brightness (0 = off, 255 = full).
 *        Only active when WS2812_USE_BRIGHTNESS == 1.
 */
void WS2812_SetBrightness(uint8_t brightness);

/**
 * @brief Set number of LEDs to drive (≤ WS2812_MAX_LEDS).
 */
void WS2812_SetNumLEDs(uint8_t count);

/**
 * @brief Build PWM buffer and stream it via TIMA1 + DMA.
 *        Blocks until transfer + reset pulse complete.
 */
void WS2812_Send(void);

/**
 * @brief Call from DMA_IRQHandler() in main.c.
 */
void WS2812_DMA_IRQHandler(void);

#endif /* WS2812_H_ */