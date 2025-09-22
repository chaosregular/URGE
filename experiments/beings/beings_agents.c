// beings_agents.c
#include "beings_core.h"
#include <stdlib.h>
#include <math.h>

Being* being_create(double x, double y, int id) {
    Being* being = (Being*)malloc(sizeof(Being));
    being->world_x = x;
    being->world_y = y;
    being->vitality = 1.0;
    being->age = 0.0;
    being->id = id;
    
    // Initialize with 3 internal particles
    being->num_particles = 3;
    being->particles = (PropertyParticle*)malloc(3 * sizeof(PropertyParticle));
    
    for (int i = 0; i < 3; i++) {
        for (int a = 0; a < NUM_AXES; a++) {
            being->particles[i].position[a] = 2.0 * ((double)rand() / RAND_MAX) - 1.0;
            being->particles[i].velocity[a] = 0.0;
        }
        being->particles[i].mass = 1.0;
    }
    
    // Initialize needs and center of mass
    for (int n = 0; n < NUM_NEEDS; n++) {
        being->need_levels[n] = 0.5;
    }
    being_compute_center_of_mass(being);
    
    return being;
}

void being_compute_center_of_mass(Being* being) {
    for (int a = 0; a < NUM_AXES; a++) {
        being->center_of_mass[a] = 0.0;
        double total_mass = 0.0;
        
        for (int i = 0; i < being->num_particles; i++) {
            being->center_of_mass[a] += being->particles[i].position[a] * being->particles[i].mass;
            total_mass += being->particles[i].mass;
        }
        
        if (total_mass > 0.0) {
            being->center_of_mass[a] /= total_mass;
        }
    }
}

void being_update_internal_particles(Being* being, double dt) {
    // Simple harmonic motion for internal particles
    for (int i = 0; i < being->num_particles; i++) {
        for (int a = 0; a < NUM_AXES; a++) {
            // Spring force toward center of mass
            double force = -0.1 * (being->particles[i].position[a] - being->center_of_mass[a]);
            being->particles[i].velocity[a] += force * dt;
            being->particles[i].position[a] += being->particles[i].velocity[a] * dt;
            
            // Damping
            being->particles[i].velocity[a] *= 0.99;
        }
    }
    being_compute_center_of_mass(being);
}

void being_sense_needs(Being* being, WorldCell** world, int width, int height) {
    int x = (int)being->world_x;
    int y = (int)being->world_y;
    
    // Clamp coordinates to world bounds
    if (x < 0) x = 0;
    if (x >= width) x = width - 1;
    if (y < 0) y = 0;
    if (y >= height) y = height - 1;
    
    for (int n = 0; n < NUM_NEEDS; n++) {
        being->need_levels[n] = world[y][x].channels[n];
    }
}

void being_decide_movement(Being* being, WorldCell** world, int width, int height, double* dx, double* dy) {
    int x = (int)being->world_x;
    int y = (int)being->world_y;
    
    // Clamp coordinates
    if (x < 1) x = 1;
    if (x >= width-1) x = width - 2;
    if (y < 1) y = 1;
    if (y >= height-1) y = height - 2;
    
    *dx = 0.0;
    *dy = 0.0;
    
    // Move along gradients based on internal state
    for (int n = 0; n < NUM_NEEDS; n++) {
        // Weight by how important this need is to the being
        double weight = 1.0;
        if (n == NEED_FOOD) weight = 1.0 - being->center_of_mass[AXIS_EFFORT]; // Lazy beings seek food more
        if (n == NEED_SAFETY) weight = being->center_of_mass[AXIS_CAUTION]; // Cautious beings seek safety
        if (n == NEED_SOCIAL) weight = being->center_of_mass[AXIS_SOCIAL]; // Social beings seek company
        
        *dx += world[y][x].gradient_x[n] * weight * 0.1;
        *dy += world[y][x].gradient_y[n] * weight * 0.1;
    }
    
    // Add some randomness
    *dx += ((double)rand() / RAND_MAX - 0.5) * 0.05;
    *dy += ((double)rand() / RAND_MAX - 0.5) * 0.05;
}

void being_update_vitality(Being* being, WorldCell** world, int width, int height, double decay_rate) {
    // Vitality increases when needs are met, decreases otherwise
    double satisfaction = 0.0;
    for (int n = 0; n < NUM_NEEDS; n++) {
        satisfaction += being->need_levels[n];
    }
    satisfaction /= NUM_NEEDS;
    
    being->vitality += (satisfaction - 0.5) * 0.01; // Increase if needs met, decrease otherwise
    being->vitality -= decay_rate; // Base decay
    
    // Clamp vitality
    if (being->vitality < 0.0) being->vitality = 0.0;
    if (being->vitality > 1.0) being->vitality = 1.0;
    
    being->age += 0.01;
}
