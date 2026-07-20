#ifndef INDEX_ARRAY

#define INDEX_ARRAY

#include "vocabulary.h"

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

// Display
void index_array__output(IndexArray index_array, const Vocabulary* vocabulary);

void index_array__set_output_words_per_line(size_t words_per_line);

// Operators

IndexArray intersect(IndexArray a, IndexArray b);

IndexArray join(IndexArray a, IndexArray b);

IndexArray subtract(IndexArray a, IndexArray b);

#endif
