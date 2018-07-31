#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <search.h>//lfind function

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
	assert(elemSize > 0);//if elemSize < 0, show the error
	v->iElemSize = elemSize;

	assert(initialAllocation > 0);
	v->iAllocLength = initialAllocation;

	v->iLogicLength = 0;
	v->freeFn = freeFn;

	v->pVector = malloc(elemSize * initialAllocation);
	assert(v->pVector != NULL);
}

void VectorDispose(vector *v)
{
	if(v->freeFn != NULL){
		for(int counter = 0; counter < v->iLogicLength; counter++){
			v->freeFn((char*)v->pVector + v->iElemSize * counter);
		}
	}
}

int VectorLength(const vector *v)
{ 
	return v->iLogicLength; 
}

void *VectorNth(const vector *v, int position)
{ 
	assert(position >= 0 && position < v->iLogicLength);
	return (char*)v->pVector + v->iElemSize * position; 
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
	assert(position >= 0 && position < v->iLogicLength);
	if(v->freeFn != NULL){
		v->freeFn((char*)v->pVector + v->iElemSize * position);
	}
	memcpy((char*)v->pVector + v->iElemSize * position, elemAddr, v->iElemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
	assert(position >= 0 && position <= v->iLogicLength);
	if(v->iLogicLength == v->iAllocLength){
		v->pVector = realloc(v->pVector, v->iAllocLength * 2 * v->iElemSize);//doesn't need user to free memory
		assert(v->pVector != NULL);
		v->iAllocLength *= 2;
	}
	memmove((char*)v->pVector + v->iElemSize * (position + 1), (char*)v->pVector + v->iElemSize * position, v->iElemSize * (v->iLogicLength - position));//if position == v->iLogicLength
	memcpy((char*)v->pVector + v->iElemSize * position, elemAddr, v->iElemSize);
	v->iLogicLength++;

}

void VectorAppend(vector *v, const void *elemAddr)
{
	if(v->iLogicLength == v->iAllocLength){
		v->pVector = realloc(v->pVector, v->iAllocLength * 2 * v->iElemSize);//doesn't need user to free memory
		assert(v->pVector != NULL);
		v->iAllocLength = v->iAllocLength * 2;
	}
	memcpy((char*)v->pVector + v->iElemSize * (v->iLogicLength), elemAddr, v->iElemSize);
	v->iLogicLength++;
}

void VectorDelete(vector *v, int position)
{
	assert(position >= 0 && position < v->iLogicLength);
	if(v->freeFn != NULL){
		v->freeFn((char*)v->pVector + v->iElemSize * position);
	}
	memmove((char*)v->pVector + v->iElemSize * position, (char*)v->pVector + v->iElemSize * (position + 1), v->iElemSize * (v->iLogicLength - position));
	v->iLogicLength--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
	assert(compare != NULL);
	qsort(v->pVector, v->iLogicLength, v->iElemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
	assert(mapFn != NULL);
	for(int i = 0; i < v->iLogicLength; i++){
		mapFn(VectorNth(v, i), auxData);
	}
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{
	assert(startIndex >= 0 && startIndex <= v->iLogicLength);
	assert(searchFn != NULL);
	assert(key != NULL);
	void* pKeyAddress;
	size_t num = v->iLogicLength - startIndex;
	if(isSorted){
		pKeyAddress = bsearch(key, (char*)v->pVector + v->iElemSize * startIndex, v->iLogicLength - startIndex, v->iElemSize, searchFn);
	}
	else{
		pKeyAddress = lfind(key, (char*)v->pVector + v->iElemSize * startIndex, &num, v->iElemSize, searchFn);//the type of the third argument is different from bsearch function, it's size_t * rather than size_t.  
	}

	if(pKeyAddress){
		return (int)(((char*)pKeyAddress - (char*)v->pVector)/v->iElemSize);
	}
	else{
		return -1; 
	}
} 
