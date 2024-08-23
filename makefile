CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lncurses

SRC = main.c
OBJ = $(SRC:.c=.o)
EXEC = task_manager

all: $(EXEC)
$(EXEC): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJ) $(EXEC)
depend: $(SRC)
	makedepend $(SRC)
.PHONY: all clean depend
