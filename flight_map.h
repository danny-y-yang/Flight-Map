#include <stdio.h>

// The maximum length of a city name
#define MAX_CITY_LEN 80

// The data type of the flight map. The details of the structure is hidden from
// users.
struct map_t;
typedef struct map_t map_t;

// EdgeNodes and CityNodes
struct city;
typedef struct city city;
struct city* cityCreator(const char*);
struct edge;
typedef struct edge edge;
struct edge* edgeCreator(city*);
struct stack;
typedef struct stack stack;

void city_free(city*);
void edge_free(edge*);

// Creates an empty flight map
map_t* map_create();

// Frees the memory associated with the flight map
void map_free(map_t*);

// Adds a new city to the map. Returns 1 if a new city was successfully added,
// or 0 otherwise. The map should keep its own copy of the name.
int add_city(map_t*, const char*);

// Removes an existing city from the map. Returns 1 if a city was removed, 0
// otherwise.
int remove_city(map_t*, const char*);

// Returns the number of cities in the map
int num_cities(map_t*);

// Creates a bi-directional link between two specified cities. Returns 1 if a
// link was created, 0 otherwise.
int link_cities(map_t*, const char*, const char*);

// Removes a bi-directional link between two specified cities. Returns 1 if a
// link was removed, 0 otherwise.
int unlink_cities(map_t*, const char*, const char*);

// Allocates and returns a list of the cities linked to the given city. Order is
// not specified. The final element of the list should be a null pointer. If the
// city is not on the map, return NULL.
const char** linked_cities(map_t*, const char*);

// Find any path from the source city to the destination city, allocating and
// returning a list of city names. The output list will always begin with the
// source name, end with the destination name, and list the path of cities
// traversed in between (note that if we're finding a path from a city to
// itself, the list will contain just that city one time). The final element of
// the list should be a null pointer. If there is no path, or either of the
// cities don't exist, return NULL.
const char** find_path(map_t*, const char*, const char*);

// Writes a map to a file, such that it can be re-constructed by the
// corresponding import function. Note: city names will not contain whitespace,
// so feel free to use any whitespace to separate cities.
void map_export(map_t*, FILE*);

// Imports a map from a file, returning a newly constructed map.
map_t* map_import(FILE*);
