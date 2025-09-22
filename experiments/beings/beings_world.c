void world_init(WorldCell** world, int w, int h);
void world_update_gradients(WorldCell** world, int w, int h);
void world_diffuse_resources(WorldCell** world, int w, int h, double rate);
double world_get_need_level(WorldCell** world, int x, int y, WorldNeed need);
