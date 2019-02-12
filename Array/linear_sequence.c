#include <stdlib.h>
#include "linear_sequence.h"
   
#define PERCENT_LOW_LINE 0.5
#define GROWTH_FACTOR 2
  
  
typedef struct {
    LSQ_BaseTypeT *value;
    LSQ_IntegerIndexT realSize;
    LSQ_IntegerIndexT logicalSize;
} ArrayStruct;
  
typedef struct {
    LSQ_IntegerIndexT index;
    ArrayStruct *array;
} Iterator;
  
static void setSize(ArrayStruct *array, LSQ_IntegerIndexT size) {
    array->realSize = size;
    array->value = (LSQ_BaseTypeT *) realloc(array->value, size * sizeof(LSQ_BaseTypeT));
}
  
extern LSQ_HandleT LSQ_CreateSequence(void) { //
    ArrayStruct *newArray = (ArrayStruct *) malloc(sizeof(ArrayStruct));
    if (newArray == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    newArray->value = (LSQ_BaseTypeT *) malloc(2 * sizeof(LSQ_BaseTypeT));
    newArray->realSize = 2;
    newArray->logicalSize = 0;
    return  newArray;
}
  
extern void LSQ_DestroySequence(LSQ_HandleT handle) { ///
    ArrayStruct *tmpArray = (ArrayStruct *) handle;
    if (tmpArray == LSQ_HandleInvalid)
        return;
    free(tmpArray->value);
    free(tmpArray);
    return;
}
  
extern LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle) { //
    ArrayStruct *tmpArray = (ArrayStruct *) handle;
    if (tmpArray == LSQ_HandleInvalid)
        return 0;
    return (tmpArray->logicalSize);
}
  
extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *)iterator;
    return (tmpIterator != LSQ_HandleInvalid
            && !LSQ_IsIteratorPastRear(iterator) && !LSQ_IsIteratorBeforeFirst(iterator));
}
  
extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *)iterator;
    return (tmpIterator != LSQ_HandleInvalid
            && tmpIterator->index >= tmpIterator->array->logicalSize);
}
  
extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *)iterator;
    return (tmpIterator != LSQ_HandleInvalid && tmpIterator->index < 0);
}
  
extern LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *)iterator;
    if (!LSQ_IsIteratorDereferencable(iterator))
        return LSQ_HandleInvalid;
    return &(tmpIterator->array->value[tmpIterator->index]);
}
  
extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index) {
    ArrayStruct *tmpArray = (ArrayStruct *) handle;
    if (tmpArray == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    Iterator *tmpIterator = (Iterator *) malloc(sizeof(Iterator));
    tmpIterator->array = tmpArray;
    tmpIterator->index = index;
    return tmpIterator;
}
  
extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle) {
    ArrayStruct *tmpArray = (ArrayStruct *) handle;
    if (tmpArray == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    Iterator *tmpIterator = (Iterator *) malloc(sizeof(Iterator));
    tmpIterator->array = tmpArray;
    tmpIterator->index = 0;
    return tmpIterator;
}
  
extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle) {
    ArrayStruct *tmpArray = (ArrayStruct *) handle;
    if (tmpArray == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    Iterator *tmpIterator = (Iterator *) malloc(sizeof(Iterator));
    if (tmpIterator == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    tmpIterator->array = tmpArray;
    tmpIterator->index = LSQ_GetSize(handle);
    return tmpIterator;
}
  
extern void LSQ_DestroyIterator(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *)iterator;
    free(tmpIterator);
}
  
extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *)iterator;
    if (tmpIterator == LSQ_HandleInvalid)
        return;
    tmpIterator->index++;
}
  
extern void LSQ_RewindOneElement(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *)iterator;
    if (tmpIterator == LSQ_HandleInvalid)
        return;
    tmpIterator->index--;
}
  
extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift) {
    Iterator *tmpIterator = (Iterator *)iterator;
    if (tmpIterator == LSQ_HandleInvalid)
        return;
    tmpIterator->index += shift;
}
  
extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos) {
    Iterator *tmpIterator = (Iterator *)iterator;
    if (tmpIterator == LSQ_HandleInvalid)
        return;
    tmpIterator->index = pos;
}
  
extern void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element) {
    ArrayStruct *tmpArray = (ArrayStruct *) handle;
    if (tmpArray == LSQ_HandleInvalid)
        return;
    if (tmpArray->logicalSize == tmpArray->realSize) {
        LSQ_IntegerIndexT size = tmpArray->realSize * GROWTH_FACTOR;
        setSize(tmpArray, size);
    }
 
    for (LSQ_IntegerIndexT i = tmpArray->logicalSize; i > 0; i--) {
        tmpArray->value[i] = tmpArray->value[i - 1];
    }
    tmpArray->value[0] = element;
    tmpArray->logicalSize++;
}
  
extern void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element) {
    ArrayStruct *tmpArray = (ArrayStruct *) handle;
    if (tmpArray == LSQ_HandleInvalid)
        return;
    if (tmpArray->logicalSize == tmpArray->realSize) {
        LSQ_IntegerIndexT size = tmpArray->realSize * GROWTH_FACTOR;
        setSize(tmpArray, size);
    }
    tmpArray->value[tmpArray->logicalSize] = element;
    tmpArray->logicalSize++;
}
  
extern void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement) {
    Iterator *tmpIterator = (Iterator *)iterator;
    if (tmpIterator == LSQ_HandleInvalid)
        return;
  
    if (tmpIterator->array->logicalSize == tmpIterator->array->realSize) {
        LSQ_IntegerIndexT size = tmpIterator->array->realSize * GROWTH_FACTOR;
        setSize(tmpIterator->array, size);
    }
  
    for (LSQ_IntegerIndexT i = tmpIterator->array->logicalSize; i > tmpIterator->index; i--) {
        tmpIterator->array->value[i] = tmpIterator->array->value[i - 1];
    }
    tmpIterator->array->value[tmpIterator->index] = newElement;
    tmpIterator->array->logicalSize++;
}
  
extern void LSQ_DeleteFrontElement(LSQ_HandleT handle) {
    ArrayStruct *tmpArray = (ArrayStruct *) handle;
    if (tmpArray == LSQ_HandleInvalid || tmpArray->logicalSize == 0)
        return;
 
    tmpArray->logicalSize--;
    for (LSQ_IntegerIndexT i = 0; i < tmpArray->logicalSize; i++) {
        tmpArray->value[i] = tmpArray->value[i + 1];
    }
    if (tmpArray->logicalSize < tmpArray->realSize * PERCENT_LOW_LINE) {
        int size = tmpArray->realSize / GROWTH_FACTOR;
        if (size == 0)
            tmpArray->realSize = 5;
        setSize(tmpArray, size);
    }
}
  
extern void LSQ_DeleteRearElement(LSQ_HandleT handle) {
    ArrayStruct *tmpArray = (ArrayStruct *) handle;
    if (tmpArray == LSQ_HandleInvalid || tmpArray->logicalSize == 0)
        return;
 
    tmpArray->logicalSize--;
    if (tmpArray->logicalSize < tmpArray->realSize * PERCENT_LOW_LINE) {
        int size = tmpArray->realSize / GROWTH_FACTOR;
        if (size == 0)
            tmpArray->realSize = 5;
        setSize(tmpArray, size);
    }
}
  
extern void LSQ_DeleteGivenElement(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *)iterator;
    if (!LSQ_IsIteratorDereferencable(iterator))
        return;
  
    tmpIterator->array->logicalSize--;
    for (LSQ_IntegerIndexT i = tmpIterator->index; i < tmpIterator->array->logicalSize; i++) {
        tmpIterator->array->value[i] = tmpIterator->array->value[i + 1];
    }
    if (tmpIterator->array->logicalSize <= tmpIterator->array->realSize * PERCENT_LOW_LINE) {
        int size = tmpIterator->array->realSize / GROWTH_FACTOR;
        if (size == 0)
            tmpIterator->array->realSize = 5;
        setSize(tmpIterator->array, size);
    }
}