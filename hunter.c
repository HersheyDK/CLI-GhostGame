#include "defs.h"

/*  Function: initHunter()
    Description: Creates a new HunterType struct and initializes its fields

    in/out: HunterType **hunter - Pointer to the newly created HunterType struct
    in: GhostType *ghost - Pointer to the GhostType struct to add the hunter to
    in: HouseType *house - Pointer to the HouseType struct to add the hunter to
    in: char name[] - The name of the hunter
    in/out: id - The id of the current hunter that will be decremented 
    in: ev - The type of evidence the hunter is able to pick up 
    
    Returns: None
*/
void initHunter(HunterType **hunter, GhostType *ghost, HouseType *house, char name[], int *id, EvidenceType ev) {
    (*hunter) = safeMalloc(sizeof(HunterType));
    strcpy((*hunter)->name, name);
    (*hunter)->evidence = ev;
    (*hunter)->fear = 0;
    (*hunter)->boredom = 0;
    // The first room in the house is the van
    (*hunter)->room = house->rooms->head->data;
    (*hunter)->id = *id;
    (*id)--;
    (*hunter)->sharedEv = house->evidence;
    (*hunter)->ghostEv = ghost->evList;
    (*hunter)->allHunters = house->hunterList;
    // This will make logging game completion simpler 
    (*hunter)->sufficientEv = C_FALSE;
    l_hunterInit(name, ev);
}

/*  Function: createHunterList()
    Description: Creates a new HunterListType struct and initializes its fields

    in: None

    Returns: HunterListType* - Pointer to the newly created HunterListType struct
*/
HunterListType* createHunterList() {
    HunterListType *hunterList = safeMalloc(sizeof(HunterListType));
    hunterList->size = 0;
    return hunterList;
}

/*  Function: startHunterThread()
    Description: Creates a new thread for the hunter and starts it

    in: HunterType *hunter - Pointer to the HunterType struct to start the thread for
    
    Returns: pthread_t* - Pointer to the newly created thread
*/
pthread_t* startHunterThread(HunterType *hunter) {
    pthread_t* thread = safeMalloc(sizeof(pthread_t));
    pthread_create(thread, NULL, hunterLogic, hunter);
    return thread;
}

/*  Function: hunterLogic()
    Description: The main logic for the hunter thread

    in/out: void *hunterPtr - Pointer to the HunterType struct to run the logic for
    
    Returns: void* - NULL
*/
void *hunterLogic(void *hunterPtr) {
    HunterType *hunter = (HunterType*) hunterPtr;
    
    // Only loop as long as they are not too bored or scared 
    while(hunter->boredom < BOREDOM_MAX && hunter->fear < FEAR_MAX) {
        usleep(HUNTER_WAIT);
        
        // Check if the ghost is in the room
        sem_wait(&hunter->room->roomSem);
        int ghostInRoom = hunter->room->ghost != NULL;
        sem_post(&hunter->room->roomSem);

        if(ghostInRoom) {
            hunter->fear++;
            hunter->boredom = 0;
        } else {
            hunter->boredom++;
        }

        // Perform a random action
        HunterActionType hunterAction = randInt(0, HUNTER_ACTION_COUNT);
        int sufficient = C_FALSE;
        switch(hunterAction) {
            case HUNTER_MOVE_ROOM: 
                moveRoomHunt(hunter);
                break;
            case COLLECT_EV:
                collectEvidence(hunter);
                break;
            case REVIEW:
                sufficient = review(hunter);
                break;
            default:
                break;
        }

        if(sufficient) {
            hunter->sufficientEv = C_TRUE;
            l_hunterExit(hunter->name, LOG_EVIDENCE);
            break;
        }
    }

    
    // Check if the hunter is bored or scared
    if(hunter->boredom >= BOREDOM_MAX) {
        l_hunterExit(hunter->name, LOG_BORED);
    }

    if(hunter->fear >= FEAR_MAX) {
        l_hunterExit(hunter->name, LOG_FEAR);
    }

    // Remove the hunter from the room's hunter list
    hunterExit(hunter);

    return NULL;
}

/*  Function: hunterExit()
    Description: Removes the hunter from the room's hunter list

    in: HunterType *hunter - Pointer to the HunterType struct to remove
    
    Returns: None
*/
void hunterExit(HunterType *hunter) {
    if (!hunter || !hunter->room) return; // Check for NULL pointers
    sem_wait(&hunter->room->roomSem);
    delHunter(hunter->room->hunterList, hunter->id);
    sem_post(&hunter->room->roomSem);
}

/*  Function: moveRoomHunt()
    Description: Moves the hunter to a random connected room

    in/out: HunterType *hunter - Pointer to the HunterType struct to move
    
    Returns: None
*/
void moveRoomHunt(HunterType *hunter) {
    if (!hunter || !hunter->room) return; // Check for NULL pointers

    RoomType *currRoom = hunter->room;
    RoomType *newRoom = findRandomConnectedRoom(currRoom);
    
    if (!newRoom) return; // Check if new room selection was successful

    lockSemaphors(&newRoom->roomSem, &currRoom->roomSem);
    hunter->room = newRoom;
    addHunter(newRoom->hunterList, hunter);
    delHunter(currRoom->hunterList, hunter->id);
    l_hunterMove(hunter->name, newRoom->name);
    unlockSemaphors(&newRoom->roomSem, &currRoom->roomSem);
}

/*  Function: collectEvidence()
    Description: Collects a random piece of evidence from the room

    in/out: HunterType *hunter - Pointer to the HunterType struct to collect the evidence
    
    Returns: None
*/
void collectEvidence(HunterType *hunter) {
    if (!hunter || !hunter->room || !hunter->room->evidenceList || !hunter->sharedEv) return; // Check for NULL pointers
    lockSemaphors(&hunter->sharedEv->evSem, &hunter->room->roomSem);

    // This will return the evidence or unknown if there isn't that type of evidence in the list 
    EvidenceType ev = removeEvidence(hunter->room->evidenceList, hunter->evidence);
    sem_post(&hunter->room->roomSem);

    // Check if the evidence is unknown
    if(ev == EV_UNKNOWN) {
        sem_post(&hunter->sharedEv->evSem);
        return;
    }
    
    // Add the evidence to the hunter's shared list
    addEvidence(hunter->sharedEv, ev); 
    sem_post(&hunter->sharedEv->evSem);
    l_hunterCollect(hunter->name, hunter->evidence, hunter->room->name);
}

/*  Function: review()
    Description: Checks if the hunter has collected sufficient evidence

    in: HunterType *hunter - Pointer to the HunterType struct to check
    
    Returns: int - 1 if the hunter has collected sufficient evidence, 0 otherwise
*/
int review(HunterType *hunter) {
    if (!hunter || !hunter->ghostEv || !hunter->sharedEv) return C_FALSE; // Check for NULL pointers
    sem_wait(&hunter->sharedEv->evSem);
    EvidenceNodeType *currNode = hunter->ghostEv->head;
    int foundCounter = 0;

    // Loop through the hunter's evidence list
    for(int i = 0; i < hunter->ghostEv->size; i++) {
        EvidenceNodeType *currFoundEvNode = hunter->sharedEv->head;
        for(int j = 0; j < hunter->sharedEv->size; j++) {
            if(currFoundEvNode->data == currNode->data) {
                foundCounter++;
                break;
            }
            currFoundEvNode = currFoundEvNode->next;
        }
        currNode = currNode->next;
    }
    sem_post(&hunter->sharedEv->evSem);

    // Check if the hunter has found all the evidence
    if(foundCounter == 3) {
        l_hunterReview(hunter->name, LOG_SUFFICIENT);
        return C_TRUE;
    }
    
    l_hunterReview(hunter->name, LOG_INSUFFICIENT);
    return C_FALSE;
}

/*  Function: addHunter()
    Description: Adds the hunter to the hunter list

    in/out: HunterListType *dest - Pointer to the HunterListType struct to add to
    in: HunterType *src - Pointer to the HunterType struct to add
    
    Returns: None
*/
void addHunter(HunterListType *dest, HunterType *src) {
    if (!dest || !src) return; // Check for NULL pointers
    dest->hunters[dest->size] = src;
    dest->size++;
}

/*  Function: delHunter()
    Description: Removes the hunter from the hunter list

    in/out: HunterListType *src - Pointer to the HunterListType struct to remove from
    in: int hunterId - The id of the hunter to remove from the list
    
    Returns: int - C_TRUE if the hunter was found and removed, C_FALSE otherwise
    Notes: This function is taken from Tyler's a3
*/
int delHunter(HunterListType *src, int hunterId) {
     if (!src) return C_FALSE; // Check for NULL pointer
    // Loop through the array's elements
    for(int i = 0; i < src->size; i++) {
        // Check if we have found the element
        if(src->hunters[i]->id == hunterId) {
            // Shift all the elements in the array 
            // which will overwrite the starting index
            for(int j = i; j < src->size - 1; j++) {
                src->hunters[j] = src->hunters[j + 1];  
            }
        
            // Update the size
            src->size = src->size - 1;
            
            // The element was found and removed
            return C_TRUE;
        } 
    }
    
    // The element could not be found
    return C_FALSE; 
}

/*  Function: cleanupHunterList()
    Description: Frees all dynamically allocated memory in the HunterListType struct

    in/out: HunterListType *hunterList - Pointer to the HunterListType struct to free
    
    Returns: None
*/
void cleanupHunterList(HunterListType *hunterList) {
    if (!hunterList) return; // Check for NULL pointer
    // Free all the hunters
    for(int i = 0; i < hunterList->size; i++) {
        free(hunterList->hunters[i]);
    }

    free(hunterList);
}
