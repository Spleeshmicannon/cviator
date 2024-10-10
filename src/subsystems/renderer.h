#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <stdio.h>

#include "logger.h"
#include "state.h"

#include <cglm/cglm.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
    glBindTexture(GL_TEXTURE_2D, state->texture);

    // texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    uint8_t * data = stbi_load("./assets/circle.png", &width, &height, &nrChannels, STBI_rgb_alpha);

    // loading texture
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );

    stbi_image_free(data);

    glGenerateMipmap(GL_TEXTURE_2D);

    // other settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set clear colour
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    CV_LOG_INFO("Setup the OpenGL basics");

    if(!CV_setup_shaders_(state)) 
    {
        return false;
    }

    GLuint uni_id = glGetUniformLocation(state->program, "texture_data");
    glUniform1i(uni_id, 0);

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
        state->attrib_stride * sizeof(float), 
        (void*)0
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1, // location = 1
        3, // vec3?
        GL_FLOAT, // float
        GL_FALSE,  
        state->attrib_stride * sizeof(float), 
        (void*)(4 * sizeof(float))
    );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        2, // location = 2
        1, // float?
        GL_FLOAT, // float
        GL_FALSE,  
        state->attrib_stride * sizeof(float), 
        (void*)(7 * sizeof(float))
    );
    glEnableVertexAttribArray(2);
}

void CV_renderer_update(CV_state * state)
{
    glUseProgram(state->program);
    CV_renderer_set_vao(state);
    glBindBuffer(GL_ARRAY_BUFFER, state->vbo);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state->texture);

    glClear(GL_COLOR_BUFFER_BIT);
    
    glDrawArrays(GL_POINTS, 0, state->count);

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
    GLuint geometry_shader;

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

    buffer = CV_read_file_("./assets/geometry.glsl", &size);

    CV_LOG_INFO("Compiling geometry shader");
    return_value = CV_compile_shader_(buffer, size, &geometry_shader, GL_GEOMETRY_SHADER);
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
    glAttachShader(state->program, geometry_shader);

    glLinkProgram(state->program);

    GLint isLinked = 0;
    glGetProgramiv(state->program, GL_LINK_STATUS, (int*)&isLinked);

    if(isLinked == GL_FALSE)
    {
        GLint logSize = 0;
        glGetProgramiv(state->program, GL_INFO_LOG_LENGTH, &logSize);

        char* errorLog = (char*)malloc(logSize * sizeof(char));
        glGetProgramInfoLog(state->program, logSize, &logSize, errorLog);

        CV_LOG_ERROR("Failed to link program (OpenGL shaders): %s", errorLog);

        glDeleteProgram(state->program);
        free(errorLog);
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
