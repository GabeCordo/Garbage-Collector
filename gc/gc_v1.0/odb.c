#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "sdb.h"
#include "odb.h"

object_record_t *object_database_lookup(object_database_t *database, void *ptr) {
	
	object_record_t *placeholder = NULL;
	ITERATE_DB_OBJECTS_BEGIN(database, placeholder) {
		if (placeholder->memory == ptr) {
			return placeholder;
		}
	} ITERATE_DB_OBJECT_END;
	return NULL;
}

void object_database_add(object_database_t *database, void *ptr, int units, struct_record_t *struct_rec) {
	
	//check to see if we already have a head to the obj database
	object_record_t *record_new = object_database_lookup(database, ptr);
	if(!record_new) {
		assert(0);
	}
	record_new = calloc(1, sizeof(object_record_t));

	record_new->next = NULL;
	record_new->memory = ptr;
	record_new->units = units;
	record_new->struct_record = struct_rec;
	
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

void *xcalloc(object_database_t *database, char *struct_name, int units) {
	
	struct_record_t *struct_record = struct_database_lookup(database->struct_database, struct_name);
	assert(struct_record); //if we get a zero value returned, crash
	
	void *ptr = calloc(units, struct_record->size);
	object_database_add(database, ptr, units, struct_record);
	return ptr;
}

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

void xfree(object_database_t *database, void *ptr) {
	
	object_record_t *obj_record = object_database_lookup(database, ptr);
	if (!obj_record) {
		assert(0);
	}
	
	object_database_rem(database, obj_record);
}