#include <SDL2/SDL.h>
#include <emscripten/emscripten.h>

SDL_Window *window;
SDL_Renderer *renderer;
int running = 1;

// the bitmap of the game of life (use C++20)
#include <vector>
#include <array>
#include <algorithm>
#include <chrono>
#include <random>
#include <iostream>
#include <chrono>
#include <thread>

constexpr int width = 640;
constexpr int height = 480;

std::vector<std::array<bool, width>> bitmap(height);

void init_bitmap() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 60);

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            bitmap[row][col] = dis(gen) > (col + row);
        }
    }
}

void step() {
    std::vector<std::array<bool, width>> new_bitmap(height);

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int neighbors = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (i == 0 && j == 0) continue;
                    int r = row + i;
                    int c = col + j;
                    if (r >= 0 && r < height && c >= 0 && c < width) {
                        neighbors += bitmap[r][c];
                    }
                }
            }

            if (bitmap[row][col]) {
                new_bitmap[row][col] = neighbors == 2 || neighbors == 3;
            } else {
                new_bitmap[row][col] = neighbors == 3;
            }
        }
    }

    bitmap = new_bitmap;
}

void main_loop() {
    // Event handling
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = 0;
            emscripten_cancel_main_loop(); // Stop the main loop in Emscripten
        }
    }

    // Update logic
    // (This is where you would update your game's state, move objects, etc.)

    // Rendering
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (bitmap[row][col]) {
                SDL_Rect rect = { col*10, row*10, 10, 10 }; // x, y, width, height
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    SDL_RenderPresent(renderer); // Display the rendered frame
    static std::chrono::system_clock::time_point last_update = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update).count() > 700) {
        last_update = now;
        step();
    }
}

int main() {
    init_bitmap();
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create an SDL window
    window = SDL_CreateWindow("SDL2 Emscripten Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Set up the main loop with Emscripten
    emscripten_set_main_loop(main_loop, 0, 1);

    // Clean up (this won't be reached in Emscripten until the loop is cancelled)
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
