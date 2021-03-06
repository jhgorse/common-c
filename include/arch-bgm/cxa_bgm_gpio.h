/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * @author Christopher Armenio
 */
#ifndef CXA_BGM_GPIO_H_
#define CXA_BGM_GPIO_H_


// ******** includes ********
#include <cxa_gpio.h>

#include <em_gpio.h>


// ******** global macro definitions ********


// ******** global type definitions *********
/**
 * @public
 * @brief "Forward" declaration of the cxa_bgm_gpio_t object
 */
typedef struct cxa_bgm_gpio cxa_bgm_gpio_t;


/**
 * @private
 */
struct cxa_bgm_gpio
{
	cxa_gpio_t super;

	cxa_gpio_polarity_t polarity;
	cxa_gpio_direction_t dir;

	GPIO_Port_TypeDef port;
	unsigned int pinNum;

	bool lastVal;

	cxa_gpio_cb_onInterrupt_t cb_interrupt;
	void* cb_interrupt_userVar;
};


// ******** global function prototypes ********
/**
 * @public
 * @brief Initializes the bgm GPIO object for immediate use as an input.
 *
 * @param[in] gpioIn pointer to a pre-allocated GPIO object
 * @param[in] portNumIn port number of the target pin
 * @param[in] pinNumIn pin number describing which pin to use
 * @param[in] polarityIn the polarity of the GPIO pin. See @ref cxa_gpio.h for a discussion of polarity.
 */
void cxa_bgm_gpio_init_input(cxa_bgm_gpio_t *const gpioIn,
							 const GPIO_Port_TypeDef portNumIn, const unsigned int pinNumIn,
							 const cxa_gpio_polarity_t polarityIn);


/**
 * @public
 * @brief Initializes the bgm GPIO object for immediate use as an output.
 *
 * @param[in] gpioIn pointer to a pre-allocated bgm GPIO object
 * @param[in] portNumIn port number of the target pin
 * @param[in] pinNumIn pin number describing which pin to use
 * @param[in] polarityIn the polarity of the GPIO pin. See @ref cxa_gpio.h for a discussion of polarity.
 * @param[in] initValIn the initial logical value of the output pin. This initial value is written
 *		before setting the direction of the pin. Assuming the pin has not been previously initialized
 *		(fresh boot of the processor) it should be configured as a high-impedance input. Thus, the
 *		pin will transition directly from a high-impedance input to the specified logic value.
 */
void cxa_bgm_gpio_init_output(cxa_bgm_gpio_t *const gpioIn,
							  const GPIO_Port_TypeDef portNumIn, const unsigned int pinNumIn,
							  const cxa_gpio_polarity_t polarityIn, const bool initValIn);


/**
 * @public
 * @brief Initializes the bgm GPIO object for later use.
 * This function simply stores the bgm pin number.
 * <b>It does not perform any actions which will change the current state/direction/polarity of the pin</b>
 * If a pin is configured using this function, it is recommended to call the following functions before use:
 *     1. cxa_gpio_setPolarity
 *     2. cxa_gpio_setValue			(only if configuring as an output)
 *     3. cxa_gpio_setDirection
 *
 * @param[in] gpioIn pointer to a pre-allocated bgm GPIO object
 * @param[in] portNumIn port number of the target pin
 * @param[in] pinNumIn pin number describing which pin to use
 */
void cxa_bgm_gpio_init_safe(cxa_bgm_gpio_t *const gpioIn,
							const GPIO_Port_TypeDef portNumIn, const unsigned int pinNumIn);


#endif
