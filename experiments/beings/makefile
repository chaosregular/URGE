# Updated Makefile with SDL2
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SDL_FLAGS = -lSDL2
SRC = beings_main.c beings_world.c beings_agents.c
OBJ = $(SRC:.c=.o)
TARGET = beings_sim

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lm $(SDL_FLAGS)

%.o: %.c beings_core.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: clean
