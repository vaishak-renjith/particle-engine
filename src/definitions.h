#pragma once

// Global constants
// You need to run `make clean` after changing these values
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

const int PIXEL_SIZE = 20;

const int GRID_WIDTH = SCREEN_WIDTH / PIXEL_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / PIXEL_SIZE;

// Particle macros
#define VOID (int)0x000000FF
#define SAND (int)0x00FFFFFF
#define WATER (int)0xFF0000FF
