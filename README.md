# Garbage Collector ~ For Active Memory Leak Detection

The garbage collector is memory leak detection (mld) algorithm that uses wrapper functions around
the standard C calloc() and free() functions for monitoring memory references. This ensures that
dynamic allocation on the heap remains monitored in a similar fashion to languages such as Java.

## Header File

* at the top of your file use #include "mld_api.h" to import the library.
* under '~/gc/tests/tests.c' is a basic implementation to get you started with the mld program.

## Implementation
* The Structure Database is static (initialized at compile time) 
	* Keeps track of all types (primitive and objects) used by the application.
	* Structure Records hold information including:
		1. the name of the type/object
		2. size of bytes required
		3. the fields belonging to object types such a structs
	* Field Records are used to keep track of data fields held within struct objects
* The Object Database is dynamic (can grow during runtime)
	* Acts as "nodes" within a cyclic graph that map-out the dynamic objects within the application

## Graph Theory
* The mld algorithm makes use of the **depth-first search (dfs) algorithm** to find memory leaks.
* Making the assumption global object/variables are visible and all scopes and cannot be leaked:
	* You must declare these as root-object record which will be the start of all cyclic graph searches
* Object records are "nodes", where the pointers to each-other are represented by "edges"
	* If an object record cannot be reached by at **least** one root, it is classified as a leak
	* using "visited" enums, leaked objects will be marked as never visited

## Key Functions
! These are not the only requirements of the mld library but are the wrapper functions previously discussed

1. calloc() --> void *allocate(object_database_t *database, char *struct_name, int units);
2. free() --> void deallocate(object_database_t *database, void *ptr);

