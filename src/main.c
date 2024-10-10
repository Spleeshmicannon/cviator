#include "subsystems/logger.h"
#include "subsystems/platform.h"
#include "subsystems/renderer.h"
#include "subsystems/gpu_compute.h"

#include <string.h>
#include <time.h>

#define BUFFER_ATTRIB_COUNT 8 // 4 + 3 + 1
#define BUFFER_ITEM_COUNT 50000
#define BUFFER_TOTAL_SIZE sizeof(float) * BUFFER_ATTRIB_COUNT * BUFFER_ITEM_COUNT

int main()
{
    CV_state state;
    memset(&state, 0, sizeof(CV_state));

    state.dims.x = 1920;
    state.dims.y = 1080;
    state.pos.x = SDL_WINDOWPOS_CENTERED;
    state.pos.y = SDL_WINDOWPOS_CENTERED;
    state.running = true;

    strcpy(state.name, "CVIATOR");

    if(!CV_init_video(&state))
    {
        CV_LOG_FATAL("Failed to initialise CVIATOR");
        return -1;
    }

    if(!CV_renderer_init(&state))
    {
        CV_LOG_FATAL("Failed to setup renderer");
        CV_shutdown(&state);
        return -2;
    }

    if(!CV_gpu_compute_init(&state))
    {
        CV_LOG_FATAL("Failed to setup compute shader pipeline");
        CV_renderer_shutdown(&state);
        CV_shutdown(&state);
        return -3;
    }

    state.vertices = malloc(BUFFER_TOTAL_SIZE);
    state.buffer_size = BUFFER_TOTAL_SIZE;
    state.attrib_stride = BUFFER_ATTRIB_COUNT;
    state.count = BUFFER_ITEM_COUNT;

    memset(state.vertices, 0, BUFFER_TOTAL_SIZE);

    for(int i = 0; i < BUFFER_ITEM_COUNT; ++i)
    {
        float *vertices_float = state.vertices;

        // setting x,y
        vertices_float[(i * BUFFER_ATTRIB_COUNT) + 0] = 
            (((float)rand() / (float)RAND_MAX) * 2.0f) - 1.0f;

        vertices_float[(i * BUFFER_ATTRIB_COUNT) + 1] = 
            (((float)rand() / (float)RAND_MAX) * 2.0f) - 1.0f;
        
        // setting mass
        vertices_float[(i * BUFFER_ATTRIB_COUNT) + 7] = 
            (((float)rand() / (float)RAND_MAX) * 0.005f) + 0.0025f;
    }

    CV_gpu_compute_setup_buffer(&state);
    CV_renderer_set_particle_count(&state);
    
    while(state.running)
    {
        struct timespec start, end;

        clock_gettime(CLOCK_MONOTONIC, &start);

        (void)CV_process_events(&state);
        CV_dispatch(&state);
        CV_pass_ssbo_to_vbo(&state);
        CV_renderer_update(&state);
        CV_SwapBuffer(&state);
        
        clock_gettime(CLOCK_MONOTONIC, &end);

        double fps = 1 / (((double)end.tv_sec + 1.0e-9*end.tv_nsec) - 
           ((double)start.tv_sec + 1.0e-9*start.tv_nsec));

        snprintf(state.name, 255, "CVIATOR - FPS: %f", fps);
        CV_updateWindowTitle(&state);
    }

    CV_gpu_compute_shutdown(&state);
    CV_renderer_shutdown(&state);
    CV_shutdown(&state);

    free(state.vertices);

    return 0;
}
