#include "subsystems/logger.h"
#include "subsystems/platform.h"
#include "subsystems/renderer.h"
#include <string.h>

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

    CV_renderer_init(&state);

    while(state.running)
    {
        (void)CV_process_events(&state);
        CV_renderer_update(&state);
        CV_SDL_SwapBuffer(&state);
    }

    CV_shutdown(&state);
}
