OPT = -Wall -Wextra -pthread -g
OBJ_FILES = main.o utils.o logger.o house.o ghost.o hunter.o room.o evidence.o
BIN_NAME = a5

a5: $(OBJ_FILES)
	gcc $(OPT) -o $(BIN_NAME) $(OBJ_FILES)

main.o: main.c defs.h
	gcc $(OPT) -c main.c defs.h

utils.o: utils.c defs.h
	gcc $(OPT) -c utils.c defs.h

logger.o: logger.c defs.h
	gcc $(OPT) -c logger.c defs.h

house.o: house.c defs.h
	gcc $(OPT) -c house.c defs.h

ghost.o: ghost.c defs.h
	gcc $(OPT) -c ghost.c defs.h

hunter.o: hunter.c defs.h
	gcc $(OPT) -c hunter.c defs.h

room.o: room.c defs.h
	gcc $(OPT) -c room.c defs.h
 
evidence.o: evidence.c defs.h
	gcc $(OPT) -c evidence.c defs.h

clean:
	rm -f $(BIN_NAME) $(OBJ_FILES) defs.h.gch
