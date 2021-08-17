#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,HashSetHashFunction hashfn, 
				HashSetCompareFunction comparefn, HashSetFreeFunction freefn){
					assert(numBuckets>0);
					assert(elemSize>0);
					assert(hashfn!=NULL);
					assert(comparefn!=NULL);
					h->elementNumbers=0;
					h->buckets=(vector**)malloc(sizeof(vector*)*numBuckets);
					vector ** cur=h->buckets;
					vector ** end=h->buckets+numBuckets;
					while(cur!=end){
						*cur=malloc(sizeof(vector));
						VectorNew(*cur,elemSize,freefn,4);
						cur++;
					}
					h->elementsSize=elemSize;
					h->HashSetCompareFunction=comparefn;
					h->HashSetFreeFunction=freefn;
					h->HashSetHashFunction=hashfn;
					h->numBuckets=numBuckets;
}

void HashSetDispose(hashset *h){
	vector** currentVectorPointer=h->buckets;
	for(int i=0;i<h->numBuckets;i++){
		VectorDispose(*currentVectorPointer);
		free(*currentVectorPointer);
		currentVectorPointer++;
	}
	free((void*)h->buckets);
}

int HashSetCount(const hashset *h){
	 return h->elementNumbers; 
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData){
	assert(mapfn!=NULL);
	vector ** cur=h->buckets;
	for(int i=0;i<h->numBuckets;i++){
		VectorMap(*cur,mapfn,auxData);
		cur++;
	}
}

void HashSetEnter(hashset *h, const void *elemAddr){
	assert(elemAddr!=NULL);
	int hashCode=h->HashSetHashFunction(elemAddr,h->numBuckets);
	assert(hashCode>-1);
	assert(hashCode<h->numBuckets);
	vector** currentVectorPointer=h->buckets+hashCode;
	int position=VectorSearch(*currentVectorPointer,elemAddr,h->HashSetCompareFunction,0,false);
	if(position==-1){
		VectorAppend(*currentVectorPointer,elemAddr);
		h->elementNumbers++;
	}else{
		VectorReplace(*currentVectorPointer,elemAddr,position);
	}
}

void *HashSetLookup(const hashset *h, const void *elemAddr){
	assert(elemAddr!=NULL);
	int hashCode=h->HashSetHashFunction(elemAddr,h->numBuckets);
	assert(hashCode>-1);
	assert(hashCode<h->numBuckets);
	vector** currentVectorPointer=h->buckets+hashCode;

	int find=VectorSearch(*currentVectorPointer,elemAddr,h->HashSetCompareFunction,0,false);
	if(find!=-1){
		return VectorNth(*currentVectorPointer,find);
	}
	return NULL; 
}
