#include "list.h"
#include <stdio.h>
// Makes a new, empty list, and returns its reference on success. 
// Returns a NULL pointer on failure.
List* List_create()
{
    //Inital Creation of a static variables to maintain the number of calls to List_Create()
    static int createCount = 0;
    static int nodesUsed = 0;
    static int freeNodesCount = 0;
    static int freeListCount = 0;
    static List  headArr[LIST_MAX_NUM_HEADS];
    static Node  nodeArr[LIST_MAX_NUM_NODES];
    static Node * freedNodes[LIST_MAX_NUM_NODES];//Pointer to the freedNodes should be checked if freeNodeIx == LIST_MAX_NUM_NODES
    static List * freeListArr[LIST_MAX_NUM_HEADS];//Pointer to pointers representing free head arrays

     
    //INITALIZATION STAGE
    if(createCount == 0) //Assigning each list to point to the global Nodes
    {
        for(int i = 0; i < LIST_MAX_NUM_HEADS; i++)
        {
            headArr[i].currNode = NULL;
            headArr[i].head = NULL;
            headArr[i].tail = NULL;
            headArr[i].position = 0;
            headArr[i].nodeCount = 0;
            
            //GLOBAL STATIC ACCESS VARIABLES
            headArr[i].nodeArr = nodeArr;
            headArr[i].nodeArrIdx = &(nodesUsed);
            headArr[i].freedNodesCount = &(freeNodesCount);
            headArr[i].freeNodesArr = freedNodes;
            headArr[i].freeListArr = freeListArr;
            headArr[i].freeListCount = &(freeListCount);
        }
    }

    //Distribution of LISTS 
    if(createCount < LIST_MAX_NUM_HEADS) //Try accessing the inital unused lists
    {
        return &(headArr[createCount++]);
    }else if(createCount >= LIST_MAX_NUM_HEADS && freeListCount > 0)//Try accessing any free lists
    {
        return freeListArr[--freeListCount];
    }else
    {
        return NULL; //No more lists are available
    }
}

// Returns the number of items in pList.
int List_count(List* pList)
{
    return pList->nodeCount;
}

// Returns a pointer to the first item in pList and makes the first item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_first(List* pList)
{
    if(pList->nodeCount == 0)
    {
        pList->currItemPtr = NULL;
        return NULL;
    }else
    {
        pList->currNode = pList->head;
        pList->currItemPtr = pList->head->dataPtr;
        pList->position = 1;
        return pList->currItemPtr;
    }
}

// Returns a pointer to the last item in pList and makes the last item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_last(List* pList)
{
    assert(pList != NULL);

    if(pList->nodeCount == 0)
    {
        pList->currItemPtr = NULL;
        return NULL;
    }else
    {
        pList->currNode = pList->tail;
        pList->currItemPtr = pList->tail->dataPtr;
        pList->position = 1;
        return pList->currItemPtr;
    }
    
}

// Advances pList's current item by one, and returns a pointer to the new current item.
// If this operation advances the current item beyond the end of the pList, a NULL pointer 
// is returned and the current item is set to be beyond end of pList.
void* List_next(List* pList)
{
    assert(pList != NULL);

    if(pList->currNode == pList->tail)//The Current Node is at the tail
    {
        //Wont increment the pointer currNode but will made the currItemPtr = NULL
        pList->currItemPtr = NULL;
        pList->position = -1;
        return NULL;

    }else if((pList->currNode == pList->head) && (pList->position == 0))//List iterator is out of bounds at the head of the list
    {
        pList->currItemPtr = pList->head->dataPtr;
        pList->position = 1; //Back in array bounds
        return pList->currItemPtr;
    }else
    {
        pList->currNode = pList->currNode->next;
        pList->currItemPtr = pList->currNode->dataPtr;
        pList->position = 1;
        return pList->currItemPtr;
    }
    
}

// Backs up pList's current item by one, and returns a pointer to the new current item. 
// If this operation backs up the current item beyond the start of the pList, a NULL pointer 
// is returned and the current item is set to be before the start of pList.
void* List_prev(List* pList)
{
    assert(pList != NULL);

    if(pList->currNode == pList->head)//Current node is at the head
    {
        pList->currItemPtr = NULL;
        pList->position = 0; //Current Pointer is beyond the start of the list
        return NULL; 
    }else if((pList->currNode == pList->tail) && (pList->position == -1)) //List_Prev() is called while the list iterator is out of bounds at the tail
    {
        pList->currItemPtr = pList->tail->dataPtr;
        pList->position = 1; //In list bounds
        return pList->currItemPtr;
    }
    else
    {
        pList->currNode = pList->currNode->prev;
        pList->currItemPtr = pList->currNode->dataPtr;
        pList->position = 1;
        return pList->currItemPtr;
    }
    
}

// Returns a pointer to the current item in pList.
void* List_curr(List* pList)
{
    assert(pList != NULL); //List Cannot Be Null
    return pList->currItemPtr;
}

// Adds the new item to pList directly after the current item, and makes item the current item. 
// If the current pointer is before the start of the pList, the item is added at the start. If 
// the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_add(List* pList, void* pItem)
{
    Node * newNode;
    //Check to ensure that a sufficient number of nodes are available
    if( *(pList->nodeArrIdx) == LIST_MAX_NUM_NODES)
    {
        //Checking the freedNodes memory space
        if(!(*(pList->freedNodesCount))) //If freedNodesCount == 0
            return -1;

        *(pList->freedNodesCount) -= 1;
        newNode = pList->freeNodesArr[*(pList->freedNodesCount)]; //Grab a node from freedNodes
        newNode->dataPtr = pItem;

    }else
    {
        //Grab the avaliable node from the static global node array
        newNode = &(pList->nodeArr[*(pList->nodeArrIdx)]);
        newNode->dataPtr = pItem;

        //Modify the static freeNodeIdx
        *(pList->nodeArrIdx) += 1;
    }


    int rtVal = -1;
    //If the list is empty 
    if(pList->nodeCount == 0)
    {
        pList->head = newNode;
        pList->position = 1;
        pList->currNode = newNode;
        pList->currItemPtr = pItem;
        pList->tail = newNode;
        pList->currNode->next = NULL;
        pList->currNode->prev = NULL;
        pList->nodeCount += 1;
        
        return 0;
    }

    //If the list has one node corner case 
    if(pList->nodeCount == 1)
    {
        if(pList->position != 0) //Regualr insertion after the current node applies to iterator beyond the list for this corner case
        {
            newNode->next = pList->currNode->next;
            newNode->prev = pList->currNode;
            pList->position = 1;
            pList->currNode->next = newNode;
            pList->currNode = newNode;
            pList->currItemPtr = pItem;
            pList->tail = newNode;
            pList->nodeCount += 1;
        }else
        {
            newNode->next = pList->currNode;
            newNode->prev = NULL;
            pList->position = 1;
            pList->currNode->prev = newNode; //Point current head to the new node
            pList->head = newNode; //Update the current head
            pList->currNode= newNode;
            pList->currItemPtr = pItem;
            pList->nodeCount += 1;
        }
        return 0;
    }

    //Check to see if the current node is at the tail
    if(pList->currNode == pList->tail)
    {
        
        newNode->next = NULL;
        newNode->prev = pList->currNode; //Points to the current tail
        pList->currNode->next = newNode; //Points the current tail to the new node
        pList->currNode = newNode; //Moves the new node to the current node
        pList->tail = newNode; //Points the tail at the newNode
        pList->currItemPtr = newNode->dataPtr;//Set the new data pointer
        pList->nodeCount += 1;//Increase the node count
        rtVal = 0;

    }else if(pList->currNode == pList->head) //If the current node is the head
    {
        //pList Iterator is before the head
        if(pList->position == 0)
        {
            newNode->next = pList->currNode; //Sets the newNode to point to the current head
            newNode->prev = NULL; //The newNode prev will point before the head
            pList->head = newNode; //The head is updated
            pList->currNode = newNode; //Point the current node to the head
            pList->currNode->next->prev = pList->currNode; //Update the next nodes previous pointer
            pList->currItemPtr = newNode->dataPtr; 
            pList->nodeCount += 1;
            rtVal = 0;
        }else
        {
            newNode->next = pList->currNode->next; //Sets the newNode to point to the current head next
            newNode->prev = pList->currNode; //The newNode prev will point to the head
            pList->currNode->next = newNode;
            pList->currNode->next->prev = newNode; //The current head next node prev pointer to point to new node
            pList->currNode = newNode; //Make the current node the new node
            pList->currItemPtr = newNode->dataPtr;//Set the new currItemPtr
            pList->nodeCount += 1;
            rtVal = 0;
        }
        

    }else //If the node is not the head or tail
    {
        newNode->next = pList->currNode->next; //Point the newNode to whateveter current node is pointing too
        newNode->prev = pList->currNode; //Link the newNode to the previous node which is the current node
        pList->currNode->next = newNode; //Point the current node to the new node
        pList->currNode = newNode; //Update the current node to the new Node
        pList->currNode->next->prev = pList->currNode; //Update the node past current node prev pointer to point to currNode
        pList->currItemPtr = pList->currNode->dataPtr;
        pList->nodeCount += 1;
        rtVal = 0; 
    }
    return rtVal;
}

// Adds item to pList directly before the current item, and makes the new item the current one. 
// If the current pointer is before the start of the pList, the item is added at the start. 
// If the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_insert(List* pList, void* pItem)
{

    if(pList == NULL)
        return -1;
    if(pList->nodeCount == 0)
        return List_add(pList,pItem);
    //Move the list iterator once step back
    List_prev(pList);
    List_add(pList,pItem);
    return 0;
}

// Adds item to the end of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_append(List* pList, void* pItem)
{
    if(pList == NULL)
        return -1;
    if(pList->nodeCount == 0)
        return List_add(pList,pItem);
    List_last(pList);
    return List_add(pList,pItem);
}

// Adds item to the front of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_prepend(List* pList, void* pItem)
{
    if(pList == NULL)
        return -1;
    if(pList->nodeCount == 0)
        return List_add(pList,pItem);
    List_first(pList);
    return List_insert(pList,pItem);
}

// Return current item and take it out of pList. Make the next item the current one.
// If the current pointer is before the start of the pList, or beyond the end of the pList,
// then do not change the pList and return NULL.
void* List_remove(List* pList)
{
    if(pList == NULL)
        return NULL;

    //Simple bounds check on the pList
    if((pList->position == 0) || (pList->position == -1))
        return NULL; //pList is beyound the bounds
    
    Node * removeMe;
    if(pList->nodeCount == 0)
        return NULL;
        
    //Check to see if pList has only one Node
    if(pList->nodeCount == 1)
    {
        
        removeMe = pList->currNode;
        pList->head = NULL;
        pList->nodeCount = 0;
        pList->currNode = NULL;
        pList->currItemPtr = NULL;
        pList->tail = NULL;
        pList->position = 0; //Reset the position to beyond the head 

        //Return the node to the GLOBAL STATIC FREE NODES array
        removeMe->next = NULL;
        removeMe->prev = NULL;
        pList->freeNodesArr[*(pList->freedNodesCount)] = removeMe;
        *(pList->freedNodesCount) += 1;
        return removeMe->dataPtr;
        
    }else if(pList->currNode == pList->tail)
    {
        removeMe = pList->tail;
        pList->currNode = pList->currNode->prev; //Update the current node to point behind the tail
        pList->tail = pList->currNode; //Now the tail points to the last node in the list not the node to remove
        pList->tail->next = NULL; //Update the node repersented by the tail to point to null;
        pList->nodeCount -= 1;
        pList->currItemPtr = pList->currNode->dataPtr; 

        //Since the tail is the last node in the list pList will have its iterator pointer put out of bounds
        pList->currItemPtr = NULL;
        pList->position = -1;

        //Return the node to the GLOBAL STATIC FREE NODES array
        removeMe->next = NULL;
        removeMe->prev = NULL;
        pList->freeNodesArr[*(pList->freedNodesCount)] = removeMe;
        *(pList->freedNodesCount) += 1;
        return removeMe->dataPtr;

    }else if(pList->currNode == pList->head)
    {
        removeMe = pList->currNode;
        pList->currNode = pList->currNode->next; //Advance to the next node
        pList->head = pList->currNode; //Update the head
        pList->currNode->prev = NULL; 
        pList->nodeCount -= 1; //Decrease the node count
        pList->currItemPtr = pList->currNode->dataPtr;

        //Return the node to the GLOBAL STATIC FREE NODES array
        removeMe->next = NULL;
        removeMe->prev = NULL;
        pList->freeNodesArr[*(pList->freedNodesCount)] = removeMe;
        *(pList->freedNodesCount) += 1; //Update the freeded nodes count
        return removeMe->dataPtr;
    }else
    {
        removeMe = pList->currNode;
        pList->currNode->prev->next = pList->currNode->next;
        pList->currNode->next->prev = pList->currNode->prev;
        pList->currNode = pList->currNode->next;
        pList->currItemPtr = pList->currNode->dataPtr;
        pList->nodeCount -= 1;
        
        //Return the node to the GLOBAL STATIC FREE Nodes array
        removeMe->next = NULL;
        removeMe->prev = NULL;
        pList->freeNodesArr[*(pList->freedNodesCount)] = removeMe;
        *(pList->freedNodesCount) += 1;
        return removeMe->dataPtr;
    }
}

// Adds pList2 to the end of pList1. The current pointer is set to the current pointer of pList1. 
// pList2 no longer exists after the operation; its head is available
// for future operations.
void List_concat(List* pList1, List* pList2)
{
    if(pList1 == NULL || pList2 == NULL) //Lists will not add
        exit(-1);

    if(pList1->nodeCount == 0 && pList2->nodeCount >= 1) //pList1 has no nodes but pList2 has nodes
    {
        pList1->head = pList2->head;
        pList1->tail = pList2->tail;
        pList1->nodeCount = pList2->nodeCount;
        pList1->currNode = pList1->head;
        pList1->currItemPtr = pList1->head->dataPtr;
        pList1->position = 1;
    }else if(pList2->nodeCount >= 1)
    {
        //Standard Case

        //Joining the list 
        pList1->tail->next = pList2->head;
        pList2->head->prev = pList1->tail;
        pList1->nodeCount += pList2->nodeCount; //Update the node count
        pList1->tail = pList2->tail; //Update the tail of the new list
    }

    //Free the pList2 
    pList2->head = NULL;
    pList2->tail = NULL;
    pList2->nodeCount = 0;
    pList2->currItemPtr = NULL;
    pList2->currNode = NULL;

    pList2->freeListArr[*(pList2->freeListCount)] = pList2; // Returning pList2 to freeHeadArr
    *(pList2->freeListCount) += 1; //Update the number of free lists

}

// Delete pList. pItemFreeFn is a pointer to a routine that frees an item. 
// It should be invoked (within List_free) as: (*pItemFreeFn)(itemToBeFreedFromNode);
// pList and all its nodes no longer exists after the operation; its head and nodes are 
// available for future operations.
void List_free(List* pList, FREE_FN pItemFreeFn)
{
    assert(pList != NULL);
    //Remove all nodes from the list
    while(pList->nodeCount > 0)
    {
        List_last(pList);
        if(pItemFreeFn !=  NULL)
            (*pItemFreeFn)(List_remove(pList)); //Calling remove function
        else
            List_remove(pList);  
    }
    //Free the list
    pList->nodeCount = 0;
    pList->head = NULL;
    pList->tail = NULL;
    pList->currNode = NULL;
    pList->currItemPtr = NULL;

    //Returing the list to free space
    pList->freeListArr[*(pList->freeListCount)] = pList;
    *(pList->freeListCount) += 1;
}

// Return last item and take it out of pList. Make the new last item the current one.
// Return NULL if pList is initially empty.
void* List_trim(List* pList)
{
    if(pList == NULL)
        return NULL;
    
    if(pList->nodeCount == 0)
        return NULL;
    
    List_last(pList);
    void* temp =  List_remove(pList);//Temp Item
    List_last(pList);
    return temp;
}

// Search pList, starting at the current item, until the end is reached or a match is found. 
// In this context, a match is determined by the comparator parameter. This parameter is a
// pointer to a routine that takes as its first argument an item pointer, and as its second 
// argument pComparisonArg. Comparator returns 0 if the item and comparisonArg don't match, 
// or 1 if they do. Exactly what constitutes a match is up to the implementor of comparator. 
// 
// If a match is found, the current pointer is left at the matched item and the pointer to 
// that item is returned. If no match is found, the current pointer is left beyond the end of 
// the list and a NULL pointer is returned.
void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg)
{
    if(pList == NULL)
        return NULL;
    
    if(pList->nodeCount == 0)
    {
        List_last(pList);
        List_next(pList);
        return NULL;
    }
    
    while(pList->position != -1)//List has not reached the tail
    {
        //Compare current element to the list against the compairson argument if its correct return the current pointer to the item else advance the iterator 
        if((*pComparator)(List_curr(pList),pComparisonArg) == 1)
        {
            return List_curr(pList);
        }
        List_next(pList);
    }
    return NULL; //Only executes if pList->position == -1 which repersents beyond the tail of the list
}