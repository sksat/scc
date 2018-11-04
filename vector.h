#ifndef VECTOR_H_
#define VECTOR_H_

#include <stddef.h> // size_t

typedef struct {
	size_t size, reserved;
	void **data;
} vector_t;

vector_t* vector_new(size_t size);
void vector_delete(vector_t *vec);

void vector_set(vector_t *vec, size_t pos, void *ptr);
void* vector_get(vector_t *vec, size_t pos);
void vector_push_back(vector_t *vec, void *ptr);

#endif
