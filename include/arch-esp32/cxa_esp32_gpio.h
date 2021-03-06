/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * @author Christopher Armenio
 */
#ifndef CXA_ESP32_GPIO_H_
#define CXA_ESP32_GPIO_H_


// ******** includes ********
#include <cxa_gpio.h>
#include <driver/gpio.h>


// ******** global macro definitions ********


// ******** global type definitions *********
/**
 * @public
 * @brief "Forward" declaration of the cxa_esp32_gpio_t object
 */
typedef struct cxa_esp32_gpio cxa_esp32_gpio_t;


/**
 * @private
 */
struct cxa_esp32_gpio
{
	cxa_gpio_t super;

	cxa_gpio_polarity_t polarity;
	cxa_gpio_direction_t dir;

	gpio_num_t pinNum;

	bool lastVal;
};


// ******** global function prototypes ********
/**
 * @public
 * @brief Initializes the esp32 GPIO object for immediate use as an input.
 *
 * @param[in] gpioIn pointer to a pre-allocated esp32 GPIO object
 * @param[in] pinNumIn pin number describing which pin to use
 * @param[in] polarityIn the polarity of the GPIO pin. See @ref cxa_gpio.h for a discussion of polarity.
 */
void cxa_esp32_gpio_init_input(cxa_esp32_gpio_t *const gpioIn, const gpio_num_t pinNumIn, const cxa_gpio_polarity_t polarityIn);


/**
 * @public
 * @brief Initializes the esp32 GPIO object for immediate use as an output.
 *
 * @param[in] gpioIn pointer to a pre-allocated esp32 GPIO object
 * @param[in] pinNumIn pin number describing which pin to use
 * @param[in] polarityIn the polarity of the GPIO pin. See @ref cxa_gpio.h for a discussion of polarity.
 * @param[in] initValIn the initial logical value of the output pin. This initial value is written
 *		before setting the direction of the pin. Assuming the pin has not been previously initialized
 *		(fresh boot of the processor) it should be configured as a high-impedance input. Thus, the
 *		pin will transition directly from a high-impedance input to the specified logic value.
 */
void cxa_esp32_gpio_init_output(cxa_esp32_gpio_t *const gpioIn, const gpio_num_t pinNumIn, const cxa_gpio_polarity_t polarityIn, const bool initValIn);


/**
 * @public
 * @brief Initializes the esp32 GPIO object for later use.
 * This function simply stores the esp32 pin number.
 * <b>It does not perform any actions which will change the current state/direction/polarity of the pin</b>
 * If a pin is configured using this function, it is recommended to call the following functions before use:
 *     1. cxa_gpio_setPolarity
 *     2. cxa_gpio_setValue			(only if configuring as an output)
 *     3. cxa_gpio_setDirection
 *
 * @param[in] gpioIn pointer to a pre-allocated esp32 GPIO object
 * @param[in] pinNumIn pin number describing which pin to use
 */
void cxa_esp32_gpio_init_safe(cxa_esp32_gpio_t *const gpioIn, const gpio_num_t pinNumIn);


/**
 * @public
 * @brief Sets the pullup/pull down mode of the pin
 */
void cxa_esp32_gpio_setPullMode(cxa_esp32_gpio_t *const gpioIn, gpio_pull_mode_t pullModeIn);


#endif
