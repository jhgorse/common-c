/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * @author Christopher Armenio
 */
#include "cxa_dummy_gpio.h"


// ******** includes ********
#include <cxa_assert.h>


#define CXA_LOG_LEVEL				CXA_LOG_LEVEL_TRACE
#include <cxa_logger_implementation.h>


// ******** local macro definitions ********


// ******** local type definitions ********


// ******** local function prototypes ********
static void scm_setDirection(cxa_gpio_t *const superIn, const cxa_gpio_direction_t dirIn);
static cxa_gpio_direction_t scm_getDirection(cxa_gpio_t *const superIn);
static void scm_setPolarity(cxa_gpio_t *const superIn, const cxa_gpio_polarity_t polarityIn);
static cxa_gpio_polarity_t scm_getPolarity(cxa_gpio_t *const superIn);
static void scm_setValue(cxa_gpio_t *const superIn, const bool valIn);
static bool scm_getValue(cxa_gpio_t *const superIn);


// ********  local variable declarations *********


// ******** global function implementations ********
void cxa_dummy_gpio_init_input(cxa_dummy_gpio_t *const gpioIn, const cxa_gpio_polarity_t polarityIn)
{
	cxa_assert(gpioIn);

	// save our references / internal state
	gpioIn->polarity = polarityIn;
	cxa_logger_init(&gpioIn->logger, "gpio");

	// initialize our super class
	cxa_gpio_init(&gpioIn->super, scm_setDirection, scm_getDirection, scm_setPolarity, scm_getPolarity, scm_setValue, scm_getValue, NULL);

	// set our initial direction
	cxa_gpio_setDirection(&gpioIn->super, CXA_GPIO_DIR_INPUT);
}


void cxa_dummy_gpio_init_output(cxa_dummy_gpio_t *const gpioIn, const cxa_gpio_polarity_t polarityIn, const bool initValIn)
{
	cxa_assert(gpioIn);

	// save our references / internal state
	gpioIn->polarity = polarityIn;
	cxa_logger_init(&gpioIn->logger, "gpio");

	// initialize our super class
	cxa_gpio_init(&gpioIn->super, scm_setDirection, scm_getDirection, scm_setPolarity, scm_getPolarity, scm_setValue, scm_getValue, NULL);

	// set our initial value (before we set direction to avoid glitches)
	cxa_gpio_setValue(&gpioIn->super, initValIn);

	// now set our direction
	cxa_gpio_setDirection(&gpioIn->super, CXA_GPIO_DIR_OUTPUT);
}


// ******** local function implementations ********
static void scm_setDirection(cxa_gpio_t *const superIn, const cxa_gpio_direction_t dirIn)
{
	cxa_assert(superIn);
	cxa_assert( (dirIn == CXA_GPIO_DIR_INPUT) ||
				(dirIn == CXA_GPIO_DIR_OUTPUT) );

	// get a pointer to our class
	cxa_dummy_gpio_t *const gpioIn = (cxa_dummy_gpio_t *const)superIn;

	cxa_logger_trace(&gpioIn->logger, "new direction: %s", (dirIn == CXA_GPIO_DIR_INPUT) ? "input" : "output");
	gpioIn->dir = dirIn;

	// TODO: set your direction here
}


static cxa_gpio_direction_t scm_getDirection(cxa_gpio_t *const superIn)
{
	cxa_assert(superIn);

	// get a pointer to our class
	cxa_dummy_gpio_t *const gpioIn = (cxa_dummy_gpio_t *const)superIn;

	return gpioIn->dir;
}


static void scm_setPolarity(cxa_gpio_t *const superIn, const cxa_gpio_polarity_t polarityIn)
{
	cxa_assert(superIn);
	cxa_assert( (polarityIn == CXA_GPIO_POLARITY_NONINVERTED) ||
				(polarityIn == CXA_GPIO_POLARITY_INVERTED) );

	// get a pointer to our class
	cxa_dummy_gpio_t *const gpioIn = (cxa_dummy_gpio_t *const)superIn;

	gpioIn->polarity = polarityIn;
}


static cxa_gpio_polarity_t scm_getPolarity(cxa_gpio_t *const superIn)
{
	cxa_assert(superIn);

	// get a pointer to our class
	cxa_dummy_gpio_t *const gpioIn = (cxa_dummy_gpio_t *const)superIn;

	return gpioIn->polarity;
}


static void scm_setValue(cxa_gpio_t *const superIn, const bool valIn)
{
	cxa_assert(superIn);

	// get a pointer to our class
	cxa_dummy_gpio_t *const gpioIn = (cxa_dummy_gpio_t *const)superIn;

	cxa_logger_trace(&gpioIn->logger, "new logic value: %d", valIn);

	// TODO: set your value here
}


static bool scm_getValue(cxa_gpio_t *const superIn)
{
	cxa_assert(superIn);

	// get a pointer to our class
	cxa_dummy_gpio_t *const gpioIn = (cxa_dummy_gpio_t *const)superIn;

	// TODO: return your GPIO logic value (don't forget to adjust for polarity)

	return 0;
}
