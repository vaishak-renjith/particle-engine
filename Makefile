# # Makefile for SDL2 program on Windows using PowerShell
# 
# # Compiler
# CC = g++
# # Compiler flags
# CFLAGS = -Wall -Wextra -pedantic
# 
# # SDL paths (adjust these to match your installation paths)
# SDL_INCLUDE = -I"C:\Dev\c++\sdl2\particle-engine\include"
# SDL_LIB = -L"C:\Dev\c++\sdl2\particle-engine\lib" -lSDL2 -lSDL2main
# 
# # Source files
# SRC = $(wildcard src/*.cpp)
# OBJ = $(SRC:.cpp=.o)
# 
# # Output executable
# TARGET = main.exe
# 
# # Default target
# all: $(TARGET)
# 
# # Compilation rule
# $(TARGET): $(OBJ)
# 	$(CC) $(CFLAGS) $(SDL_INCLUDE) -o $(TARGET) $(OBJ) $(SDL_LIB)
# 
# # Object file rule
# %.o: %.cpp
# 	$(CC) $(CFLAGS) $(SDL_INCLUDE) -c $< -o $@
# 
# # Clean rule
# clean:
# 	del $(TARGET) $(OBJ)
# 
# # Declare 'clean' as a phony target
# .PHONY: all clean

# Linux
all:
	g++ src/*.cpp -lSDL2 -lSDL2main -o main.elf
