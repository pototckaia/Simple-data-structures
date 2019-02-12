#include <stdlib.h>
#include <math.h>
#include "linear_sequence_assoc.h"
 
 
#define MAXIMUM(a, b) ((a) > (b) ? (a) : (b))
 
typedef struct Node_ {
    LSQ_BaseTypeT value;
    LSQ_IntegerIndexT key;
    LSQ_IntegerIndexT height;
    struct Node_ *parent;
    struct Node_ *leftChild;
    struct Node_ *rightChild;
} Node;
 
typedef struct {
    Node *root;
    LSQ_IntegerIndexT size;
    Node *nodePastRear;
    Node *nodeBeforeFirst;
} Tree;
 
typedef struct {
    Tree *tree;
    Node *node;
} Iterator;
 
static Iterator *createIterator(Tree *, Node *);
static Node *createNode(LSQ_BaseTypeT , LSQ_IntegerIndexT , Node *);
static Node *getMinNode(Node *);
static Node *getMaxNode(Node *);
static Node *getSuccessor(Node *);
static Node *getPredecessor(Node *);
static Node *getByKey(Node *, LSQ_IntegerIndexT );
static LSQ_IntegerIndexT getBalanceFactor(Node *);
static void fixHeight(Node *);
static void replaceNode(Tree *, Node *, Node *);
static void balancing(Tree *, Node *);
static void freeNode(Node *);
 
LSQ_HandleT LSQ_CreateSequence(void) {
    Tree *newTree = (Tree *) malloc(sizeof(Tree));
    if (newTree == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    newTree->root = NULL;
    newTree->size = 0;
    newTree->nodePastRear = createNode(0, 0, NULL);
    newTree->nodeBeforeFirst = createNode(0, 0, NULL);
    return newTree;
}
 
void LSQ_DestroySequence(LSQ_HandleT handle) {
    Tree *tmpTree = (Tree *) handle;
    if (tmpTree == LSQ_HandleInvalid)
        return;
    if (tmpTree->root != LSQ_HandleInvalid) {
        freeNode(tmpTree->root);
    }
    free(tmpTree->nodeBeforeFirst);
    free(tmpTree->nodePastRear);
    free(tmpTree);
}
 
LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle){
    Tree *tmpTree = (Tree *) handle;
    if (tmpTree == LSQ_HandleInvalid) {
        return 0;
    }
    return tmpTree->size;
}
 
 
int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *) iterator;
    return (tmpIterator != LSQ_HandleInvalid && tmpIterator->tree != LSQ_HandleInvalid
            && !LSQ_IsIteratorPastRear(tmpIterator) && !LSQ_IsIteratorBeforeFirst(tmpIterator));
}
 
extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *) iterator;
    return (tmpIterator != LSQ_HandleInvalid && tmpIterator->node == tmpIterator->tree->nodePastRear);
}
 
extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator){
    Iterator *tmpIterator = (Iterator *) iterator;
    return (tmpIterator != LSQ_HandleInvalid && tmpIterator->node == tmpIterator->tree->nodeBeforeFirst);
}
 
extern LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator) {
    if (!LSQ_IsIteratorDereferencable(iterator)) {
        return LSQ_HandleInvalid;
    }
    Iterator *tmpIterator = (Iterator *) iterator;
    return &(tmpIterator->node->value);
}
 
extern LSQ_IntegerIndexT LSQ_GetIteratorKey(LSQ_IteratorT iterator) {
    if (!LSQ_IsIteratorDereferencable(iterator)){
        return -1;
    }
    Iterator *tmpIterator = (Iterator *) iterator;
    return (tmpIterator->node->key);
}
 
extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index) {
    Tree *tmpTree = (Tree *) handle;
    if (tmpTree == LSQ_HandleInvalid) {
        return LSQ_HandleInvalid;
    }
    Node *tmpNode = getByKey(tmpTree->root, index);
    if (tmpNode == LSQ_HandleInvalid) {
        tmpNode = tmpTree->nodePastRear;
    }
    return createIterator(tmpTree, tmpNode);
 
}
 
extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle) {
    Tree *tmpTree = (Tree *) handle;
    if (tmpTree == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    Node *tmpNode = getMinNode(tmpTree->root);
    if (tmpNode == LSQ_HandleInvalid) {
        tmpNode = tmpTree->nodePastRear;
    }
    return createIterator(tmpTree, tmpNode);
}
 
extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle) {
    Tree *tmpTree = (Tree *) handle;
    if (tmpTree == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    return createIterator(tmpTree, tmpTree->nodePastRear);
}
 
extern void LSQ_DestroyIterator(LSQ_IteratorT iterator){
    Iterator *tmpIterator = (Iterator *) iterator;
    free(tmpIterator);
}
 
 
extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *)iterator;
    if (tmpIterator == LSQ_HandleInvalid || tmpIterator->tree == LSQ_HandleInvalid || LSQ_IsIteratorPastRear(tmpIterator))
        return;
 
    if (LSQ_IsIteratorBeforeFirst(tmpIterator)) {
        tmpIterator->node = getMinNode(tmpIterator->tree->root);
    }
    else {
        tmpIterator->node = getSuccessor(tmpIterator->node);
    }
 
     if (tmpIterator->node == LSQ_HandleInvalid) {
        tmpIterator->node = tmpIterator->tree->nodePastRear;
    }
}
 
extern void LSQ_RewindOneElement(LSQ_IteratorT iterator) {
    Iterator *tmpIterator = (Iterator *) iterator;
    if (tmpIterator == LSQ_HandleInvalid || tmpIterator->tree == LSQ_HandleInvalid || LSQ_IsIteratorBeforeFirst(tmpIterator))
        return;
 
    if (LSQ_IsIteratorPastRear(tmpIterator)) {
        tmpIterator->node = getMaxNode(tmpIterator->tree->root);
    } else {
        tmpIterator->node = getPredecessor(tmpIterator->node);
    }
 
    if (tmpIterator->node == LSQ_HandleInvalid) {
        tmpIterator->node = tmpIterator->tree->nodeBeforeFirst;
    }
}
 
 
extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift) {
    Iterator *tmpIterator = (Iterator *) iterator;
    if (tmpIterator == LSQ_HandleInvalid || shift == 0)
        return;
    if (shift > 0) {
        for (LSQ_IntegerIndexT i = shift; i != 0 && !LSQ_IsIteratorPastRear(tmpIterator); i--) {
            LSQ_AdvanceOneElement(tmpIterator);
        }
    }
    else {
        for (LSQ_IntegerIndexT i = shift; i != 0 && !LSQ_IsIteratorBeforeFirst(tmpIterator); i++) {
            LSQ_RewindOneElement(tmpIterator);
        }
    }
}
 
extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos){ ///
    Iterator *tmpIterator = (Iterator *) iterator;
    if (tmpIterator == LSQ_HandleInvalid) {
        return;
    }
    tmpIterator->node = tmpIterator->tree->nodeBeforeFirst;
    for (LSQ_IntegerIndexT i = 0; i < pos && !LSQ_IsIteratorPastRear(tmpIterator); i++) {
        LSQ_AdvanceOneElement(tmpIterator);
    }
}
 
extern void LSQ_InsertElement(LSQ_HandleT handle, LSQ_IntegerIndexT key, LSQ_BaseTypeT value) {
    Tree *tmpTree = (Tree *) handle;
    if (tmpTree == LSQ_HandleInvalid)
        return;
 
    Node *tmpNode = tmpTree->root;
    Node *parent = LSQ_HandleInvalid;
 
    while (tmpNode != LSQ_HandleInvalid) {
        parent = tmpNode;
        if (key < tmpNode->key) // == ?
            tmpNode = tmpNode->leftChild;
        else if (key > tmpNode->key)
            tmpNode = tmpNode->rightChild;
        else if (key == tmpNode->key) {
            tmpNode->value = value;
            return;
        }
    }
 
    Node *newNode = createNode(value, key, parent);
    if (parent == LSQ_HandleInvalid) {
        tmpTree->root = newNode;
    }
    else if (key < parent->key) {
        parent->leftChild = newNode;
    }
    else {
        parent->rightChild = newNode;
    }
 
    tmpTree->size++;
 
    if (parent == LSQ_HandleInvalid) {
        parent = tmpTree->root;
    }
    tmpNode = parent;
    while (tmpNode != LSQ_HandleInvalid) {
        fixHeight(tmpNode);
        tmpNode = tmpNode->parent;
    }
 
    tmpNode = parent;
    while (tmpNode != LSQ_HandleInvalid) {
        balancing(tmpTree, tmpNode);
        tmpNode = tmpNode->parent;
    }
 
}
 
extern void LSQ_DeleteFrontElement(LSQ_HandleT handle) {
    Tree *tmpTree = (Tree *) handle;
    if (tmpTree == LSQ_HandleInvalid || tmpTree->root == LSQ_HandleInvalid)
        return;
    Node *tmpNode = getMinNode(tmpTree->root);
    LSQ_DeleteElement(handle, tmpNode->key);
}
 
extern void LSQ_DeleteRearElement(LSQ_HandleT handle) {
    Tree *tmpTree = (Tree *) handle;
    if (tmpTree == LSQ_HandleInvalid || tmpTree->root == LSQ_HandleInvalid)
        return;
    Node *tmpNode = getMaxNode(tmpTree->root);
    LSQ_DeleteElement(handle, tmpNode->key);
}
 
extern void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_IntegerIndexT key) {
    Tree *tmpTree = (Tree *) handle;
    if (tmpTree == LSQ_HandleInvalid || tmpTree->root == LSQ_HandleInvalid || LSQ_GetSize(tmpTree) == 0)
        return;
 
    Node *tmpNode = getByKey(tmpTree->root, key);
    if (tmpNode == LSQ_HandleInvalid)
        return;
 
    Node *parent = tmpNode->parent;
    if (tmpNode->rightChild == LSQ_HandleInvalid && tmpNode->leftChild == LSQ_HandleInvalid) {
        replaceNode(tmpTree, tmpNode, LSQ_HandleInvalid);
    }
    else if (tmpNode->leftChild == LSQ_HandleInvalid) {
        replaceNode(tmpTree, tmpNode, tmpNode->rightChild);
    }
    else if (tmpNode->rightChild == LSQ_HandleInvalid) {
        replaceNode(tmpTree, tmpNode, tmpNode->leftChild);
    }
    else {
        Node *successorNode = getMinNode(tmpNode->rightChild);
        if (successorNode->parent != tmpNode) {
            replaceNode(tmpTree, successorNode, successorNode->rightChild);
            successorNode->rightChild = tmpNode->rightChild;
            successorNode->rightChild->parent = successorNode;
        }
        replaceNode(tmpTree, tmpNode, successorNode);
        successorNode->leftChild = tmpNode->leftChild;
        successorNode->leftChild->parent = successorNode;
    }
 
    tmpTree->size--;
    free(tmpNode);
    if (parent == LSQ_HandleInvalid) {
        parent = tmpTree->root;
    }
 
    tmpNode = parent;
    while (tmpNode != LSQ_HandleInvalid) {
        fixHeight(tmpNode);
        tmpNode = tmpNode->parent;
    }
    tmpNode = parent;
    while (tmpNode != LSQ_HandleInvalid) {
        balancing(tmpTree, tmpNode);
        tmpNode = tmpNode->parent;
    }
}
 
 
static void freeNode(Node *root) {
    if (root->leftChild != LSQ_HandleInvalid) {
        freeNode(root->leftChild);
    }
    if (root->rightChild != LSQ_HandleInvalid) {
        freeNode(root->rightChild);
    }
    free(root);
}
 
static Iterator *createIterator(Tree *tree, Node *node) {
    Iterator *newIterator = (Iterator *) malloc(sizeof(Iterator));
    if (newIterator == LSQ_HandleInvalid) {
        return LSQ_HandleInvalid;
    }
    newIterator->tree = tree;
    newIterator->node = node;
    return newIterator;
}
 
static Node *createNode(LSQ_BaseTypeT value, LSQ_IntegerIndexT key, Node *parent) {
    Node *tmpNode = (Node *) malloc(sizeof(Node));
    if (tmpNode == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    tmpNode->value = value;
    tmpNode->key = key;
    tmpNode->height = 0;
    tmpNode->parent = parent;
    tmpNode->leftChild = tmpNode->rightChild = LSQ_HandleInvalid;
    return tmpNode;
}
 
static Node *getMinNode(Node *root) {
    if (root == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    while (root->leftChild != LSQ_HandleInvalid) {
        root = root->leftChild;
    }
    return root;
}
 
static Node *getMaxNode(Node *root) {
    if (root == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    while (root->rightChild != LSQ_HandleInvalid) {
        root = root->rightChild;
    }
    return root;
}
 
static Node *getSuccessor(Node *node) {
    if (node == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    if (node->rightChild != LSQ_HandleInvalid)
        return getMinNode(node->rightChild);
    Node *parent = node->parent;
    Node *tmpNode = node;
    while (parent != LSQ_HandleInvalid && tmpNode == parent->rightChild) {
        tmpNode = parent;
        parent = parent->parent;
    }
    return parent;
}
 
static Node *getPredecessor(Node *node) {
    if (node == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;
    if (node->leftChild != LSQ_HandleInvalid)
        return getMaxNode(node->leftChild);
    Node *parent = node->parent;
    Node *tmpNode = node;
    while (parent != LSQ_HandleInvalid && tmpNode == parent->leftChild) {
        tmpNode = parent;
        parent = parent->parent;
    }
    return parent;
}
 
static Node *getByKey(Node *root, LSQ_IntegerIndexT key){
    while (root != LSQ_HandleInvalid) {
        if (root->key > key) {
            root = root->leftChild;
        }
        else if (root->key < key) {
            root = root->rightChild;
        }
        else
            return root;
    }
    return LSQ_HandleInvalid;
}
 
static LSQ_IntegerIndexT getHeight(Node *node) {
    return ((node != LSQ_HandleInvalid) ? node->height : -1);
}
 
static LSQ_IntegerIndexT getBalanceFactor(Node *node) {
    return (getHeight(node->leftChild) - getHeight(node->rightChild)); // node != NULL
}
 
static void fixHeight(Node *node) {
    int t = MAXIMUM(getHeight(node->leftChild), getHeight(node->rightChild)) + 1;
    node->height = MAXIMUM(getHeight(node->leftChild), getHeight(node->rightChild)) + 1; // node != NULL
}
 
static void replaceNode(Tree *tree, Node *node, Node *substitute) {
    if (node == LSQ_HandleInvalid)
        return;
    if (substitute != LSQ_HandleInvalid)
        substitute->parent = node->parent;
    if (node->parent == LSQ_HandleInvalid)
        tree->root = substitute;
    else if (node->parent->leftChild == node)
        node->parent->leftChild = substitute;
    else
        node->parent->rightChild = substitute;
}
 
static void leftRotation(Tree *tree, Node *root) {
    if (root == LSQ_HandleInvalid || root->rightChild == LSQ_HandleInvalid)
        return;
    Node *newRoot = root->rightChild;
 
    root->rightChild = newRoot->leftChild;
    if (newRoot->leftChild != LSQ_HandleInvalid) {
        newRoot->leftChild->parent = root;
    }
    newRoot->leftChild = root;
 
    newRoot->parent = root->parent;
    if (root->parent == LSQ_HandleInvalid) {
        tree->root = newRoot;
    }
    else if (root->parent->leftChild == root) {
        root->parent->leftChild = newRoot;
    }
    else {
        root->parent->rightChild = newRoot;
    }
    root->parent = newRoot;
    fixHeight(root);
    fixHeight(newRoot);
}
 
static void rightRotation(Tree *tree, Node *root) {
    if (root == LSQ_HandleInvalid || root->leftChild == LSQ_HandleInvalid)
        return;
    Node *newRoot = root->leftChild;
 
    root->leftChild = newRoot->rightChild;
    if (newRoot->rightChild != LSQ_HandleInvalid) {
        newRoot->rightChild->parent = root;
    }
    newRoot->rightChild = root;
 
    newRoot->parent = root->parent;
    if (root->parent == LSQ_HandleInvalid) {
        tree->root = newRoot;
    }
    else if (root->parent->leftChild == root) {
        root->parent->leftChild = newRoot;
    }
    else {
        root->parent->rightChild = newRoot;
    }
    root->parent = newRoot;
    fixHeight(root);
    fixHeight(newRoot);
}
 
static void rightLeftRotation(Tree *tree, Node *root) {
    if (root == LSQ_HandleInvalid || root->rightChild == LSQ_HandleInvalid)
        return;
    rightRotation(tree, root->rightChild);
    leftRotation(tree, root);
}
 
static void leftRightRotation(Tree *tree, Node *root) {
    if (root == LSQ_HandleInvalid || root->leftChild == LSQ_HandleInvalid)
        return;
    leftRotation(tree, root->leftChild);
    rightRotation(tree, root);
}
 
static void balancing(Tree *tree, Node *root) {
    if (root == LSQ_HandleInvalid)
        return;
    if (getBalanceFactor(root) == 2) {
        if (getBalanceFactor(root->leftChild) >= 0) {
            rightRotation(tree, root);
        }
        else {
            leftRightRotation(tree, root);
        }
    }
    else if (getBalanceFactor(root) == -2) {
        if (getBalanceFactor(root->rightChild) <= 0) {
            leftRotation(tree, root);
        }
        else {
            rightLeftRotation(tree, root);
        }
 
    }
}
