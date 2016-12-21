/**
 * @file
 * @copyright 2016 opencxa.org
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author Christopher Armenio
 */
#ifndef CXA_ESP32_BTLE_CLIENT_H_
#define CXA_ESP32_BTLE_CLIENT_H_


// ******** includes ********
#include <cxa_btle_client.h>


// ******** global macro definitions ********


// ******** global type definitions *********
typedef struct
{
	cxa_btle_client_t super;
}cxa_esp32_btle_client_t;


// ******** global function prototypes ********
cxa_btle_client_t* cxa_esp32_btle_client_getSingleton(void);

#endif