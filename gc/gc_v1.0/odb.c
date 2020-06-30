#include "odb.h"

/*
 * Function:  object_database_lookup
 * ---------------------------------
 * looks up an object record within the object database using the
 * pointer to the object memory as a reference (point of comparison)
 *
 *  *database: the object database we will look for a record
 *	*ptr: a pointer to the dynamic object
 *
 *  returns: a pointer to the object database record that contains the
 *			 memory address to the pointer argument
 *	exception: if no memory address matched a record, returns NULL
 */
object_record_t *object_database_lookup(object_database_t *database, void *ptr) {
	
	object_record_t *placeholder = NULL;
	ITERATE_DB_OBJECTS_BEGIN(database, placeholder) {
		if (placeholder->memory == ptr) {
			return placeholder;
		}
	} ITERATE_DB_OBJECT_END;
	return NULL;
}


/*
 * Function:  object_database_add
 * ------------------------------
 * looks up an object record within the object database using the
 * pointer to the object memory as a reference (point of comparison)
 *
 *  *database: the object database we will look for a record
 *	*ptr: a pointer to the dynamic object
 *	units: the number of objects we are callocing
 *	*struct_rec: the struct record we are adding
 *	is_root: ROOT_TRUE for Global variables/object, else ROOT_FALSE
 *			 (This allows us to use graph theory to search for leaks)
 *
 *  returns: nothing
 */
void object_database_add(object_database_t *database, void *ptr, int units, struct_record_t *struct_rec, obj_bool_t is_root) {
	
	//check to see if we already have a head to the obj database
	object_record_t *record_new = object_database_lookup(database, ptr);
	if(record_new) {
		assert(0);
	}
	record_new = calloc(1, sizeof(object_record_t));

	record_new->next = NULL;
	record_new->memory = ptr;
	record_new->units = units;
	record_new->struct_record = struct_rec;
	record_new->is_root = is_root;
	record_new->is_visited = VISITED_FALSE;
	
	if(!database->head) {
		//the next pointer will already be NULL no need to re-declare
		database->head = record_new;
		database->num_of_records++;
		return;
	}
		
	//replace the head
	record_new->next = database->head;
	database->head = record_new;
	database->num_of_records++;
}

/*
 * Function:  allocate
 * -------------------
 * allocated a new memory block on the heap for the specific struct or
 * primitive object (if initialized) through the memory leak detector
 * 
 * !this is a wrapper class for the C library calloc() function
 *
 *  *database: the object database we will add the calloc reference to
 *	*struct_name: a characyer of strings representing the structure type
 *  units: the number of items to be allocated
 *
 *  returns: a void pointer to the memory address of the newly allocated 
 *			 dynamic object
 */
void *allocate(object_database_t *database, char *struct_name, int units) {
	
	struct_record_t *struct_record = struct_database_lookup(database->struct_database, struct_name);
	assert(struct_record); //if we get a zero value returned, crash
	
	void *ptr = calloc(units, struct_record->size);
	object_database_add(database, ptr, units, struct_record, ROOT_FALSE);
	return ptr;
}

/*
 * Function:  object_database_rem
 * ------------------------------
 * looks up an object record within the object database, if it
 * exists then we will delete it from the object database
 *
 *  *database: the object database we will remove a record from
 *	*record: the object record we want to remove
 *
 *  returns: nothing
 */
void object_database_rem(object_database_t *database, object_record_t *record) {
	
	free(record->memory);
	
	//check to see if the record is at the head of the linked list
	if(database->head == record) {
		database->head = record->next;
		database->num_of_records--;
		free(record);
		return;
	}
	
	object_record_t *obj_record_before = NULL;
	ITERATE_DB_OBJECTS_BEGIN(database, obj_record_before) {
		if (obj_record_before->next == record) {
			obj_record_before->next = record->next;
			database->num_of_records--;
			free(record);
			return;
		}
	} ITERATE_DB_OBJECT_END;
}

/*
 * Function:  deallocate
 * ---------------------
 * frees the dynamic object on the heap and removes the object
 * record from the object database so there are no issues with
 * dangling pointers within the object database
 *
 *	!this is a wrapper call for the c free(), please use this if
 *	 you called with allocate() otherwise the graph will mess up
 *
 *  *database: the object database we will be removing the record
 *	*ptr: pointer to the memory address of the dynamic object
 *
 *  returns: nothing
 */
void deallocate(object_database_t *database, void *ptr) {
	
	object_record_t *obj_record = object_database_lookup(database, ptr);
	if (!obj_record) {
		assert(0);
	}
	
	object_database_rem(database, obj_record);
}

/*
 * Function:  object_database_add_root
 * -----------------------------------
 * adds a root (parent) object into the object database, these are
 * global variables/objects that are accessible in all scopes and
 * should not be able to "leak"
 *
 * !these root objects are used to transerved through the object
 *  graph to find leaks within objects/fields
 *
 *  *database: the object database we will be adding the record
 *	*ptr: pointer to the memory address of the dynamic object
 *	*struct_name: the array of chars representing the type_t
 *	units: the number items to be allocated
 *
 *  returns: nothing
 */
void object_database_add_root(object_database_t *database, void *ptr, char *struct_name, int units) {
	
	//make sure that the structure we are adding to the object database actually exists, otherwise
	//we will end up getting a segmentation fault when trying to memory-lookup it's struct type
	struct_record_t *struct_record = struct_database_lookup(database->struct_database, struct_name);
	assert(struct_record);
	
	//proceed as normal to create a new obj record BUT pass in a ROOT_TRUE value instead
	object_database_add(database, ptr, units, struct_record, ROOT_TRUE);
}

/*
 * Function:  object_record_set_as_root
 * ------------------------------------
 * switches an object record currently in the object database
 * to a root object PLEASE DON'T JUST ROOT ANYTHING, PLEASE 0.0
 *
 * !these root objects are used to transerved through the object
 *  graph to find leaks within objects/fields
 *
 *  *database: the object database we will be modifying the record
 *	*obj_ptr: pointer to the object record
 *
 *  returns: nothing
 */
void object_record_set_as_root(object_database_t *database, void *obj_ptr) {
	
	//ensure that the pointer to an ambigious object has been stored within the object database otherwise
	//when trying to change the obj_bool_t type we will get a SEG FAULT trying to access a random mem addr
	object_record_t *object_record = object_database_lookup(database, obj_ptr);
	assert(object_record);
	
	//change the object records status to be of type 'root object'
	object_record->is_root = ROOT_TRUE;
}