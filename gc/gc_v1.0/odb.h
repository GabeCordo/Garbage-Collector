#ifndef HEADER_FILE_ODB
#define HEADER_FILE_ODB

	#include <stdio.h>
	#include <stdlib.h>
	#include <assert.h>
	#include "sdb.h"

	/* type declarations */
	
	typedef enum obj_bool_ {
		ROOT_FALSE,
		ROOT_TRUE
	} obj_bool_t;
	
	typedef enum mld_visited_ {
		VISITED_FALSE,
		VISITED_TRUE
	} mld_visited_t;
	
	typedef struct _object_record_t object_record_t;
		
	struct _object_record_t {
		object_record_t *next;
		void *memory;
		unsigned int units;
		struct_record_t *struct_record;
		obj_bool_t is_root;
		mld_visited_t is_visited;
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
	
	/* function declarations */
	
	object_record_t *object_database_lookup(object_database_t *database, void *ptr);
	
	void object_database_add_root(object_database_t *database, void *ptr, char *struct_name, int units);
	void object_record_set_as_root(object_database_t *database, void *obj_ptr);
	
	void object_database_add(object_database_t *database, void *ptr, int units, struct_record_t *struct_rec, obj_bool_t is_root);
	void *allocate(object_database_t *database, char *struct_name, int units);
	
	void object_database_rem(object_database_t *database, object_record_t *record);
	void deallocate(object_database_t *database, void *ptr);
	
#endif /* HEADER_FILE_ODB */