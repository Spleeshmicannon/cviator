#ifndef STATE_H
#define STATE_H

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>

#define CV_STRING_SIZE 255

typedef struct 
{
    uint32_t x,y;
} CV_point;

typedef struct 
{
    // user
    char name[255];
    CV_point dims;
    CV_point pos;
    size_t buffer_size, attrib_stride, count;
    void* vertices;

    // flags
    bool running;

    // OpenGL Compute shader
    GLuint compute_shader, compute_program;
    GLuint ssbo;

    // OpenGL
    GLuint vao, vbo, texture, program;
    
    // SDL
    SDL_Window* window_;
    SDL_GLContext glContext_;

} CV_state;

#endif // STATE_H
