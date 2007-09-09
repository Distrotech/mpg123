/*
	legacy_module.c: dummy interface to modular code loader for legacy build system

	copyright 2007 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://www.mpg123.org
	initially written by Nicholas J Humfrey
*/

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "debug.h"
#include "module.h"


/* This is implemented in the module that is staticly compiled in */
extern mpg123_module_t mpg123_output_module_info;


/* Open a module */
mpg123_module_t*
open_module( const char* type, const char* name )
{
	mpg123_module_t *mod = NULL;
	
	/* Select the module info structure, based on the desired type */
	if (strcmp(type, "output")==0) {
		mod = &mpg123_output_module_info;
	} else {
		error1("Unable to open module type '%s'.", type);
		return NULL;
	}
	
	/* Check the module compiled in is the module requested */
	if (strcmp(name, mod->name)!=0) {
		error1("Unable to open requesed module '%s'.", name);
		error1("The only available staticly compiled module is '%s'.", mod->name);
		return NULL;
	}
	
	// Debugging info
	debug1("Details of static module type '%s':", type);
	debug1("  api_version=%d", mod->api_version);
	debug1("  name=%s", mod->name);
	debug1("  description=%s", mod->description);
	debug1("  revision=%s", mod->revision);
	debug1("  handle=%x", (unsigned int)mod->handle);

	return mod;
}


void close_module( mpg123_module_t* module )
{
	debug("close_module()");
}


void list_modules()
{
	debug("list_modules()" );
}


