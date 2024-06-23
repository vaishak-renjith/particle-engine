OS = LINUX

CC = g++
CFLAGS = -Wall -Wextra -pedantic

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)


ifeq ($(OS), WINDOWS)
	DEL = del
	SDL_INCLUDE = -I".\include"
	SDL_LIB = -L".\lib" -lSDL2 -lSDL2main
	TARGET = main.exe
else
	DEL = rm
	SDL_LIB = -lSDL2 -lSDL2main
	TARGET = main.elf
endif

# Default target
all: $(TARGET)

# Compilation rule
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(SDL_INCLUDE) -o $(TARGET) $(OBJ) $(SDL_LIB)

# Object file rule
%.o: %.cpp
	$(CC) $(CFLAGS) $(SDL_INCLUDE) -c $< -o $@

# Clean rule
clean:
	$(DEL) $(TARGET) $(OBJ)

# Declare 'clean' as a phony target
.PHONY: all clean
