#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdb.h"

/*
 * Function:  struct_record_print
 * ------------------------------
 * prints the data within the struct record in a
 * visually apealing format for debugging
 *
 *  *record: a pointer to the record type
 */
void struct_record_print(struct_record_t *record) {
	
	printf("Record (%p)", &record);
	printf("%s\t| size = %i\t| fields = %i", record->name, record->size, record->num_of_fields);
	
}

/*
 * Function:  struct_database_print
 * --------------------------------
 * prints the data within the struct database in a
 * visually apealing format for debugging with as-
 * istance of the struct_record_print function
 *
 *  *database: the database we will iterate over for
 *			   each record we will print
 */
void struct_database_print(struct_database_t *database) {
	
	printf("# of Structures Registered = %i", database->num_of_records);
	
	struct_record_t *struct_ptr = NULL;
	ITERATE_STRUCT_RECORDS_BEGIN(database, struct_ptr) {
		
		struct_record_print(struct_ptr);
		
	} ITERATE_STRUCT_RECORDS_END;
		
}

/*
 * Function:  struct_record_add
 * ----------------------------
 * adds a new record into the structure database, ensuring
 * that the appropriate pointers will be changed where re-
 * quired
 *
 *  *database: the struct database we will add the new record to
 *	*new: the new struct record we will add to the list of records
 *
 *  returns: 0, which will avoid the macro asserting a failure
 */
int struct_record_add(struct_database_t *database, struct_record_t *new) {
	
	struct_record_t *head = database->head;
	//check to see if the struct database has a head
	if (!head) {
		database->head = new;
		new->next = NULL;
		database->num_of_records++;
	}
	
	//else replace the new record as the head
	new->next = head;
	database->head = new;
	database->num_of_records++;
	return 0;
}

/*
 * Function:  struct_database_lookup
 * -----------------------------------------
 * looks for a given record within the structures database
 * by matching the array of chars argument with the one
 * stored within the record type
 *
 *  *database: the struct database we will be looking in
 *	*name: the name of the struct record to lookup
 *
 *  returns: the pointer to the struct record that matches
 *			 the name argument passed to the function
 */
struct_record_t* struct_database_lookup(struct_database_t *database, char *name) {
	
	struct_record_t *struct_ptr = NULL;
	ITERATE_STRUCT_RECORDS_BEGIN(database, struct_ptr) {
		
		if (!strcmp(name, struct_ptr->name))
			return struct_ptr;
		
	} ITERATE_STRUCT_RECORDS_END;
	
	return NULL;
}