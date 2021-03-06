/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * @author Christopher Armenio
 */
#include "cxa_consoleMenu_menu.h"


// ******** includes ********
#include <string.h>
#include <cxa_assert.h>


// ******** local macro definitions ********
#define MAX_NAME_LEN_CHARS					20


// ******** local type definitions ********


// ******** local function prototypes ********


// ********  local variable declarations *********


// ******** global function implementations ********
void cxa_consoleMenu_menu_init(cxa_consoleMenu_menu_t *const cmmIn)
{
	cxa_assert(cmmIn);

	// setup our internal state
	cxa_array_init(&cmmIn->entries, sizeof(*cmmIn->entries_raw), (void*)cmmIn->entries_raw, sizeof(cmmIn->entries_raw));
}


void cxa_consoleMenu_menu_addItem_callback(cxa_consoleMenu_menu_t *const cmmIn, const char *nameIn, cxa_consoleMenu_menu_itemCb_t cbIn, void *userVarIn)
{
	cxa_assert(cmmIn);
	cxa_assert(nameIn);

	// make sure our name is an appropriate length
	size_t nameLen_chars = strlen(nameIn);
	cxa_assert( (nameLen_chars > 0) && (nameLen_chars <= MAX_NAME_LEN_CHARS) );

	// setup our new item entry
	cxa_consoleMenu_menu_itemEntry_t newItemEntry = {.name=nameIn, .type=CXA_CM_MENU_ITEM_TYPE_CALLBACK, .cb=cbIn, .userVar=userVarIn};

	// add to our array
	cxa_assert(cxa_array_append(&cmmIn->entries, &newItemEntry));
}


void cxa_consoleMenu_menu_addItem_subMenu(cxa_consoleMenu_menu_t *const parentCmmIn, const char *nameIn, cxa_consoleMenu_menu_t *const childCmmIn)
{
	cxa_assert(parentCmmIn);
	cxa_assert(nameIn);
	cxa_assert(childCmmIn);

	// make sure our name is an appropriate length
	size_t nameLen_chars = strlen(nameIn);
	cxa_assert( (nameLen_chars > 0) && (nameLen_chars <= MAX_NAME_LEN_CHARS) );

	// setup our new item entry
	cxa_consoleMenu_menu_itemEntry_t newItemEntry = {.name=nameIn, .type=CXA_CM_MENU_ITEM_TYPE_SUBMENU, .subMenu=childCmmIn};

	// add to our array and set our parent menu for our child
	cxa_assert(cxa_array_append(&parentCmmIn->entries, &newItemEntry));
	childCmmIn->parentMenu = (void*)parentCmmIn;
}


// ******** local function implementations ********
