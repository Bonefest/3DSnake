#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "systems.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

class Game {
public:
    Game(const char* title, int width, int height): m_lastTime(0.0), m_deltaTime(0.0) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_pwindow = glfwCreateWindow(width, height, title, NULL, NULL);
        if(m_pwindow == NULL) {
            std::cout << "Unable to create window!\n";
            glfwTerminate();
        }
        glfwMakeContextCurrent(m_pwindow);

        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Unable to initialize GLAD!\n";
        }


        glfwSetFramebufferSizeCallback(m_pwindow, framebuffer_size_callback);
        glEnable(GL_DEPTH_TEST);

        m_inputSystem = new InputProcessingSystem();
        m_renderingSystem = new RenderingSystem();
        m_movingSystem = new MovingSystem();

        initSnake();
    }

    ~Game() {
        delete m_inputSystem;
        delete m_renderingSystem;
        delete m_movingSystem;
        glfwTerminate();
    }

    void initSnake() {
        entt::entity snake = m_registry.create();
        m_registry.assign<Snake>(snake);

        Snake& snakeComponent = m_registry.get<Snake>(snake);
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.parts.push_back(Part{glm::vec3(0.0f, 0.0f, 0.0f)});
        snakeComponent.speed = 5.0f;
    }

    void run() {
        while(!glfwWindowShouldClose(m_pwindow)) {
            double currentTime = glfwGetTime();
            m_deltaTime = currentTime - m_lastTime;
            m_lastTime = currentTime;

            processInput();
            update();
            draw();
        }

    }

    void processInput() {
        if(glfwGetKey(m_pwindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(m_pwindow, true);

        m_inputSystem->processInput(m_registry, m_dispatcher, m_pwindow);

        glfwPollEvents();
    }

    void update() {
        m_movingSystem->update(m_registry, m_dispatcher, m_deltaTime);
    }

    void draw() {
        glClearColor(0.73f, 0.88f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_renderingSystem->draw(m_registry, m_dispatcher);

        glfwSwapBuffers(m_pwindow);
        glfwPollEvents();
    }

private:
    GLFWwindow* m_pwindow;

    double m_lastTime;
    double m_deltaTime;

    entt::registry m_registry;
    entt::dispatcher m_dispatcher;

    InputProcessingSystem* m_inputSystem;
    RenderingSystem* m_renderingSystem;
    MovingSystem* m_movingSystem;
};

#endif // GAME_H
