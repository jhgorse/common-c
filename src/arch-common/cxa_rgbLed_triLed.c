/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * @author Christopher Armenio
 */
#include "cxa_rgbLed_triLed.h"


// ******** includes ********
#include <cxa_assert.h>

#define CXA_LOG_LEVEL			CXA_LOG_LEVEL_TRACE
#include <cxa_logger_implementation.h>


// ******** local macro definitions ********


// ******** local type definitions ********


// ******** local function prototypes ********
static void scm_setRgb(cxa_rgbLed_runLoop_t *const superIn, const uint8_t r_255In, const uint8_t g_255In, const uint8_t b_255In);


// ********  local variable declarations *********


// ******** global function implementations ********
void cxa_rgbLed_triLed_init(cxa_rgbLed_triLed_t *const ledIn,
							cxa_led_t *const led_rIn, cxa_led_t *const led_gIn, cxa_led_t *const led_bIn,
							int threadIdIn)
{
	cxa_assert(ledIn);
	cxa_assert(led_rIn);
	cxa_assert(led_gIn);
	cxa_assert(led_bIn);

	// save our references
	ledIn->led_r = led_rIn;
	ledIn->led_g = led_gIn;
	ledIn->led_b = led_bIn;

	// initialize our superclass
	cxa_rgbLed_runLoop_init(&ledIn->super, scm_setRgb, threadIdIn)
;}


// ******** local function implementations ********
static void scm_setRgb(cxa_rgbLed_runLoop_t *const superIn, const uint8_t r_255In, const uint8_t g_255In, const uint8_t b_255In)
{
	cxa_rgbLed_triLed_t *const ledIn = (cxa_rgbLed_triLed_t *const)superIn;
	cxa_assert(ledIn);

	cxa_led_setSolid(ledIn->led_r, r_255In);
	cxa_led_setSolid(ledIn->led_g, g_255In);
	cxa_led_setSolid(ledIn->led_b, b_255In);
}
