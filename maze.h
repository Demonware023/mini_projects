#ifndef MAZE_H
#define MAZE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define WIDTH 24
#define HEIGHT 24
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

typedef struct {
    float x, y;
    float angle;
} Player;

typedef struct {
    int x, y;
} Position;

void initializeMaze(char maze[HEIGHT][WIDTH]);
void printMaze(char maze[HEIGHT][WIDTH]);
void generateMaze(char maze[HEIGHT][WIDTH], Position pos);
bool isWithinBounds(int x, int y);
bool isMovable(char maze[HEIGHT][WIDTH], int x, int y);
void removeWalls(char maze[HEIGHT][WIDTH], Position current, Position next);
Position getRandomNeighbor(char maze[HEIGHT][WIDTH], Position pos);
void renderMaze(SDL_Renderer* renderer, char maze[HEIGHT][WIDTH], Player* player, bool showMap, SDL_Texture* texture);
void movePlayer(Player* player, char maze[HEIGHT][WIDTH], float dx, float dy);
bool loadMaze(const char *filename, char maze[HEIGHT][WIDTH]);
void drawMap(SDL_Renderer* renderer, char maze[HEIGHT][WIDTH], Player* player);
SDL_Texture* loadTexture(SDL_Renderer* renderer, const char *file);

#endif // MAZE_H

