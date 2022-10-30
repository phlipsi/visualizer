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
#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <string.h>

#include <vertex.h>
#include <fragment.h>

#include <screen_vertex.h>
#include <screen_fragment.h>

#include <destination.h>
#include <framebuffer.h>
#include <shader.h>
#include <program.h>
#include <renderbuffer.h>
#include <texture.h>
#include <vertexarray.h>
#include <buffer.h>

#include "cube.h"
#include "destination.h"

const int ATTRIBUTE_POSITION = 0;
const int ATTRIBUTE_NORMAL = 1;
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
    program.bind(Cube::ATTRIBUTE_POSITION, "i_position");
    program.bind(Cube::ATTRIBUTE_NORMAL, "i_normal");
    program.bind(Cube::ATTRIBUTE_TEXTURE_COORD, "i_texture_coord");
    program.link();
    Cube cube;

    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, width, height);
    //glViewport(-1, -1, 1, 1);

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

    Shader screen_vertex_shader(GL_VERTEX_SHADER, screen_vertex);
    Shader screen_fragment_shader(GL_FRAGMENT_SHADER, screen_fragment);
    Program screen_program;
    screen_program.attach(std::move(screen_vertex_shader));
    screen_program.attach(std::move(screen_fragment_shader));
    screen_program.bind(Destination::ATTRIBUTE_POSITION, "i_position");
    screen_program.bind(Destination::ATTRIBUTE_TEXTURE_COORD, "i_texture_coord");
    screen_program.link();
    {
        auto usage = screen_program.use();

        glm::mat2 projection = glm::mat2(
            1.0f, 0.0f,
            0.0f, static_cast<float>(width) / height
        );
        usage.set_uniform("projection", projection);
        usage.set_uniform("alpha", glm::radians(30.0f));
    }
    Destination destination(width, height);
    {
        auto usage = program.use();
        glm::mat4 projection2 = glm::perspective(glm::radians(45.0f), 1.0f/*static_cast<float>(width) / static_cast<float>(height)*/, 0.1f, 100.0f);
        usage.set_uniform("projection", projection2);

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
        usage.set_uniform("view", view);

        glm::vec3 light{ 0.0f, 0.0f, 0.0f };
        usage.set_uniform("light", light);
    }

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    bool quit = false;
    while (!quit) {
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

        int x, y;
        SDL_GetMouseState(&x, &y);
        {
            auto binding = destination.bind_as_target();
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto usage = program.use();
            glm::mat4 model = glm::mat4(1.0);
            model = glm::translate(model, glm::vec3(1.0, 0, -5));
            model = glm::rotate(model, glm::radians(360.0f * (static_cast<float>(x) / width - 0.5f)), glm::vec3(0, 1, 0));
            model = glm::rotate(model, glm::radians(360.0f * (static_cast<float>(y) / height - 0.5f)), glm::vec3(1, 0, 0));
            usage.set_uniform("model", model);
            glActiveTexture(GL_TEXTURE0);
            auto texture_binding = texture.bind(GL_TEXTURE_2D);
            cube.draw();
        }
        {
            glDisable(GL_DEPTH_TEST);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            auto usage = screen_program.use();
            const long ticks = SDL_GetTicks();
            const float gamma = 2 * static_cast<float>(M_PI) * ticks / 10000.0f;
            usage.set_uniform("gamma", gamma);
            destination.draw();
        }

        SDL_GL_SwapWindow(window);
        SDL_Delay(1);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
