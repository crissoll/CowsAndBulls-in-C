#ifndef INDEX_ARRAY

#define INDEX_ARRAY

#include "word.h"

typedef struct {
    size_t size;
    size_t* indexes;
}IndexArray;


// Creation
void index_array__init(IndexArray *array, size_t size);

// Deletion
void index_array__free_content(IndexArray* array);

// Copy
IndexArray index_array__copy(const IndexArray *src);

// Display
void index_array__print(IndexArray index_array,Vocabolary vocabolary);

// Operators

IndexArray intersect(const IndexArray a,const IndexArray b);

IndexArray join(const IndexArray a, const IndexArray b);

IndexArray subtract(const IndexArray a,const IndexArray b);

IndexArray index_array__match_letters(
        const IndexArray* index_array,
        const char* letters,
        const Vocabolary* vocabolary
    );


#endif