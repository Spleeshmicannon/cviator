#include "subsystems/logger.h"
#include "subsystems/platform.h"
#include "subsystems/renderer.h"
#include <string.h>

#define BUFFER_ATTRIB_COUNT 2
#define BUFFER_ITEM_COUNT 20000
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

    if(CV_renderer_init(&state))
    {
        CV_LOG_FATAL("Failed to setup renderer");
        CV_shutdown(&state);
        return -2;
    }

    state.vertices = malloc(BUFFER_TOTAL_SIZE);
    state.buffer_size = BUFFER_TOTAL_SIZE;
    state.attrib_stride = BUFFER_ATTRIB_COUNT;

    while(state.running)
    {
        (void)CV_process_events(&state);
        CV_renderer_update(&state);
        CV_SwapBuffer(&state);
    }

    CV_renderer_shutdown(&state);
    CV_shutdown(&state);

    free(state.vertices);

    return 0;
}
