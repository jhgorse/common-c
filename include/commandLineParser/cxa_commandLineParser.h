/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * @author Christopher Armenio
 */
#ifndef CXA_COMMAND_LINE_PARSER_H_
#define CXA_COMMAND_LINE_PARSER_H_


// ******** includes ********
#include <stdbool.h>
#include <cxa_array.h>
#include <cxa_stringUtils.h>
#include <cxa_config.h>


// ******** global macro definitions ********
#ifndef CXA_COMMAND_LINE_PARSER_MAXNUM_OPTIONS
	#define CXA_COMMAND_LINE_PARSER_MAXNUM_OPTIONS		10
#endif


// ******** global type definitions *********
/**
 * @public
 * @brief "Forward" declaration of the cxa_commandLineParser_t object
 */
typedef struct cxa_commandLineParser cxa_commandLineParser_t;


typedef void (*cxa_commandLineParser_cb_noArg_t)(cxa_commandLineParser_t *const clpIn, void * userVarIn);
typedef void (*cxa_commandLineParser_cb_arg_t)(cxa_commandLineParser_t *const clpIn, cxa_stringUtils_parseResult_t *argIn, void *userVarIn);


/**
 * @private
 */
typedef struct
{
	char* shortOpt;
	char* longOpt;
	char* description;
	bool isRequired;

	bool hasArgument;
	bool isArgumentRequired;
	cxa_stringUtils_dataType_t expectedArgType;

	void* userVar;
	cxa_commandLineParser_cb_arg_t cb_arg;
	cxa_commandLineParser_cb_noArg_t cb_noArg;
}cxa_commandLineParser_optionEntry_t;


/**
 * @private
 */
struct cxa_commandLineParser
{
	char* progName;
	char* description;

	cxa_array_t options;
	cxa_commandLineParser_optionEntry_t options_raw[CXA_COMMAND_LINE_PARSER_MAXNUM_OPTIONS];
};


// ******** global function prototypes ********
void cxa_commandLineParser_init(cxa_commandLineParser_t *const clpIn, char *const progNameIn, char *const descriptionIn);

bool cxa_commandLineParser_addOption_noArg(cxa_commandLineParser_t *const clpIn,
										   char *const shortOptIn, char *const longOptIn, char *const descIn, bool isRequiredIn,
										   cxa_commandLineParser_cb_noArg_t cbIn, void *const userVarIn);
bool cxa_commandLineParser_addOption_arg(cxa_commandLineParser_t *const clpIn,
										 char *const shortOptIn, char *const longOptIn, char *const descIn, bool isRequiredIn,
										 bool isArgumentRequiredIn, cxa_stringUtils_dataType_t expectedArgumentTypeIn,
										 cxa_commandLineParser_cb_arg_t cbIn, void *const userVarIn);

void cxa_commandLineParser_printUsage(cxa_commandLineParser_t *const clpIn);

bool cxa_commandLineParser_parseOptions(cxa_commandLineParser_t *const clpIn, int argc, char* argv[]);



#endif // CXA_COMMAND_LINE_PARSER_H_
