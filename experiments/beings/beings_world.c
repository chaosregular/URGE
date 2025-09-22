// beings_world.c
#include "beings_core.h"
#include <stdlib.h>
#include <math.h>

WorldCell** world_init(int width, int height) {
    WorldCell** world = (WorldCell**)malloc(height * sizeof(WorldCell*));
    for (int y = 0; y < height; y++) {
        world[y] = (WorldCell*)malloc(width * sizeof(WorldCell));
        for (int x = 0; x < width; x++) {
            // Initialize with random resources
            world[y][x].channels[NEED_FOOD] = 0.5 + 0.5 * ((double)rand() / RAND_MAX);
            world[y][x].channels[NEED_SAFETY] = 0.5 + 0.5 * ((double)rand() / RAND_MAX);
            world[y][x].channels[NEED_SOCIAL] = 0.3 + 0.7 * ((double)rand() / RAND_MAX);
            
            for (int n = 0; n < NUM_NEEDS; n++) {
                world[y][x].gradient_x[n] = 0.0;
                world[y][x].gradient_y[n] = 0.0;
            }
        }
    }
    return world;
}

void world_update_gradients(WorldCell** world, int w, int h) {
    // Simple gradient computation using finite differences
    for (int y = 1; y < h-1; y++) {
        for (int x = 1; x < w-1; x++) {
            for (int n = 0; n < NUM_NEEDS; n++) {
                world[y][x].gradient_x[n] = (world[y][x+1].channels[n] - world[y][x-1].channels[n]) / 2.0;
                world[y][x].gradient_y[n] = (world[y+1][x].channels[n] - world[y-1][x].channels[n]) / 2.0;
            }
        }
    }
}

void world_diffuse_resources(WorldCell** world, int w, int h, double rate) {
    // Simple diffusion to regenerate resources
    for (int y = 1; y < h-1; y++) {
        for (int x = 1; x < w-1; x++) {
            for (int n = 0; n < NUM_NEEDS; n++) {
                double avg = (world[y-1][x].channels[n] + world[y+1][x].channels[n] +
                             world[y][x-1].channels[n] + world[y][x+1].channels[n]) / 4.0;
                world[y][x].channels[n] = world[y][x].channels[n] + rate * (avg - world[y][x].channels[n]);
                
                // Clamp to [0,1]
                if (world[y][x].channels[n] < 0.0) world[y][x].channels[n] = 0.0;
                if (world[y][x].channels[n] > 1.0) world[y][x].channels[n] = 1.0;
            }
        }
    }
}

