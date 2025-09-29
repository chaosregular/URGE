// universe_004_batch.c - Enhanced with structure cataloging
// DeesSeek ~ 2025.07.31
// gcc universe_osc1_batch_04.c -o universe_osc1_batch_04 -lSDL2 -lm
// chaosregular(x)2025-07-31_16:38:39 debugging
//universe_004_batch.c:245:27: error: 'f' undeclared (first use in this function)
//  245 |             tmp_cf = fabs(f/last_max_f);
// return base * pow(cos(pow(r, n_order)), k_exponent);
// chaosregular(x)2025-08-13_14:49:14 testy n-order 2.2 k_exponent 5.0
// chaosregular(x)2025-08-13_15:01:15 testy n-order 0.5 k_exponent 2.0
// chaosregular(x)2025-08-13_15:53:02 testy n-order 2.2 k_exponent 0.5 MAX_STABILIZATION_STEPS 1000000
#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Structure analysis parameters
#define MAX_STRUCTURES 10000
#define BUFFER_SIZE 50
#define STAB_THRESHOLD 1e-8
#define MAX_STABILIZATION_STEPS 1000000
#define EQUIVALENCE_THRESHOLD 1e-7
#define EQUIVALENCE_THRESHOLD_STICK 1e-6
// Physics parameters (carefully tuned)
#define MASS_0 33e-2
#define ATN_COEF 0.995 // 0.995
#define WIDTH 1300
#define HEIGHT 850
#define MAX_PARTICLES 500
//#define INITIAL_FLIP_DISTANCE 12.0
#define  n_order 2.2
#define  k_exponent 5.0

#define INITIAL_FLIP_DISTANCE  0.714286
#define V_0_INIT 0.0001
#define ADD_DIST_MUL 2.0
#define SPHERE_DIST_MUL 1.0
#define Z_SCALE 1.2
#define Z_SCALE_CONST 0.05
#define G_CONSTANT_ATTRACTIVE 7.0
#define G_CONSTANT_REPULSIVE 5.0
#define INITIAL_TIME_STEP 1e-4
#define     MAX_TIME_STEP 0.005
#define TIME_STEP_INC 1.002
#define TIME_STEP_DEC 0.9
#define MAX_DISPLACEMENT_RATIO 1e-2
#define MIN_DISPLACEMENT_RATIO 1e-4

typedef struct {
    double x, y, z;
    double vx, vy, vz;
    double mass;
    Uint8 r, g, b;
    double r_min; //chaosregular(x)2025-08-01_06:32:37    (for time step)
    // For structure analysis
    double min_dist_buffer[BUFFER_SIZE];
    int buffer_index;
    int buffer_count;
} Particle;

typedef struct {
    int n;
    int hits; //chaosregular(x)2025-08-01_21:46:49
    int sticks;
    double* distances;
} Structure;

typedef struct {
    int count;
    Structure structures[MAX_STRUCTURES];
} StructureCatalog;

// Global state
Particle particles[MAX_PARTICLES];
int particle_count = 2;
int paused = 0;
int stick_seq[MAX_PARTICLES];
double time_step = INITIAL_TIME_STEP;
double last_distance = 0.0;
double last_max_f = -0.0;
double last_max_v = -0.0;
double view_scale = 1.0;
double center_x = WIDTH/2.0;
double center_y = HEIGHT/2.0;
double gmin_z = 1e9, gmax_z = -1e9;
Uint32 simulation_start_time;
Uint32 last_title_update = 0;
int step_count = 0;
double total_simulation_time = 0.0;

// key 'i' initializes like batch mode with setn particles 'o' = setn--, 'p' = setn++
int setn = 2;

// Structure analysis
StructureCatalog global_catalog;
int current_run = 0;
int batch_mode = 0;
int min_n = 2, max_n = 10, runs_per_n = 5;
int current_n = 2;
int stabilization_steps = 0;
int found_structures = 0;

// Function prototypes
void initialize_system();
void initialize_system_with_n(int n);
void add_particle();
void update_view();
void project_3d_to_2d(double x, double y, double z, int *screen_x, int *screen_y);
void update_physics();
void record_minimal_distances();
int is_stabilized();
void get_structure_fingerprint(double* fingerprint);
int is_new_structure(double* fingerprint);
void save_to_catalog_stick(double* fingerprint);
void run_batch_simulation();
void render_particles(SDL_Renderer* renderer);
void update_title(SDL_Window* window);
void print_setup();

double flip_force(double r) {
    double r2 = r * r;
    double rr = r2 * r;               // r*3
//    double rr = r2 * r2 * r;          // r^5
//    double rr = r2 * r2 * r2 * r;     // r^7
    return G_CONSTANT_ATTRACTIVE / r2 - G_CONSTANT_REPULSIVE / rr;
}

// Wave-modulated force
double wave_force(double r) {
   double base = flip_force(r);
   return base * pow(cos(pow(r, n_order)), k_exponent);
}

int compare_doubles(const void* a, const void* b) {
    double da = *(const double*)a;
    double db = *(const double*)b;
    return (da > db) - (da < db);
}

void print_setup()
{
    printf("#defines:\nMAX_STRUCTURES %i\nBUFFER_SIZE %i\nSTAB_THRESHOLD %e\nMAX_STABILIZATION_STEPS %i\nEQUIVALENCE_THRESHOLD %e\nMASS_0 %f\n",
        MAX_STRUCTURES,BUFFER_SIZE,STAB_THRESHOLD,MAX_STABILIZATION_STEPS,EQUIVALENCE_THRESHOLD,MASS_0);
    printf("ATN_COEF %f\nWIDTH %i\nHEIGHT %i\nMAX_PARTICLES %i\nINITIAL_FLIP_DISTANCE %f\nV_0_INIT %f\nADD_DIST_MUL %f\n",
     ATN_COEF, WIDTH, HEIGHT, MAX_PARTICLES, INITIAL_FLIP_DISTANCE, V_0_INIT, ADD_DIST_MUL);

    printf("Z_SCALE %e\nZ_SCALE_CONST %e\nG_CONSTANT_ATTRACTIVE %f\nG_CONSTANT_REPULSIVE %f\nINITIAL_TIME_STEP %e\nMAX_DISPLACEMENT_RATIO %f\nMIN_DISPLACEMENT_RATIO %f\n",
    Z_SCALE, Z_SCALE_CONST, G_CONSTANT_ATTRACTIVE, G_CONSTANT_REPULSIVE, INITIAL_TIME_STEP, MAX_DISPLACEMENT_RATIO, MIN_DISPLACEMENT_RATIO);
}

void initialize_system() {
    particle_count = 2;
    particles[0] = (Particle){0, 0, 0, 0, 0, 0, 1.0, 255, 0, 0};
    particles[1] = (Particle){INITIAL_FLIP_DISTANCE, 0, 0, 0, 0, 0, 1.0, 0, 255, 0};
    center_x = WIDTH/2.0;
    center_y = HEIGHT/2.0;
    view_scale = HEIGHT / (4.0 * INITIAL_FLIP_DISTANCE);
    
    // Initialize distance buffers
    for (int i = 0; i < particle_count; i++) {
        memset(particles[i].min_dist_buffer, 0, sizeof(double)*BUFFER_SIZE);
        particles[i].buffer_index = 0;
        particles[i].buffer_count = 0;
    }
}

void initialize_system_with_n(int n) {
    particle_count = n;
    double radius = INITIAL_FLIP_DISTANCE * pow(n, 1.0/3.0) * 0.5;
    
    // Create a spherical distribution
    for (int i = 0; i < n; i++) {
        double u = (double)rand()/RAND_MAX;
        double v = (double)rand()/RAND_MAX;
        double theta = u * 2.0 * M_PI;
        double phi = acos(2.0 * v - 1.0);
        double r = radius * pow((double)rand()/RAND_MAX, 1.0/3.0);
        
        particles[i].x = r * sin(phi) * cos(theta);
        particles[i].y = r * sin(phi) * sin(theta);
        particles[i].z = r * cos(phi);
        particles[i].vx = (2.0*(double)rand()/RAND_MAX - 1.0) * 0.1;
        particles[i].vy = (2.0*(double)rand()/RAND_MAX - 1.0) * 0.1;
        particles[i].vz = (2.0*(double)rand()/RAND_MAX - 1.0) * 0.1;
        particles[i].mass = MASS_0;
        particles[i].r = rand() % 256;
        particles[i].g = rand() % 256;
        particles[i].b = 221;
        
        // Initialize distance buffer
        memset(particles[i].min_dist_buffer, 0, sizeof(double)*BUFFER_SIZE);
        particles[i].buffer_index = 0;
        particles[i].buffer_count = 0;
    }
    
    // Reset analysis state
    stabilization_steps = 0;
}


// chaosregular(x)2025-08-01_03:13:12
void initialize_system_with_n_crx(int n) {
    particle_count = n;
    step_count = 0;
    total_simulation_time = 0.0;    
    double radius = INITIAL_FLIP_DISTANCE * SPHERE_DIST_MUL ;// * pow(n, 1.0/3.0) * 0.5;
    
    // Create a spherical distribution
    for (int i = 0; i < n; i++) {
        double u = (double)rand()/RAND_MAX;
        double v = (double)rand()/RAND_MAX;
        double theta = u * 2.0 * M_PI;
        double phi = acos(2.0 * v - 1.0);
        //double r = radius * pow((double)rand()/RAND_MAX, 1.0/3.0); 
        double r = SPHERE_DIST_MUL * radius + radius * pow((double)rand()/RAND_MAX, 1.0/3.0); // crx
        
        particles[i].x = r * sin(phi) * cos(theta);
        particles[i].y = r * sin(phi) * sin(theta);
        particles[i].z = r * cos(phi);
        particles[i].vx = (2.0*(double)rand()/RAND_MAX - 0.50) * V_0_INIT;
        particles[i].vy = (2.0*(double)rand()/RAND_MAX - 0.50) * V_0_INIT;
        particles[i].vz = (2.0*(double)rand()/RAND_MAX - 0.50) * V_0_INIT;
        particles[i].mass = MASS_0;
        particles[i].r = rand() % 256;
        particles[i].g = rand() % 256;
        particles[i].b = 221;
        
        // Initialize distance buffer
        memset(particles[i].min_dist_buffer, 0, sizeof(double)*BUFFER_SIZE);
        particles[i].buffer_index = 0;
        particles[i].buffer_count = 0;
    }
    
    // Reset analysis state
    stabilization_steps = 0;
}
// chaosregular(x)2025-08-01_03:13:12

void add_particle() {
    if (particle_count >= MAX_PARTICLES) return;
    
    int ref = rand() % particle_count;
    double angle1 = 2 * M_PI * (rand() / (double)RAND_MAX);
    double angle2 = M_PI * (rand() / (double)RAND_MAX - 0.5);
    double distance = INITIAL_FLIP_DISTANCE * ADD_DIST_MUL * (0.9 + 0.1*(rand()/(double)RAND_MAX));
    
    particles[particle_count].x = particles[ref].x + distance * cos(angle1) * cos(angle2);
    particles[particle_count].y = particles[ref].y + distance * sin(angle1) * cos(angle2);
    particles[particle_count].z = particles[ref].z + distance * sin(angle2);
    particles[particle_count].vx = particles[ref].vx;
    particles[particle_count].vy = particles[ref].vy;
    particles[particle_count].vz = particles[ref].vz;
    particles[particle_count].mass = MASS_0;
    particles[particle_count].r = rand() % 256;
    particles[particle_count].g = rand() % 256;
    particles[particle_count].b = 221;
    
    // Initialize distance buffer
    memset(particles[particle_count].min_dist_buffer, 0, sizeof(double)*BUFFER_SIZE);
    particles[particle_count].buffer_index = 0;
    particles[particle_count].buffer_count = 0;
    
    particle_count++;
}

void update_view() {
    if (particle_count == 0) return;
    
    double min_x = 1e9, max_x = -1e9;
    double min_y = 1e9, max_y = -1e9;
    double min_z = 1e9, max_z = -1e9;
    
    for (int i = 0; i < particle_count; i++) {
        if (particles[i].x < min_x) min_x = particles[i].x;
        if (particles[i].x > max_x) max_x = particles[i].x;
        if (particles[i].y < min_y) min_y = particles[i].y;
        if (particles[i].y > max_y) max_y = particles[i].y;
        if (particles[i].z < min_z) min_z = particles[i].z;
        if (particles[i].z > max_z) max_z = particles[i].z;
    }
    
    gmin_z = min_z;
    gmax_z = max_z;
    
    double size_x = max_x - min_x;
    double size_y = max_y - min_y;
    double max_size = fmax(size_x, size_y);
    
    if (max_size < 1e-5) max_size = INITIAL_FLIP_DISTANCE;
    
    double target_size = 0.98 * fmin(WIDTH, HEIGHT);
    view_scale = target_size / max_size;
    center_x = (min_x + max_x) / 2.0;
    center_y = (min_y + max_y) / 2.0;
}

void project_3d_to_2d(double x, double y, double z, int *screen_x, int *screen_y) {
    *screen_x = WIDTH/2.0 + 0.98*view_scale * (x - center_x);
    *screen_y = HEIGHT/2.0 + 0.98*view_scale * (y - center_y);
}

void update_physics() {
    if (paused) return;
    // Calculate forces
    double max_velocity = 0.0;
    double tmp_cf, max_force = 0.0;
    for (int i = 0; i < particle_count; i++) {
        double fx = 0, fy = 0, fz = 0;
        double r_minimal = 1e19;
        for (int j = 0; j < particle_count; j++) {
            if (i == j) continue;
            double f = 0.0;
            double dx = particles[j].x - particles[i].x;
            double dy = particles[j].y - particles[i].y;
            double dz = particles[j].z - particles[i].z;
            double r = sqrt(dx*dx + dy*dy + dz*dz);
            if (r > 0) {
                //f = flip_force(r);
                f = wave_force(r);
                if(f > max_force) max_force = f;
                fx += f * dx / r;
                fy += f * dy / r;
                fz += f * dz / r;
                last_distance = r;
                if(r<r_minimal)r_minimal = r;
            }
            tmp_cf = fabs(f/last_max_f);
            if(tmp_cf > 1) tmp_cf = 1.0;
            particles[i].r = round(tmp_cf*254.0);
        }
        particles[i].r_min = r_minimal;
        
        // Update velocity
        particles[i].vx += time_step * fx / particles[i].mass;
        particles[i].vy += time_step * fy / particles[i].mass;
        particles[i].vz += time_step * fz / particles[i].mass;
        
        // Velocity-based coloring
        tmp_cf = sqrt(particles[i].vx*particles[i].vx + 
                     particles[i].vy*particles[i].vy + 
                     particles[i].vz*particles[i].vz)/last_max_v;
        if(tmp_cf > 1) tmp_cf = 1;
        particles[i].g = round(tmp_cf*254.0);
        
        // Apply damping
        particles[i].vx *= ATN_COEF;
        particles[i].vy *= ATN_COEF;
        particles[i].vz *= ATN_COEF;

        // Track max velocity
        double v = sqrt(particles[i].vx*particles[i].vx + 
                       particles[i].vy*particles[i].vy + 
                       particles[i].vz*particles[i].vz);
        if (v > max_velocity) max_velocity = v;
        
        // Update force history
        last_max_f = max_force;
        last_max_v = max_velocity;
    }

    // Update positions
    //double max_disp = 0.0;
    double max_dispra = 0.0;

    for (int i = 0; i < particle_count; i++) {
        double dx = particles[i].vx * time_step;
        double dy = particles[i].vy * time_step;
        double dz = particles[i].vz * time_step;
        //double disp = sqrt(dx*dx + dy*dy + dz*dz);
        double dispra = sqrt(dx*dx + dy*dy + dz*dz)/particles[i].r_min;// chaosregular(x)2025-08-01_06:46:56
        if (dispra > max_dispra) max_dispra = dispra;

        particles[i].x += dx;
        particles[i].y += dy;
        particles[i].z += dz;
    }
    
    // Adjust time step dynamically
    if (max_dispra > MAX_DISPLACEMENT_RATIO) {
        time_step *= TIME_STEP_DEC;
    } else if (max_dispra < MIN_DISPLACEMENT_RATIO) {
        if(time_step<MAX_TIME_STEP)time_step *= TIME_STEP_INC; //chaosregular(x)2025-08-01_04:08:52 max
    }

    // if (max_disp > MAX_DISPLACEMENT_RATIO * INITIAL_FLIP_DISTANCE) {
    //     time_step *= 0.7;
    // } else if (max_disp < MIN_DISPLACEMENT_RATIO * INITIAL_FLIP_DISTANCE) {
    //     if(time_step<0.01)time_step *= 1.001; //chaosregular(x)2025-08-01_04:08:52 max
    // }

//chaosregular(x)2025-07-31_17:18:06 added MIN_DISPLACEMENT_RATIO instead of MAX..
    
    // Record minimal distances for structure analysis
    record_minimal_distances();
    
    // Update view only in interactive mode
    if (!batch_mode) {
        update_view();
    }
}

void record_minimal_distances() {
    for (int i = 0; i < particle_count; i++) {
        double min_dist = 1e20;
        for (int j = 0; j < particle_count; j++) {
            if (i == j) continue;
            double dx = particles[j].x - particles[i].x;
            double dy = particles[j].y - particles[i].y;
            double dz = particles[j].z - particles[i].z;
            double dist = sqrt(dx*dx + dy*dy + dz*dz);
            if (dist < min_dist) min_dist = dist;
        }


        particles[i].min_dist_buffer[particles[i].buffer_index] = min_dist;
        particles[i].buffer_index = (particles[i].buffer_index + 1) % BUFFER_SIZE;
        if (particles[i].buffer_count < BUFFER_SIZE) {
            particles[i].buffer_count++;
// chaosregular(x)2025-07-31_17:08:38
//            printf("buff fill p[%i].buffer_count=%i\n",i,particles[i].buffer_count);            
        }
    }
}

// chaosregular(x)2025-07-31_16:47:51
// checking stabilization detection
int is_stabilized() {

    for (int i = 0; i < particle_count; i++) {
        if (particles[i].buffer_count < BUFFER_SIZE) {
//            printf("startup %i\n",particles[i].buffer_count);
            return 0; // Not enough data
        }

        // double min_val = particles[i].min_dist_buffer[0];
        // double max_val = min_val;

// chaosregular(x)2025-07-31_16:49:50 changed values
        double min_val = 1e19;
        double max_val = 0.0;
//chaosregular(x)2025-07-31_16:49:50

        for (int j = 1; j < BUFFER_SIZE; j++) {
            double val = particles[i].min_dist_buffer[j];
            if (val < min_val) min_val = val;
            if (val > max_val) max_val = val;
        }
        
        if (max_val - min_val > STAB_THRESHOLD) {
// chaosregular(x)2025-07-31_17:07:39            
//            printf("val %f\n",(max_val - min_val));
            return 0; // Still changing
        }
    }
    return 1; // Stabilized
}

void get_structure_fingerprint(double* fingerprint) {
    for (int i = 0; i < particle_count; i++) {
        double sum = 0.0;
        for (int j = 0; j < BUFFER_SIZE; j++) {
            sum += particles[i].min_dist_buffer[j];
        }
        fingerprint[i] = sum / BUFFER_SIZE;
    }
    qsort(fingerprint, particle_count, sizeof(double), compare_doubles);
}

int is_new_structure(double* fingerprint) {
    for (int i = 0; i < global_catalog.count; i++) {
        if (global_catalog.structures[i].n != particle_count) continue;
        
        int equivalent = 1;
        for (int j = 0; j < particle_count; j++) {
            if (fabs(global_catalog.structures[i].distances[j] - fingerprint[j]) > EQUIVALENCE_THRESHOLD) {
                equivalent = 0;
                break;
            }
        }
        
        if (equivalent) {
            global_catalog.structures[i].hits++;
            return 0; // Found matching structure
        }
    }
    return 1; // New structure
}

// void save_to_catalog(double* fingerprint) {
//     if (global_catalog.count >= MAX_STRUCTURES) return;
    
//     Structure* s = &global_catalog.structures[global_catalog.count];
//     s->n = particle_count;
//     s->distances = malloc(particle_count * sizeof(double));
//     s->hits = 1; //chaosregular(x)2025-08-01_21:49:02
//     memcpy(s->distances, fingerprint, particle_count * sizeof(double));

//     global_catalog.count++;
//     found_structures++;
    
//     // Print structure info
//     printf("NS(%d)[%i]: [", particle_count,current_run);
//     for (int i = 0; i < particle_count; i++) {
//         printf("%.8f", s->distances[i]);
//         if (i < particle_count-1) printf(", ");
//     }
//     printf("]\n");
// }

void save_to_catalog_stick(double* fingerprint) {
    if (global_catalog.count >= MAX_STRUCTURES) return;
    
    Structure* s = &global_catalog.structures[global_catalog.count];
    s->n = particle_count;
    s->distances = malloc(particle_count * sizeof(double));
    s->hits = 1; //chaosregular(x)2025-08-01_21:49:02
    memcpy(s->distances, fingerprint, particle_count * sizeof(double));

    global_catalog.count++;
    found_structures++;

//chaosregular(x)2025-08-01_23:44:39
    double stick = s->distances[0];
    int sticks = 1;
    for (int i = 0; i < particle_count; i++) {
        if(fabs(stick - s->distances[i]) > EQUIVALENCE_THRESHOLD_STICK)
        {
            sticks++;
            stick = s->distances[i];
        }    
    }
    s->sticks = sticks;
//chaosregular(x)2025-08-01_23:44:47
    
    // Print structure info
    printf("NS(%d/%i) r=%i: [", particle_count, sticks, current_run);
    for (int i = 0; i < particle_count; i++) {
        printf("%.7f", s->distances[i]);
        if (i < particle_count-1) printf(", ");
    }
    printf("]\n");
}


void run_batch_simulation() {
    batch_mode = 1;
    srand(time(NULL)); // Seed RNG properly
    
    printf("Starting batch simulation for n=%d to %d (%d runs each)\n", 
           min_n, max_n, runs_per_n);
    printf("----------------------------------------\n");
    print_setup();
    printf("----------------------------------------\n");
    
    // Initialize catalog
    global_catalog.count = 0;
    found_structures = 0;
    
    for (current_n = min_n; current_n <= max_n; current_n++) {
        for (current_run = 0; current_run < runs_per_n; current_run++) {
            // Initialize system with current_n particles
            initialize_system_with_n_crx(current_n);
            
            // Run simulation until stabilized
            stabilization_steps = 0;
            while (!is_stabilized() && stabilization_steps < MAX_STABILIZATION_STEPS) {
                update_physics();
                stabilization_steps++;
            }
            
            if (stabilization_steps >= MAX_STABILIZATION_STEPS) {
                printf("n=%d, run=%d: Failed to stabilize within %d steps\n", 
                       current_n, current_run, MAX_STABILIZATION_STEPS);
                continue;
            }
            
            // Analyze structure
            double fingerprint[current_n];
            get_structure_fingerprint(fingerprint);

            if (is_new_structure(fingerprint)) {

                save_to_catalog_stick(fingerprint);
            }

        }
    }
    
    printf("\nBatch simulation complete!\n");
    printf("Discovered %d unique  structures for n=%d to %d\n", 
           found_structures, min_n, max_n);
    printf("----------------------------------------\n");

    // Dump results
    double prob = 0.0;

    // for (int i = 0; i < global_catalog.count; i++) {
    //     prob = global_catalog.structures[i].hits*100.0/runs_per_n;
    //     printf("S(%i/%i) [%i] %06.3f (%i/%i)\n",global_catalog.structures[i].n,global_catalog.structures[i].sticks,i,prob,global_catalog.structures[i].hits,runs_per_n);
    // }    

    for (int i = 0; i < global_catalog.count; i++) {
        //stick seq

        //stick_seq[i]
        double stick = global_catalog.structures[i].distances[0];
        int stind = 0;
        stick_seq[stind] = 0;
        for (int k = 0; k < particle_count; k++) {
            if(fabs(stick - global_catalog.structures[i].distances[k]) > EQUIVALENCE_THRESHOLD_STICK)
            {
                stind++;
                stick_seq[stind] = 1;
                stick = global_catalog.structures[i].distances[k];
            }
            else
            {
                stick_seq[stind]+=1;
            }
        }
        prob = global_catalog.structures[i].hits*100.0/runs_per_n;
        printf("S(");
        for(int k=0;k<global_catalog.structures[i].sticks;k++)
        {
            if(k<(global_catalog.structures[i].sticks-1))
            {
                printf("%i-",stick_seq[k]);
            }else printf("%i",stick_seq[k]);
        }
        printf(")");        
//        printf("S(%i/%i) [%i] %06.3f (%i/%i)\n",global_catalog.structures[i].n,global_catalog.structures[i].sticks,i,prob,global_catalog.structures[i].hits,runs_per_n);
        printf(" [%i] %06.3f (%i/%i)\n",i,prob,global_catalog.structures[i].hits,runs_per_n);
    }    
    printf("fingerprints:\n");
    for (int i = 0; i < global_catalog.count; i++) {
        
        printf("S(%d/%i) [%i]: [", global_catalog.structures[i].n, global_catalog.structures[i].sticks,i);
        for (int k = 0; k < particle_count; k++) {
            printf("%.6f", global_catalog.structures[i].distances[k]);
            if (k < particle_count-1) printf(", ");
        }
        printf("]\n");
    }    

    // Clean up
    for (int i = 0; i < global_catalog.count; i++) {
        free(global_catalog.structures[i].distances);
    }
    
    // Exit after batch processing
    exit(0);
}

int main(int argc, char* argv[]) {
    // Check for batch mode arguments
    if (argc == 5 && strcmp(argv[1], "--batch") == 0) {
        min_n = atoi(argv[2]);
        max_n = atoi(argv[3]);
        runs_per_n = atoi(argv[4]);
        
        if (min_n < 2) min_n = 2;
        if (max_n > MAX_PARTICLES) max_n = MAX_PARTICLES;
        if (min_n > max_n) max_n = min_n;
        
        run_batch_simulation();
    }
    
    // Interactive mode
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("3D Force-Flip Universe", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
        SDL_RENDERER_ACCELERATED);
    
    srand(SDL_GetTicks());
    simulation_start_time = SDL_GetTicks();
    initialize_system();
    
    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_n) add_particle();
                if (event.key.keysym.sym == SDLK_SPACE) paused = !paused;
                if (event.key.keysym.sym == SDLK_r) initialize_system();
                if (event.key.keysym.sym == SDLK_i) initialize_system_with_n_crx(setn);
                if (event.key.keysym.sym == SDLK_p) setn++;
                if (event.key.keysym.sym == SDLK_o) if(setn>1)setn--;
                if (event.key.keysym.sym == SDLK_b) {
                    // Start batch simulation in a separate thread in real application
                    printf("Batch mode not supported in interactive session. Use command line.\n");
                }
            }
        }
        
        update_physics();
        total_simulation_time += time_step;
        step_count++;
        
        update_title(window);
        render_particles(renderer);
        SDL_Delay(1);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void render_particles(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 240);
    SDL_RenderClear(renderer);
    
    for (int i = 0; i < particle_count; i++) {
        double depth_scale = 1.0 / (Z_SCALE_CONST + 
            (fabs(particles[i].z - gmin_z + (0.1*(gmax_z - gmin_z))) / 
            (Z_SCALE * (gmax_z - gmin_z))));
        
        unsigned char alpha_z = 10 + 250/(1000 * depth_scale);
        int size = 1 + (int)(1 * depth_scale);
        if (size < 1) size = 1;
        
        SDL_SetRenderDrawColor(renderer, 
            particles[i].r, 
            particles[i].g, 
            particles[i].b, 
            alpha_z);
        
        int sx, sy;
        project_3d_to_2d(particles[i].x, particles[i].y, particles[i].z, &sx, &sy);
        
        SDL_Rect rect = { sx - size/2, sy - size/2, size, size };
        SDL_RenderFillRect(renderer, &rect);
    }
    
    SDL_RenderPresent(renderer);
}

void update_title(SDL_Window* window) {
    if (SDL_GetTicks() - last_title_update > 250) {
        char title[256];
        snprintf(title, sizeof(title), 
            "3D Force-Flip Universe | Scale: %.2f | dt: %.6f | Part: %d | Steps: %d | Time: %.2fs | iN %i",
            view_scale, time_step, particle_count, step_count, total_simulation_time, setn);
        
        SDL_SetWindowTitle(window, title);
        last_title_update = SDL_GetTicks();
    }
}