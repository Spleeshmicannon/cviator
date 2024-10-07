#ifndef GPU_COMPUTE_H
#define GPU_COMPUTE_H

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include "logger.h"
#include "renderer.h"

bool CV_gpu_compute_init(CV_state* state)
{
    long size;
    char* buffer = CV_read_file_("assets/compute.glsl", &size);

    if(!CV_compile_shader_(
            buffer, 
            size,
            &state->compute_shader, 
            GL_COMPUTE_SHADER
        )) 
    {
        CV_LOG_ERROR("Failed to compile compute shader");
        free(buffer);
        return false;
    }

    free(buffer);

    state->compute_program = glCreateProgram();

    glAttachShader(state->compute_program, state->compute_shader);

    glLinkProgram(state->compute_program);

    GLint isLinked = 0;
    glGetProgramiv(state->compute_program, GL_LINK_STATUS, (int*)&isLinked);

    if(isLinked == GL_FALSE)
    {
        GLint logSize = 0;
        glGetProgramiv(state->compute_program, GL_INFO_LOG_LENGTH, &logSize);

        char* errorLog = (char*)alloca(logSize * sizeof(char));
        glGetProgramInfoLog(state->compute_program, logSize, &logSize, errorLog);

        CV_LOG_ERROR("Failed to link compute shader program: %s", errorLog);

        glDeleteProgram(state->compute_program);
    }

    return true;
}

void CV_gpu_compute_setup_buffer(CV_state* state)
{
    glGenBuffers(1, &state->ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, state->ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, state->buffer_size, state->vertices, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, state->ssbo);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, state->vbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, state->vbo);
}

void CV_retrieve_vbo(CV_state*state)
{
    glBindBuffer(GL_ARRAY_BUFFER, state->vbo);
    float* vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    if(vertices == NULL) {
        printf("vbo NULL vertices\n");
        return;
    }
    printf("vbo: %f\n", vertices[0]);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void CV_retrieve_ssbo(CV_state*state)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, state->ssbo);
    float* vertices = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    if(vertices == NULL) {
        printf("ssbo NULL vertices\n");
        return;
    }
    printf("ssbo: %f\n", vertices[0]);
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void CV_dispatch(CV_state*state)
{
    // params -> x,y,z
    glUseProgram(state->compute_program);
    glDispatchCompute(state->count / 1000, 1,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glUseProgram(0); // unbind
}

void CV_pass_ssbo_to_vbo(CV_state*state)
{
    glBindBuffer(GL_COPY_READ_BUFFER, state->ssbo);
    glBindBuffer(GL_COPY_WRITE_BUFFER, state->vbo);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, state->buffer_size);
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void CV_gpu_compute_shutdown(CV_state* state)
{
    glDeleteProgram(state->compute_program);
    glDeleteBuffers(1, &state->ssbo);
}

#endif // GPU_COMPUTE_H
