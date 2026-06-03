#include <stdlib.h>

#include "io/cab_output.h"
#include "core/word.h"
#include "core/index_array.h"


void index_array__init(IndexArray *array, size_t size){
    array->size = size;
    if(size > 0)
        array->indexes = malloc(size * sizeof(*array->indexes));
    else
        array->indexes = NULL;
}

void index_array__free_content(IndexArray* array){
    if (array->size > 0 && array->indexes != NULL) {
        free(array->indexes);
        array->indexes = NULL; // Evita dangling pointer
    }
}

IndexArray index_array__copy(const IndexArray *src){
    IndexArray dest;
    index_array__init(&dest, src->size);
    for(size_t i = 0; i < src->size; i++){
        dest.indexes[i] = src->indexes[i];
    }
    return dest;
}


IndexArray intersect(const IndexArray a,const IndexArray b){
    IndexArray result;
    index_array__init(&result, a.size < b.size ? a.size : b.size);

    size_t i = 0, j = 0, k = 0;
    while(i < a.size && j < b.size){
        if(a.indexes[i] == b.indexes[j]){
            result.indexes[k++] = a.indexes[i];
            i++;
            j++;
        } else if(a.indexes[i] < b.indexes[j]){
            i++;
        } else {
            j++;
        }
    }
    result.size = k; // Aggiorna la dimensione del risultato
    return result;
}


IndexArray subtract(const IndexArray a,const IndexArray b){
    IndexArray result;
    index_array__init(&result,a.size);

    size_t i = 0, j = 0, k = 0;

    while(i < a.size && j < b.size){
        if(a.indexes[i] == b.indexes[j]){
            i++;
            j++;
        } else if(a.indexes[i] < b.indexes[j]){
            result.indexes[k++] = a.indexes[i];
            i++;
        } else {
            j++;
        }
    }

    while(i < a.size){
        result.indexes[k++] = a.indexes[i];
        i++;
    }

    result.size = k;

    return result;
}


IndexArray join(const IndexArray a, const IndexArray b){
    IndexArray result;
    index_array__init(&result,a.size+b.size);

    size_t i = 0, j = 0, k = 0;

    while(i < a.size && j < b.size){
        
        if(a.indexes[i] == b.indexes[j]){
            result.indexes[k++] = a.indexes[i];
            i++;
            j++;
        } else if(a.indexes[i] < b.indexes[j]){
            result.indexes[k++] = a.indexes[i];
            i++;
        } else { 
            result.indexes[k++] = b.indexes[j];
            j++;
        }
    }

    while (i < a.size) {
        result.indexes[k++] = a.indexes[i++];
    }
    while (j < b.size) {
        result.indexes[k++] = b.indexes[j++];
    }
    
    result.size = k;
    return result;
}


void index_array__print(IndexArray index_array,const Vocabolary* vocabolary){
    size_t j = 0;
    for(size_t i = 0; i < index_array.size;i++){
        word__print(vocabolary->words[index_array.indexes[i]]);
        output(" ");
        if(++j > 10){
            output("\n");
            j = 0;
        }
    }
    output("\n");
}
