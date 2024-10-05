#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include "logger.h"
#include "state.h"
#include "circle.h"

bool CV_renderer_init(CV_state * state)
{
    glewExperimental = GL_TRUE; // setting internal glew var
    GLenum glewError = glewInit();
    if(glewError != GLEW_OK)
    {
        CV_LOG_FATAL("Failed to wrangle with glew on the following error: ", glewGetErrorString(glewError));
    }

    glGenVertexArrays(1, &state->vao);
    glGenBuffers(1, &state->vbo);

    glBindVertexArray(state->vao);
    glBindBuffer(GL_ARRAY_BUFFER, state->vbo);

    glGenTextures(1, &state->texture);

    // texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // loading texture
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        BMP_CIRCLE_WIDTH,
        BMP_CIRCLE_HEIGHT,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        BMP_CIRCLE
    );

    // other settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set clear colour
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    CV_LOG_INFO("Setup the OpenGL basics");

    return true;
}

void CV_renderer_update(CV_state * state)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

#endif // RENDERER_H
