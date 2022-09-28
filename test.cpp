/*
    Minimal SDL2 + OpenGL3 example.
    Author: https://github.com/koute
    This file is in the public domain; you can do whatever you want with it.
    In case the concept of public domain doesn't exist in your jurisdiction
    you can also use this code under the terms of Creative Commons CC0 license,
    either version 1.0 or (at your option) any later version; for details see:
        http://creativecommons.org/publicdomain/zero/1.0/
    This software is distributed without any warranty whatsoever.
    Compile and run with: gcc opengl3_hello.c `sdl2-config --libs --cflags` -lGL -Wall && ./a.out
*/

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <stdio.h>
#include <string.h>

#include <vertex.h>
#include <fragment.h>

const int ATTRIBUTE_POSITION = 0;
const int ATTRIBUTE_COLOR = 1;

int main(int argc, char * argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    static const int width = 800;
    static const int height = 600;

    SDL_Window * window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()) {
        return 1;
    }
    GLuint vs, fs, program;

    vs = glCreateShader(GL_VERTEX_SHADER);
    fs = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar *const vertex_shaders[] = { vertex };
    const GLint vertex_shader_lengths[] = { sizeof(vertex) - 1 };
    glShaderSource(vs, 1, vertex_shaders, vertex_shader_lengths);
    glCompileShader(vs);

    GLint status;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE)
    {
        fprintf(stderr, "vertex shader compilation failed\n");
        return 1;
    }

    const GLchar *const fragment_shaders[] = { fragment };
    const GLint fragment_shader_lengths[] = { sizeof(fragment) - 1 };
    glShaderSource(fs, 1, fragment_shaders, fragment_shader_lengths);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE)
    {
        fprintf(stderr, "fragment shader compilation failed\n");
        return 1;
    }

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glBindAttribLocation(program, ATTRIBUTE_POSITION, "i_position");
    glBindAttribLocation(program, ATTRIBUTE_COLOR, "i_color");
    glLinkProgram(program);

    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.0, 0.0, 0.0);
    glViewport(0, 0, width, height);
    //glViewport(-1, -1, 1, 1);

    GLuint vao, vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(ATTRIBUTE_POSITION);
    glEnableVertexAttribArray(ATTRIBUTE_COLOR);

    glVertexAttribPointer(ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);
    glVertexAttribPointer(ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void *)(4 * sizeof(float)));

    GLfloat g_vertex_buffer_data[] = {
    /*  R, G, B, A,     X,     Y,     Z */
        1, 0, 0, 1, -1.0f, -1.0f, 2.0f,
        0, 1, 0, 1,  1.0f, -1.0f, 2.0f,
        0, 0, 1, 1,  0.0f,  1.0f, 2.0f,

        1, 1, 1, 1, -1.0f, -1.0f,  1.5f,
        1, 1, 1, 1,  0.0f, -1.0f,  1.5f,
        1, 1, 1, 1, -1.0f,  1.0f,  1.5f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);

    /*const GLfloat projection[4 * 4] = {
         2.0f / width,  0.0f,           0.0f,          0.0f,
         0.0f,         -2.0f / height,  0.0f,          0.0f,
         0.0f,          0.0f,          -2.0f / 100.0f, 0.0f,
        -1.0f,          1.0f,          -1.0f,          1.0f
    };*/
    const GLfloat f = 10.0f;
    const GLfloat projection[4 * 4] = {
         1.0f, 0.0f,               0.0f, 0.0f,
         0.0f, 1.0f,               0.0f, 0.0f,
         0.0f, 0.0f,  (f + 1) / (f - 1), 1.0f,
         0.0f, 0.0f, -2.0 * f / (f - 1), 0.0f
    };
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, projection);

    bool quit = false;
    while (!quit) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        quit = true;
                    }
                    break;
            }
        }

        const long ticks = SDL_GetTicks();
        g_vertex_buffer_data[34] = 1.0f * sin(ticks / 2000.0) + 1.5f;
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(g_vertex_buffer_data), g_vertex_buffer_data);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        SDL_GL_SwapWindow(window);
        SDL_Delay(1);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
