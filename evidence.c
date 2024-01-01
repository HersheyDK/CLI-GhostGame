#include "defs.h"

/*  Function: createEvidenceList()
    Description: Creates a new EvidenceListType struct and initializes its fields

    in: None

    Returns: EvidenceListType* - Pointer to the newly created EvidenceListType struct
*/
EvidenceListType* createEvidenceList() {
    EvidenceListType *newEvidenceList = safeMalloc(sizeof(EvidenceListType));
    newEvidenceList->size = 0;
    newEvidenceList->head = NULL;
    newEvidenceList->tail = NULL;
    sem_init(&newEvidenceList->evSem, 0, 1);

    return newEvidenceList;
}

/*  Function: addEvidence()
    Description: Adds a new EvidenceType to the end of the EvidenceListType

    in/out: EvidenceListType *evidenceList - Pointer to the EvidenceListType to add to
    in: EvidenceType evidenceType - The EvidenceType to add to the list
    
    Returns: None
*/
void addEvidence(EvidenceListType *evidenceList, EvidenceType evidenceType) {
    if (!evidenceList) return; // Check for NULL pointer

    EvidenceNodeType *newNode;  
    createEvidenceNode(evidenceType, &newNode);
    
    // Add the node to the end of the list
    if(evidenceList->tail == NULL) {
        evidenceList->head = newNode;
        evidenceList->tail = newNode;
    } else {
        evidenceList->tail->next = newNode;
        evidenceList->tail = newNode;
    }
    
    // Increment the size of the list
    evidenceList->size++; 
}

/*  Function: printEvidence()
    Description: Prints out all of the evidence in the list 

    in: EvidenceListType *evidenceList - Pointer to the EvidenceListType to read from
    
    Returns: None
*/
void printEvidence(FILE* logFile, EvidenceListType *evList) {
    if(!evList || evList->size == 0) return;
    EvidenceNodeType *currNode = evList->head;

    for(int i = 0; i < evList->size; i++) {
        // Print out all of the evidence nodes in the list
        char evStr[MAX_STR];
        evidenceToString(currNode->data, evStr);
        printf(" - %s\n", evStr);
        fprintf(logFile, " - %s\n", evStr);
        currNode = currNode->next;
    }
}

/*  Function: createEvidenceNode()
    Description: Creates a new EvidenceNodeType struct and initializes its fields

    in: EvidenceType evidenceType - The EvidenceType to add to the list
    out: EvidenceNodeType **node - Pointer to the newly created EvidenceNodeType struct
    
    Returns: None
*/
void createEvidenceNode(EvidenceType evidenceType, EvidenceNodeType **node) {
    *node = safeMalloc(sizeof(EvidenceNodeType));

    (*node)->data = evidenceType;
    (*node)->next = NULL; 
}

/*  Function: randomEvidence()
    Description: Returns a random EvidenceType from the EvidenceListType

    in: EvidenceListType *evidenceList - Pointer to the EvidenceListType to get the random EvidenceType from
    
    Returns: EvidenceType - The random EvidenceType
*/
EvidenceType randomEvidence(EvidenceListType *evidenceList) {
    EvidenceNodeType *currEv = evidenceList->head;
    int randIndex = randInt(0, evidenceList->size);

    // Loop through the list until the random index is reached
    for(int i = 0; i < randIndex; i++) {
        currEv = currEv->next;
    }

    return currEv->data;
}

/*  Function: removeEvidence()
    Description: Removes the given EvidenceType from the EvidenceListType

    in/out: EvidenceListType *evidenceList - Pointer to the EvidenceListType to remove from
    in: EvidenceType evidenceType - The EvidenceType to remove from the list
    
    Returns: EvidenceType - The removed EvidenceType
*/
EvidenceType removeEvidence(EvidenceListType *evidenceList, EvidenceType evidenceType) {
    if(evidenceList->size == 0) return EV_UNKNOWN;
    
    EvidenceNodeType *prevNode = NULL;
    EvidenceNodeType *currEv = evidenceList->head;
    
    // Loop through the list until the given EvidenceType is found
    while (currEv && currEv->data != evidenceType) {
        prevNode = currEv;
        currEv = currEv->next;
    }

    if (!currEv) return EV_UNKNOWN; // If the element is not found

    // Remove the current element from the list
    if (!prevNode) {
        evidenceList->head = currEv->next;
    } else {
        prevNode->next = currEv->next;
    }

    if (currEv == evidenceList->tail) {
        evidenceList->tail = prevNode;
    }

    evidenceList->size--;

    EvidenceType data = currEv->data;

    free(currEv);
    
    return data;
}

/*  Function: cleanupEvidenceList()
    Description: Frees all the memory allocated to the EvidenceListType

    in/out: EvidenceListType *evidenceList - Pointer to the EvidenceListType to free
    
    Returns: None
*/
void cleanupEvidenceList(EvidenceListType *evidenceList) {
    if (!evidenceList) return; // Check for NULL pointer

    EvidenceNodeType *currentNode = evidenceList->head;
    EvidenceNodeType *nextNode = NULL;

    // Loop until the end of the list
    while(currentNode != NULL) {

        nextNode = currentNode->next;
        free(currentNode);
        currentNode = nextNode;
    }  

    free(evidenceList);
}
