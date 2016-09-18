#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flight_map.h"

// A safe version of malloc, that will exit the program in case your allocation
// fails.
void *checked_malloc(size_t size) {
     void *ptr = malloc(size);
     if (ptr == NULL) {
          fprintf(stderr, "memory allocation failed\n");
          exit(1);
     }
     return ptr;
}

const map_t*  mapMemory;

typedef struct city {
	char* cityName;
	struct city* nextCity;
	struct edge* edges; 
}

struct city* cityCreator(const char* name) {
	struct city* newCity = malloc(sizeof(city));
	strcpy(newCity -> cityName, name);
	newCity->nextCity = NULL;
	newCity->edges = NULL;
	return newCity;
}
	

typedef struct edge {
	city* neighbor;
	edge* nextEdge;
}

struct edge* edgeCreator(city* newNeighbor) {
	struct edge* newEdge = malloc(sizeof(edge));
	newEdge->neighbor = newNeighbor;
	newEdge->nextEdge = NULL;
	return newEdge;
}

typedef struct map_t {
	int numCities;
    struct city* head;
}

map_t* map_create() {
	/* Initalizes memory needed for a new map */
    map_t* newMap = malloc(sizeof(struct map_t));
	mapMemory = newMap;
	newMap->head = NULL;
	newMap->numCities = 0;
	return newMap;	
}

void map_free(map_t* map) {
     city_free(map->head);
     free(map);
}

/* Helper function that frees a city's edges */
void edge_free(edge* currEdge) {
	if (currEdge == NULL) {
		return;
	} else {
		edge_free(currEdge->nextEdge, &(currEdge->nextEdge->neighbor));
		free(currEdge);
		return;
	}
}

/* Helper function that frees a map's cities */
void city_free(city* currCity) {
	if (currCity == NULL) {
		return;
	} else {
		city_free(currCity->nextCity);
		free(currCity->cityName);
		edge_free(currCity->edges);
		free(currCity);
		return;
	}
}

int add_city(map_t* map, const char* name) {

	/* No cities have been created yet */
	if (!(map->head)) {
		map->head = cityCreator(name);
		(map->numCities)++;
		return 1;

	/* Iterate through the current cities, shortcircuiting
	if the city already exists, else make a new city at the end */
	} else {
		city* currCity = map->head;
		while (currCity->nextCity != NULL) {
			if (strcmp(currCity->cityName, name)) {
				return 0;
			}
			currCity = currCity->nextCity;
		}
		currCity->nextCity = cityCreator(name);
		(map->numCities)++;
		return 1; 
	}
}

void removeHelper(city** cityPtr, const char* name, map_t* map) {
	
	
	/* If there is only 1 city */
	if (map->head->nextCity == NULL) {
		if (strcmp(map->head->cityName, name)) {
			map->head = NULL;
			(map->values)--;
			free(map->head);
			return 1;
		} else {
			return 0;
		}
	
	/* Iterate through the cities, and remove the city if it exists */
	} else {
		city* currCity = map->head;
		while (!strcmp(currCity->nextCity->cityName, name)) {
			if (currCity -> nextCity == NULL) {
				return 0;
			}
			currCity = currCity -> nextCity;
		}
		
		city* toRemove = currCity->nextCity;
		currCity->nextCity = toRemove->nextCity;
		map->numCities--;
		free(toRemove);
		return 1;
}

int remove_city(map_t* map, const char* name) {
	/* There are no cities in the map */
	if (!(map->head)) {
		return 0;
	}
}

int num_cities(map_t* map) {
	return map->numCities; 
}

int link_cities(map_t* map, const char* city1_name, const char* city2_name) {
     	if (strcmp(city1_name, city2_name)) {
		return 0;
	}
	/* Find city 1 and city 2 */
	city* currCity = map->head;
	city* city1 = NULL;
	city* city2 = NULL;
	while ((city1 != NULL && city2 != NULL) || currCity != NULL) {
		if (strcmp(currCity->name, city1_name)) {
			city1 = currCity;
		} 
		if (strcmp(currCity->name, city2_name)) {
			city2 = currCity;
		}
		currCity = currCity->nextCity;
	}
	
	/* Either city 1 or city 2 doesn't exist */
	if (currCity == NULL) {
		return 0;
	} 
	
	/* Create an edge between city 1 and city 2 */
	edge* city1Edge = city1->edges;

	// City has no neighbors
	if (!city1Edge) {
		city1->edges = edgeCreator(city1);
	}
	// Creates link, or shortcircuits if link already exists
	while (city1Edge->nextEdge != NULL) {
		if (strcmp(city1Edge->name), city2_name) {
			return 0;
		} 
		city1Edge = city1Edge->nextEdge;
		city1->numEdges++;
	}
	city1Edge->nextEdge = edgeCreator(city1);
	

	/* Create an edge between city 2 and city 1 */
	edge* city2Edge = city2->edges;
	
	if (!city2Edge) {
		city2->edges = edgeCreator(city2);
	}
	while (city2Edge->nextEdge != NULL) {
		if (strcmp(city2Edge->name), city1_name) {
			return 0;
		} 
		city2Edge = city2Edge->nextEdge;
		city2->numEdges++;
	}
	city2Edge->nextEdge = edgeCreator(city1);

	return 1;
}

int unlink_cities(map_t* map, const char* city1_name, const char* city2_name) {
     // YOUR CODE HERE
}

const char** linked_cities(map_t* map, const char* city_name) {
	neighborsSize = city_name->numEdges;
	char** neighbors[neighborsSize + 1];
	edge* currEdge = city_name->edges;
	
	for (int i = 0; i < neighborsSize - 1; i++) {
		neighbors[i] = currEdge->edge->cityName;
		currEdge = currEdge->nextEdge;
	}

	neighbors[neighborsSize] = NULL;

	return neighbors;
}

const char** find_path(map_t* map, const char* src_name, const char* dst_name) {
     // YOUR CODE HERE
}

void map_export(map_t* map, FILE* f) {
     // YOUR CODE HERE
}

map_t* map_import(FILE* f) {
     // YOUR CODE HERE
     return NULL;
}
