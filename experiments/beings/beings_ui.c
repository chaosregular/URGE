// beings_ui.c
#include "beings_core.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define CELL_SIZE 8

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int ui_init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    
    window = SDL_CreateWindow("Beings Simulation", 
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    
    return 1;
}

void ui_cleanup() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void ui_draw_world(WorldCell** world, int width, int height) {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // Draw world cells
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            SDL_Rect rect = {
                x * CELL_SIZE,
                y * CELL_SIZE,
                CELL_SIZE,
                CELL_SIZE
            };
            
            // Color based on resource levels (Food=Red, Safety=Green, Social=Blue)
            int r = (int)(world[y][x].channels[NEED_FOOD] * 255);
            int g = (int)(world[y][x].channels[NEED_SAFETY] * 255);
            int b = (int)(world[y][x].channels[NEED_SOCIAL] * 255);
            
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void ui_draw_beings(Being** beings, int count) {
    for (int i = 0; i < count; i++) {
        // Color based on internal state (Effort=Red, Social=Green, Caution=Blue)
        int r = (int)((beings[i]->center_of_mass[AXIS_EFFORT] + 1.0) * 127.5);
        int g = (int)((beings[i]->center_of_mass[AXIS_SOCIAL] + 1.0) * 127.5);
        int b = (int)((beings[i]->center_of_mass[AXIS_CAUTION] + 1.0) * 127.5);
        
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        
        // Draw being as a circle
        int center_x = (int)(beings[i]->world_x * CELL_SIZE);
        int center_y = (int)(beings[i]->world_y * CELL_SIZE);
        int radius = 5;
        
        for (int w = -radius; w <= radius; w++) {
            for (int h = -radius; h <= radius; h++) {
                if (w*w + h*h <= radius*radius) {
                    SDL_RenderDrawPoint(renderer, center_x + w, center_y + h);
                }
            }
        }
        
        // Draw vitality as a surrounding ring
        int vitality_radius = (int)(radius * beings[i]->vitality) + 2;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int a = 0; a < 360; a += 10) {
            double angle = a * M_PI / 180.0;
            int vx = center_x + vitality_radius * cos(angle);
            int vy = center_y + vitality_radius * sin(angle);
            SDL_RenderDrawPoint(renderer, vx, vy);
        }
    }
}

void ui_render_frame(WorldCell** world, int width, int height, Being** beings, int count) {
    ui_draw_world(world, width, height);
    ui_draw_beings(beings, count);
    SDL_RenderPresent(renderer);
}
