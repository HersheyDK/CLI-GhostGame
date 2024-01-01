#include "defs.h"

/*  Function: initHouse()
    Description: Creates a new HouseType struct and initializes its fields

    in/out: HouseType **house - Pointer to the newly created HouseType struct
    
    Returns: None
*/
void initHouse(HouseType **house) {
    (*house) = safeMalloc(sizeof(HouseType));

    (*house)->rooms = createConnectedRoomList();
    (*house)->evidence = createEvidenceList();
    // This get initialized later on in the main method
    (*house)->hunterList = NULL;
}

/*  Function: randomRoomInHouse()
    Description: Returns a random room in the house except the van

    in: HouseType *house - Pointer to the HouseType struct to search
    
    Returns: RoomType* - Pointer to the room if found, NULL otherwise
*/
RoomType* randomRoomInHouse(HouseType *house) {
    RoomNodeType *currRoom = house->rooms->head;
    // Find a random room in the house but not the van as this is used to spawn the ghost
    int randIndex = randInt(1, house->rooms->size);

    for(int i = 0; i < randIndex; i++) {
        currRoom = currRoom->next;
    }

    return currRoom->data; 
}

/*  Function: cleanupHouse()
    Description: Frees all dynamically allocated memory in the HouseType struct

    in/out: HouseType *house - Pointer to the HouseType struct to free
    
    Returns: None
*/
void cleanupHouse(HouseType *house) {
    cleanupHunterList(house->hunterList);
    cleanupRoomListData(house->rooms);
    cleanupRoomList(house->rooms);
    cleanupEvidenceList(house->evidence);
    free(house);
}

/*  Function: populateRooms()
    Description: Adds the rooms and their connections to the house

    in/out: HouseType *house - Pointer to the HouseType struct to free
    
    Returns: None
*/
void populateRooms(HouseType* house) {
    // First, create each room
    
    // CreateRoom assumes that we dynamically allocate a room, initializes the values, and returns a RoomType*
    // Create functions are pretty typical, but it means errors are harder to return aside from NULL
    struct Room* van                = createRoom("Van");
    struct Room* hallway            = createRoom("Hallway");
    struct Room* master_bedroom     = createRoom("Master Bedroom");
    struct Room* boys_bedroom       = createRoom("Boy's Bedroom");
    struct Room* bathroom           = createRoom("Bathroom");
    struct Room* basement           = createRoom("Basement");
    struct Room* basement_hallway   = createRoom("Basement Hallway");
    struct Room* right_storage_room = createRoom("Right Storage Room");
    struct Room* left_storage_room  = createRoom("Left Storage Room");
    struct Room* kitchen            = createRoom("Kitchen");
    struct Room* living_room        = createRoom("Living Room");
    struct Room* garage             = createRoom("Garage");
    struct Room* utility_room       = createRoom("Utility Room");

    // This adds each room to each other's room lists
    // All rooms are two-way connections
    connectRooms(van, hallway);
    connectRooms(hallway, master_bedroom);
    connectRooms(hallway, boys_bedroom);
    connectRooms(hallway, bathroom);
    connectRooms(hallway, kitchen);
    connectRooms(hallway, basement);
    connectRooms(basement, basement_hallway);
    connectRooms(basement_hallway, right_storage_room);
    connectRooms(basement_hallway, left_storage_room);
    connectRooms(kitchen, living_room);
    connectRooms(kitchen, garage);
    connectRooms(garage, utility_room);

    // Add each room to the house's room list
    addRoom(&house->rooms, van);
    addRoom(&house->rooms, hallway);
    addRoom(&house->rooms, master_bedroom);
    addRoom(&house->rooms, boys_bedroom);
    addRoom(&house->rooms, bathroom);
    addRoom(&house->rooms, basement);
    addRoom(&house->rooms, basement_hallway);
    addRoom(&house->rooms, right_storage_room);
    addRoom(&house->rooms, left_storage_room);
    addRoom(&house->rooms, kitchen);
    addRoom(&house->rooms, living_room);
    addRoom(&house->rooms, garage);
    addRoom(&house->rooms, utility_room);
}
