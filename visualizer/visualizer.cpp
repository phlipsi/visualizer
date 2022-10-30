#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <memory>

#include <scene.vert.h>
#include <scene.frag.h>
#include <screen.vert.h>
#include <screen.frag.h>

#include <destination.h>
#include <shader.h>
#include <program.h>

#include "batch.h"
#include "parameter.h"
#include "ring.h"
#include "shape.h"
#include "transform.h"

float sawtooth(float t) {
    float scale = 2.0f / static_cast<float>(M_PI);
    float sum = sinf(static_cast<float>(M_PI) * t);
    sum -= sinf(2 * static_cast<float>(M_PI) * t) / 2.0;
    sum += sinf(3 * static_cast<float>(M_PI) * t) / 3.0;
    sum -= sinf(4 * static_cast<float>(M_PI) * t) / 4.0;
    return scale * sum;
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

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
    static const int size = std::max(width, height);

    SDL_Window * window = SDL_CreateWindow("visualizer",
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           width,
                                           height,
                                           SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return EXIT_FAILURE;
    }

    Program scene_shader;
    scene_shader.attach(Shader(GL_VERTEX_SHADER, scene_vertex_shader));
    scene_shader.attach(Shader(GL_FRAGMENT_SHADER, scene_fragment_shader));
    scene_shader.bind(visualizer::Batch::ATTRIBUTE_POSITION, "position");
    scene_shader.bind(visualizer::Batch::ATTRIBUTE_COLOR, "color");
    scene_shader.bind(visualizer::Batch::ATTRIBUTE_GLOW, "glow");
    scene_shader.link();
    {
        auto usage = scene_shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / height /*1.0f*/, 0.1f, 100.0f);
        usage.set_uniform("projection", projection);

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
        usage.set_uniform("view", view);

        glm::vec3 light{ 0.0f, 0.0f, 0.0f };
        usage.set_uniform("light", light);
    }

    Program screen_shader;
    screen_shader.attach(Shader(GL_VERTEX_SHADER, screen_vertex_shader));
    screen_shader.attach(Shader(GL_FRAGMENT_SHADER, screen_fragment_shader));
    screen_shader.bind(Destination::ATTRIBUTE_POSITION, "position");
    screen_shader.bind(Destination::ATTRIBUTE_TEXTURE_COORD, "texture_coord");
    screen_shader.link();
    {
        auto usage = screen_shader.use();

        glm::mat2 projection = glm::mat2(
            1.0f, 0.0f,
            0.0f, 1.0f
            //0.0f, static_cast<float>(width) / height
        );
        usage.set_uniform("projection", projection);
    }
    Destination destination(width, height);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glViewport(0, 0, width, height);
    visualizer::Parameters parameters;
    parameters.add_parameter("ring.triangle.angle", [](long ms) { return glm::radians(180.0f * sawtooth(ms / 4000.0f)); });
    parameters.add_parameter("ring.triangle.width", [](long ms) { return sinf(2 * static_cast<float>(M_PI) * ms / (3 * 2000.0f / 4)) / 6.0f + 1.0f; });
    parameters.add_parameter("ring.triangle.height", [](long ms) { return -sinf(2 * static_cast<float>(M_PI) * ms / (3 * 2000.0f / 4)) / 6.0f + 1.0f; });
    parameters.add_parameter("ring.rectangle.angle", [](long ms) { return glm::radians(180.0f * sawtooth(ms / 3000.0f)); });
    parameters.add_parameter("ring.rectangle.width", [](long ms) { return sinf(2 * static_cast<float>(M_PI) * ms / (2 * 2000.0f / 3)) / 6.0f + 1.0f; });
    parameters.add_parameter("ring.rectangle.height", [](long ms) { return -sinf(2 * static_cast<float>(M_PI) * ms / (2 * 2000.0f / 3)) / 6.0f + 1.0f; });
    parameters.add_parameter("ring.angle", [](long ms) { return glm::radians(180.0f * sawtooth(ms / 5000.0f)); });
    //float angle1 = 0.0f;
    //float angle2 = 0.0f;
    float scale = 0.3f;
    auto ring = std::make_shared<visualizer::Ring>(6, 1.0f, std::initializer_list<std::shared_ptr<visualizer::Object>>{
        std::make_shared<visualizer::Rotate>(std::make_shared<visualizer::Scale>(std::make_shared<visualizer::Deform>(std::make_shared<visualizer::Triangle>(glm::vec3(1.0f, 1.0f, 0.0f), 0.0f), parameters.get_parameter("ring.triangle.width"), parameters.get_parameter("ring.triangle.height")), scale), parameters.get_parameter("ring.triangle.angle")),
        std::make_shared<visualizer::Rotate>(std::make_shared<visualizer::Scale>(std::make_shared<visualizer::Deform>(std::make_shared<visualizer::Rectangle>(glm::vec3(1.0f, 0.0f, 0.0f), 0.0f), parameters.get_parameter("ring.rectangle.width"), parameters.get_parameter("ring.rectangle.height")), scale), parameters.get_parameter("ring.rectangle.angle")) });
    auto rotating_ring = std::make_shared<visualizer::Rotate>(ring, parameters.get_parameter("ring.angle"));
    visualizer::Batch batch;

    const glm::mat4 model{glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f))};

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

        {
            auto binding = destination.bind_as_target();
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            const long ticks = SDL_GetTicks();
            auto usage = scene_shader.use();
            parameters.set_time(ticks);
            batch.clear();
            rotating_ring->draw(batch, model);
            batch.draw();
        }
        {
            glDisable(GL_DEPTH_TEST);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glViewport(0.0f, 0.0f, width, height);
            glClear(GL_COLOR_BUFFER_BIT);

            auto usage = screen_shader.use();
            destination.draw();
        }
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
