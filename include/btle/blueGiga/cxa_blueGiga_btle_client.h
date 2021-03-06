/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * @author Christopher Armenio
 */
#ifndef CXA_BLUEGIGA_BTLE_CLIENT_H_
#define CXA_BLUEGIGA_BTLE_CLIENT_H_


// ******** includes ********
#include <cxa_blueGiga_gpio.h>
#include <cxa_blueGiga_i2cMaster.h>
#include <cxa_blueGiga_types.h>
#include <cxa_btle_uuid.h>
#include <cxa_fixedByteBuffer.h>
#include <cxa_gpio.h>
#include <cxa_i2cMaster.h>
#include <cxa_ioStream.h>
#include <cxa_logger_header.h>
#include <cxa_protocolParser_bgapi.h>
#include <cxa_softWatchDog.h>
#include <cxa_stateMachine.h>
#include <cxa_btle_central.h>


// ******** global macro definitions ********
#define CXA_BLUEGIGA_BTLE_MAX_PACKET_SIZE		64


#ifndef CXA_BLUEGIGA_BTLE_MAX_NUM_GPIOS
	#define CXA_BLUEGIGA_BTLE_MAX_NUM_GPIOS		2
#endif


// ******** global type definitions *********
/**
 * @public
 */
typedef struct cxa_blueGiga_btle_client cxa_blueGiga_btle_client_t;


/**
 * @private
 */
typedef enum
{
	CXA_BLUEGIGA_BTLE_PROCEDURE_TYPE_READ,
	CXA_BLUEGIGA_BTLE_PROCEDURE_TYPE_WRITE
}cxa_blueGiga_btle_client_procedureType_t;


/**
 * @private
 */
typedef void (*cxa_blueGiga_btle_client_cb_onResponse_t)(cxa_blueGiga_btle_client_t *const btlecIn, bool wasSuccessfulIn, cxa_fixedByteBuffer_t *const payloadIn, void* userVarIn);


/**
 * @private
 */
struct cxa_blueGiga_btle_client
{
	cxa_btle_central_t super;

	cxa_gpio_t *gpio_reset;
	cxa_protocolParser_bgapi_t protoParse;

	cxa_fixedByteBuffer_t fbb_rx;
	uint8_t fbb_rx_raw[CXA_BLUEGIGA_BTLE_MAX_PACKET_SIZE];

	cxa_fixedByteBuffer_t fbb_tx;
	uint8_t fbb_tx_raw[CXA_BLUEGIGA_BTLE_MAX_PACKET_SIZE];

	struct
	{
		cxa_softWatchDog_t watchdog;

		cxa_blueGiga_classId_t classId;
		cxa_blueGiga_methodId_t methodId;

		cxa_blueGiga_btle_client_cb_onResponse_t cb_onResponse;
		void* userVar;
	}inFlightRequest;

	bool hasBootFailed;
	bool isActiveScan;

	cxa_blueGiga_gpio_t gpios[CXA_BLUEGIGA_BTLE_MAX_NUM_GPIOS];
	cxa_blueGiga_i2cMaster_t i2cMaster;

	cxa_stateMachine_t stateMachine_conn;

	cxa_logger_t logger;
};


// ******** global function prototypes ********
void cxa_blueGiga_btle_client_init(cxa_blueGiga_btle_client_t *const btlecIn, cxa_ioStream_t *const iosIn, cxa_gpio_t *const gpio_resetIn, int threadIdIn);

bool cxa_blueGiga_btle_client_sendCommand(cxa_blueGiga_btle_client_t *const btlecIn,
										  cxa_blueGiga_classId_t classIdIn, cxa_blueGiga_methodId_t methodIdIn, cxa_fixedByteBuffer_t *const payloadIn,
										  cxa_blueGiga_btle_client_cb_onResponse_t cb_onResponseIn, void* userVarIn);

cxa_gpio_t* cxa_blueGiga_btle_client_getGpio(cxa_blueGiga_btle_client_t *const btlecIn, uint8_t portNumIn, uint8_t chanNumIn);

cxa_i2cMaster_t* cxa_blueGiga_btle_client_getI2cMaster(cxa_blueGiga_btle_client_t *const btlecIn);

#endif
