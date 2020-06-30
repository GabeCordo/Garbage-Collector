#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gc_api.h"

typedef struct college_ {
	char name[60];
	char location[100];
	float rating;
} college_t;

typedef struct student_ {
	char name[30];
	unsigned int num;
	unsigned int age;
	float average;
	struct student_ *college;
} student_t;

int main(int argc, char *argv[]) {
	
	struct_database_t *database_struct = calloc(1, sizeof(struct_database_t));
	MLD_SUPPORT_PRIMITIVES(database_struct); //this will allow us to pass "int", "float", "double" types 
	
	static struct_field_t college_fields[] = {
		FIELD_INFO(college_t, name, CHAR, 0),
		FIELD_INFO(college_t, location, CHAR, 0),
		FIELD_INFO(college_t, rating, FLOAT, 0)
	};
	RECORD_INSERT(database_struct, college_t, college_fields);
	
	static struct_field_t student_fields[] = {
		FIELD_INFO(student_t, name, CHAR, 0),
		FIELD_INFO(student_t, num, UINT32, 0),
		FIELD_INFO(student_t, age, UINT32, 0),
		FIELD_INFO(student_t, average, FLOAT, 0),
		FIELD_INFO(student_t, college, OBJ_PTR, college_t)
	};
	RECORD_INSERT(database_struct, student_t, student_fields);
	
	struct_database_print(database_struct);
	
	object_database_t *database_object = calloc(1, sizeof(object_database_t));
	database_object->struct_database = database_struct;
	
	student_t *gabe = allocate(database_object, "student_t", 1);
	deallocate(database_object, gabe);
	
	return 0;
}