/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * @author Christopher Armenio
 */
#ifndef CXA_MQTT_MESSAGE_SUBSCRIBE_H_
#define CXA_MQTT_MESSAGE_SUBSCRIBE_H_


// ******** includes ********
#include <cxa_mqtt_message.h>


// ******** global macro definitions ********


// ******** global type definitions *********


// ******** global function prototypes ********
bool cxa_mqtt_message_subscribe_init(cxa_mqtt_message_t *const msgIn, uint16_t packetIdIn, char *const topicFilterIn, cxa_mqtt_qosLevel_t qosLevelIn);


/**
 * @protected
 */
bool cxa_mqtt_message_subscribe_validateReceivedBytes(cxa_mqtt_message_t *const msgIn);

#endif /* CXA_MQTT_MESSAGE_SUBSCRIBE_H_ */
