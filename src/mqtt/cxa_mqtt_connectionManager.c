/**
 * Copyright 2015 opencxa.org
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cxa_mqtt_connectionManager.h>


// ******** includes ********
#include <stdlib.h>
#include <string.h>
#include <cxa_assert.h>
#include <cxa_console.h>
#include <cxa_network_wifiManager.h>
#include <cxa_nvsManager.h>
#include <cxa_stateMachine.h>
#include <cxa_timeBase.h>
#include <cxa_uniqueId.h>

#include <cxa_config.h>

#define CXA_LOG_LEVEL					CXA_LOG_LEVEL_TRACE
#include <cxa_logger_implementation.h>


// ******** local macro definitions ********
#define SET_CREDS_TIMEOUT_MS					10000

#define NVSKEY_SERVER_CERT					"serverCert"
#define NVSKEY_CLIENT_CERT					"clientCert"
#define NVSKEY_CLIENT_KEY					"clientKey"


// ******** local type definitions ********
typedef enum
{
	STATE_WAIT_CREDS,
	STATE_ASSOCIATING,
	STATE_CONNECTING,
	STATE_CONNECTED,
	STATE_CONNECT_STANDOFF,
}state_t;


// ******** local function prototypes ********
static void wifiManCb_onAssociated(const char *const ssidIn, void* userVarIn);
static void wifiManCb_onUnassociated(void* userVarIn);

static void mqttClientCb_onConnect(cxa_mqtt_client_t *const clientIn, void* userVarIn);
static void mqttClientCb_onConnectFail(cxa_mqtt_client_t *const clientIn, cxa_mqtt_client_connectFailureReason_t reasonIn, void* userVarIn);
static void mqttClientCb_onDisconnect(cxa_mqtt_client_t *const clientIn, void* userVarIn);

static void stateCb_waitCredentials_enter(cxa_stateMachine_t *const smIn, int nextStateIdIn, void *userVarIn);
static void stateCb_waitCredentials_state(cxa_stateMachine_t *const smIn, void *userVarIn);
static void stateCb_associating_enter(cxa_stateMachine_t *const smIn, int nextStateIdIn, void *userVarIn);
static void stateCb_connecting_enter(cxa_stateMachine_t *const smIn, int nextStateIdIn, void *userVarIn);
static void stateCb_connected_enter(cxa_stateMachine_t *const smIn, int nextStateIdIn, void *userVarIn);
static void stateCb_connectStandOff_enter(cxa_stateMachine_t *const smIn, int nextStateIdIn, void *userVarIn);
static void stateCb_connectStandOff_state(cxa_stateMachine_t *const smIn, void *userVarIn);

static void consoleCb_areCredentialsSet(cxa_array_t *const argsIn, cxa_ioStream_t *const ioStreamIn, void* userVarIn);
static void consoleCb_clearCredentials(cxa_array_t *const argsIn, cxa_ioStream_t *const ioStreamIn, void* userVarIn);
static void consoleCb_setServerRootCertificate(cxa_array_t *const argsIn, cxa_ioStream_t *const ioStreamIn, void* userVarIn);
static void consoleCb_setClientCertificate(cxa_array_t *const argsIn, cxa_ioStream_t *const ioStreamIn, void* userVarIn);
static void consoleCb_setPrivateKey(cxa_array_t *const argsIn, cxa_ioStream_t *const ioStreamIn, void* userVarIn);

static bool receiveCString(cxa_ioStream_t *const ioStreamIn, uint8_t *const targetBufferIn, size_t expectedNumBytesIn, const char *const nvsKeyIn);


// ********  local variable declarations *********
static cxa_mqtt_client_network_t mqttClient;

static cxa_timeDiff_t td_connStandoff;
static uint32_t connStandoff_ms;

static cxa_stateMachine_t stateMachine;
static cxa_logger_t logger;

static char* mqtt_hostName;
static uint16_t mqtt_portNum;

static bool isSet_serverRootCert = false;
static char serverRootCert[1221+1];				// +1 is null term

static bool isSet_clientCert = false;
static char clientCert[1225+1];					// +1 is null term

static bool isSet_clientPrivateKey = false;
static char clientPrivateKey[1676+1];			// +1 is null term


// ******** global function implementations ********
void cxa_mqtt_connManager_init(char *const hostNameIn, uint16_t portNumIn, int threadIdIn)
{
	// save our references
	mqtt_hostName = hostNameIn;
	mqtt_portNum = portNumIn;

	// setup our connection standoff
	cxa_timeDiff_init(&td_connStandoff);
	srand(cxa_timeBase_getCount_us());

	// setup our logger
	cxa_logger_init(&logger, "connectionManager");

	// setup our state machine
	cxa_stateMachine_init(&stateMachine, "mqttConnMan", threadIdIn);
	cxa_stateMachine_addState(&stateMachine, STATE_ASSOCIATING, "assoc", stateCb_associating_enter, NULL, NULL, NULL);
	cxa_stateMachine_addState(&stateMachine, STATE_WAIT_CREDS, "waitForCreds", stateCb_waitCredentials_enter, stateCb_waitCredentials_state, NULL, NULL);
	cxa_stateMachine_addState(&stateMachine, STATE_CONNECTING, "connecting", stateCb_connecting_enter, NULL, NULL, NULL);
	cxa_stateMachine_addState(&stateMachine, STATE_CONNECTED, "connected", stateCb_connected_enter, NULL, NULL, NULL);
	cxa_stateMachine_addState(&stateMachine, STATE_CONNECT_STANDOFF, "standOff", stateCb_connectStandOff_enter, stateCb_connectStandOff_state, NULL, NULL);
	cxa_stateMachine_setInitialState(&stateMachine, STATE_ASSOCIATING);

	// setup our WiFi
	cxa_network_wifiManager_addListener(NULL, NULL, NULL, wifiManCb_onAssociated, wifiManCb_onUnassociated, NULL, NULL);

	// and our mqtt client
	cxa_mqtt_client_network_init(&mqttClient, cxa_uniqueId_getHexString(), threadIdIn);
	cxa_mqtt_client_addListener(&mqttClient.super, mqttClientCb_onConnect, mqttClientCb_onConnectFail, mqttClientCb_onDisconnect, NULL, NULL);

	// don't forget our console commands
	cxa_console_addCommand("mqtt_areCredsSet", "returns whether mqtt credentials are set", NULL, 0, consoleCb_areCredentialsSet, NULL);
	cxa_console_addCommand("mqtt_clearCreds", "clear credentials", NULL, 0, consoleCb_clearCredentials, NULL);
	cxa_console_addCommand("mqtt_setSrvCert", "sets the TLS root certificate (send bytes after cmd)", NULL, 0, consoleCb_setServerRootCertificate, NULL);
	cxa_console_addCommand("mqtt_setClCert", "sets the TLS client certificate (send bytes after cmd)", NULL, 0, consoleCb_setClientCertificate, NULL);
	cxa_console_addCommand("mqtt_setPrvKey", "sets the TLS private key (send bytes after cmd)", NULL, 0, consoleCb_setPrivateKey, NULL);

	// try to load our credentials
	isSet_serverRootCert = cxa_nvsManager_get_cString(NVSKEY_SERVER_CERT, serverRootCert, sizeof(serverRootCert));
	isSet_clientCert = cxa_nvsManager_get_cString(NVSKEY_CLIENT_CERT, clientCert, sizeof(clientCert));
	isSet_clientPrivateKey = cxa_nvsManager_get_cString(NVSKEY_CLIENT_KEY, clientPrivateKey, sizeof(clientPrivateKey));
}


void cxa_mqtt_connManager_setTlsCredentials(const char* serverRootCertIn, size_t serverRootCertLen_bytesIn,
		  	  	  	  	  	  	  	  	    const char* clientCertIn, size_t clientCertLen_bytesIn,
											const char* clientPrivateKeyIn, size_t clientPrivateKeyLen_bytesIn)
{
	cxa_assert(serverRootCertIn);
	cxa_assert(clientCertIn);
	cxa_assert(clientPrivateKeyIn);

	cxa_assert(serverRootCertLen_bytesIn == sizeof(serverRootCert));
	cxa_assert(clientCertLen_bytesIn == sizeof(clientCert));
	cxa_assert(clientPrivateKeyLen_bytesIn == sizeof(clientPrivateKey));

	// store our references
	memcpy(serverRootCert, serverRootCertIn, serverRootCertLen_bytesIn);
	memcpy(clientCert, clientCertIn, clientCertLen_bytesIn);
	memcpy(clientPrivateKey, clientPrivateKeyIn, clientPrivateKeyLen_bytesIn);

	isSet_serverRootCert = true;
	isSet_clientCert = true;
	isSet_clientPrivateKey = true;

	// if we're waiting for credentials, we're already associated...start connecting
	if( cxa_stateMachine_getCurrentState(&stateMachine) == STATE_WAIT_CREDS )
	{
		cxa_stateMachine_transition(&stateMachine, STATE_ASSOCIATING);
		return;
	}
}


bool cxa_mqtt_connManager_areCredentialsSet(void)
{
	return (isSet_clientCert && isSet_clientPrivateKey && isSet_serverRootCert);
}


cxa_mqtt_client_t* cxa_mqtt_connManager_getMqttClient(void)
{
	return &mqttClient.super;
}


// ******** local function implementations ********
static void wifiManCb_onAssociated(const char *const ssidIn, void* userVarIn)
{
	cxa_logger_info(&logger, "wifi associated");
	cxa_stateMachine_transition(&stateMachine, (cxa_mqtt_connManager_areCredentialsSet()) ? STATE_CONNECTING : STATE_WAIT_CREDS);
}


static void wifiManCb_onUnassociated(void* userVarIn)
{
	cxa_logger_warn(&logger, "wifi unassociated");

	// ensure we are disconnected regardless
	cxa_mqtt_client_disconnect(&mqttClient.super);

	cxa_stateMachine_transition(&stateMachine, STATE_ASSOCIATING);
}


static void mqttClientCb_onConnect(cxa_mqtt_client_t *const clientIn, void* userVarIn)
{
	cxa_stateMachine_transition(&stateMachine, STATE_CONNECTED);
}


static void mqttClientCb_onConnectFail(cxa_mqtt_client_t *const clientIn, cxa_mqtt_client_connectFailureReason_t reasonIn, void* userVarIn)
{
	if( cxa_stateMachine_getCurrentState(&stateMachine) == STATE_CONNECTING )
	{
		cxa_logger_warn(&logger, "connection failed: %d", reasonIn);
		cxa_stateMachine_transition(&stateMachine, STATE_CONNECT_STANDOFF);
	}
}


static void mqttClientCb_onDisconnect(cxa_mqtt_client_t *const clientIn, void* userVarIn)
{
	if( cxa_stateMachine_getCurrentState(&stateMachine) == STATE_CONNECTED )
	{
		cxa_logger_warn(&logger, "disconnected");
		cxa_stateMachine_transition(&stateMachine, STATE_CONNECT_STANDOFF);
	}
}


static void stateCb_waitCredentials_enter(cxa_stateMachine_t *const smIn, int nextStateIdIn, void *userVarIn)
{
	cxa_logger_info(&logger, "waiting for credentials");
}


static void stateCb_waitCredentials_state(cxa_stateMachine_t *const smIn, void *userVarIn)
{
	if( cxa_mqtt_connManager_areCredentialsSet() )
	{
		// make sure our credentials are committed before we transition
		cxa_logger_info(&logger, "credentials set, committing");
		cxa_nvsManager_commit();

		cxa_stateMachine_transition(&stateMachine, STATE_ASSOCIATING);
	}
}


static void stateCb_associating_enter(cxa_stateMachine_t *const smIn, int nextStateIdIn, void *userVarIn)
{
	cxa_logger_info(&logger, "associating");
	if( cxa_network_wifiManager_getState() == CXA_NETWORK_WIFISTATE_ASSOCIATED ) cxa_stateMachine_transition(&stateMachine, STATE_CONNECTING);
}


static void stateCb_connecting_enter(cxa_stateMachine_t *const smIn, int nextStateIdIn, void *userVarIn)
{
	cxa_logger_info(&logger, "connecting");

	// make _sure_we have credentials
	if( !cxa_mqtt_connManager_areCredentialsSet() )
	{
		cxa_stateMachine_transition(&stateMachine, STATE_WAIT_CREDS);
		return;
	}

	// if we made it here, we have the proper credentials...try to connect
	if( !cxa_mqtt_client_network_connectToHost_clientCert(&mqttClient, mqtt_hostName, mqtt_portNum,
														  serverRootCert, sizeof(serverRootCert),
														  clientCert, sizeof(clientCert),
														  clientPrivateKey, sizeof(clientPrivateKey)) )
	{
		cxa_logger_warn(&logger, "failed to start network connection");
		cxa_stateMachine_transition(&stateMachine, STATE_CONNECT_STANDOFF);
		return;
	}
}


static void stateCb_connected_enter(cxa_stateMachine_t *const smIn, int nextStateIdIn, void *userVarIn)
{
	cxa_logger_info(&logger, "connected");
}


static void stateCb_connectStandOff_enter(cxa_stateMachine_t *const smIn, int nextStateIdIn, void *userVarIn)
{
	connStandoff_ms = rand() % 1000 + 500;
	cxa_logger_info(&logger, "retry connection after %d ms", connStandoff_ms);
	cxa_timeDiff_setStartTime_now(&td_connStandoff);
}


static void stateCb_connectStandOff_state(cxa_stateMachine_t *const smIn, void *userVarIn)
{
	if( cxa_timeDiff_isElapsed_ms(&td_connStandoff, connStandoff_ms) )
	{
		cxa_stateMachine_transition(&stateMachine, STATE_CONNECTING);
	}
}


static void consoleCb_areCredentialsSet(cxa_array_t *const argsIn, cxa_ioStream_t *const ioStreamIn, void* userVarIn)
{
	if( cxa_stateMachine_getCurrentState(&stateMachine) == STATE_WAIT_CREDS )
	{
		cxa_ioStream_writeLine(ioStreamIn, "NO");
	}
	else
	{
		cxa_ioStream_writeLine(ioStreamIn, "YES");
	}
}


static void consoleCb_clearCredentials(cxa_array_t *const argsIn, cxa_ioStream_t *const ioStreamIn, void* userVarIn)
{
	cxa_nvsManager_erase(NVSKEY_SERVER_CERT);
	cxa_nvsManager_erase(NVSKEY_CLIENT_CERT);
	cxa_nvsManager_erase(NVSKEY_CLIENT_KEY);

	isSet_clientCert = false;
	isSet_clientPrivateKey = false;
	isSet_serverRootCert = false;

	// disconnect our client to force us to wait for credentials
	cxa_mqtt_client_disconnect(&mqttClient.super);
	cxa_ioStream_writeLine(ioStreamIn, "YES");
}


static void consoleCb_setServerRootCertificate(cxa_array_t *const argsIn, cxa_ioStream_t *const ioStreamIn, void* userVarIn)
{
	isSet_serverRootCert = receiveCString(ioStreamIn, (uint8_t*)serverRootCert, sizeof(serverRootCert), NVSKEY_SERVER_CERT);
}


static void consoleCb_setClientCertificate(cxa_array_t *const argsIn, cxa_ioStream_t *const ioStreamIn, void* userVarIn)
{
	isSet_clientCert = receiveCString(ioStreamIn, (uint8_t*)clientCert, sizeof(clientCert), NVSKEY_CLIENT_CERT);
}


static void consoleCb_setPrivateKey(cxa_array_t *const argsIn, cxa_ioStream_t *const ioStreamIn, void* userVarIn)
{
	isSet_clientPrivateKey = receiveCString(ioStreamIn, (uint8_t*)clientPrivateKey, sizeof(clientPrivateKey), NVSKEY_CLIENT_KEY);
}


static bool receiveCString(cxa_ioStream_t *const ioStreamIn, uint8_t *const targetBufferIn, size_t expectedNumBytesIn, const char *const nvsKeyIn)
{
	cxa_assert(ioStreamIn);
	cxa_assert(targetBufferIn);

	size_t numRxBytes = 0;
	cxa_timeDiff_t td_timeout;
	cxa_timeDiff_init(&td_timeout);

	uint8_t rxByte;
	bool isStringNullTerminated = false;
	while( numRxBytes < expectedNumBytesIn )
	{
		cxa_ioStream_readStatus_t rs = cxa_ioStream_readByte(ioStreamIn, &rxByte);
		if( rs == CXA_IOSTREAM_READSTAT_GOTDATA )
		{
			// save the byte
			targetBufferIn[numRxBytes++] = rxByte;

			// if we get a null byte, we're done
			if( rxByte == 0 )
			{
				isStringNullTerminated = true;
				break;
			}

			// if we made it here, continue receiving bytes
			cxa_timeDiff_setStartTime_now(&td_timeout);
		}
		else if( rs == CXA_IOSTREAM_READSTAT_ERROR )
		{
			cxa_ioStream_writeLine(ioStreamIn, "NO");
			return false;
		}

		// make sure we haven't timed out
		if( cxa_timeDiff_isElapsed_ms(&td_timeout, SET_CREDS_TIMEOUT_MS) )
		{
			cxa_ioStream_writeLine(ioStreamIn, "NO");
			return false;
		}
	}

	// if we made it here, we need to make sure that the string was null-terminated
	if( !isStringNullTerminated )
	{
		cxa_ioStream_writeLine(ioStreamIn, "NO");
		return false;
	}

	// if we made it here, we received the null-terminated c string...now store it to NVS
	if( !cxa_nvsManager_set_cString(nvsKeyIn, (char *const)targetBufferIn) )
	{
		cxa_ioStream_writeLine(ioStreamIn, "NO");
		return false;
	}

	// if we made it here, we were successful
	cxa_ioStream_writeLine(ioStreamIn, "YES");
	return true;
}
