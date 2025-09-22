void being_sense_needs(Being* being, WorldCell** world, int w, int h);
void being_decide_movement(Being* being, WorldCell** world, double* dx, double* dy);
void being_update_vitality(Being* being, WorldCell** world);
int being_should_reproduce(Being* a, Being* b);
Being* being_reproduce(Being* parent1, Being* parent2, int new_id);
