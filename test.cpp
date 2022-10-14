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
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <string.h>

#include <vertex.h>
#include <fragment.h>

#include "shader.h"
#include "program.h"
#include "texture.h"

const int ATTRIBUTE_POSITION = 0;
const int ATTRIBUTE_TEXTURE_COORD = 2;

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

    Shader vertex_shader(GL_VERTEX_SHADER, vertex);
    Shader fragment_shader(GL_FRAGMENT_SHADER, fragment);
    Program program;
    program.attach(std::move(vertex_shader));
    program.attach(std::move(fragment_shader));
    program.bind(ATTRIBUTE_POSITION, "i_position");
    program.bind(ATTRIBUTE_TEXTURE_COORD, "i_texture_coord");
    program.link();
    program.use();

    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, width, height);
    //glViewport(-1, -1, 1, 1);

    GLuint vao, vbo, ebo;

    Texture texture;
    {
        auto binding = texture.bind(GL_TEXTURE_2D);
        binding.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        binding.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        binding.set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        binding.set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        unsigned char texture_data[] = { 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00 };
        binding.image_2d(0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glEnableVertexAttribArray(ATTRIBUTE_POSITION);
    glEnableVertexAttribArray(ATTRIBUTE_TEXTURE_COORD);

    glVertexAttribPointer(ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(0 * sizeof(float)));
    glVertexAttribPointer(ATTRIBUTE_TEXTURE_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(3 * sizeof(float)));
    //      (6++-)   (7+++)
    //           *--------*
    //          /|       /|
    //   (4+--)/ |(5+-+)/ |
    //        *--------*  |
    //        |  *-----|--*
    //        | /(2-+-)| /(3-++)
    //        |/       |/
    //        *--------*
    //        (0---)   (1--+)

    GLfloat box_vertices[] = {
         /*  X      Y      Z     S     T  */
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // (0---)
            -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // (1--+)
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // (2-+-)
            -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, // (3-++)

             0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // (4+--)
             0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // (5+-+)
             0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // (6++-)
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, // (7+++)
    };
    unsigned int box_triangles[] = {
        0, 1, 5, // front
        0, 5, 4,

        0, 1, 3, // bottom
        0, 3, 2,

        0, 2, 6, // left
        0, 6, 4,

        1, 3, 7, // right
        1, 7, 5,

        4, 5, 7, // top
        4, 7, 6,

        2, 3, 7, // back
        2, 7, 6
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(box_vertices), box_vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(box_triangles), box_triangles, GL_STATIC_DRAW);

    glm::mat4 projection2 = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
    program.set_uniform("projection", projection2);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    program.set_uniform("view", view);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
        //g_vertex_buffer_data[34] = 2.0f * sinf(ticks / 2000.0f) - 4.0f;
        //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(g_vertex_buffer_data), g_vertex_buffer_data);

        glActiveTexture(GL_TEXTURE0);
        auto binding = texture.bind(GL_TEXTURE_2D);
        glBindVertexArray(vao);

        glm::mat4 model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0, 0, -5));
        model = glm::rotate(model, glm::radians(360.0f * ticks / 3000.0f), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(360.0f * ticks / 2000.0f), glm::vec3(0, 1, 0));
        program.set_uniform("model", model);
        glDrawElements(GL_TRIANGLES, sizeof(box_triangles), GL_UNSIGNED_INT, nullptr);

        SDL_GL_SwapWindow(window);
        SDL_Delay(1);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
