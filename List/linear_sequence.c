#include <stdlib.h>
#include "linear_sequence.h"

typedef struct Node_ {
    LSQ_BaseTypeT value;
    struct Node_ *next;
    struct Node_ *prev;
} Node;

typedef struct {
    Node *nodeBeforFirst;
    Node *nodePastReer;
    LSQ_IntegerIndexT size;
} DblList;

typedef struct {
    DblList *list;
    Node *node;
} Iterator;

extern LSQ_HandleT LSQ_CreateSequence(void) {
    DblList *tmpList = (DblList *) malloc(sizeof(DblList));
    if (tmpList == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    tmpList->size = 0;
    tmpList->nodeBeforFirst = (Node *) malloc(sizeof(Node));
    if (tmpList->nodeBeforFirst == LSQ_HandleInvalid) {
        free(tmpList);
        return LSQ_HandleInvalid;
    }
    tmpList->nodePastReer = (Node *) malloc(sizeof(Node));
    if (tmpList->nodePastReer == LSQ_HandleInvalid) {
        free(tmpList->nodeBeforFirst);
        free(tmpList);
        return LSQ_HandleInvalid;
    }
    tmpList->nodeBeforFirst->prev = LSQ_HandleInvalid;
    tmpList->nodeBeforFirst->next = tmpList->nodePastReer;
    tmpList->nodePastReer->next = LSQ_HandleInvalid;
    tmpList->nodePastReer->prev = tmpList->nodeBeforFirst;
    return tmpList;
}

extern void LSQ_DestroySequence(LSQ_HandleT handle) {
    DblList *tmpList = (DblList *) handle;
    if (tmpList == LSQ_HandleInvalid)
        return;
    Node *tmpNode = tmpList->nodeBeforFirst;
    while (tmpNode != LSQ_HandleInvalid) {
        Node *nextNode = tmpNode->next;
        free(tmpNode);
        tmpNode = nextNode;
    }
    free(handle);
    handle = LSQ_HandleInvalid;
}

extern LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle) {
    DblList *tmpList = (DblList *) handle;
    return ((tmpList == LSQ_HandleInvalid) ? 0: tmpList->size);
}

extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *) iterator;
    return (tmpIterator != LSQ_HandleInvalid && tmpIterator->node != LSQ_HandleInvalid &&
            tmpIterator->list != LSQ_HandleInvalid && !LSQ_IsIteratorBeforeFirst(iterator) &&
            !LSQ_IsIteratorPastRear(iterator));
}

extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *) iterator;
    return ((tmpIterator != LSQ_HandleInvalid) && (tmpIterator->list != LSQ_HandleInvalid)
            && tmpIterator->node == tmpIterator->list->nodePastReer);
}

extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *) iterator;
    return ((tmpIterator != LSQ_HandleInvalid) && (tmpIterator->list != LSQ_HandleInvalid)
            && tmpIterator->node == tmpIterator->list->nodeBeforFirst);
}

extern LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *) iterator;
    if (!LSQ_IsIteratorDereferencable(iterator))
        return LSQ_HandleInvalid;
    return &(tmpIterator->node->value);
}

extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index) {
    DblList *tmpList = (DblList *) handle;
    if (tmpList == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;

    Node *tmpNode = tmpList->nodeBeforFirst->next;
    for (LSQ_IntegerIndexT i = 0; tmpNode->next != LSQ_HandleInvalid && i < index; i++) {
        tmpNode = tmpNode->next;
    }

    Iterator *tmpIterator = (Iterator *) malloc(sizeof(Iterator));
    if (tmpIterator == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    tmpIterator->node = tmpNode;
    tmpIterator->list = tmpList;
    return tmpIterator;
}

extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle) {
    DblList *tmpList = (DblList *) handle;
    if (tmpList == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;

    Node *tmpNode = tmpList->nodeBeforFirst->next;
    Iterator *tmpIterator = (Iterator *) malloc(sizeof(Iterator));
    if (tmpIterator == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    tmpIterator->node = tmpNode;
    tmpIterator->list = tmpList;
    return tmpIterator;
}

extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle) {
    DblList *tmpList = (DblList *) handle;
    if (tmpList == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;

    Node *tmpNode = tmpList->nodePastReer;
    Iterator *tmpIterator = (Iterator *) malloc(sizeof(Iterator));
    if (tmpIterator == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    tmpIterator->node = tmpNode;
    tmpIterator->list = tmpList;
    return tmpIterator;
}

extern void LSQ_DestroyIterator(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *) iterator;
    if (tmpIterator == LSQ_HandleInvalid)
        return;
    free(iterator);
}

extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *) iterator;
    if (tmpIterator == LSQ_HandleInvalid || tmpIterator->node->next == LSQ_HandleInvalid)
        return;
    tmpIterator->node = tmpIterator->node->next;
}

extern void LSQ_RewindOneElement(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *) iterator;
    if (tmpIterator == LSQ_HandleInvalid || tmpIterator->node->prev == LSQ_HandleInvalid)
        return;
    tmpIterator->node = tmpIterator->node->prev;
}

extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift) {
    Iterator *tmpIterator = (Iterator *) iterator;
    if (tmpIterator == LSQ_HandleInvalid || shift == 0)
        return;
    LSQ_IntegerIndexT i = shift;
    if (shift > 0){
        while (i != 0 && tmpIterator->node->next != LSQ_HandleInvalid) {
            tmpIterator->node = tmpIterator->node->next;
            i--;
        }
    }
    else {
        while (i != 0 && tmpIterator->node->prev != LSQ_HandleInvalid) {
            tmpIterator->node = tmpIterator->node->prev;
            i++;
        }
    }
}

extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos) {
    Iterator *tmpIterator = (Iterator *) iterator;
    if (tmpIterator == LSQ_HandleInvalid || tmpIterator->node == LSQ_HandleInvalid ||
        tmpIterator->list == LSQ_HandleInvalid)
        return;

    tmpIterator->node = tmpIterator->list->nodeBeforFirst;
    for (LSQ_IntegerIndexT i = 0; i <= pos && tmpIterator->node->next != LSQ_HandleInvalid; i++) {
        tmpIterator->node = tmpIterator->node->next;
    }
}

extern void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element) {
    DblList *tmpList = (DblList *) handle;
    if (tmpList == LSQ_HandleInvalid)
        return;
    Node *newNode = (Node *) malloc(sizeof(Node));
    if (newNode == LSQ_HandleInvalid)
        return;
    newNode->value = element;
    newNode->prev = tmpList->nodeBeforFirst;
    newNode->next = tmpList->nodeBeforFirst->next;

    tmpList->nodeBeforFirst->next->prev = newNode;
    tmpList->nodeBeforFirst->next = newNode;
    tmpList->size++;
}

extern void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element) {
    DblList *tmpList = (DblList *) handle;
    if (tmpList == LSQ_HandleInvalid)
        return;
    Node *newNode = (Node *) malloc(sizeof(Node));
    if (newNode == LSQ_HandleInvalid)
        return;
    newNode->value = element;
    newNode->next = tmpList->nodePastReer;
    newNode->prev = tmpList->nodePastReer->prev;

    tmpList->nodePastReer->prev->next = newNode;
    tmpList->nodePastReer->prev = newNode;
    tmpList->size++;
}

extern void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement) {
    Iterator *tmpIterator = (Iterator *) iterator;
    if (tmpIterator == LSQ_HandleInvalid)
        return;
    Node *newNode = (Node *) malloc(sizeof(Node));
    if (newNode == LSQ_HandleInvalid)
        return;
    newNode->value = newElement;
    newNode->prev = tmpIterator->node->prev;
    newNode->next = tmpIterator->node;

    tmpIterator->node->prev->next = newNode;
    tmpIterator->node->prev = newNode;
    tmpIterator->node = newNode;
    tmpIterator->list->size++;
}

extern void LSQ_DeleteFrontElement(LSQ_HandleT handle) {
    DblList *tmpList = (DblList *) handle;
    if (tmpList == LSQ_HandleInvalid || tmpList->nodeBeforFirst->next == tmpList->nodePastReer)
        return;

    Node *tmpNode = tmpList->nodeBeforFirst->next;
    tmpNode->next->prev = tmpList->nodeBeforFirst;
    tmpList->nodeBeforFirst->next = tmpNode->next;

    tmpList->size--;
    free(tmpNode);
}

extern void LSQ_DeleteRearElement(LSQ_HandleT handle) {
    DblList *tmpList = (DblList *) handle;
    if (tmpList == LSQ_HandleInvalid || tmpList->nodePastReer->prev == tmpList->nodeBeforFirst)
        return;

    Node *tmpNode = tmpList->nodePastReer->prev;
    tmpNode->prev->next = tmpList->nodePastReer;
    tmpList->nodePastReer->prev = tmpNode->prev;

    tmpList->size--;
    free(tmpNode);
}

extern void LSQ_DeleteGivenElement(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *)iterator;
    if (!LSQ_IsIteratorDereferencable(iterator))
        return;

    Node *tmpNode = tmpIterator->node;
    tmpNode->next->prev = tmpNode->prev;
    tmpNode->prev->next = tmpNode->next;
    tmpIterator->node = tmpNode->next;

    tmpIterator->list->size--;
    free(tmpNode);
}