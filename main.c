#include "maze.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <map file>\n", argv[0]);
        return 1;
    }

    const char *mapFile = argv[1];

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Maze Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* wallTexture = loadTexture(renderer, "wall_texture.png");
    if (!wallTexture) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    char maze[HEIGHT][WIDTH];

    if (!loadMaze(mapFile, maze)) {
        fprintf(stderr, "Failed to load maze from file: %s\n", mapFile);
        SDL_DestroyTexture(wallTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    printMaze(maze);

    Player player = {1.5, 1.5, 0}; // Starting position and angle

    // Generate the maze starting from position (1,1)
    initializeMaze(maze);
    srand(time(NULL));
    generateMaze(maze, (Position){1, 1});

    bool quit = false;
    bool showMap = true;
    SDL_Event e;
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Key states array
    bool keyStates[SDL_NUM_SCANCODES] = {false};

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                keyStates[e.key.keysym.scancode] = true;
                if (e.key.keysym.sym == SDLK_m) {
                    showMap = !showMap;
                }
            } else if (e.type == SDL_KEYUP) {
                keyStates[e.key.keysym.scancode] = false;
            } else if (e.type == SDL_MOUSEMOTION) {
                int newMouseX = e.motion.x;
                player.angle += (newMouseX - mouseX) * 0.005; // Adjust sensitivity as needed
                mouseX = newMouseX;
            }
        }

        // Handle multiple key presses for movement
        float dx = 0, dy = 0;

        if (keyStates[SDL_SCANCODE_W]) {
            dx += cos(player.angle) * 0.1;
            dy += sin(player.angle) * 0.1;
        }
        if (keyStates[SDL_SCANCODE_S]) {
            dx -= cos(player.angle) * 0.1;
            dy -= sin(player.angle) * 0.1;
        }
        if (keyStates[SDL_SCANCODE_A]) {
            dx += sin(player.angle) * 0.1;
            dy -= cos(player.angle) * 0.1;
        }
        if (keyStates[SDL_SCANCODE_D]) {
            dx -= sin(player.angle) * 0.1;
            dy += cos(player.angle) * 0.1;
        }

        if (dx != 0 || dy != 0) {
            movePlayer(&player, maze, dx, dy);
        }

        // Handle multiple key presses for rotation
        if (keyStates[SDL_SCANCODE_LEFT]) {
            player.angle -= 0.1;
        }
        if (keyStates[SDL_SCANCODE_RIGHT]) {
            player.angle += 0.1;
        }

        renderMaze(renderer, maze, &player, showMap, wallTexture);
    }

    SDL_DestroyTexture(wallTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

