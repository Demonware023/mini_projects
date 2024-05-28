#include "maze.h"

void initializeMaze(char maze[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            maze[y][x] = (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1) ? '#' : ' ';
        }
    }
}

void printMaze(char maze[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", maze[y][x]);
        }
        printf("\n");
    }
}

void generateMaze(char maze[HEIGHT][WIDTH], Position pos) {
    Position stack[WIDTH * HEIGHT];
    int stackSize = 0;
    maze[pos.y][pos.x] = ' ';
    stack[stackSize++] = pos;

    while (stackSize > 0) {
        Position current = stack[stackSize - 1];
        Position next = getRandomNeighbor(maze, current);

        if (next.x != -1 && next.y != -1) {
            removeWalls(maze, current, next);
            maze[next.y][next.x] = ' ';
            stack[stackSize++] = next;
        } else {
            stackSize--;
        }
    }
}

bool isWithinBounds(int x, int y) {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

bool isMovable(char maze[HEIGHT][WIDTH], int x, int y) {
    return isWithinBounds(x, y) && maze[y][x] == ' ';
}

void removeWalls(char maze[HEIGHT][WIDTH], Position current, Position next) {
    int dx = next.x - current.x;
    int dy = next.y - current.y;

    if (dx == 2) {
        maze[current.y][current.x + 1] = ' ';
    } else if (dx == -2) {
        maze[current.y][current.x - 1] = ' ';
    }

    if (dy == 2) {
        maze[current.y + 1][current.x] = ' ';
    } else if (dy == -2) {
        maze[current.y - 1][current.x] = ' ';
    }
}

Position getRandomNeighbor(char maze[HEIGHT][WIDTH], Position pos) {
    Position neighbors[4];
    int count = 0;

    if (isMovable(maze, pos.x + 2, pos.y)) {
        neighbors[count++] = (Position){pos.x + 2, pos.y};
    }
    if (isMovable(maze, pos.x - 2, pos.y)) {
        neighbors[count++] = (Position){pos.x - 2, pos.y};
    }
    if (isMovable(maze, pos.x, pos.y + 2)) {
        neighbors[count++] = (Position){pos.x, pos.y + 2};
    }
    if (isMovable(maze, pos.x, pos.y - 2)) {
        neighbors[count++] = (Position){pos.x, pos.y - 2};
    }

    if (count > 0) {
        return neighbors[rand() % count];
    } else {
        return (Position){-1, -1};
    }
}

void renderMaze(SDL_Renderer* renderer, char maze[HEIGHT][WIDTH], Player* player, bool showMap, SDL_Texture* texture) {
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255); // Sky color
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 169, 169, 169, 255); // Ground color
    SDL_Rect ground = {0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2};
    SDL_RenderFillRect(renderer, &ground);

    const int numRays = SCREEN_WIDTH;
    const float fov = 3.14159 / 4;
    const float angleStep = fov / numRays;
    float rayAngle = player->angle - (fov / 2);

    for (int i = 0; i < numRays; i++) {
        float rayX = player->x;
        float rayY = player->y;
        float rayCos = cos(rayAngle);
        float raySin = sin(rayAngle);

        while (isWithinBounds((int)rayX, (int)rayY) && maze[(int)rayY][(int)rayX] == ' ') {
            rayX += rayCos * 0.1;
            rayY += raySin * 0.1;
        }

        float distance = sqrt((rayX - player->x) * (rayX - player->x) + (rayY - player->y) * (rayY - player->y));
        int lineHeight = (int)(SCREEN_HEIGHT / (distance * cos(rayAngle - player->angle)));
        int drawStart = (SCREEN_HEIGHT / 2) - (lineHeight / 2);
        int drawEnd = (SCREEN_HEIGHT / 2) + (lineHeight / 2);

        SDL_Rect wallRect = {i, drawStart, 1, lineHeight};
        SDL_RenderCopy(renderer, texture, NULL, &wallRect);

        rayAngle += angleStep;
    }

    if (showMap) {
        drawMap(renderer, maze, player);
    }

    SDL_RenderPresent(renderer);
}

void movePlayer(Player* player, char maze[HEIGHT][WIDTH], float dx, float dy) {
    float newX = player->x + dx;
    float newY = player->y + dy;

    if (isMovable(maze, (int)newX, (int)player->y)) {
        player->x = newX;
    }

    if (isMovable(maze, (int)player->x, (int)newY)) {
        player->y = newY;
    }
}

bool loadMaze(const char *filename, char maze[HEIGHT][WIDTH]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open map file");
	return false;
    }

    for (int y = 0; y < HEIGHT; y++) {
        if (fgets(maze[y], WIDTH + 2, file) == NULL) { // +2 to account for newline and null terminator
            fprintf(stderr, "Unexpected end of file\n");
            fclose(file);
            return false;
        }

        // Remove the newline character at the end of each line
        maze[y][strcspn(maze[y], "\n")] = '\0';

        for (int x = 0; x < WIDTH; x++) {
            if (maze[y][x] != '#' && maze[y][x] != '.') {
                fprintf(stderr, "Invalid character in map file: %c\n", maze[y][x]);
                fclose(file);
                return false;
            }
        }
    }

    fclose(file);
    return true;
}

void drawMap(SDL_Renderer* renderer, char maze[HEIGHT][WIDTH], Player* player) {
    const int cellSize = 20; // Size of each cell in the map
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for walls

    // Draw the maze
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (maze[y][x] == '#') {
                SDL_Rect wallRect = {x * cellSize, y * cellSize, cellSize, cellSize};
                SDL_RenderFillRect(renderer, &wallRect);
            }
        }
    }

    // Draw the player
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for player
    SDL_Rect playerRect = {(int)(player->x * cellSize) - 2, (int)(player->y * cellSize) - 2, 4, 4};
    SDL_RenderFillRect(renderer, &playerRect);

    // Draw the player's line of sight
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for line of sight
    SDL_RenderDrawLine(renderer, (int)(player->x * cellSize), (int)(player->y * cellSize),
                       (int)((player->x + cos(player->angle) * 5) * cellSize), (int)((player->y + sin(player->angle) * 5) * cellSize));
}

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char *file) {
    SDL_Surface* surface = IMG_Load(file);
    if (!surface) {
        fprintf(stderr, "Unable to load image %s! SDL_image Error: %s\n", file, IMG_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        fprintf(stderr, "Unable to create texture from %s! SDL Error: %s\n", file, SDL_GetError());
        return NULL;
    }

    return texture;
}
