while (running) {
    // 1. Update world
    world_diffuse_resources(world, width, height, diffusion_rate);
    world_update_gradients(world, width, height);
    
    // 2. Update beings
    for (each being) {
        being_sense_needs(being, world, width, height);
        being_update_internal_particles(being);
        
        double dx, dy;
        being_decide_movement(being, world, &dx, &dy);
        being->world_x += dx;
        being->world_y += dy;
        
        being_update_vitality(being, world);
        being_adapt_to_environment(being, world);
    }
    
    // 3. Handle reproduction
    check_reproduction(beings, count);
    
    // 4. Remove dead beings
    remove_low_vitality_beings(beings, &count);
    
    // 5. Visualize
    ui_draw_frame(renderer, world, beings, count);
}

