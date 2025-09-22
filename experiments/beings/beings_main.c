// beings_main.c
#include "beings_core.h"
#include <stdio.h>
#include <stdlib.h>

#define WORLD_WIDTH 100
#define WORLD_HEIGHT 100
#define NUM_BEINGS 10

int main() {
    printf("Initializing Beings Simulation Framework...\n");
    
    // Initialize world
    WorldCell** world = world_init(WORLD_WIDTH, WORLD_HEIGHT);
    printf("World initialized (%dx%d)\n", WORLD_WIDTH, WORLD_HEIGHT);
    
    // Create beings
    Being* beings[NUM_BEINGS];
    for (int i = 0; i < NUM_BEINGS; i++) {
        double x = (double)rand() / RAND_MAX * WORLD_WIDTH;
        double y = (double)rand() / RAND_MAX * WORLD_HEIGHT;
        beings[i] = being_create(x, y, i);
        printf("Created being %d at (%.1f, %.1f)\n", i, x, y);
    }
    
    // Main simulation loop (simple test)
    for (int step = 0; step < 10; step++) {
        printf("\nStep %d:\n", step);
        
        // Update world
        world_diffuse_resources(world, WORLD_WIDTH, WORLD_HEIGHT, 0.01);
        world_update_gradients(world, WORLD_WIDTH, WORLD_HEIGHT);
        
        // Update beings
        for (int i = 0; i < NUM_BEINGS; i++) {
            being_update_internal_particles(beings[i], 0.1);
            
            printf("Being %d: ", i);
            printf("Effort=%.2f, ", beings[i]->center_of_mass[AXIS_EFFORT]);
            printf("Social=%.2f, ", beings[i]->center_of_mass[AXIS_SOCIAL]);
            printf("Caution=%.2f\n", beings[i]->center_of_mass[AXIS_CAUTION]);
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
    
    printf("Simulation completed successfully!\n");
    return 0;
}
