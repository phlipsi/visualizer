#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <scene.vert.h>
#include <scene.frag.h>

#include <buffer.h>
#include <shader.h>
#include <program.h>
#include <vertexarray.h>

#include "batch.h"

GLfloat triangle_vertices[] = {
/*   X      Y                   Z     NX    NY    NZ    R     G     B  */
    -0.5f, -sqrtf(3.0f) / 6.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f, -sqrtf(3.0f) / 6.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.0f,  sqrtf(3.0f) / 3.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f
};

class Object {
public:
    virtual ~Object() = default;
    virtual void draw(Batch &batch, const glm::mat4 &model) const = 0;
};

class Triangle : public Object {
public:
    Triangle() : vertices(-0.5f, -sqrtf(3.0f) / 6.0f, 0.0f, 1.0f,
                           0.5f, -sqrtf(3.0f) / 6.0f, 0.0f, 1.0f,
                           0.0f,  sqrtf(3.0f) / 3.0f, 0.0f, 1.0f,
                           0.0f,  0.0f,               0.0f, 1.0f)
    { }

    void draw(Batch &batch, const glm::mat4 &model) const override {
        batch.add_triangle(model * vertices, glm::vec3(1.0f, 1.0f, 0.0), 0.0f);
    }

private:
    glm::mat4 vertices;
};

class Rectangle : public Object {
public:
    Rectangle() : top_left(-0.5f, -0.5f, 0.0f, 1.0f,
                            0.5f,  0.5f, 0.0f, 1.0f,
                           -0.5f,  0.5f, 0.0f, 1.0f,
                            0.0f,  0.0f, 0.0f, 1.0f),
                  bottom_right(-0.5f, -0.5f, 0.0f, 1.0f,
                                0.5f, -0.5f, 0.0f, 1.0f,
                                0.5f,  0.5f, 0.0f, 1.0f,
                                0.0f, 0.0f, 0.0f, 1.0f)
    { }

    void draw(Batch &batch, const glm::mat4 &model) const override {
        batch.add_triangle(model * top_left, glm::vec3(1.0f, 0.0f, 0.0), 0.0f);
        batch.add_triangle(model * bottom_right, glm::vec3(1.0f, 0.0f, 0.0), 0.0f);
    }

private:
    glm::mat4 top_left;
    glm::mat4 bottom_right;
};

class Transform : public Object {
public:
    explicit Transform(std::shared_ptr<Object> object) : object(object) { }

    void draw(Batch &batch, const glm::mat4& model) const override {
        object->draw(batch, model * get_transformation());
    }

private:
    std::shared_ptr<Object> object;

    virtual glm::mat4 get_transformation() const = 0;
};

class Scale : public Transform {
public:
    Scale(std::shared_ptr<Object> object, float factor) : Transform(object), factor(factor) { }

private:
    float factor;

    glm::mat4 get_transformation() const override {
        return glm::scale(glm::mat4{ 1.0f }, glm::vec3(factor, factor, 1.0));
    }
};

class Rotate : public Transform {
public:
    Rotate(std::shared_ptr<Object> object, const float &angle) : Transform(object), angle(&angle) { }

private:
    const float *angle;

    glm::mat4 get_transformation() const override {
        return glm::rotate(glm::mat4{ 1.0f }, glm::radians(*angle), glm::vec3(0.0f, 0.0f, 1.0f));
    }
};

class Ring : public Object {
public:
    Ring(int num_repetitions, float radius, const std::initializer_list<std::shared_ptr<Object>> & objects)
      : num_repetitions(num_repetitions),
        radius(radius),
        objects(objects)
    { }

    void draw(Batch &batch, const glm::mat4 &model) const override {
        const int num_objects = objects.size();
        const float angle = 360.0f / (num_repetitions * num_objects);
        for (int i = 0; i < num_repetitions; ++i) {
            for (int j = 0; j < num_objects; j++) {
                glm::mat4 rotation = model;
                rotation = glm::rotate(rotation, glm::radians(angle * (j + num_objects * i)), glm::vec3(0.0f, 0.0f, 1.0f));
                rotation = glm::translate(rotation, glm::vec3(0.0f, radius, 0.0f));
                objects[j]->draw(batch, rotation);
            }
        }
    }

private:
    int num_repetitions;
    float radius;
    std::vector<std::shared_ptr<Object>> objects;
};

GLfloat rectangle_vertices[] = {
/*   X      Y     Z     NX    NY    NZ    R     G     B  */
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
};


/*class Rectangle {
public:
    Rectangle() {
        auto binding = vao.bind();
        binding.enable_attribute(0);
        binding.enable_attribute(1);
        binding.enable_attribute(2);

        auto buffer_binding = vertices.bind(GL_ARRAY_BUFFER);
        buffer_binding.data(sizeof(rectangle_vertices), rectangle_vertices, GL_STATIC_DRAW);

        buffer_binding.vertex_attrib_pointer(binding, 0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void *)(0 * sizeof(float)));
        buffer_binding.vertex_attrib_pointer(binding, 1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void *)(3 * sizeof(float)));
        buffer_binding.vertex_attrib_pointer(binding, 2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void *)(6 * sizeof(float)));
    }

    void draw() const {
        auto binding = vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, sizeof(rectangle_vertices));
    }

private:
    VertexArray vao;
    Buffer vertices;
};*/

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
    scene_shader.bind(Batch::ATTRIBUTE_POSITION, "position");
    scene_shader.bind(Batch::ATTRIBUTE_COLOR, "color");
    scene_shader.bind(Batch::ATTRIBUTE_GLOW, "glow");
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

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glViewport(0, 0, width, height);
    float angle = 0.0f;
    std::shared_ptr<Object> o1 = std::make_shared<Rotate>(std::make_shared<Scale>(std::make_shared<Triangle>(), 0.3f), angle);
    std::shared_ptr<Object> o2 = std::make_shared<Rotate>(std::make_shared<Scale>(std::make_shared<Rectangle>(), 0.3f), angle);
    auto objects = std::make_shared<Ring>(6, 1.0f, { o1, o2 });
    Ring ring(6, 1.0f, { std::make_shared<Rotate>(std::make_shared<Scale>(std::make_shared<Triangle>(), 0.3f), angle), std::make_shared<Rotate>(std::make_shared<Scale>(std::make_shared<Rectangle>(), 0.3f), angle) });
    //ring.add_object(std::make_shared<Rotate>(std::make_shared<Scale>(std::make_shared<Triangle>(), 0.3f), angle));
    //ring.add_object(std::make_shared<Rotate>(std::make_shared<Scale>(std::make_shared<Rectangle>(), 0.3f), angle));
    //Triangle triangle;
    Batch batch;
    //Rectangle rectangle;

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
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            const long ticks = SDL_GetTicks();
            auto usage = scene_shader.use();
            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
            model = glm::rotate(model, 2 * static_cast<float>(M_PI) * ticks / 4000.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            angle = -360.0f * ticks / 2000.0f;
            //model = glm::scale(model, glm::vec3(0.2f * sin(2 * M_PI * ticks / 5000.0f) + 1.0f, -0.1f * sin(2 * M_PI * ticks / 5000.0f) + 1.0f, 1.0f));
            batch.clear();
            //triangle.draw(batch, model);
            ring.draw(batch, model);
            batch.draw();
        }

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
