#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>

#include "logger.h"
#include "state.h"

bool CV_init_video(CV_state *state)
{
    if(SDL_Init(SDL_INIT_VIDEO)) 
    {
        CV_LOG_FATAL("Failed to initialise SDL video! - ", SDL_GetError());
        return false;
    }

    // setting opengl version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    state->window_ = SDL_CreateWindow(
        state->name,
        state->pos.x,
        state->pos.y,
        state->dims.x,
        state->dims.y,
        SDL_WINDOW_OPENGL
    );

    if(state->window_ == NULL) 
    {
        CV_LOG_FATAL("Failed to initialise window! - ", SDL_GetError());
        return false;
    }

    state->glContext_ = SDL_GL_CreateContext(state->window_);
    if(state->glContext_ == NULL)
    {
        CV_LOG_FATAL("Failed to setup opengl context! - ", SDL_GetError());
        return false;
    }

    CV_LOG_INFO("Initialised SDL2 Window");

    return true;
}

SDL_Event CV_process_events(CV_state *state)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT: 
                CV_LOG_TRACE("OS Quit called")
                state->running = false;
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                CV_LOG_TRACE("Key Pressed")
                state->running = !((event.key.keysym.sym == SDLK_ESCAPE) || 
                    (event.key.keysym.sym == SDLK_SPACE));
                break;
        }
    }

    return event;
}

void CV_SwapBuffer(CV_state* state)
{
    SDL_GL_SwapWindow(state->window_);
}

void CV_shutdown(CV_state *state)
{
    SDL_DestroyWindow(state->window_);
    SDL_Quit();

    CV_LOG_TRACE("Exited Successfully");
}

#endif // PLATFORM_H
