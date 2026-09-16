#ifndef INDEX_ARRAY
#define INDEX_ARRAY

#include <stddef.h>

typedef struct {
    size_t size;
    size_t* indexes;
} IndexArray;

// Creation
void index_array__init(IndexArray* array, size_t size);

// Deletion
void index_array__free_content(IndexArray* array);

// Copy
IndexArray index_array__copy(const IndexArray* src);

// Operators
IndexArray intersect(IndexArray a, IndexArray b);

IndexArray join(IndexArray a, IndexArray b);

IndexArray subtract(IndexArray a, IndexArray b);

#endif
