#ifndef HEADER_FILE_SDB
#define HEADER_FILE_SDB
	
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <assert.h>
	
	/* constants */
	
	#define MAXIMUM_NAME_DESCRIPTOR 120
	
	/* type declarations */
	
	typedef enum types {
		UINT8,
		UINT32,
		INT32,
		CHAR,
		OBJ_PTR,
		FLOAT,
		DOUBLE,
		OBJ_STRUCT,
		VOID //VOID POINTER (void *)
	} types_t;
	
	typedef struct struct_field_ {
		char name[MAXIMUM_NAME_DESCRIPTOR];
		types_t data_type; //one of the enum types described by types_t
		unsigned int size;
		unsigned int offset;
		char name_structure[MAXIMUM_NAME_DESCRIPTOR]; //useful for OBJ_PTR and OBJ_STRUCT
	} struct_field_t;
	
	typedef struct _struct_record_t struct_record_t;
	
	struct _struct_record_t {
		struct_record_t *next; //pointer to the next struct record in the linked list
		char name[MAXIMUM_NAME_DESCRIPTOR];
		unsigned int size; //size of the structure
		unsigned int num_of_fields; //number of fields the structure contains
		struct_field_t *fields; //pointer to an array of fields
	};
	
	typedef struct struct_database_ {
		struct_record_t *head; //linked list of struct records
		unsigned int num_of_records;
	} struct_database_t;
	
	/* macro deffinitions */
	
	#define OFFSET_OF(struct_name, field_name)	\
		(unsigned int)&(((struct_name *)0)->field_name) //zero pointer to the begining to the struct till the field-name
		
	#define SIZE_OF_FIELD(struct_name, field_name)	\
		sizeof(((struct_name *)0)->field_name) //compute the size of the field within the struct argument
		
	#define FIELD_INFO(struct_name, field_name, type_name, nested_struct_name)	\
		{#field_name, 															\
		 type_name,																\
		 SIZE_OF_FIELD(struct_name, field_name),								\
		 OFFSET_OF(struct_name, field_name),									\
		 #nested_struct_name}													
	
	#define RECORD_INSERT(database_name, struct_name, fields_array)					\
		do {																		\
			struct_record_t *record = calloc(1, sizeof(struct_record_t));			\
			strncpy(record->name, #struct_name, MAXIMUM_NAME_DESCRIPTOR);			\
			record->size = sizeof(struct_name);										\
			record->num_of_fields = sizeof(fields_array) / sizeof(struct_field_t);	\
			record->fields = fields_array;											\
			if(struct_record_add(database_name, record)) {							\
				assert(0);										        			\
			}																		\
		} while(0);
		
	#define MLD_SUPPORT_PRIMITIVES(struct_database_name)		\
			RECORD_INSERT(struct_database_name, int, 0);		\
			RECORD_INSERT(struct_database_name, float, 0);		\
			RECORD_INSERT(struct_database_name, double, 0);
			
	#define ITERATE_STRUCT_RECORDS_BEGIN(struct_database, struct_ptr)					\
	{																					\
		for(struct_ptr = struct_database->head;											\
			struct_ptr;																	\
			struct_ptr = struct_ptr->next) {											\
	
	#define ITERATE_STRUCT_RECORDS_END }}
	
	/* function declarations */
	
	void struct_record_print(struct_record_t *record); //needs to further expand
	void struct_database_print(struct_database_t *database);
	
	int struct_record_add(struct_database_t *database, struct_record_t *new);
	struct_record_t* struct_database_lookup(struct_database_t *database, char *name);

#endif /* HEADER_FILE_SDB */