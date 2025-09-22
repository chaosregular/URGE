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
