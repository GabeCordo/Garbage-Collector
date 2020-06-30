#ifndef HEADER_FILE_GC_API
#define HEADER_FILE_GC_API

	/* standard imports used by every header files in the mld project */
	#include <stdio.h>
	#include <stdlib.h>
	#include <assert.h>
	
	/* import all the functionality needed to store structs and primitives */
	#include "sdb.h"
	
	/* import all the functionality needed to store objects */
	#include "odb.h"
	
	/* type declarations */
	
	typedef enum debug_mode_ {
		DEBUG_FALSE,
		DEBUG_TRUE
	} debug_mode_t;
	
	/* function declarations */
	
	void mld_algorithm_trigger(object_database_t *database);
	void mld_algorithm_report(object_database_t *database, debug_mode_t debug_mode_active);

#endif /* HEADER_FILE_GC_API */