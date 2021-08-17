#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define size 4
void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation){
    v->allocatedLength=initialAllocation;
    v->free_fn=freeFn;
    v->elemSize=elemSize;
    v->logicalSize=0;
    v->elements=malloc(v->allocatedLength*elemSize);
    v->firstSize=initialAllocation;
    assert(v->elements!=NULL);
}

void VectorDispose(vector *v){
    if(v->free_fn!=NULL){
        for(int i=0;i<v->logicalSize;i++){
            void * curPointer=(char*)v->elements+i*v->elemSize;
            v->free_fn(curPointer);
        }
    }
    free(v->elements);
}

int VectorLength(const vector *v){ 
    return v->logicalSize; 
}

void *VectorNth(const vector *v, int position){ 
    assert(position<v->logicalSize);
    assert(position>=0);
    void* answerPointer=(char*)v->elements+v->elemSize*position;
    return answerPointer; 
}

void VectorReplace(vector *v, const void *elemAddr, int position){
    assert(position>-1);
    assert(position<=v->logicalSize);
    void* replacingElement=(char*)v->elements+position*v->elemSize;
    if(v->free_fn!=NULL){
    v->free_fn(replacingElement);
    }
    memcpy(replacingElement,elemAddr,v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position){
    assert(position>=0);
    assert(position<=v->logicalSize);
    if(v->logicalSize==v->allocatedLength){
       v->allocatedLength+=v->firstSize;
       v->elements=realloc(v->elements,v->allocatedLength*v->elemSize);
       assert(v->elements!=NULL);
    }
    for(int i=v->logicalSize;i>position;i--){
        void * beforeElement=(char*)v->elements+v->elemSize*(i-1);
        void * currentElement=(char*)v->elements+v->elemSize*i;
        memcpy(currentElement,beforeElement,v->elemSize);
    }
    void* insertElement=(char*)v->elements+position*v->elemSize;
    memcpy(insertElement,elemAddr,v->elemSize);
    v->logicalSize++;
}

void VectorAppend(vector *v, const void *elemAddr){
    if(v->logicalSize==v->allocatedLength){
       v->allocatedLength+=size;
       v->elements=realloc(v->elements,v->allocatedLength*v->elemSize);
       assert(v->elements!=NULL);
    }
    void * lastElement=(char*)v->elements+v->logicalSize*v->elemSize;
    memcpy(lastElement,elemAddr,v->elemSize);
    v->logicalSize++;
}

void VectorDelete(vector *v, int position){
    assert(position>=0);
    assert(position<=v->logicalSize);
    void* deleteElement=(char*)v->elements+position*v->elemSize;
   if(v->free_fn!=NULL){
    v->free_fn(deleteElement);
    }
    for(int i=position;i<v->logicalSize-1;i++){
        void * before=(char*)v->elements+v->elemSize*i;
        void * copy=(char*)v->elements+v->elemSize*(i+1);
        memcpy(before,copy,v->elemSize);
        if(v->free_fn!=NULL){
        v->free_fn(copy);
        }
    }
    v->logicalSize--;
}

void VectorSort(vector *v, VectorCompareFunction compare){
    assert(compare!=NULL);
    qsort(v->elements,v->logicalSize,v->elemSize,compare);

}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData){
    assert(mapFn!=NULL);
    for(int i=0;i<v->logicalSize;i++){
        void * current=(char*)v->elements+i*v->elemSize;
        mapFn(current,auxData);
    }
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{ 
    assert(startIndex>=0);
    assert(startIndex<=v->logicalSize);
    int * index;
    if(isSorted){
    index=(int*)bsearch(key,(char*)v->elements+v->elemSize*startIndex,v->logicalSize,v->elemSize,searchFn);
        if(index!=NULL){
        return *index;
         }
    }else{
        for(int i=startIndex;i<v->logicalSize;i++){
            void * curElement=(char*)v->elements+v->elemSize*i;
            if(searchFn(key,curElement)==0){
                return i;
            }
        }
    }
   return kNotFound;
    } 
