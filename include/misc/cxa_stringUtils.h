/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * @author Christopher Armenio
 */
#ifndef CXA_STRINGUTILS_H_
#define CXA_STRINGUTILS_H_


/**
 * @file
 * This is a file which contains utility functions for manipulating C-strings
 *
 * @author Christopher Armenio
 */


// ******** includes ********
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <cxa_config.h>
#include <cxa_fixedByteBuffer.h>


// ******** global macro definitions ********
#define CXA_STRINGUTILS_NULL2EMPTY(strIn)				(((strIn) == NULL) ? "" : (strIn))

#ifndef CXA_LINE_ENDING
	#define CXA_LINE_ENDING			"\r\n"
#endif


// ******** global type definitions *********
typedef enum
{
	CXA_STRINGUTILS_DATATYPE_UNKNOWN,
	CXA_STRINGUTILS_DATATYPE_INTEGER,
	CXA_STRINGUTILS_DATATYPE_DOUBLE,
	CXA_STRINGUTILS_DATATYPE_STRING
}cxa_stringUtils_dataType_t;


typedef struct
{
	cxa_stringUtils_dataType_t dataType;

	union
	{
		unsigned long val_uint;
		long val_int;
		double val_double;
		char* val_string;
	};
}cxa_stringUtils_parseResult_t;


// ******** global function prototypes ********
bool cxa_stringUtils_startsWith(const char* targetStringIn, const char* prefixStringIn);
bool cxa_stringUtils_startsWith_withLengths(const char* targetStringIn, size_t targetStringLen_bytesIn, const char* prefixStringIn, size_t prefixStringLen_bytesIn);
bool cxa_stringUtils_endsWith_withLengths(const char* targetStringIn, size_t targetStringLen_bytesIn, const char* suffixStringIn);

bool cxa_stringUtils_contains(const char* targetStringIn, const char* elementIn);
bool cxa_stringUtils_contains_withLengths(const char* targetStringIn, size_t targetStringLen_bytesIn, const char* elementIn, size_t elementLen_bytesIn);

bool cxa_stringUtils_concat(char *targetStringIn, const char *sourceStringIn, size_t maxSizeTarget_bytesIn);
bool cxa_stringUtils_concat_formattedString(char *targetStringIn, size_t maxSizeTarget_bytesIn, const char *fmtIn, ...);
bool cxa_stringUtils_concat_withLengths(char *targetStringIn, size_t maxSizeTarget_bytesIn, const char *sourceStringIn, size_t sourceStringLen_bytesIn);

bool cxa_stringUtils_copy(char *targetStringIn, const char* sourceStringIn, size_t sizeofTargetStringIn);

/**
 *
 * @param targetStringIn
 * @param maxSize_bytesIn maximum _buffer_ size (or maxStrLen-1)
 * @param stringLen_bytesOut
 * @return
 */
bool cxa_stringUtils_strlen(const char *targetStringIn, size_t maxSize_bytesIn, size_t* stringLen_bytesOut);

bool cxa_stringUtils_equals(const char* str1In, const char* str2In);
bool cxa_stringUtils_equals_withLengths(const char* str1In, size_t str1Len_bytes, const char* str2In, size_t str2Len_bytes);
bool cxa_stringUtils_equals_ignoreCase(const char* str1In, const char* str2In);

size_t cxa_stringUtils_countOccurences(const char* targetStringIn, const char* elementIn);
size_t cxa_stringUtils_countOccurences_withLengths(const char* targetStringIn, size_t targetStringLen_bytesIn, const char* elementIn, size_t elementStringLen_bytesIn);
ssize_t cxa_stringUtils_indexOfFirstOccurence_withLengths(const char* targetStringIn, size_t targetStringLen_bytesIn, const char* elementIn, size_t elementLen_bytesIn);

char* cxa_stringUtils_getLastCharacters(const char* targetStringIn, size_t numCharsIn);

bool cxa_stringUtils_replaceFirstOccurence(const char *targetStringIn, const char *stringToReplaceIn, const char *replacementStringIn);
bool cxa_stringUtils_replaceFirstOccurence_withLengths(const char *targetStringIn, size_t targetStringLen_bytesIn,
													   const char *stringToReplaceIn, size_t stringToReplaceLen_bytesIn,
													   const char *replacementStringIn, size_t replacementStringLen_bytesIn);

void cxa_stringUtils_trim(char *const targetStringIn);

bool cxa_stringUtils_bytesToHexString(uint8_t* bytesIn, size_t numBytesIn, bool transposeIn, char* hexStringOut, size_t maxLenHexString_bytesIn);
bool cxa_stringUtils_hexStringToBytes(const char *const hexStringIn, size_t numBytesIn, bool transposeIn, uint8_t* bytesOut);

bool cxa_stringUtils_ipStringToUint32(const char *const ipStringIn, uint32_t *const ipBytesOut);

bool cxa_stringUtils_parseString(char *const strIn, cxa_stringUtils_parseResult_t* parseResultOut);
const char* cxa_stringUtils_getStringForDataType(cxa_stringUtils_dataType_t dataTypeIn);



#endif // CXA_STRINGUTILS_H_
