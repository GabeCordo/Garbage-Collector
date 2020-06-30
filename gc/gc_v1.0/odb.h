#ifndef HEADER_FILE_ODB
#define HEADER_FILE_ODB

	#include "sdb.h"

	/* type deffinitions */
	
	typedef struct _object_record_t object_record_t;
		
	struct _object_record_t {
		object_record_t *next;
		void *memory;
		unsigned int units;
		struct_record_t *struct_record;
	};
		
	typedef struct object_database_ {
		struct_database_t *struct_database;
		object_record_t *head;
		unsigned int num_of_records;
	} object_database_t;
	
	/* macro deffinitions */
	
	#define ITERATE_DB_OBJECTS_BEGIN(database, record_ptr)	\
	{														\
		object_record_t *node = NULL, *next = NULL;		\
		for(node = database->head; node; node = next) {		\
			record_ptr = node;								\
			next = node->next;
	
	#define ITERATE_DB_OBJECT_END }}
	
	/* function definitions */
	
	void *xcalloc(object_database_t *database, char *struct_name, int units);
	void xfree(object_database_t *database, void *ptr);
	
#endif /* HEADER_FILE_ODB */