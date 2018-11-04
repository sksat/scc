#include <stdio.h>
#include <stdlib.h> // malloc/free
#include "vector.h"

vector_t* vector_new(size_t size){
	vector_t *vec = malloc(sizeof(vector_t));
	vec->size = size;
	vec->reserved = size * sizeof(void*);
	vec->data = malloc(vec->reserved);
	return vec;
}

void vector_delete(vector_t *vec){
	free(vec->data);
	free(vec);
}

void vector_set(vector_t *vec, size_t pos, void *ptr){
	vec->data[pos] = ptr;
}

void* vector_get(vector_t *vec, size_t pos){\
	return vec->data[pos];
}

void vector_push_back(vector_t *vec, void *ptr){
	size_t tmp = (vec->size + 1) * sizeof(void*);
	if(tmp > vec->reserved){
		vec->data = realloc(vec->data, tmp);
		if(vec->data == NULL){
			fprintf(stderr, "realloc error");
			exit(1);
		}
	}
	vec->reserved = tmp;
	vec->data[vec->size] = ptr;
	vec->size++;
}
