#include "defs.h"

/* 
    Logs the hunter being created.
    in: hunter - the hunter name to log
    in: equipment - the hunter's equipment
*/
void l_hunterInit(char* hunter, enum EvidenceType equipment) {
    if (!LOGGING) return;
    char evStr[MAX_STR];
    evidenceToString(equipment, evStr);
    FILE *logFile = fopen("./output.txt", "a");
    const char logString[] = "%-17s [%s] is a [%s] hunter\n";
    printf(logString, "[HUNTER INIT]", hunter, evStr);
    fprintf(logFile, logString, "[HUNTER INIT]", hunter, evStr);
    fclose(logFile);
}

/*
    Logs the hunter moving into a new room.
    in: hunter - the hunter name to log
    in: room - the room name to log
*/
void l_hunterMove(char* hunter, char* room) {
    if (!LOGGING) return;
    FILE *logFile = fopen("./output.txt", "a");
    const char logString[] = "%-17s [%s] has moved into [%s]\n";
    printf(logString, "[HUNTER MOVE]", hunter, room);
    fprintf(logFile, logString, "[HUNTER MOVE]", hunter, room);
    fclose(logFile);
}

/*
    Logs the hunter exiting the house.
    in: hunter - the hunter name to log
    in: reason - the reason for exiting, either LOG_FEAR, LOG_BORED, or LOG_EVIDENCE
*/
void l_hunterExit(char* hunter, enum LoggerDetails reason) {
    if (!LOGGING) return;
    FILE *logFile = fopen("./output.txt", "a");
    const char logString[] = "%-17s [%s] exited because ";
    printf(logString, "[HUNTER EXIT]", hunter);
    fprintf(logFile, logString, "[HUNTER EXIT]", hunter);

    switch (reason) {
        case LOG_FEAR:
            printf("[FEAR]\n");
            fprintf(logFile, "[FEAR]\n");
            break;
        case LOG_BORED:
            printf("[BORED]\n");
            fprintf(logFile, "[BORED]\n");
            break;
        case LOG_EVIDENCE:
            printf("[EVIDENCE]\n");
            fprintf(logFile, "[EVIDENCE]\n");
            break;
        default:
            printf("[UNKNOWN]\n");
            fprintf(logFile, "[UNKNOWN]\n");
    }
    fclose(logFile);
}

/*
    Logs the hunter reviewing evidence.
    in: hunter - the hunter name to log
    in: result - the result of the review, either LOG_SUFFICIENT or LOG_INSUFFICIENT
*/
void l_hunterReview(char* hunter, enum LoggerDetails result) {
    if (!LOGGING) return;
    FILE *logFile = fopen("./output.txt", "a");
    const char logString[] = "%-17s [%s] reviewed evidence and found ";
    printf(logString, "[HUNTER REVIEW]", hunter);
    fprintf(logFile, logString, "[HUNTER REVIEW]", hunter);
    switch (result) {
        case LOG_SUFFICIENT:
            printf("[SUFFICIENT]\n");
            fprintf(logFile, "[SUFFICIENT]\n");
            break;
        case LOG_INSUFFICIENT:
            printf("[INSUFFICIENT]\n");
            fprintf(logFile, "[INSUFFICIENT]\n");
            break;
        default:
            printf("[UNKNOWN]\n");
            fprintf(logFile, "[UNKNOWN]\n");
    }
    fclose(logFile);
}

/*
    Logs the hunter collecting evidence.
    in: hunter - the hunter name to log
    in: evidence - the evidence type to log
    in: room - the room name to log
*/
void l_hunterCollect(char* hunter, enum EvidenceType evidence, char* room) {
    if (!LOGGING) return;
    char evStr[MAX_STR];
    evidenceToString(evidence, evStr);
    FILE *logFile = fopen("./output.txt", "a");
    const char logString[] = "%-17s [%s] found [%s] in [%s] and [COLLECTED]\n";
    printf(logString, "[HUNTER EVIDENCE]", hunter, evStr, room);
    fprintf(logFile, logString, "[HUNTER EVIDENCE]", hunter, evStr, room);
    fclose(logFile);
}

/*
    Logs the ghost moving into a new room.
    in: room - the room name to log
*/
void l_ghostMove(char* room) {
    if (!LOGGING) return;
    FILE *logFile = fopen("./output.txt", "a");
    const char logString[] = "%-17s Ghost has moved into [%s]\n";
    printf(logString, "[GHOST MOVE]", room);
    fprintf(logFile, logString, "[GHOST MOVE]", room);
    fclose(logFile); 
}

/*
    Logs the ghost exiting the house.
    in: reason - the reason for exiting, either LOG_FEAR, LOG_BORED, or LOG_EVIDENCE
*/
void l_ghostExit(enum LoggerDetails reason) {
    if (!LOGGING) return;
    FILE *logFile = fopen("./output.txt", "a");
    const char logString[] = "%-17s Exited because ";
    printf(logString, "[GHOST EXIT]");
    fprintf(logFile, logString, "[GHOST EXIT]");
    switch (reason) {
        case LOG_FEAR:
            printf("[FEAR]\n");
            fprintf(logFile, "[FEAR]\n");
            break;
        case LOG_BORED:
            printf("[BORED]\n");
            fprintf(logFile, "[BORED]\n");
            break;
        case LOG_EVIDENCE:
            printf("[EVIDENCE]\n");
            fprintf(logFile, "[EVIDENCE]\n");
            break;
        default:
            printf("[UNKNOWN]\n");
            fprintf(logFile, "[UNKNOWN]\n");
    }
    fclose(logFile);
}

/*
    Logs the ghost leaving evidence in a room.
    in: evidence - the evidence type to log
    in: room - the room name to log
*/
void l_ghostEvidence(enum EvidenceType evidence, char* room) {
    if (!LOGGING) return;
    char evStr[MAX_STR];
    evidenceToString(evidence, evStr);
    FILE *logFile = fopen("./output.txt", "a");
    const char logString[] = "%-17s Ghost left [%s] in [%s]\n";
    printf(logString, "[GHOST EVIDENCE]", evStr, room);
    fprintf(logFile, logString, "[GHOST EVIDENCE]", evStr, room );
    fclose(logFile);    
}

/*
    Logs the ghost being created.
    in: ghost - the ghost type to log
    in: room - the room name that the ghost is starting in
*/
void l_ghostInit(enum GhostClass ghost, char* room) {
    if (!LOGGING) return;
    char ghostStr[MAX_STR];
    ghostToString(ghost, ghostStr);
    FILE *logFile = fopen("./output.txt", "w");
    const char logString[] = "%-17s Ghost is a [%s] in room [%s]\n";
    printf(logString, "[GHOST INIT]", ghostStr, room);
    fprintf(logFile, logString, "[GHOST INIT]", ghostStr, room);
    fclose(logFile);
}

/*
    Logs the final status of the game and who won
    in: ghost - the ghost that was in the game
    in: hunters - the hunters that played the game 
    in: hunterEvList - the evidence collected by the hunters during the game
*/
void l_gameComplete(GhostType *ghost, HunterListType *hunters, EvidenceListType *hunterEvList) {
    if(!LOGGING) return;
    FILE *logFile = fopen("./output.txt", "a");
    const char lineSeperate[] = "--------------------------------\n";
    // Header
    printf(lineSeperate);
    fprintf(logFile, lineSeperate);
    printf("%-40s\n", "All done! Let's see the results:");
    fprintf(logFile, "%-40s\n", "All done! Let's see the results:");
    printf(lineSeperate);
    fprintf(logFile, lineSeperate);
    
    HunterListType *boredHunters = createHunterList();
    HunterListType *scaredHunters = createHunterList();
    
    // Find out how many hunters were to scared or bored to continue hunting
    for(int i = 0; i < hunters->size; i++) {
        HunterType *hunter = hunters->hunters[i];
        if(hunter->boredom >= BOREDOM_MAX) addHunter(boredHunters, hunter);
        if(hunter->fear >= FEAR_MAX) addHunter(scaredHunters, hunter);
    }
    
    // Setup some booleans that might change how we print
    int ghostGotBored = ghost->boredomTimer >= BOREDOM_MAX;
    int allHuntersLeft = boredHunters->size + scaredHunters->size == 4;

    if(!allHuntersLeft) {
        char ghostStr[MAX_STR];
        ghostToString(ghost->class, ghostStr);

        if(ghostGotBored) {
            printf("%-40s\n", "The ghost was no longer interested in haunting this house!");
            fprintf(logFile, "%-40s\n", "The ghost was no longer interested in haunting this house!");
        }

        printf("The ghost was discovered to be a %-30s.\n", ghostStr);
        fprintf(logFile, "The ghost was discovered to be a %-30s.\n", ghostStr);

        printf("%-40s\n", "The hunters were able to determine the type of ghost!");
        fprintf(logFile, "%-40s\n", "The hunters were able to determine the type of ghost!");

        printf("%-40s\n\n", "The hunters have won the game.");
        fprintf(logFile, "%-40s\n\n", "The hunters have won the game.");
    } else {
        printf("%-40s\n\n", "The ghost won and will continue to haunt the house.");
        fprintf(logFile, "%-40s\n\n", "The ghost won and will continue to haunt the house.");
    }

    printf("%-40s\n", "Hunters found the following evidence:");
    fprintf(logFile, "%-40s\n", "Hunters found the following evidence:");
    printEvidence(logFile, hunterEvList);

    printf("\n");

    printf("%-40s\n", "The evidence needed for the ghost is:");
    fprintf(logFile, "%-40s\n", "The evidence needed for the ghost is:");
    printEvidence(logFile, ghost->evList);

    printf("\n");
    
    int huntersGotBored = boredHunters->size > 0;
    int huntersGotScared = scaredHunters->size > 0;

    if(huntersGotBored) {
        printf("%-40s\n", "The following hunters became too bored to continue the hunt:");
        fprintf(logFile, "%-40s\n", "The following hunters became too bored to continue the hunt:");

        for(int i = 0; i < boredHunters->size; i++) {
            HunterType *hunter = boredHunters->hunters[i];
            printf("- %-37s\n", hunter->name);
            fprintf(logFile, "- %-37s\n", hunter->name);
        }

        printf("\n");
    }

    if(huntersGotScared) {
        printf("%-40s\n", "The following hunters became too scared to continue the hunt:");
        fprintf(logFile, "%-40s\n", "The following hunters became too scared to continue the hunt:");
        for(int i = 0; i < scaredHunters->size; i++) {
            HunterType *hunter = scaredHunters->hunters[i];
            printf("- %-37s\n", hunter->name);
            fprintf(logFile, "- %-37s\n", hunter->name);
        }

        printf("\n");
    }

    // Cleanup all of the memory we used 
    fclose(logFile);
    free(boredHunters);
    free(scaredHunters);
}
