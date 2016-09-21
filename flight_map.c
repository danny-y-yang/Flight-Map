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

//////////////////////// STRUCT DECLARATIONS ////////////////////////
typedef struct city city;
typedef struct edge edge;
typedef struct stack stack;

struct city {
	char* cityName;
	struct city* nextCity;
	struct edge* edges; 
	int numEdges;
	int visited;
};

struct edge {
	city* neighbor;
	edge* nextEdge;
};

struct map_t{
	int numCities;
    struct city* head;
};

struct stack {
	int size;
	city** stackCities;
};

/////////////// STRUCT CONSTRUCTORS AND DESTRUCTORS ///////////////////

struct edge* edgeCreator(city* newNeighbor) {
	struct edge* newEdge = checked_malloc(sizeof(edge));
	newEdge->neighbor = newNeighbor;
	newEdge->nextEdge = NULL;
	return newEdge;
}

void edge_free(edge* currEdge) {
	if (currEdge == NULL) {
		return;
	} else {
		edge_free(currEdge->nextEdge);
		free(currEdge);
		return;
	}
}

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

struct stack* stackCreator(int maxSize) {
	struct stack* newStack = checked_malloc(sizeof(stack));
	newStack->size = 0;
	newStack->stackCities = checked_malloc(sizeof(city*) * maxSize);
	return newStack;
};

void stackDestructor(stack* myStack) {
	free(myStack->stackCities);
	free(myStack);
}

void stackPush(stack* myStack, city* newCity) {
	myStack->stackCities[myStack->size] = newCity;
	(myStack->size)++;
}

city* stackPop(stack* myStack) {
	if (myStack->size == 0) {
		return NULL;
	}
	city* toReturn = myStack->stackCities[myStack->size - 1];
	myStack->stackCities[myStack->size - 1] = NULL;
	(myStack->size)--;
	return toReturn;
}

/////////////////////// METHODS TO IMPLEMENT /////////////////////////

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

int removeCityHelper(city** cityPtr, const char* name, map_t* map) {
	
	/* Iterate through the cities and remove the city, or return 0 if the city doesn't exist */
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

	/* Remove the city from all edges */
	city* findEdgesToremove = map->head;
	while (findEdgesToremove != NULL) {
		unlink_cities(map, findEdgesToremove->cityName, name);
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
		return removeCityHelper(&(currCity->nextCity), name, map);
	}
}

int num_cities(map_t* map) {
	return map->numCities; 
}

int link_cities(map_t* map, const char* city1_name, const char* city2_name) {
     if (strcmp(city1_name, city2_name) == 0) {
		return 0;
	}
	/* Find city 1 and city 2 in the map*/
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

	// City has no neighbors
	edge* city1Edge = city1->edges;

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
		// If the city already exists, and is the last node
		if (strcmp(city1Edge->neighbor->cityName, city2_name) == 0) {
				return 0;
		} else {
			city1Edge->nextEdge = edgeCreator(city2);
			(city1->numEdges)++;
		}
	}

	/* Create an edge between city 2 and city 1 */
	edge* city2Edge = city2->edges;
	
	// City has no neighbors
	if (!city2Edge) {
		city2->edges = edgeCreator(city1);
		(city2->numEdges)++;
	} else {
		// Creates link, or shortcircuits if link already exists
		while (city2Edge->nextEdge != NULL) {
			if (strcmp(city2Edge->neighbor->cityName, city1_name) == 0) {
				return 0;
			} 
			city2Edge = city2Edge->nextEdge;
		}
		// If the city already exists, and is the last node
		if (strcmp(city2Edge->neighbor->cityName, city1_name) == 0) {
				return 0;
		} 
		city2Edge->nextEdge = edgeCreator(city1);
		(city2->numEdges++);
	}
	return 1;
	
}

int unlinkHelper(edge** edgePtr, city* currCity, const char* name) {

	/* Iterate through edges and remove the city */
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
		if (unlinkHelper(&(city1->edges->nextEdge), city1, city2_name) == 0) {
			return 0;
		};
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
