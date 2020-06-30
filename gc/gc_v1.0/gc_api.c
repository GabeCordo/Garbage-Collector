#include "gc_api.h"

/*
 * Function:  mld_algorithm_init
 * -----------------------------
 * resets the "visited" flags to VISITED_FALSE to remove
 * the status' that remain from the last mld check
 *
 *  *database: the object database we will be modifying
 *
 *  returns: nothing
 */
void mld_algorithm_init(object_database_t *database) {
	
	object_record_t *ptr = NULL;
	ITERATE_DB_OBJECTS_BEGIN(database, ptr) {
		
		ptr->is_visited = VISITED_FALSE;
		
	} ITERATE_DB_OBJECT_END;
}

/*
 * Function:  object_database_next_root
 * ------------------------------------
 * searches for the next object record that is flagged as a root
 * object, every root object must be searched through to verify
 * the status of memory leaks
 *
 *  *database: the object database we will looking through
 *	*root_curr: the memory address of the "exhausted" root object
 *			L-> "exhausted": already "visited"
 *
 *  returns: the object record that is flagged as "ROOT_TRUE"
 *			 after the memory address of the last root object
 */
object_record_t *object_database_next_root(object_database_t *database, object_record_t *root_curr) {
	
	ITERATE_DB_OBJECTS_BEGIN(database, root_curr) {
			
			if (root_curr->is_root == ROOT_TRUE)
				return root_curr;
				
	} ITERATE_DB_OBJECT_END;
	
	return NULL;
}

/*
 * Function:  object_database_explore_children
 * -------------------------------------------
 * recrusivly searches through the parent-object record's
 * children object and there fields to see if there are any
 * issues, flagging each as "visited" if iteration is succ.
 *
 *  *database: the object database we will looking through
 *	*parent_object: the object record we will start looking through
 *					with regard to fields, etc.
 *		L-> used recusivly to look-through the next child
 *
 *  returns: nothing
 */
void object_database_explore_children(object_database_t *database, object_record_t *parent_object) {
	
	unsigned int i, num_of_fields;
	char *parent_obj_ptr = NULL, *child_obj_offset = NULL;
	void *child_object_address = NULL;
	struct_field_t *field_info = NULL;
	
	object_record_t *child_object_record = NULL;
	struct_record_t *parent_struct_record = parent_object->struct_record;
	
	assert(parent_object->is_visited); //we want to make sure the parent object has already been visited
	for (i=0; i<parent_object->units; i++) {
		
		parent_obj_ptr = (char *)(parent_object->memory) + (i * parent_struct_record->size);
		for (num_of_fields=0; num_of_fields < parent_struct_record->num_of_fields; num_of_fields++) {
			
			field_info = &parent_struct_record->fields[num_of_fields];
			if (field_info->data_type == OBJ_STRUCT) break;
			
			//we want to get the address where the next level object is stored within the child
			child_obj_offset = parent_obj_ptr + field_info->offset;
			memcpy(&child_object_address, child_obj_offset, sizeof(void *));
			
			//child_object_address is the next child object in the graph
			if (!child_obj_offset) continue;
				
			child_object_record = object_database_lookup(database, child_object_address);
			assert(child_object_address); //crash if the value is NULL
			
			if (!child_object_record->is_visited) {
				child_object_record->is_visited = VISITED_TRUE;
				if (field_info->data_type != VOID)
					object_database_explore_children(database, child_object_record);
			} else {
				continue;
			}
		}
	}
}

/*
 * Function:  mld_algorithm_trigger
 * --------------------------------
 * starts looking through the cylic graph that has been
 * created through object record (+ field) additions to
 * the object database using depth-first-search to find
 * any possible leaks
 *
 *  *database: the object database we will looking through
 *
 *  returns: nothing
 */
void mld_algorithm_trigger(object_database_t *database) {
	
	mld_algorithm_init(database);
	
	object_record_t *root_object = object_database_next_root(database, NULL);
	
	//repeat untill we have iterated over every root object
	while (root_object) {
		
		if(root_object->is_visited) {
			root_object = object_database_next_root(database, root_object);
			continue; //jump to the loop condition to see if it is NULL
		}
		root_object->is_visited = VISITED_TRUE;
		
		object_database_explore_children(database, root_object);
		root_object = object_database_next_root(database, root_object);
		
	}
}

/*
 * Function:  mld_algorithm_report
 * -------------------------------
 * after the mld algrithm has been run, this function will
 * check to see if any objects have not been "visited" by
 * the algorithm indicating it is a "leak"
 *
 * !if a developed wishes for these to be not be auto freed
 *	he must activate the debug macro that will just print
 *	any possible leaks with printf()
 *
 *  *database: the object database we will looking through
 *  debug_mode_active: whether the function with auto deallocate
 *
 *  returns: nothing
 */
void mld_algorithm_report(object_database_t *database, debug_mode_t debug_mode_active) {
	
	object_record_t *object_record = database->head;
	ITERATE_DB_OBJECTS_BEGIN(database, object_record) {
					
		if(object_record->is_visited == VISITED_FALSE) {
			printf("MEMORY LEAK (%p) it has been handled for you.", &object_record);
			//if debug mode is false, we want to auto deallocate
			if(!debug_mode_active)
				deallocate(database, object_record->memory);
		}
					
	} ITERATE_DB_OBJECT_END;
}