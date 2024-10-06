#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <stdio.h>

#include "logger.h"
#include "state.h"
#include "../circle.h"

bool CV_setup_shaders_(CV_state * state);
bool CV_compile_shader_(const char* data, const int size, GLuint *shader, GLuint type);
char* CV_read_file_(const char* name, long* size);

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

    if(CV_setup_shaders_(state)) 
    {
        return false;
    }

    return true;
}

void CV_renderer_shutdown(CV_state* state)
{
    glDeleteProgram(state->program);
}

void CV_renderer_set_particle_count(CV_state * state)
{
    glBindBuffer(GL_ARRAY_BUFFER, state->vbo);
    glBufferData(GL_ARRAY_BUFFER, state->buffer_size, state->vertices, GL_DYNAMIC_DRAW);
}

void CV_renderer_set_vao(CV_state * state)
{
    glBindVertexArray(state->vao);
    glVertexAttribPointer(
        0, // location = 0
        4, // vec4?
        GL_FLOAT, // float
        GL_FALSE,  
        state->attrib_stride, 
        (void*)0
    );
    glEnableVertexAttribArray(0);
}

void CV_renderer_update(CV_state * state)
{
    CV_renderer_set_vao(state);
    glBindBuffer(GL_ARRAY_BUFFER, state->vbo);

    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

char* CV_read_file_(const char* name, long* size)
{
    FILE* file = fopen(name, "r");

    if(file == NULL)
    {
        CV_LOG_ERROR("Failed to read file with name %s", name);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* buffer = malloc(*size);
    fread(buffer, *size, 1, file);
    fclose(file);
    return buffer;
}

bool CV_setup_shaders_(CV_state * state)
{
    bool return_value = false;
    GLuint fragment_shader;
    GLuint vertex_shader;

    // compiling 
    
    long size;
    char* buffer = CV_read_file_("./assets/fragment.glsl", &size);
    
    CV_LOG_INFO("Compiling fragment shader");
    return_value = CV_compile_shader_(buffer, size, &fragment_shader, GL_FRAGMENT_SHADER);
    free(buffer);

    buffer = CV_read_file_("./assets/vertex.glsl", &size);
    
    CV_LOG_INFO("Compiling vertex shader");
    return_value = CV_compile_shader_(buffer, size, &vertex_shader, GL_VERTEX_SHADER);
    free(buffer);

    if(!return_value)
    {
        CV_LOG_ERROR("Failed to compile shaders, not proceeding with linking");
        return false;
    }

    // linking

    state->program = glCreateProgram();

    glAttachShader(state->program, vertex_shader);
    glAttachShader(state->program, fragment_shader);

    glLinkProgram(state->program);

    GLint isLinked = 0;
    glGetProgramiv(state->program, GL_LINK_STATUS, (int*)&isLinked);

    if(isLinked == GL_FALSE)
    {
        GLint logSize = 0;
        glGetProgramiv(state->program, GL_INFO_LOG_LENGTH, &logSize);

        char* errorLog = (char*)malloc(logSize * sizeof(char));
        glGetProgramInfoLog(state->program, logSize, &logSize, errorLog);

        CV_LOG_ERROR("Failed to link program (OpenGL shaders): ", errorLog);

        glDeleteProgram(state->program);
        return false;
    }

    glUseProgram(state->program);

    return return_value;
}

bool CV_compile_shader_(const char* data, const int size, GLuint *shader, GLuint type)
{
    *shader = glCreateShader(type);

	const char* shaders[1] = { data };
	glShaderSource(*shader, 1, shaders, &size);
	glCompileShader(*shader);

	GLint success = 0;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE)
	{
		GLint logSize = 0;
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logSize);

		char* errorLog = (char*)malloc(logSize * sizeof(char));
		glGetShaderInfoLog(*shader, logSize, &logSize, errorLog);
        
        CV_LOG_ERROR("Failed to compile shader: %s", errorLog);

        free(errorLog);
		glDeleteShader(*shader);

		return false;
	}

    return true;
}

#endif // RENDERER_H
