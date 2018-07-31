#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	assert(elemSize > 0);
	assert(numBuckets > 0);
	assert(hashfn != NULL);
  	assert(comparefn != NULL);
  	h->freefn = freefn;
	h->elemSize = elemSize;
	h->numBuckets = numBuckets;
	h->hashfn = hashfn;
	h->comparefn = comparefn;
	h->buckets = (vector *)malloc(sizeof(vector) * numBuckets);
	for(int i = 0; i < numBuckets; i++){
		VectorNew(h->buckets + i, elemSize, freefn, 4);
	}
}

void HashSetDispose(hashset *h)
{
	if(h->freefn != NULL){
		for(int i = 0; i < h->numBuckets; i++){
			VectorDispose(h->buckets + i);
		}
	}
	free(h->buckets);
}

int HashSetCount(const hashset *h)
{
	int counter = 0;
	for(int i = 0; i < h->numBuckets; i++){
		counter += VectorLength(h->buckets + i);
	}
	return counter;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
	assert(mapfn != NULL);
	for(int i = 0; i < h->numBuckets; i++){
		VectorMap(h->buckets + i, mapfn, auxData);
	}
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
	assert(elemAddr != NULL);
	int iMapAnswer = h->hashfn(elemAddr, h->numBuckets);
	assert(iMapAnswer >= 0 && iMapAnswer < h->numBuckets);
	int position = VectorSearch(h->buckets + iMapAnswer, elemAddr, h->comparefn, 0, false);//maybe lfind can change to bsearch, just change false to true
	if(position != -1){//find
		VectorReplace(h->buckets + iMapAnswer, elemAddr, position);
	}
	else{
		VectorAppend(h->buckets + iMapAnswer, elemAddr);//unsorted
	}
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{ 
	assert(elemAddr != NULL);
	int iMapAnswer = h->hashfn(elemAddr, h->numBuckets);
	assert(iMapAnswer >= 0 && iMapAnswer < h->numBuckets);
	int position;
	position = VectorSearch(h->buckets + iMapAnswer, elemAddr, h->comparefn, 0, false);//maybe lfind can change to bsearch, just change false to true


	if(position != -1){
		return VectorNth(h->buckets + iMapAnswer, position);
	}
	else{
		return NULL; 
	}
}
