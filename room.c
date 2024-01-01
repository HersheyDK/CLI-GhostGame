#include "defs.h"

/*  Function: addRoom()
    Description: Adds a new RoomType to the end of the RoomListType

    in/out: RoomListType **list - Pointer to the RoomListType to add to
    in: RoomType *room - The RoomType to add to the list
    
    Returns: None
*/
void addRoom(RoomListType **list, RoomType *room) {
    if (!list || !(*list) || !room) return; // Check for NULL pointers
    RoomNodeType *newNode;
    createRoomNode(room, &newNode);
    
    // Add the node to the end of the list
    if((*list)->tail == NULL) {
        (*list)->tail = newNode;
        (*list)->head = newNode;
    } else {
        (*list)->tail->next = newNode;
        (*list)->tail = newNode;
    }
    
    (*list)->size++;
}

/*  Function: connectRooms()
    Description: Connects two rooms together including the return connection

    in/out: RoomType *connectFrom - Pointer to the RoomType to connect from
    in/out: RoomType *connectTo - Pointer to the RoomType to connect to
    
    Returns: None
*/
void connectRooms(RoomType* connectFrom, RoomType* connectTo) {
    if (!connectFrom || !connectTo) return; // Check for NULL pointers
                                            
    addRoom(&connectFrom->connectedRooms, connectTo);
    addRoom(&connectTo->connectedRooms, connectFrom);
}

/*  Function: createRoomNode()
    Description: Creates a new RoomNodeType struct and initializes its fields

    in: RoomType *room - The RoomType to add to the list
    out: RoomNodeType **node - Pointer to the newly created RoomNodeType struct
    
    Returns: None
*/
void createRoomNode(RoomType* room, RoomNodeType **node) {
    (*node) = safeMalloc(sizeof(RoomNodeType));

    (*node)->data = room;
    (*node)->next = NULL; 
}

/*  Function: createRoom()
    Description: Creates a new RoomType struct and initializes its fields

    in: char* name - The name of the room to create
    
    Returns: RoomType* - Pointer to the newly created RoomType struct
*/
struct Room* createRoom(char* name) {
    struct Room *newRoom = safeMalloc(sizeof(struct Room));
    if (!newRoom) return NULL; // Check if memory allocation was successful

    strcpy(newRoom->name, name);
    newRoom->connectedRooms = createConnectedRoomList();
    if (!newRoom->connectedRooms) { // Check if connected room list creation was successful
        free(newRoom);
        return NULL;
    }
    newRoom->evidenceList = createEvidenceList(); 
    if (!newRoom->evidenceList) { // Check if evidence list creation was successful
        cleanupRoomList(newRoom->connectedRooms);
        free(newRoom);
        return NULL;
    }
    newRoom->ghost = NULL;
    newRoom->hunterList = createHunterList();
    if (!newRoom->hunterList) { // Check if hunter list creation was successful
        cleanupEvidenceList(newRoom->evidenceList);
        cleanupRoomList(newRoom->connectedRooms);
        free(newRoom);
        return NULL;
    }
    sem_init(&newRoom->roomSem, 0, 1);
    
    return newRoom;
}

/*  Function: createConnectedRoomList()
    Description: Creates a new RoomListType struct and initializes its fields

    Returns: RoomListType* - Pointer to the newly created RoomListType struct
*/
RoomListType* createConnectedRoomList() {
    RoomListType *roomList = safeMalloc(sizeof(RoomListType));

    roomList->tail = NULL;
    roomList->head = NULL;
    roomList->size = 0;
    
    return roomList;
}

/*  Function: findRandomConnectedRoom()
    Description: Returns a random RoomType from the RoomListType

    in: RoomType *currentRoom - Pointer to the RoomType to search from

    Returns: RoomType* - Pointer to the room if found, NULL otherwise
*/
RoomType* findRandomConnectedRoom(RoomType *currentRoom) {
    if (!currentRoom) return NULL; // Check for NULL pointer
    RoomListType *roomList = currentRoom->connectedRooms;
    int roomIndex = randInt(0, roomList->size);
    RoomNodeType *room = roomList->head;

    // Loop through the list until we reach the random index
    for (int x = 0; x < roomIndex; x++) {
        room = room->next;
    }

    return room->data;
}

/*  Function: cleanupRoomListData()
    Description: Frees all dynamically allocated memory in the RoomListType struct

    in/out: RoomListType *list - Pointer to the RoomListType struct to free

    Returns: None
*/
void cleanupRoomListData(RoomListType* list) {
    if (!list) return; // Check for NULL pointer
    RoomNodeType *currentNode = list->head;

    // Loop through the list and free all the data
    while(currentNode != NULL) {
        cleanupEvidenceList(currentNode->data->evidenceList);
        cleanupRoomList(currentNode->data->connectedRooms);
        cleanupHunterList(currentNode->data->hunterList);
        free(currentNode->data);
        currentNode = currentNode->next;
    }
}

/*  Function: cleanupRoomList()
    Description: Frees all dynamically allocated memory in the RoomListType struct

    in/out: RoomListType *list - Pointer to the RoomListType struct to free

    Returns: None
    Notes: This function is taken from Tyler's a4
*/
void cleanupRoomList(RoomListType* list) {
    if (!list) return; // Check for NULL pointer
    // Start at the head of the list
    RoomNodeType *currentNode = list->head;
    RoomNodeType *nextNode = NULL;

    // Loop until the end of the list
    while(currentNode != NULL) {
        // Free the data while keeping a pointer to the next node
        nextNode = currentNode->next;
        free(currentNode);
        currentNode = nextNode;
    }

    free(list);
}

