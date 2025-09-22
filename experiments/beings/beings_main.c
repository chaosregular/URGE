// Updated beings_main.c with SDL visualization
#include "beings_core.h"
#include "beings_ui.c"  // Include UI functions
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define WORLD_WIDTH 100
#define WORLD_HEIGHT 100
#define NUM_BEINGS 10

int main() {
    printf("Initializing Beings Simulation Framework...\n");
    
    // Initialize SDL
    if (!ui_init()) {
        return 1;
    }
    
    // Initialize world and beings
    WorldCell** world = world_init(WORLD_WIDTH, WORLD_HEIGHT);
    Being* beings[NUM_BEINGS];
    
    for (int i = 0; i < NUM_BEINGS; i++) {
        double x = (double)rand() / RAND_MAX * WORLD_WIDTH;
        double y = (double)rand() / RAND_MAX * WORLD_HEIGHT;
        beings[i] = being_create(x, y, i);
    }
    
    // Main simulation loop
    int running = 1;
    SDL_Event event;
    int step = 0;
    
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                }
            }
        }
        
        // Update world
        world_diffuse_resources(world, WORLD_WIDTH, WORLD_HEIGHT, 0.01);
        world_update_gradients(world, WORLD_WIDTH, WORLD_HEIGHT);
        
        // Update beings
        for (int i = 0; i < NUM_BEINGS; i++) {
            being_sense_needs(beings[i], world, WORLD_WIDTH, WORLD_HEIGHT);
            being_update_internal_particles(beings[i], 0.1);
            
            double dx, dy;
            being_decide_movement(beings[i], world, WORLD_WIDTH, WORLD_HEIGHT, &dx, &dy);
            beings[i]->world_x += dx;
            beings[i]->world_y += dy;
            
            being_update_vitality(beings[i], world, WORLD_WIDTH, WORLD_HEIGHT, 0.001);
        }
        
        // Render frame
        ui_render_frame(world, WORLD_WIDTH, WORLD_HEIGHT, beings, NUM_BEINGS);
        
        // Cap frame rate
        SDL_Delay(50);
        step++;
        
        // Print status every 100 steps
        if (step % 100 == 0) {
            printf("Step %d: ", step);
            double avg_vitality = 0.0;
            for (int i = 0; i < NUM_BEINGS; i++) {
                avg_vitality += beings[i]->vitality;
            }
            avg_vitality /= NUM_BEINGS;
            printf("Average vitality: %.3f\n", avg_vitality);
        }
    }
    
    // Cleanup
    for (int i = 0; i < NUM_BEINGS; i++) {
        free(beings[i]->particles);
        free(beings[i]);
    }
    
    for (int y = 0; y < WORLD_HEIGHT; y++) {
        free(world[y]);
    }
    free(world);
    
    ui_cleanup();
    printf("Simulation completed after %d steps!\n", step);
    return 0;
}
