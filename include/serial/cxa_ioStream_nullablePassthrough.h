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
#ifndef CXA_IOSTREAM_NULLABLE_PASSTHROUGH_H_
#define CXA_IOSTREAM_NULLABLE_PASSTHROUGH_H_


// ******** includes ********
#include <cxa_ioStream.h>


// ******** global macro definitions ********


// ******** global type definitions *********
typedef struct
{
	cxa_ioStream_t nonnullStream;
	cxa_ioStream_t* nullableStream;

	size_t numBytesWritten;
}cxa_ioStream_nullablePassthrough_t;


// ******** global function prototypes ********
void cxa_ioStream_nullablePassthrough_init(cxa_ioStream_nullablePassthrough_t *const npIn);

cxa_ioStream_t* cxa_ioStream_nullablePassthrough_getNonullStream(cxa_ioStream_nullablePassthrough_t *const npIn);

cxa_ioStream_t* cxa_ioStream_nullablePassthrough_getNullableStream(cxa_ioStream_nullablePassthrough_t *const npIn);
void cxa_ioStream_nullablePassthrough_setNullableStream(cxa_ioStream_nullablePassthrough_t *const npIn,
														cxa_ioStream_t *const ioStreamIn);

size_t cxa_ioStream_nullablePassthrough_getNumBytesWritten(cxa_ioStream_nullablePassthrough_t *const npIn);
void cxa_ioStream_nullablePassthrough_resetNumByesWritten(cxa_ioStream_nullablePassthrough_t *const npIn);


#endif
