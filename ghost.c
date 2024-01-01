#include "defs.h"

void *ghostLogic(void*);

/*  Function: initGhost()
    Description: Creates a new GhostType struct and initializes its fields

    in: HouseType *house - Pointer to the HouseType struct to add the ghost to
    in/out: GhostType **ghost - Pointer to the newly created GhostType struct
    
    Returns: None
*/
void initGhost(HouseType *house, GhostType **ghost) {
    // Allocate memory for the new ghost
    (*ghost) = safeMalloc(sizeof(GhostType));
    GhostClass ghostClass = randomGhost();
    (*ghost)->evList = createEvidenceList();
    if (!(*ghost)->evList) { // Check if evidence list creation was successful
        free(*ghost);
        return;
    }
    (*ghost)->class = ghostClass;
    
    // Add the appropriate evidence to the ghost's evidence list
    switch (ghostClass) {
        case POLTERGEIST:
            addEvidence((*ghost)->evList, EMF);
            addEvidence((*ghost)->evList, TEMPERATURE);
            addEvidence((*ghost)->evList, FINGERPRINTS);
            break;
        case BANSHEE:
            addEvidence((*ghost)->evList, EMF);
            addEvidence((*ghost)->evList, TEMPERATURE);
            addEvidence((*ghost)->evList, SOUND);
            break;
        case BULLIES:
            addEvidence((*ghost)->evList, EMF);
            addEvidence((*ghost)->evList, FINGERPRINTS);
            addEvidence((*ghost)->evList, SOUND);
            break;
        case PHANTOM:
            addEvidence((*ghost)->evList, TEMPERATURE);
            addEvidence((*ghost)->evList, FINGERPRINTS);
            addEvidence((*ghost)->evList, SOUND);
            break;
        default:
            break;
    }

    // Initialize the rest of the ghost's fields
    RoomType *spawnRoom = randomRoomInHouse(house);
    (*ghost)->currentRoom = spawnRoom;
    spawnRoom->ghost = (*ghost);
    (*ghost)->allHunters = house->hunterList;

    (*ghost)->boredomTimer = 0;
    l_ghostInit(ghostClass, (*ghost)->currentRoom->name);
}

/*  Function: startGhostThread()
    Description: Creates a new thread for the ghost and starts it

    in: GhostType *ghost - Pointer to the GhostType struct to start the thread for
    
    Returns: pthread_t* - Pointer to the newly created thread
*/
pthread_t* startGhostThread(GhostType *ghost) {
    // Create a new thread for the ghost
    pthread_t* thread = malloc(sizeof(pthread_t));
    pthread_create(thread, NULL, ghostLogic, ghost);
    return thread;
}

/*  Function: ghostLogic()
    Description: The main logic for the ghost thread

    in/out: void *ghostPtr - Pointer to the GhostType struct to run the logic for
    
    Returns: void* - NULL
*/
void *ghostLogic(void *ghostPtr) {
    GhostType *ghost = (GhostType*) ghostPtr;
    if (!ghost) return NULL; // Check for NULL pointer

    // Run the ghost logic until the ghost is bored
    while(ghost->boredomTimer < BOREDOM_MAX) {
        usleep(GHOST_WAIT);
        
        // Check if there is a hunter in the room
        GhostActionType ghostAction;
        struct Room *currRoom = ghost->currentRoom;
        sem_wait(&currRoom->roomSem);
        int hunterInRoom = checkIfHunterInRoom(currRoom);
        sem_post(&currRoom->roomSem);
        
        // If there is a hunter in the room, reset the boredom timer and do not allow moving from a room
        if (hunterInRoom) {
            ghost->boredomTimer = 0;
            ghostAction = randInt(0, GHOST_ACTION_COUNT - 1);
        } else {
            ghostAction = randInt(0, GHOST_ACTION_COUNT);
            ghost->boredomTimer++;
        }

        // Perform the action
        switch(ghostAction) {
            case GHOST_MOVE_ROOM: 
                ghostMoveRoom(ghost);
                break;
            case DROP_EVIDENCE:
                dropEvidence(ghost);
                break;
            default:
                break;
        }
    }
    
    // If the ghost is bored, exit
    if(ghost->boredomTimer >= BOREDOM_MAX) {
        l_ghostExit(LOG_BORED);
    }

    ghostExit(ghost);

    return NULL;
}

/*  Function: ghostExit()
    Description: Checks if there is a hunter in the room

    in: GhostType *ghost - Pointer to the ghost that is leaving
    
    Returns: None
*/
void ghostExit(GhostType *ghost) {
    sem_wait(&ghost->currentRoom->roomSem);
    ghost->currentRoom->ghost = NULL;
    sem_post(&ghost->currentRoom->roomSem);
}

/*  Function: checkIfHunterInRoom()
    Description: Checks if there is a hunter in the room

    in: RoomType *room - Pointer to the RoomType struct to check
    
    Returns: int - C_TRUE if there is a hunter in the room, C_FALSE otherwise
*/
int checkIfHunterInRoom(RoomType *room) {
    if (!room) return C_FALSE; // Check for NULL pointer
    HunterListType *hunterList = room->hunterList; 
    int size = hunterList->size;
    return size > 0;
}

/*  Function: moveRoom()
    Description: Moves the ghost to a random connected room

    in/out: GhostType *ghost - Pointer to the GhostType struct to move
    
    Returns: None
*/
void ghostMoveRoom(GhostType *ghost) {
    if (!ghost) return; // Check for NULL pointer
    RoomType *currRoom = ghost->currentRoom;
    // Find a random connected room
    RoomType *newRoom = findRandomConnectedRoom(currRoom);

    lockSemaphors(&currRoom->roomSem, &newRoom->roomSem);
    
    // Update the ghost pointers for the rooms 
    ghost->currentRoom = newRoom;
    newRoom->ghost = ghost;
    currRoom->ghost = NULL;

    unlockSemaphors(&currRoom->roomSem, &newRoom->roomSem);

    l_ghostMove(newRoom->name);
}

/*  Function: dropEvidence()
    Description: Drops a random piece of evidence in the current room

    in: GhostType *ghost - Pointer to the GhostType struct to drop the evidence
    
    Returns: None
*/
void dropEvidence(GhostType *ghost) {
    if (!ghost) return; // Check for NULL pointer
    
    // Finds a random piece of evidence in the ghosts possible evidence
    sem_wait(&ghost->currentRoom->roomSem);
    EvidenceType randEv = randomEvidence(ghost->evList);
    // Add the evidence to the current room
    addEvidence(ghost->currentRoom->evidenceList, randEv);
    sem_post(&ghost->currentRoom->roomSem);
    
    l_ghostEvidence(randEv, ghost->currentRoom->name);
}

/*  Function: cleanupGhost()
    Description: Frees the memory allocated for the GhostType struct

    in/out: GhostType *ghost - Pointer to the GhostType struct to free
    
    Returns: None
*/
void cleanupGhost(GhostType *ghost) {
    if (!ghost) return; // Check for NULL pointer
    cleanupEvidenceList(ghost->evList);
    free(ghost);
}
