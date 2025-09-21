//deepseek_c_20250729_803c45.c
//2D toroid template
// gcc CA_4nPCA_012.c -o CA_4nPCA_012 -lSDL2 -lm
// chaosregular(x)2025-08-13_18:37:30: zmiana nazwy na CA_4nPCA_001.c
// CA_4nPCA_001.c
// all 8 neighbours + node -> 9*8=72 bytes, max. hw 33 bits -> node+3.125 bit from neigh. 
// 4x8 = 24 + node's own 8 bit  -> 32 bits - nodes own calculate as average from neighbours. 4 LSB's from neighbours as address
// here (above) some tuning probably, but looks doable
// on click - get color and add 'distortion vector' in sphere around clicked state ðv(R(-3,3)G(-3,3)B(-3,3))
// może od razu make zrobić i podzielić, będzie łatwiej mieszać?
// rules - niewielki szum +-1 wszędzie
// pozycje z nparticles -> centra, początkowo kilka centrów ręcznie?
// odpychające w centrum, przechodzące w zero dla r/2 i w przyciągające na zzewnątrz -spadające do zera dla r
// r=0 -> random from particle opposite? - r/4 -> max odpych r/2 -> 0  3r/4 -> max przyc r -> 0
// począkowo można siatkę przyjąć 10x10 i węzły na przemian
// może lepiej od razu mieć n-Particle pod ręką?
// mouse down -> zasiewanie - gęstośc jak wyżej - kopie w stronę klikniętego stanu 
// chaosregular(x)2025-08-14_15:43:05 - odpalony CA z 1:1+dist, śmiga podbite do CA_4nPCA_002.c
// chaosregular(x)2025-08-14_17:14:00 CA - rules generowane szybko zbieżne do 'static' na niskich wartościach rgb
// chaosregular(x)2025-08-14_15:43:05 - podejście do integracji n-Particles, podbite do CA_4nPCA_003.c
// chaosregular(x)2025-08-14_21:30:00 jakies próby sprzęgania pierwsze , podbite do CA_4nPCA_004.c
// chaosregular(x)2025-08-14_23:59:25 próby c.d, pdbite na wypadek wtopy do  CA_4nPCA_005.c
// chaosregular(x)2025-08-15_01:27:39 'ustawiacz struktur' podbite do CA_4nPCA_006.c
// chaosregular(x)2025-08-15_14:30:05 zapas, podbite do CA_4nPCA_007.c.
// chaosregular(x)2025-08-18_16:45:39 kontunuacja po rozmowie z Seek. podbite do CA_4nPCA_008.c
// chaosregular(x)2025-08-18_21:53:37 rgb -> cokolwiek offset i ujemne
// chaosregular(x)2025-08-19_14:27:23 part rgb->CA test, podbite do CA_4nPCA_009.c.
// chaosregular(x)2025-08-23_05:29:35 eksperymenty ze strojeniem, podbite do CA_4nPCA_010.c.
// chaosregular(x)2025-08-25_23:47:50 small brains, podbite do CA_4nPCA_011.c.
// chaosregular(x)2025-08-31_14:56:00 CA_4nPCA_012.c
// chaosregular(x)2025-09-04_06:01:36 - przełązany cutoff - ograniczenie sił do lokalnych, rzędu 2
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// =========================
// CONFIGURATION SECTION nP
// =========================
#define DYNAMIC_TIMESTEP

#define SPEED_FEED_0 0.000087
//#define SPEED_BOOST 200 // chaosregular(x)2025-08-18_16:49:47 zmienione na parameters
//#define SPEED_BOOST 372311 // chaosregular(x)2025-08-19_03:01:20 zmienione na 1e-7
#define SPEED_BOOST 1e-1
#define PART_INIT_0 138
// Structure analysis parameters
#define MAX_STRUCTURES 100
#define BUFFER_SIZE 20
#define STAB_THRESHOLD 1e-8
#define MAX_STABILIZATION_STEPS 20000
#define EQUIVALENCE_THRESHOLD 1e-7
#define EQUIVALENCE_THRESHOLD_STICK 1e-6
// Physics parameters (carefully tuned)
#define MASS_0 33e-2
#define MASS_1 MASS_0
#define ATN_COEF_0 0.95 // 0.995
#define WIDTH 1300
#define HEIGHT 850
#define MAX_PARTICLES 300
//#define INITIAL_FLIP_DISTANCE 12.0
//#define INITIAL_FLIP_DISTANCE  0.893904
#define INITIAL_FLIP_DISTANCE  0.655910 // chaosregular(x)2025-08-09_22:43:24
#define ADD_DIST_MUL 1.7
#define SPHERE_DIST_MUL 0.7

#define MIN_FLIP_DISTANCE  INITIAL_FLIP_DISTANCE*0.1
#define V_0_INIT 0.1
#define Z_SCALE 1.2
#define Z_SCALE_CONST 0.04
#define G_CONSTANT_ATTRACTIVE 7.0e3
#define G_CONSTANT_REPULSIVE 5.0e3
#define INITIAL_TIME_STEP 1e-7
#define MAX_TIME_STEP 0.01
#define TIME_STEP_INC 1.02
#define TIME_STEP_DEC 0.9
#define MAX_DISPLACEMENT_RATIO 1e-2
#define MIN_DISPLACEMENT_RATIO 1e-3

#define CUTOFF_R 0.8
#define CUTOFF_WIDTH 0.2
// =========================
// CONFIGURATION SECTION CA
// =========================
#define GRID_WIDTH         200     // Main simulation grid width
#define CELL_SIZE          4       // Pixel size for rendering
#define GRID_HEIGHT        GRID_WIDTH // Main simulation grid height
#define GHOST_LAYERS       1       // Number of border layers (1 for 3x3 neighborhood)
#define FPS                60      // Target frames per second
#define GEN_2_MULT_0 10.0      // noise 'amplitude'
#define GEN_2_DENS_0 0.05    // noise 'desnity'

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
    double energy;
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
int paused_CA = 1;
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
int setn = PART_INIT_0;

// energy
double tot_energy_est = 0.0;

// distance
double max_R = 0.0;

// blob radius
double blob_r = 0.0;

// dynamic_dumping 
double ATN_COEF = ATN_COEF_0;

// Structure analysis
StructureCatalog global_catalog;
int current_run = 0;
int batch_mode = 0;
int min_n = 2, max_n = 10, runs_per_n = 5;
int current_n = 2;
int stabilization_steps = 0;
int found_structures = 0;

// mnoznik x,y,z -> grid
double mult_xyx_2_gridxy = 0.3;

// speed mapping
double speed_boost = SPEED_BOOST;

// CA generation
double  GEN_2_MULT = GEN_2_MULT_0;    // noise 'amplitude'
double  GEN_2_DENS = GEN_2_DENS_0;    // noise 'desnity'

// Np ca COUPLING
double SPEED_FEED = SPEED_FEED_0;

// Function prototypes
void initialize_system();
void initialize_system_with_n(int n);
void initialize_system_with_n_crx(int n); // 'i' key
void add_particle(double m);
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

void compute_center_of_mass(int particle_count, Particle* particles, double* cm_x, double* cm_y, double* cm_z);
double move_to_center_of_mass(int particle_count, Particle* particles); // returns rmax

// chaosregular(x)2025-09-04_05:56:21 from Seek:
// - Implement distance-limiter:
//   ```c
//   double soft_cutoff(double r, double r_cut, double width) {
//       return 0.5 * (1.0 + cos(M_PI * fmin(1.0, (r - r_cut)/width));
//   }
//   ```
// - Apply to flip-force:
//   ```c
//   f = flip_force(r) * soft_cutoff(r, CUTOFF_R, CUTOFF_WIDTH);
//   ```

// double flip_force(double r, double m1, double m2) {
//     double r2 = r * r;
//     double rr = r2 * r;               // r*3
// //    double rr = r2 * r2 * r;          // r^5
// //    double rr = r2 * r2 * r2 * r;     // r^7
//     return G_CONSTANT_ATTRACTIVE * m1 * m2 / r2 - G_CONSTANT_REPULSIVE / rr; // chaosregular(x)2025-08-14_17:47:44 / speed
// }


double soft_cutoff(double r){
   return (0.5 * (1.0 + cos(M_PI * fmin(1.0, (r - CUTOFF_R)/CUTOFF_WIDTH))));
}

double soft_cutoff1(double r, double r_cut, double width){
   return (0.5 * (1.0 + cos(M_PI * fmin(1.0, (r - r_cut)/width))));
}



double flip_force(double r) {
    double r2 = r * r;
    double rr = r2 * r;               // r*3
//    double rr = r2 * r2 * r;          // r^5
//    double rr = r2 * r2 * r2 * r;     // r^7
    double force = G_CONSTANT_ATTRACTIVE  / r2 - G_CONSTANT_REPULSIVE / rr;
    return soft_cutoff(r)*force;
}

double flip_force1(double r) {
    double r2 = r * r;
    double rr = r2 * r;               // r*3
//    double rr = r2 * r2 * r;          // r^5
//    double rr = r2 * r2 * r2 * r;     // r^7
    return G_CONSTANT_ATTRACTIVE  / r2 - G_CONSTANT_REPULSIVE / rr;
}


// CA


// Define your custom cell structure here
// DeepSeek oryginal
// typedef struct {
//     double value1;
//     double value2;
//     double velocity1;
//     double velocity2;
// } Cell;

typedef struct {
    uint8_t R;
    uint8_t G;
    uint8_t B;
} Cell;

// ======================
// GRID STRUCTURE & UTILITIES
// ======================
// Calculate array dimensions with ghost layers
#define ARRAY_WIDTH  (GRID_WIDTH + 2 * GHOST_LAYERS)
#define ARRAY_HEIGHT (GRID_HEIGHT + 2 * GHOST_LAYERS)

// 'rules' table
Cell rules[256][256][256];


// particle influenced rules
double rules_nP[256][256][256];


// Grid buffers
Cell grid[2][ARRAY_HEIGHT][ARRAY_WIDTH];
int current = 0;

// Convert grid coordinates to array indices with toroidal wrapping
#define GRID_TO_ARRAY_Y(y) ((y) + GHOST_LAYERS)
#define GRID_TO_ARRAY_X(x) ((x) + GHOST_LAYERS)

// Access cell with automatic coordinate wrapping
Cell* get_cell(int buffer, int grid_y, int grid_x) {
    // Normalize coordinates to toroidal space
    grid_y = (grid_y + GRID_HEIGHT) % GRID_HEIGHT;
    grid_x = (grid_x + GRID_WIDTH) % GRID_WIDTH;
    
    return &grid[buffer][GRID_TO_ARRAY_Y(grid_y)][GRID_TO_ARRAY_X(grid_x)];
}


// n particles functions
// Update ghost layers (borders)
void update_ghost_layers(int buffer) {
    // Top and bottom borders
    for (int x = -GHOST_LAYERS; x < GRID_WIDTH + GHOST_LAYERS; x++) {
        for (int ly = 0; ly < GHOST_LAYERS; ly++) {
            // Top border = corresponding bottom cells
            *get_cell(buffer, ly - GHOST_LAYERS, x) = 
                *get_cell(buffer, GRID_HEIGHT - GHOST_LAYERS + ly, x);
            
            // Bottom border = corresponding top cells
            *get_cell(buffer, GRID_HEIGHT + ly, x) = 
                *get_cell(buffer, ly, x);
        }
    }
    
    // Left and right borders (including corners)
    for (int y = -GHOST_LAYERS; y < GRID_HEIGHT + GHOST_LAYERS; y++) {
        for (int lx = 0; lx < GHOST_LAYERS; lx++) {
            // Left border = corresponding right cells
            *get_cell(buffer, y, lx - GHOST_LAYERS) = 
                *get_cell(buffer, y, GRID_WIDTH - GHOST_LAYERS + lx);
            
            // Right border = corresponding left cells
            *get_cell(buffer, y, GRID_WIDTH + lx) = 
                *get_cell(buffer, y, lx);
        }
    }
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

double calculate_total_energy() {
    double energy = 0.0;
    for (int i = 0; i < particle_count; i++) {
        energy += 0.5 * particles[i].mass * 
                  (particles[i].vx*particles[i].vx + 
                   particles[i].vy*particles[i].vy + 
                   particles[i].vz*particles[i].vz);
        for (int j = i+1; j < particle_count; j++) {
            double dx = particles[j].x - particles[i].x;
            double dy = particles[j].y - particles[i].y;
            double dz = particles[j].z - particles[i].z;
            double r = sqrt(dx*dx + dy*dy + dz*dz);
            energy += G_CONSTANT_ATTRACTIVE / r; // Approximate potential
//            energy += wave_force(r) * r; // Approximate potential
        }
    }
    return energy;
}

void initialize_system() {
    particle_count = 1;
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
    // initial speed
    double vinit_cor = V_0_INIT / sqrt(n+10);
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
        // particles[i].vx = (2.0*(double)rand()/RAND_MAX - 1.0) * 0.1;
        // particles[i].vy = (2.0*(double)rand()/RAND_MAX - 1.0) * 0.1;
        // particles[i].vz = (2.0*(double)rand()/RAND_MAX - 1.0) * 0.1;
        particles[i].vx = (2.0*(double)rand()/RAND_MAX - 1.0) * vinit_cor;
        particles[i].vy = (2.0*(double)rand()/RAND_MAX - 1.0) * vinit_cor;
        particles[i].vz = (2.0*(double)rand()/RAND_MAX - 1.0) * vinit_cor;
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
    srand(time(0));
    // initial speed
    double vinit_cor = V_0_INIT / sqrt(sqrt(sqrt(n)));    
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
        double r = SPHERE_DIST_MUL * radius + radius * pow((double)rand()/RAND_MAX, 1.0/10.0); // crx
        
        particles[i].x = r * sin(phi) * cos(theta);
        particles[i].y = r * sin(phi) * sin(theta);
        particles[i].z = r * cos(phi);
        particles[i].vx = (2.0*(double)rand()/RAND_MAX - 0.50) * vinit_cor;
        particles[i].vy = (2.0*(double)rand()/RAND_MAX - 0.50) * vinit_cor;
        particles[i].vz = (2.0*(double)rand()/RAND_MAX - 0.50) * vinit_cor;
        if(i%2){
            particles[i].mass = MASS_0;
        }else particles[i].mass = MASS_1;
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

void add_particle(double m) {
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
    particles[particle_count].mass = m;
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
    
    //chaosregular(x)2025-08-04_04:59:15
        max_R = 0.0;

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
            if(r>max_R)max_R = r;
            if (r > MIN_FLIP_DISTANCE) {
                f = flip_force(r); //m1, m2
//                f = flip_force(r,particles[i].mass,particles[j].mass);
                if(f > max_force) max_force = f;
                fx += f * dx / r;
                fy += f * dy / r;
                fz += f * dz / r;
                last_distance = r;
                if(r<r_minimal)r_minimal = r;
            }
            else
            {
                r = MIN_FLIP_DISTANCE;
                f = flip_force(MIN_FLIP_DISTANCE);
                //f = flip_force(MIN_FLIP_DISTANCE,particles[i].mass,particles[j].mass);
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
        // Seek After line where you compute r_min, add:
        double stability = 1.0 / (particles[i].r_min + 1e-5);
        particles[i].b = (uint8_t)fmin(255, stability * 20.0); // Scale factor adjustable
        
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
#ifdef DYNAMIC_TIMESTEP
    double max_dispra = 0.0;
#endif


// keep still
    blob_r = move_to_center_of_mass( particle_count, particles);


    for (int i = 0; i < particle_count; i++) {
        double dx = particles[i].vx * time_step;
        double dy = particles[i].vy * time_step;
        double dz = particles[i].vz * time_step;
        //double disp = sqrt(dx*dx + dy*dy + dz*dz);
        double dispra = sqrt(dx*dx + dy*dy + dz*dz)/particles[i].r_min;// chaosregular(x)2025-08-01_06:46:56
#ifdef DYNAMIC_TIMESTEP
        if (dispra > max_dispra) max_dispra = dispra;
#endif
        particles[i].x += dx;
        particles[i].y += dy;
        particles[i].z += dz;
    }

    // Seek: In update_physics(), after updating all particles:
    double max_energy = 0.0, max_stability = 0.0;
    for (int i = 0; i < particle_count; i++) {
        double energy = sqrt(particles[i].vx*particles[i].vx + particles[i].vy*particles[i].vy + particles[i].vz*particles[i].vz);
        double stability = 1.0 / (particles[i].r_min + 1e-5);
        if (energy > max_energy) max_energy = energy;
        if (stability > max_stability) max_stability = stability;
    }
    printf("Frame %d: Max energy: %.3f, Max stability: %.3f\n", step_count, max_energy, max_stability);
    
    // Adjust time step dynamically
#ifdef DYNAMIC_TIMESTEP
    if (max_dispra > MAX_DISPLACEMENT_RATIO) {
        time_step *= TIME_STEP_DEC;
    } else if (max_dispra < MIN_DISPLACEMENT_RATIO) {
        if(time_step<MAX_TIME_STEP)time_step *= TIME_STEP_INC; //chaosregular(x)2025-08-01_04:08:52 max
    }
//    ATN_COEF = 0.9999 - 10*time_step;
#endif
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
//chaosregular(x)2025-08-04_04:58:09
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
    s->energy = calculate_total_energy();
// chaosregular(x)2025-08-04_04:13:34    
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

                printf("n=%d, run=%d: Failed to stabilize within %d steps max_R = %f\n", 
                       current_n, current_run, MAX_STABILIZATION_STEPS, max_R);
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
        printf("S%i.%i(",global_catalog.structures[i].n,global_catalog.structures[i].sticks);
        for(int k=0;k<global_catalog.structures[i].sticks;k++)
        {
            if(k<(global_catalog.structures[i].sticks-1))
            {
                printf("%i-",stick_seq[k]);
            }else printf("%i",stick_seq[k]);
        }
        printf(")");        
//        printf("S(%i/%i) [%i] %06.3f (%i/%i)\n",global_catalog.structures[i].n,global_catalog.structures[i].sticks,i,prob,global_catalog.structures[i].hits,runs_per_n);
        printf(" [%i] %7.3f (%7i/%i) E=%f\n",i,prob,global_catalog.structures[i].hits,runs_per_n,global_catalog.structures[i].energy);
    }    
    printf("fingerprints:\n");
    for (int i = 0; i < global_catalog.count; i++) {
        particle_count = global_catalog.structures[i].n;
        printf("S%i.%i [%i]: [",global_catalog.structures[i].n, global_catalog.structures[i].sticks,i);
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

// ======================
// SIMULATION FUNCTIONS
// ======================

// init rules table
void init_rules() {
    double R,R1;
    double G,G1;
    double B,B1;
    double dx,dy,dz,rad,coef;
    srand(time(NULL));
    for (int z = 0; z < 255; z++) {
        for (int y = 0; y < 255; y++) {
            for (int x = 0; x < 255; x++) {
                R = (GEN_2_MULT*(0.5-((double)rand() / RAND_MAX)));
                G = (GEN_2_MULT*(0.5-((double)rand() / RAND_MAX)));
                B = (GEN_2_MULT*(0.5-((double)rand() / RAND_MAX)));
                // dx = (127.5-(double)x);
                // dy = (127.5-(double)y);
                // dz = (127.5-(double)z);
                // rad = sqrt(dx*dx+dy*dy+dz*dz);
                //coef = 20*(1-1.0/rad);
                // coef = 1.0;
                R1 = (x+R); if(R1>255.0)R1=255.0;if(R1<0.0)R1=0.0;
                G1 = (y+G); if(G1>255.0)G1=255.0;if(G1<0.0)G1=0.0;
                B1 = (z+B); if(B1>255.0)B1=255.0;if(B1<0.0)B1=0.0;
                
                // if(((double)rand() / RAND_MAX)<GEN_2_DENS){rules[x][y][z].R = (uint8_t)R1;}else{rules[x][y][z].R = x;}
                // if(((double)rand() / RAND_MAX)<GEN_2_DENS){rules[x][y][z].G = (uint8_t)G1;}else{rules[x][y][z].G = y;}
                // if(((double)rand() / RAND_MAX)<GEN_2_DENS){rules[x][y][z].B = (uint8_t)B1;}else{rules[x][y][z].B = z;}


                if(((double)rand() / RAND_MAX)<GEN_2_DENS){
                    rules[x][y][z].R = (uint8_t)R1;
                    rules[x][y][z].G = (uint8_t)G1;
                    rules[x][y][z].B = (uint8_t)B1;
                }else{
                    rules[x][y][z].R = x;
                    rules[x][y][z].G = y;
                    rules[x][y][z].B = z;
                }

                // rules[x][y][z].R = (uint8_t)(x+R*coef);
                // rules[x][y][z].G = (uint8_t)(y+G*coef);
                // rules[x][y][z].B = (uint8_t)(z+B*coef);
                if((x+y)==0){
                    printf("rules[%i][%i][%i] = (%i,%i,%i) [%f,%f,%f] rad=%f\n",x,y,z,rules[x][y][z].R,rules[x][y][z].G,rules[x][y][z].B,R,G,B,rad);
                }
            }
        }
        
    }
}

// init rules table
void init_rules_001() {
    double R;
    double G;
    double B;    
    srand(time(NULL));
    for (int z = 0; z < 255; z++) {
        for (int y = 0; y < 255; y++) {
            for (int x = 0; x < 255; x++) {
                R = 1.0-(double)rand() / RAND_MAX;
                G = 1.0-(double)rand() / RAND_MAX;
                B = 1.0-(double)rand() / RAND_MAX;

                rules[x][y][z].R = (uint8_t)(x+R*1.71); //chaosregular(x)2025-08-19_04:16:38 było 2.01
                rules[x][y][z].G = (uint8_t)(y+G*1.71);
                rules[x][y][z].B = (uint8_t)(z+B*1.71);
                if((x+y)==0){
                    printf("rules[%i][%i][%i] = (%i,%i,%i) [%f,%f,%f]\n",x,y,z,rules[x][y][z].R,rules[x][y][z].G,rules[x][y][z].B,R,G,B);
                }
            }
        }
        
    }
}

// init rules table chaosregular(x)2025-08-20_14:45:00
void init_rules_002() {
    double R,R1;
    double G,G1;
    double B,B1;
    double dx,dy,dz,rad,coef;
    srand(time(NULL));
    for (int z = 0; z < 255; z++) {
        for (int y = 0; y < 255; y++) {
            for (int x = 0; x < 255; x++) {
                R = x+10.0*(0.5-((double)rand() / RAND_MAX));
                G = y+10.0*(0.5-((double)rand() / RAND_MAX));
                B = z+10.0*(0.5-((double)rand() / RAND_MAX));
                dx = -(127.5-(double)x)/1.0;
                dy = -(127.5-(double)y)/1.0;
                dz = -(127.5-(double)z)/1.0;
                // rad = sqrt(dx*dx+dy*dy+dz*dz);
                //coef = 20*(1-1.0/rad);
                // coef = 1.0;
                R1 = (x+R); if(R1>255.0)R1=255.0;if(R1<0.0)R1=0.0;
                G1 = (y+G); if(G1>255.0)G1=255.0;if(G1<0.0)G1=0.0;
                B1 = (z+B); if(B1>255.0)B1=255.0;if(B1<0.0)B1=0.0;
                rules[x][y][z].R = R1;
                rules[x][y][z].G = G1;
                rules[x][y][z].B = B1;
                // rules[x][y][z].R = (uint8_t)(x+R*coef);
                // rules[x][y][z].G = (uint8_t)(y+G*coef);
                // rules[x][y][z].B = (uint8_t)(z+B*coef);
                if((x+y)==0){
                    printf("rules[%i][%i][%i] = (%i,%i,%i) [%f,%f,%f] rad=%f\n",x,y,z,rules[x][y][z].R,rules[x][y][z].G,rules[x][y][z].B,R,G,B,rad);
                }
            }
        }
        
    }
}

// init rules table flat 1:1 chaosregular(x)2025-08-29_03:44:10
void init_flat() {
    double R;
    double G;
    double B;    
    srand(time(NULL));
    for (int z = 0; z < 255; z++) {
        for (int y = 0; y < 255; y++) {
            for (int x = 0; x < 255; x++) {
                R = 1.0-(double)rand() / RAND_MAX;
                G = 1.0-(double)rand() / RAND_MAX;
                B = 1.0-(double)rand() / RAND_MAX;

                rules[x][y][z].R = (uint8_t)(x); 
                rules[x][y][z].G = (uint8_t)(y);
                rules[x][y][z].B = (uint8_t)(z);
            }
        }
        
    }
}

// Initialize grid - implement your logic here
// kilka centrów 'z ręki' - przyciągacze i odpychacze?
void init_simulation() {
    uint8_t R;
    uint8_t G;
    uint8_t B;    
    srand(time(NULL));
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Cell* c = get_cell(current, y, x);
            c->R = (uint8_t)(255.0*rand() / RAND_MAX);
            c->G = (uint8_t)(255.0*rand() / RAND_MAX);
            c->B = (uint8_t)(255.0*rand() / RAND_MAX);
        }
    }
    update_ghost_layers(current);
}

// Initialize grid - zeroing
// to 0 chaosregular(x)2025-08-29_03:45:09
void init_grid() {
    uint8_t R;
    uint8_t G;
    uint8_t B;    
    srand(time(NULL));
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Cell* c = get_cell(current, y, x);
            c->R = (uint8_t)(0);
            c->G = (uint8_t)(0);
            c->B = (uint8_t)(0);
        }
    }
    update_ghost_layers(current);
}

// DeepSeek oryginal
// void init_simulation() {
//     srand(time(NULL));
//     for (int y = 0; y < GRID_HEIGHT; y++) {
//         for (int x = 0; x < GRID_WIDTH; x++) {
//             Cell* c = get_cell(current, y, x);
//             c->value1 = (double)rand() / RAND_MAX;
//             c->value2 = (double)rand() / RAND_MAX;
//             c->velocity1 = 0.0;
//             c->velocity2 = 0.0;
//         }
//     }
//     update_ghost_layers(current);
// }

void update_simulation() {
    int next = 1 - current;
    
    // Process all main grid cells
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Cell* current_cell = get_cell(current, y, x);
            Cell* next_cell = get_cell(next, y, x);
            
            int sum_R = 0;
            int sum_G = 0;
            int sum_B = 0;
            
            // Iterate over neighborhood (adjust range as needed)
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    
                    Cell* neighbor = get_cell(current, y + dy, x + dx);
                    sum_R += neighbor->R;
                    sum_G += neighbor->G;
                    sum_B += neighbor->B;
                }
            }
            sum_R = (uint8_t)(sum_R/8);
            sum_G = (uint8_t)(sum_G/8);
            sum_B = (uint8_t)(sum_B/8);
            // Store result in next buffer
            next_cell->R = rules[sum_R][sum_G][sum_B].R;
            next_cell->G = rules[sum_R][sum_G][sum_B].G;
            next_cell->B = rules[sum_R][sum_G][sum_B].B;
        }
    }
    
    // Update borders for new state
    update_ghost_layers(next);
    current = next;
}


// ======================
// RENDERING FUNCTIONS
// ======================
void render_grid(SDL_Renderer* renderer) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Cell* cell = get_cell(current, x, y);
            
            // Map values to color (customize as needed)
            uint8_t r = cell->R;
            uint8_t g = cell->G;
            uint8_t b = cell->B;
            
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            
            SDL_Rect rect = {
                x * CELL_SIZE,
                y * CELL_SIZE,
                CELL_SIZE,
                CELL_SIZE
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

// ======================
// MAIN APPLICATION
// ======================
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

    SDL_Init(SDL_INIT_VIDEO);
    
    // CA window
    SDL_Window* window = SDL_CreateWindow(
        "Toroidal Grid Simulation",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        GRID_WIDTH * CELL_SIZE,
        GRID_HEIGHT * CELL_SIZE,
        0
    );
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
        SDL_RENDERER_ACCELERATED);
    
    // nP window
        SDL_Window* window_nP = SDL_CreateWindow("3D Force-Flip Universe", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer_nP = SDL_CreateRenderer(window_nP, -1, 
        SDL_RENDERER_ACCELERATED);
    // initialize nP
    srand(SDL_GetTicks());
    simulation_start_time = SDL_GetTicks();
    initialize_system();

    // initialize CA
    // init_rules();
    init_rules_001();

    init_simulation();
    
    Uint32 frame_delay = 1000 / FPS;
    int running = 1;
    int step = 0;
    int step_d = 0;
    
    while (running) {
        Uint32 frame_start = SDL_GetTicks();
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_KEYDOWN) {
                // CA
                if (event.key.keysym.sym == SDLK_z) {init_simulation();printf("z - CA states reset\n");}
                if (event.key.keysym.sym == SDLK_x) {init_rules();printf("x - CA rules reset\n");}
                if (event.key.keysym.sym == SDLK_SPACE) {paused_CA = !paused_CA;printf("SPACE - CA paused = %i\n",paused_CA);}
                if (event.key.keysym.sym == SDLK_ESCAPE) {running = 0;printf("ESC - Quit\n");}
                // nP
                if (event.key.keysym.sym == SDLK_n) {add_particle(MASS_0);printf("n -> add particle m=%f\n",MASS_0);}
                if (event.key.keysym.sym == SDLK_m) {add_particle(MASS_1);printf("m -> add particle m=%f\n",MASS_1);}
                if (event.key.keysym.sym == SDLK_q) {paused = !paused;printf("q - n Particle update physics paused = %i\n",paused);}
                //if (event.key.keysym.sym == SDLK_r) {initialize_system();printf("r - n-particles reset to 1 particle\n");}
                if (event.key.keysym.sym == SDLK_i) {initialize_system_with_n_crx(setn);printf("i - initialize n-particles to setn=%i particles\n",setn);}
                if (event.key.keysym.sym == SDLK_p) {setn++;printf("p - setn=%i particles\n",setn);}
                if (event.key.keysym.sym == SDLK_o) {if(setn>1)setn--;printf("o - setn=%i particles\n",setn);}
                if (event.key.keysym.sym == SDLK_k) {mult_xyx_2_gridxy *= 0.9;printf("k - mul=%1.6e\n",mult_xyx_2_gridxy);}
                if (event.key.keysym.sym == SDLK_l) {mult_xyx_2_gridxy *= 1.1;printf("l - mul=%1.6e\n",mult_xyx_2_gridxy);}
                if (event.key.keysym.sym == SDLK_h) {speed_boost *= 0.9;printf("h - speed_boost=%1.6e\n",speed_boost);}
                if (event.key.keysym.sym == SDLK_j) {speed_boost *= 1.1;printf("j - speed_boost=%1.6e\n",speed_boost);}


                if (event.key.keysym.sym == SDLK_a) {GEN_2_MULT *= 0.9912214;printf("a - GEN_2_MULT=%1.6f\n",GEN_2_MULT);}
                if (event.key.keysym.sym == SDLK_s) {GEN_2_MULT *= 1.0154655;printf("s - GEN_2_MULT=%1.6f\n",GEN_2_MULT);}
                if (event.key.keysym.sym == SDLK_d) {GEN_2_DENS *= 0.9911115;printf("d - GEN_2_DENS=%1.6f\n",GEN_2_DENS);}
                if (event.key.keysym.sym == SDLK_f) {GEN_2_DENS *= 1.0145488;printf("f - GEN_2_DENS=%1.6f\n",GEN_2_DENS);}


                if (event.key.keysym.sym == SDLK_t) {SPEED_FEED *= 0.9911115;printf("t - SPEED_FEED=%1.6f\n",SPEED_FEED);}
                if (event.key.keysym.sym == SDLK_y) {SPEED_FEED *= 1.0145488;printf("y - SPEED_FEED=%1.6f\n",SPEED_FEED);}

                if (event.key.keysym.sym == SDLK_c) {init_flat();printf("c - CA rules 1:1\n");}
                if (event.key.keysym.sym == SDLK_v) {init_grid();printf("v - CA states zeroed\n");}


                if (event.key.keysym.sym == SDLK_b) {
                    // Start batch simulation in a separate thread in real application
                    printf("Batch mode not supported in interactive session. Use command line.\n");
                }
            }
        }        
        if(!paused_CA){
        //CA
            // update rules chaosregular(x)2025-08-14_20:48:15
            // chaosregular(x)2025-08-15_15:04:04 'poprawki'
            for (int i = 0; i < particle_count; i++) {
                uint8_t pr,pg,pb; // particle physical location translated to rgb rules space
                uint8_t mr,mg,mb; // particle physical location translated to rgb rules space for modification source
                int gx,gy; // pozycja w GRID z x,y,z  -> gridx,gridy
                double dx,dy,dz; //(~pozycja w RGB  x,y,z -> R,G,B)
                double dvx,dvy,dvz; // zmiany prędkości 
                double mod_R,mod_G,mod_B; // zmiany rules

                // 'physical' particle space location to RGB rules space ~ proportional
                // 'normalized' to blob radius
                dx = particles[i].x/blob_r;
                dy = particles[i].y/blob_r;
                dz = particles[i].z/blob_r;
                // rgb rules space representation
                //pr = (uint8_t)(127.5+127.0*dx);
                pg = (uint8_t)(127.5+127.0*dy);
                pb = (uint8_t)(127.5+127.0*dz);

                // Seek: Replace:
                // dx = particles[i].x / blob_r;
                // pr = (uint8_t)(127.5 + 127.0 * dx);
                // With:
                double energy = sqrt(particles[i].vx*particles[i].vx + particles[i].vy*particles[i].vy + particles[i].vz*particles[i].vz);
                pr = (uint8_t)fmin(255, energy * 10.0); // Scale factor adjustable

                
                mr = (uint8_t)(127-pr);
                mg = (uint8_t)(127-pg);
                mb = (uint8_t)(127-pb);

                gx = (int)((mult_xyx_2_gridxy*dx*GRID_WIDTH)+(GRID_WIDTH/2));
                gy = (int)((mult_xyx_2_gridxy*dy*GRID_WIDTH)+(GRID_WIDTH/2));
                if(gx<0)gx=0;
                if(gy<0)gy=0;
                if(gx>(GRID_WIDTH-1))gx=GRID_WIDTH-1;
                if(gy>(GRID_WIDTH-1))gy=GRID_WIDTH-1;

                dvx = (127.5-grid[current][gx][gy].R)*SPEED_FEED;
                dvy = (127.5-grid[current][gx][gy].G)*SPEED_FEED;


                // grid[current][gx][gy].R = rules[pr][pg][pb].R;
                // grid[current][gx][gy].G = rules[pr][pg][pb].G;
                // grid[current][gx][gy].B = rules[pr][pg][pb].B;


                // grid[current][gx][gy].R = particles[i].r;
                // grid[current][gx][gy].G = particles[i].g;
                // grid[current][gx][gy].B = particles[i].b;

                grid[current][gx][gy].R = rules[particles[i].r][particles[i].g][particles[i].b].R;
                grid[current][gx][gy].G = rules[particles[i].r][particles[i].g][particles[i].b].G;
                grid[current][gx][gy].B = rules[particles[i].r][particles[i].g][particles[i].b].B;




                    // chaosregular(x)2025-08-18_16:57:24 wersja niżej była
                    // mod_R = (2.0*(double)rand()/RAND_MAX - 0.50 + particles[i].vx*speed_boost)+rules[pr][pg][pb].R;
                    // mod_G = (2.0*(double)rand()/RAND_MAX - 0.50 + particles[i].vy*speed_boost)+rules[pr][pg][pb].G;
                    // mod_B = (2.0*(double)rand()/RAND_MAX - 0.50 + particles[i].vz*speed_boost)+rules[pr][pg][pb].B;

                    // zmienione tymczasowo
                    // mod_R = (2.0*(double)rand()/RAND_MAX - 0.50)*speed_boost+rules[pr][pg][pb].R;
                    // mod_G = (2.0*(double)rand()/RAND_MAX - 0.50)*speed_boost+rules[pr][pg][pb].G;
                    // mod_B = (2.0*(double)rand()/RAND_MAX - 0.50)*speed_boost+rules[pr][pg][pb].B;

                    // chaosregular(x)2025-08-19_17:10:10 mr,mg,mb
                    // mod_R = (2.0*(double)rand()/RAND_MAX - 0.50)*speed_boost+rules[pr][pg][pb].R;
                    // mod_G = (2.0*(double)rand()/RAND_MAX - 0.50)*speed_boost+rules[pr][pg][pb].G;
                    // mod_B = (2.0*(double)rand()/RAND_MAX - 0.50)*speed_boost+rules[pr][pg][pb].B;

                    // chaosregular(x)2025-08-24_19:02:13
                    mod_R = particles[i].vx*speed_boost+rules[pr][pg][pb].R;
                    mod_G = particles[i].vy*speed_boost+rules[pr][pg][pb].G;
                    mod_B = particles[i].vz*speed_boost+rules[pr][pg][pb].B;

                    if(mod_R>255)mod_R=255.0;
                    if(mod_G>255)mod_G=255.0;
                    if(mod_B>255)mod_B=255.0;
                    if(mod_R<0)mod_R=0;//rules[x][y][z].R;
                    if(mod_G<0)mod_G=0;//rules[x][y][z].G;
                    if(mod_B<0)mod_B=0;//rules[x][y][z].B;
                    rules[mr][mg][mb].R = (uint8_t)mod_R;
                    rules[mr][mg][mb].G = (uint8_t)mod_G;
                    rules[mr][mg][mb].B = (uint8_t)mod_B;

                particles[i].vx += dvx;
                particles[i].vy += dvy;
            }
            // update rules chaosregular(x)2025-08-14_20:48:15

            update_simulation();
            step_d = (step++)%100;
            if(step_d==0)printf("step %i\n",step-1);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            
            render_grid(renderer);
            
            SDL_RenderPresent(renderer);
        }

        //nP
        update_physics();
        total_simulation_time += time_step;
        step_count++;
        
        update_title(window_nP);
        render_particles(renderer_nP);

        // Frame rate control
        // Uint32 frame_time = SDL_GetTicks() - frame_start;
        // if (frame_time < frame_delay) {
        //     SDL_Delay(frame_delay - frame_time);
        // }
        SDL_Delay(1);        
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer_nP);
    SDL_DestroyWindow(window_nP);
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
        
        unsigned char alpha_z = 10 + 240/(1000 * depth_scale);
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
        tot_energy_est = calculate_total_energy();
        char title[256];
        snprintf(title, sizeof(title), 
            "3D Force-Flip Universe | Scale: %.2f | dt: %.6f | Part: %d | Steps: %d | Time: %.2fs | iN %i | E ~ %f | R=%f",
            view_scale, time_step, particle_count, step_count, total_simulation_time, setn, tot_energy_est, blob_r);
        
        SDL_SetWindowTitle(window, title);
        last_title_update = SDL_GetTicks();
    }
}


void compute_center_of_mass(int particle_count, Particle* particles, double* cm_x, double* cm_y, double* cm_z) {
    double total_mass = 0.0;
    *cm_x = 0.0; *cm_y = 0.0; *cm_z = 0.0;
    for (int i = 0; i < particle_count; i++) {
        total_mass += particles[i].mass;
        *cm_x += particles[i].mass * particles[i].x;
        *cm_y += particles[i].mass * particles[i].y;
        *cm_z += particles[i].mass * particles[i].z;
    }
    if (total_mass > 0) {
        *cm_x /= total_mass;
        *cm_y /= total_mass;
        *cm_z /= total_mass;
    }
}

double move_to_center_of_mass(int particle_count, Particle* particles)
{
    double r_max = -0.0;
    double r;
    double total_mass = 0.0;
    double cm_x = 0.0; double cm_y = 0.0; double  cm_z = 0.0;
    for (int i = 0; i < particle_count; i++) {
        total_mass += particles[i].mass;
        cm_x += particles[i].mass * particles[i].x;
        cm_y += particles[i].mass * particles[i].y;
        cm_z += particles[i].mass * particles[i].z;
    }
    if (total_mass > 0) {
        cm_x /= total_mass;
        cm_y /= total_mass;
        cm_z /= total_mass;
        for (int i = 0; i < particle_count; i++) {
            total_mass += particles[i].mass;
            particles[i].x -= cm_x;
            particles[i].y -= cm_y;
            particles[i].z -= cm_z;
            r = sqrt(particles[i].x*particles[i].x+particles[i].y*particles[i].y+particles[i].z*particles[i].z);
            if(r>r_max)r_max=r;
        }
    }
    return r_max;
}
