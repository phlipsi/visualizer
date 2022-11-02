#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <memory>
#include <algorithm>

#include <scene.vert.h>
#include <scene.frag.h>
#include <screen.vert.h>
#include <screen.frag.h>
#include <blur.vert.h>
#include <blur.frag.h>

#include <shader.h>
#include <program.h>
#include <quad.h>

#include "batch.h"
#include "parameter.h"
#include "ring.h"
#include "scene.h"
#include "shape.h"
#include "transform.h"
#include "postprocessing.h"


extern "C" {

_declspec(dllexport) uint32_t NvOptimusEnablement = 0x00000001;

}

float sawtooth(float t) {
    float scale = 2.0f / static_cast<float>(M_PI);
    float sum = sinf(static_cast<float>(M_PI) * t);
    sum -= sinf(2 * static_cast<float>(M_PI) * t) / 2.0f;
    sum += sinf(3 * static_cast<float>(M_PI) * t) / 3.0f;
    sum -= sinf(4 * static_cast<float>(M_PI) * t) / 4.0f;
    return scale * sum;
}

void logger(GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar *message,
            const void *userParam) {
    std::cout << message << std::endl;
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

    std::cout << "Messages:" << std::endl;
    glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    glDebugMessageCallback(logger, nullptr);

    Program scene_shader;
    scene_shader.attach(Shader(GL_VERTEX_SHADER, scene_vertex_shader));
    scene_shader.attach(Shader(GL_FRAGMENT_SHADER, scene_fragment_shader));
    scene_shader.bind(visualizer::Batch::ATTRIBUTE_POSITION, "position");
    scene_shader.bind(visualizer::Batch::ATTRIBUTE_COLOR, "color");
    scene_shader.bind(visualizer::Batch::ATTRIBUTE_GLOW, "glow");
    scene_shader.link();
    {
        auto usage = scene_shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
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
    screen_shader.bind(Quad::ATTRIBUTE_POSITION, "position");
    screen_shader.bind(Quad::ATTRIBUTE_TEXTURE_COORD, "texture_coord");
    screen_shader.link();
    {
        auto usage = screen_shader.use();

        glm::mat2 projection = glm::mat2(
            static_cast<float>(size) / width, 0.0f,
            0.0f,                             static_cast<float>(size) / height
        );
        usage.set_uniform("projection", projection);
    }

    Program blur_shader;
    blur_shader.attach(Shader(GL_VERTEX_SHADER, blur_vertex_shader));
    blur_shader.attach(Shader(GL_FRAGMENT_SHADER, blur_fragment_shader));
    blur_shader.bind(Quad::ATTRIBUTE_POSITION, "position");
    blur_shader.bind(Quad::ATTRIBUTE_TEXTURE_COORD, "texture_coord");
    blur_shader.link();

    visualizer::Scene scene(size, size);

    visualizer::PostProcessing postprocessing1(size, size);
    visualizer::PostProcessing postprocessing2(size, size);

    Quad quad;


    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glViewport(0, 0, width, height);
    visualizer::Parameters parameters;
    parameters.add_parameter("ring.triangle.angle", [](long ms) { return glm::radians(180.0f * sawtooth(ms / 4000.0f)); });
    parameters.add_parameter("ring.triangle.width", [](long ms) { return sinf(2 * static_cast<float>(M_PI) * ms / (3 * 2000.0f / 4)) / 6.0f + 1.0f; });
    parameters.add_parameter("ring.triangle.height", [](long ms) { return -sinf(2 * static_cast<float>(M_PI) * ms / (3 * 2000.0f / 4)) / 6.0f + 1.0f; });
    parameters.add_parameter("ring.triangle.glow", [](long ms) { return 0.5f * sinf(2 * static_cast<float>(M_PI) * ms / 2000.0f) + 0.5f; });
    parameters.add_parameter("ring.rectangle.angle", [](long ms) { return glm::radians(180.0f * sawtooth(ms / 3000.0f)); });
    parameters.add_parameter("ring.rectangle.width", [](long ms) { return sinf(2 * static_cast<float>(M_PI) * ms / (2 * 2000.0f / 3)) / 6.0f + 1.0f; });
    parameters.add_parameter("ring.rectangle.height", [](long ms) { return -sinf(2 * static_cast<float>(M_PI) * ms / (2 * 2000.0f / 3)) / 6.0f + 1.0f; });
    parameters.add_parameter("ring.rectangle.glow", [](long ms) { return -0.5f * sinf(2 * static_cast<float>(M_PI) * ms / 2000.0f) + 0.5f; });
    parameters.add_parameter("ring.angle", [] (long ms) { return 0.0f; });
    //parameters.add_parameter("ring.angle", [] (long ms) { return glm::radians(90.0f * sinf(2 * static_cast<float>(M_PI) * ms / 5000.0f)); }); // glm::radians(180.0f * sawtooth(ms / 5000.0f));
    //parameters.add_parameter("ring.angle", [] (long ms) { return glm::radians(180.0f * sawtooth(ms / 5000.0f)); });
    //float angle1 = 0.0f;
    //float angle2 = 0.0f;
    float scale = 0.3f;
    auto ring = std::make_shared<visualizer::Ring>(6, 1.0f, std::initializer_list<std::shared_ptr<visualizer::Object>>{
        std::make_shared<visualizer::Rotate>(std::make_shared<visualizer::Scale>(std::make_shared<visualizer::Deform>(std::make_shared<visualizer::Triangle>(glm::vec3(1.0f, 1.0f, 0.0f), parameters.get_parameter("ring.triangle.glow")), parameters.get_parameter("ring.triangle.width"), parameters.get_parameter("ring.triangle.height")), scale), parameters.get_parameter("ring.triangle.angle")),
        std::make_shared<visualizer::Rotate>(std::make_shared<visualizer::Scale>(std::make_shared<visualizer::Deform>(std::make_shared<visualizer::Rectangle>(glm::vec3(1.0f, 0.0f, 0.0f), parameters.get_parameter("ring.rectangle.glow")), parameters.get_parameter("ring.rectangle.width"), parameters.get_parameter("ring.rectangle.height")), scale), parameters.get_parameter("ring.rectangle.angle")) });
    auto rotating_ring = std::make_shared<visualizer::Rotate>(ring, parameters.get_parameter("ring.angle"));
    visualizer::Batch batch;

    const glm::mat4 model{glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f))};

    const float freq = static_cast<float>(SDL_GetPerformanceFrequency());
    Uint32 old_ticks;
    Uint32 ticks = SDL_GetTicks();
    const int max_cool_down = 10;
    int cool_down = max_cool_down;
    bool quit = false;
    while (!quit) {
        old_ticks = ticks;
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
            auto binding = scene.bind_as_target();
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            const Uint64 counter = SDL_GetPerformanceCounter();
            //const Uint64 ticks = SDL_GetTicks64();
            auto usage = scene_shader.use();
            //parameters.set_time(ticks);
            parameters.set_time(1000.0f * static_cast<float>(counter) / freq);
            batch.clear();
            rotating_ring->draw(batch, model);
            batch.draw();
        }
        glDisable(GL_DEPTH_TEST);
        {
            auto usage = blur_shader.use();
            usage.set_uniform("horizontal", false);
            auto colors = scene.bind_glow_as_source(GL_TEXTURE0);
            auto target = postprocessing1.bind_as_target();

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            quad.draw();
        }
        {
            auto usage = blur_shader.use();
            usage.set_uniform("horizontal", true);
            auto colors = postprocessing1.bind_as_source(GL_TEXTURE0);
            auto target = postprocessing2.bind_as_target();

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            quad.draw();
        }
        for (int i = 0; i < 5; ++i) {
            {
                auto usage = blur_shader.use();
                usage.set_uniform("horizontal", false);
                auto colors = postprocessing2.bind_as_source(GL_TEXTURE0);
                auto target = postprocessing1.bind_as_target();

                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                quad.draw();
            }
            {
                auto usage = blur_shader.use();
                usage.set_uniform("horizontal", true);
                auto colors = postprocessing1.bind_as_source(GL_TEXTURE0);
                auto target = postprocessing2.bind_as_target();

                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                quad.draw();
            }
        }
        {
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT);

            auto usage = screen_shader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, scene.get_color_buffer_id());
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, postprocessing2.get_texture_id());
            usage.set_uniform("screen_texture", 0);
            usage.set_uniform("glow_texture", 1);
            //auto colors = scene.bind_colors_as_source(GL_TEXTURE0);
            //auto glow = postprocessing2.bind_as_source(GL_TEXTURE1);

            quad.draw();
        }
        SDL_GL_SwapWindow(window);

        if (cool_down == 0) {
            ticks = SDL_GetTicks();
            const std::string fps = std::to_string(max_cool_down * 1000.0 / static_cast<double>(ticks - old_ticks));
            SDL_SetWindowTitle(window, fps.c_str());
            cool_down = max_cool_down;
        } else {
            --cool_down;
        }
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
