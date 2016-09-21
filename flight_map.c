#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flight_map.h"

// A safe version of checked_malloc, that will exit the program in case your allocation
// fails.
void *checked_malloc(size_t size) {
     void *ptr = malloc(size);
     if (ptr == NULL) {
          fprintf(stderr, "memory allocation failed\n");
          exit(1);
     }
     return ptr;
}

const map_t* mapMemory;

typedef struct {
	char* cityName;
	struct city* nextCity;
	struct edge* edges; 
	int numEdges;
	int visited;
} city;

struct city* cityCreator(const char* name) {
	struct city* newCity = checked_malloc(sizeof(city));
	newCity->cityName = checked_malloc(sizeof(char*) * MAX_CITY_LEN);
	strcpy(newCity -> cityName, name);
	newCity->nextCity = NULL;
	newCity->edges = NULL;
	newCity->numEdges = 0;
	newCity->visited = 0;
	return newCity;
}
	

typedef struct {
	city* neighbor;
	edge* nextEdge;
} edge;

struct edge* edgeCreator(city* newNeighbor) {
	struct edge* newEdge = checked_malloc(sizeof(edge));
	newEdge->neighbor = newNeighbor;
	newEdge->nextEdge = NULL;
	return newEdge;
}

struct map_t{
	int numCities;
    struct city* head;
};

map_t* map_create() {
	/* Initalizes memory needed for a new map */
    map_t* newMap = checked_malloc(sizeof(map_t));
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
		edge_free(currEdge->nextEdge);
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
		edge_free(currCity->edges);
		free(currCity->cityName);
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
			if (strcmp(currCity->cityName, name) == 0) {
				return 0;
			}
			currCity = currCity->nextCity;
		}
		currCity->nextCity = cityCreator(name);
		(map->numCities)++;
		return 1; 
	}
}

int removeHelper(city** cityPtr, const char* name, map_t* map) {
	
	city* currCity = map->head;
	while (strcmp(currCity->nextCity->cityName, name) != 0) {
		if (currCity -> nextCity == NULL) {
			return 0;
		}
		currCity = currCity -> nextCity;
		cityPtr = &(currCity->nextCity);
	}
	
	city* toRemove = currCity->nextCity;
	*cityPtr = toRemove->nextCity;
	map->numCities--;
	free(toRemove);
	return 1;
}

int remove_city(map_t* map, const char* name) {
	/* There are no cities in the map */
	if (!(map->head)) {
		return 0;
	}
	
	/* If there is only 1 city */
	if (map->head->nextCity == NULL) {
		if (strcmp(map->head->cityName, name) == 0) {
			map->head = NULL;
			(map->numCities)--;
			free(map->head);
			return 1;
		} else {
			return 0;
		}
	
	/* Iterate through the cities, and remove the city if it exists */
	} else {
		city* currCity = map->head;
		return removeHelper(&(currCity->nextCity), name, map);
	}
}

int num_cities(map_t* map) {
	return map->numCities; 
}

int link_cities(map_t* map, const char* city1_name, const char* city2_name) {
     if (strcmp(city1_name, city2_name) == 0) {
		return 0;
	}
	/* Find city 1 and city 2 */
	city* currCity = map->head;
	city* city1 = NULL;
	city* city2 = NULL;
	while ((city1 == NULL && city2 == NULL) || currCity != NULL) {
		if (strcmp(currCity->cityName, city1_name) == 0) {
			city1 = currCity;
		} 
		if (strcmp(currCity->cityName, city2_name) == 0) {
			city2 = currCity;
		}
		currCity = currCity->nextCity;
	}
	
	/* Either city 1 or city 2 doesn't exist */
	if (city1 == NULL || city2 == NULL) {
		return 0;
	} 
	
	/* Create an edge between city 1 and city 2 */
	edge* city1Edge = city1->edges;

	// City has no neighbors
	if (!city1->edges) {
		city1->edges = edgeCreator(city2);
		(city1->numEdges)++;
	} else {
		// Creates link, or shortcircuits if link already exists
		while (city1Edge->nextEdge != NULL) {
			if (strcmp(city1Edge->neighbor->cityName, city2_name) == 0) {
				return 0;
			} 
			city1Edge = city1Edge->nextEdge;
		}
		if (strcmp(city1Edge->neighbor->cityName, city2_name) == 0) {
				return 0;
		} else {
			city1Edge->nextEdge = edgeCreator(city2);
			(city1->numEdges)++;
		}
	}
	

	/* Create an edge between city 2 and city 1 */
	edge* city2Edge = city2->edges;
	
	if (!city2Edge) {
		city2->edges = edgeCreator(city1);
		(city2->numEdges)++;
	} else {
		while (city2Edge->nextEdge != NULL) {
			if (strcmp(city2Edge->neighbor->cityName, city1_name) == 0) {
				return 0;
			} 
			city2Edge = city2Edge->nextEdge;
		}
		if (strcmp(city2Edge->neighbor->cityName, city1_name) == 0) {
				return 0;
		} 
		city2Edge->nextEdge = edgeCreator(city1);
		(city2->numEdges++);
	}
	return 1;
	
}

int unlinkHelper(edge** edgePtr, city* currCity, const char* name) {
	edge* currEdge = currCity->edges;
	while (currEdge != NULL) {
		if (strcmp(currEdge->nextEdge->neighbor->cityName, name) == 0) {
			edge* toRemove = currEdge->nextEdge;
			*edgePtr = toRemove->nextEdge;
			free(toRemove);
			return 1;
		}
		currEdge = currEdge->nextEdge;
		edgePtr = &(currEdge->nextEdge);
	}
	return 0;
}

int unlink_cities(map_t* map, const char* city1_name, const char* city2_name) {
    /* Find city 1 and city 2 */
	city* currCity = map->head;
	city* city1 = NULL;
	city* city2 = NULL;
	while ((city1 == NULL && city2 == NULL) || currCity != NULL) {
		if (strcmp(currCity->cityName, city1_name) == 0) {
			city1 = currCity;
		} 
		if (strcmp(currCity->cityName, city2_name) == 0) {
			city2 = currCity;
		}
		currCity = currCity->nextCity;
	}

	/* Either city 1 or city 2 doesn't exist */
	if (currCity == NULL) {
		return 0;
	} 

	/* Unlink city 2 from city 1 */
	if (city1->edges == NULL) {
		return 0;
	}
	if (city1->edges->nextEdge == NULL) {
		if (strcmp(city1->edges->neighbor->cityName, city2_name) == 0) {
			city1->edges = NULL;
			free(city1->edges);
			(city1->numEdges)--;
		} 
		return 0;
	} else {
		unlinkHelper(&(city1->edges->nextEdge), city1, city2_name);
	}

	/* Unlink city 2 from city 1 */
	if (city2->edges == NULL) {
		return 0;
	}
	if (city2->edges->nextEdge == NULL) {
		if (strcmp(city2->edges->neighbor->cityName, city1_name) == 0) {
			city2->edges = NULL;
			free(city2->edges);
			(city2->numEdges)--;
			return 1;
		} 
		return 0;
	} else {
		return unlinkHelper(&(city2->edges->nextEdge), city2, city1_name);
	}
}

const char** linked_cities(map_t* map, const char* city_name) {
	
	city* currCity = map->head;
	int neighborsSize = 0;
	while (currCity != NULL) {
		if (strcmp(currCity->cityName, city_name) == 0) {
			neighborsSize = currCity->numEdges;
			break;
		}
		currCity = currCity->nextCity;
	}

	const char** neighbors = checked_malloc(sizeof(char*) * (neighborsSize + 1));
	edge* currEdge = currCity->edges;
	
	for (int i = 0; i < neighborsSize; i++) {
		neighbors[i] = currEdge->neighbor->cityName;
		currEdge = currEdge->nextEdge;
	}

	neighbors[neighborsSize] = NULL;

	return neighbors;
}

struct stack {
	int size;
	city** stackCities;
};

struct stack* stackCreator(int maxSize) {
	struct stack* newStack = checked_malloc(sizeof(stack));
	newStack->size = 0;
	newStack->stackCities = checked_malloc(sizeof(city*) * maxSize);
	return newStack;
};

void stackPush(stack* myStack, city* newCity) {
	myStack->stackCities[myStack->size] = newCity;
	(myStack->size)++;
}

city* stackPop(stack* myStack) {
	city* toReturn = myStack->stackCities[myStack->size - 1];
	myStack->stackCities[myStack->size - 1] = NULL;
	(myStack->size)--;
	return toReturn;
}

void stackDestructor(stack* myStack) {
	free(myStack->stackCities);
	free(myStack);
}

int checkVisited(map_t* map, char* name) {
	city* currCity = map->head;
	while (strcmp(currCity->cityName, name) != 0) {
		currCity = currCity->nextCity;
	}
	return currCity->visited;
}

const char** find_path(map_t* map, const char* src_name, const char* dst_name) {

	/* Clear all visited nodes */
	city* currCity = map->head;
	while (currCity != NULL) {
		currCity->visited = 0;
		currCity = currCity->nextCity;
	}

	const char** path = checked_malloc(sizeof(char*) * (map->numCities + 1)) ;
	path[map->numCities] = NULL;
	int pathSize = 0;
	stack* theStack = stackCreator(map->numCities);

	/* DFS */
	city* startCity = map->head;
	while (startCity != NULL) {
		if (strcmp(startCity->cityName, src_name) == 0) {
			break;
		} else {
			return NULL;
		}
	}

	stackPush(theStack, startCity);
	while (theStack->size != 0) {
		currCity = stackPop(theStack);
		currCity->visited = 1;
		path[pathSize] = currCity->cityName;
		if (strcmp(currCity->cityName, dst_name) == 0) {
			stackDestructor(theStack);
			return path;
		}
		pathSize++;
		edge* currEdge = currCity->edges;
		while (currEdge != NULL) {
			if (checkVisited(map, currEdge->neighbor->cityName) == 0) {
				stackPush(theStack, currEdge->neighbor);
			}
			currEdge = currEdge->nextEdge;
		}

	}

	return NULL;

}

void map_export(map_t* map, FILE* f) {
     // YOUR CODE HERE
}

map_t* map_import(FILE* f) {
     // YOUR CODE HERE
     return NULL;
}


// int cities_match(const char** actual, const char** expected) {
//      for (; *actual != NULL && *expected != NULL; ++actual, ++expected) {
//           if (strcmp(*actual, *expected) != 0) {
//                return 0;
//           }
//      }
//      return (*actual == NULL && *expected == NULL);
// }

// int main() {
//      // empty_map
//      {
//           // create a map
//           map_t *map = map_create();
          
//           // check that the number of cities in the map is 0
//           assert(num_cities(map) == 0);
//           // free the map
//           map_free(map);
//      }

//      // add and link cities
//      {
//           const char** result;
//           map_t *map = map_create();
//           // add a city named "abc" to the map
//           assert(add_city(map, "abc"));
//           assert(add_city(map, "def"));
//           assert(num_cities(map) == 2);
//           // link the cities named "abc" and "def"
//           assert(link_cities(map, "abc", "def"));

//           // get a list of the cities linked to "abc"
//           result = linked_cities(map, "abc");
//           // check that the list returned matches {"def", NULL}
//           assert(cities_match(
//                       result,
//                       (const char*[]){"def", NULL}));
//           // free the returned list
//           free(result);
//           map_free(map);
//      }

//      // find path
//      {
//           const char** result;
//           map_t* map = map_create();
//           add_city(map, "abc");
//           add_city(map, "def");
//           add_city(map, "ghi");

//           link_cities(map, "abc", "def");
//           link_cities(map, "def", "ghi");

//           // get a list of cities forming a path between "abc" and "ghi"
//           result = find_path(map, "abc", "ghi");
//           assert(cities_match(
//                       result,
//                       (const char*[]){"abc", "def", "ghi", NULL}));
//           free(result);
//           map_free(map);
//      }

//      printf("ALL TESTS PASS!\n");
// }

