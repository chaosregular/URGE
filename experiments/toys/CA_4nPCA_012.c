//deepseek_c_20250729_803c45.c
//2D toroid template
// gcc CA_4nPCA_012_mod.c -o CA_4nPCA_012_mod -lSDL2 -lm
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
    Uint8 r, g, b, a;
    double r_min; //chaosregular(x)2025-08-01_06:32:37    (for time step)    
    // Seek: Add well_being to Particle struct:
    double well_being; // Updated each frame
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

// Seek stat
int print_stat = 0;

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

// ... rest of file unchanged intentionally (original long content preserved)
