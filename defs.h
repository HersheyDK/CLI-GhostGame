#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_STR         64
#define MAX_RUNS        50
#define BOREDOM_MAX     100
#define C_TRUE          1
#define C_FALSE         0
#define HUNTER_WAIT     5000
#define GHOST_WAIT      600
#define NUM_HUNTERS     4
#define EV_PER_GHOST    3
#define FEAR_MAX        10
#define LOGGING         C_TRUE

typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;
typedef enum GhostAction GhostActionType;
typedef enum HunterAction HunterActionType;

typedef struct Room RoomType;
typedef struct RoomNode RoomNodeType;
typedef struct RoomList RoomListType;
typedef struct EvidenceNode EvidenceNodeType;
typedef struct EvidenceList EvidenceListType;
typedef struct HunterList HunterListType;

typedef struct House HouseType;

typedef struct Hunter HunterType;
typedef struct Ghost GhostType;

enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };
enum GhostAction { DROP_EVIDENCE, NOTHING, GHOST_MOVE_ROOM, GHOST_ACTION_COUNT };
enum HunterAction { HUNTER_MOVE_ROOM, COLLECT_EV, REVIEW, HUNTER_ACTION_COUNT };

struct Hunter {
    int id;
    char name[MAX_STR];
    RoomType *room;
    EvidenceType evidence;
    int fear;
    int boredom;
    EvidenceListType *sharedEv;
    EvidenceListType *ghostEv;
    HunterListType *allHunters;
    int sufficientEv;
};

struct HunterList {
    HunterType *hunters[NUM_HUNTERS];
    int size;
};

struct EvidenceNode {
    struct EvidenceNode *next;
    EvidenceType data;
};

struct EvidenceList {
    int size;
    EvidenceNodeType *head;
    EvidenceNodeType *tail;
    sem_t evSem;
};

struct RoomNode {
    struct RoomNode *next;
    RoomType *data;
};

struct RoomList {
    RoomNodeType *head;
    RoomNodeType *tail;
    int size;
};

struct Room {
    char name[MAX_STR];
    EvidenceListType *evidenceList;
    RoomListType *connectedRooms;
    HunterListType *hunterList;
    GhostType *ghost;
    sem_t roomSem;
};

struct House {
    RoomListType *rooms;
    EvidenceListType *evidence;
    HunterListType *hunterList;
};

struct Ghost {
    GhostClass class;
    int boredomTimer;
    RoomType *currentRoom;
    EvidenceListType *evList;
    HunterListType *allHunters;
};

// Hunter Functions
HunterListType* createHunterList();
void initHunter(HunterType**, GhostType*, HouseType*, char[], int*, EvidenceType);
pthread_t* startHunterThread(HunterType*);
void *hunterLogic(void*);
void moveRoomHunt(HunterType*);
void addHunter(HunterListType*, HunterType*);
void collectEvidence(HunterType*);
int delHunter(HunterListType*, int);
void copyHunter(HunterType*, HunterType*);
int review(HunterType*);
void hunterExit(HunterType*);
void cleanupHunterList(HunterListType*);

// Ghost Functions
void initGhost(HouseType*, GhostType**);
void ghostMoveRoom(GhostType*);
void dropEvidence(GhostType*);
int checkIfHunterInRoom(RoomType*);
pthread_t* startGhostThread(GhostType*);
void ghostExit(GhostType*);
void cleanupGhost(GhostType*);

// Room Functions
struct Room* createRoom(char*);
void connectRooms(RoomType*, RoomType*);
void createRoomNode(RoomType*, RoomNodeType**);
RoomListType* createConnectedRoomList();
RoomType* findRandomConnectedRoom(RoomType*);
void cleanupRoomListData(RoomListType*);
void cleanupRoomList(RoomListType*);

// Evidence Functions 
EvidenceListType* createEvidenceList();
void createEvidenceNode (EvidenceType, EvidenceNodeType**);
void addEvidence (EvidenceListType*, EvidenceType);
void printEvidence(FILE*, EvidenceListType*);
EvidenceType removeEvidence(EvidenceListType*, EvidenceType);
EvidenceType randomEvidence(EvidenceListType*);
void cleanupEvidenceList(EvidenceListType*);

// House Functions 
void initHouse(HouseType**);
void addRoom(RoomListType**, RoomType*);
void populateRooms(HouseType*);
RoomType* randomRoomInHouse(HouseType*);
void cleanupHouse(HouseType*);

// Helper Utilies
int randInt(int,int);        // Pseudo-random number generator function
float randFloat(float, float);  // Pseudo-random float generator function
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
void ghostToString(GhostClass, char*); // Convert a ghost type to a string, stored in output paremeter
void evidenceToString(EvidenceType, char*); // Convert an evidence type to a string, stored in output parameter
void* safeMalloc(size_t);
void lockSemaphors(sem_t*, sem_t*);
void unlockSemaphors(sem_t*, sem_t*);

// Logging Utilities
void l_hunterInit(char*, enum EvidenceType);
void l_hunterMove(char*, char*);
void l_hunterReview(char*, enum LoggerDetails);
void l_hunterCollect(char*, enum EvidenceType, char*);
void l_hunterExit(char*, enum LoggerDetails);
void l_ghostInit(enum GhostClass, char*);
void l_ghostMove(char*);
void l_ghostEvidence(enum EvidenceType, char*);
void l_ghostExit(enum LoggerDetails);
void l_gameComplete(GhostType*, HunterListType*, EvidenceListType*);
