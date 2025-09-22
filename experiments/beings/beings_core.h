// Property triad axes
typedef enum {
    AXIS_EFFORT,      // Hardworking (1.0) vs Lazy (-1.0)
    AXIS_SOCIAL,      // Social (1.0) vs Solitary (-1.0)  
    AXIS_CAUTION,     // Cautious (1.0) vs Bold (-1.0)
    AXIS_GENEROSITY,  // Generous (1.0) vs Selfish (-1.0)
    NUM_AXES
} PropertyAxis;

// World needs (Maslow-like)
typedef enum {
    NEED_FOOD,
    NEED_SAFETY, 
    NEED_SOCIAL,
    NEED_MEANING,
    NUM_NEEDS
} WorldNeed;

// Internal particle for property space
typedef struct {
    double position[NUM_AXES];  // Position in property space
    double velocity[NUM_AXES];
    double mass;
    int axis_preference;        // Which axis this particle most affects
} PropertyParticle;

// A being/agent
typedef struct {
    // External state
    double world_x, world_y;
    double vitality;           // Energy/health (0.0-1.0)
    double age;
    
    // Internal state (property space)
    PropertyParticle* particles;
    int num_particles;
    double center_of_mass[NUM_AXES];  // Current property values
    
    // Memory/adaptation
    double need_levels[NUM_NEEDS];
    double satisfaction_history[100];  // Circular buffer
    int history_index;
    
    // Lineage
    int generation;
    int parent_id[2];          // For inheritance
} Being;

// CA World cell
typedef struct {
    double channels[NUM_NEEDS];  // Resource levels
    double gradient_x[NUM_NEEDS]; // Precomputed gradients
    double gradient_y[NUM_NEEDS];
} WorldCell;

// Simulation parameters
typedef struct {
    int world_width, world_height;
    int max_beings;
    double time_step;
    double vitality_decay_rate;
    double reproduction_threshold;
    double mutation_rate;
} SimulationParams;
