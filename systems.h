#ifndef SYSTEMS_H_INCLUDED
#define SYSTEMS_H_INCLUDED

#include "common.h"
#include "renderer.h"
#include "components.h"
#include "3rdparty/entt.hpp"

class ISystem {
public:
    virtual ~ISystem() { }
    virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, double delta) { }
    virtual void draw(entt::registry& registry, entt::dispatcher& dispatcher) { }
    virtual void processInput(entt::registry& registry, entt::dispatcher& dispatcher, GLFWwindow* window) { }
};

class RenderingSystem: public ISystem {
public:
    RenderingSystem() {

        m_renderer.setProjectionMatrix(glm::perspective(45.0f, Constants::SCREEN_WIDTH/Constants::SCREEN_HEIGHT, 0.1f, 100.0f));
        m_renderer.setViewMatrix(glm::lookAt(glm::vec3(0.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    }
    void draw(entt::registry& registry, entt::dispatcher& dispatcher) {
        auto snakeView = registry.view<Snake>();
        snakeView.each([&](entt::entity snake, Snake& snakeComponent){
            if(!snakeComponent.parts.empty()) {
                for(std::size_t i = 0; i < snakeComponent.parts.size(); i++) {
                    m_renderer.renderCube(snakeComponent.parts[i], glm::vec3(0.0f, 0.0f, 0.0f));
                }
            }
        });
        //m_renderer.renderCube(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        m_renderer.present();
    }

private:
    Renderer m_renderer;
};

class MovingSystem: public ISystem {
public:
    void update(entt::registry& registry, entt::dispatcher& dispatcher, double delta) {
        m_elapsedTime += delta;
        auto snakeView = registry.view<Snake>();
        snakeView.each([&](entt::entity snake, Snake& snakeComponent) {
            if(m_elapsedTime > 0.3f) {
                for(std::size_t i = snakeComponent.parts.size() - 1; i > 0; i--) {
                    snakeComponent.parts[i] = snakeComponent.parts[i - 1];
                }

                m_elapsedTime = 0.0f;
            }

            snakeComponent.parts[0] += directionToVector(snakeComponent.movingDirection) * float(delta) * snakeComponent.speed;
            glm::vec3 target = snakeComponent.parts[snakeComponent.parts.size() - 2];
            glm::vec3 position = snakeComponent.parts[snakeComponent.parts.size() - 1];

            snakeComponent.parts[snakeComponent.parts.size() - 1] += glm::normalize(target - position) * float(delta) * snakeComponent.speed;
        });

    }
private:
    double m_elapsedTime;
};

class InputProcessingSystem: public ISystem {
public:
    InputProcessingSystem(): m_elapsedTime(0.0), m_nextDirection(LEFT) { }

    void update(entt::registry& registry, entt::dispatcher& dispatcher, double delta) {
        m_elapsedTime += delta;
        if(m_elapsedTime > 0.3) {
            auto snakeView = registry.view<Snake>();
            snakeView.each([&](entt::entity snake, Snake& snakeComponent) {
                snakeComponent.movingDirection = m_nextDirection;
            });

            m_elapsedTime = 0.0;
        }
    }

    void processInput(entt::registry& registry, entt::dispatcher& dispatcher, GLFWwindow* window) {
        auto snakeView = registry.view<Snake>(); //registry.view<Snake, PlayerConfiguration>();
        snakeView.each([&](entt::entity snake, Snake& snakeComponent) {
            if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && snakeComponent.movingDirection != BOTTOM) {
                m_nextDirection = TOP;
            } else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && snakeComponent.movingDirection != TOP) {
                m_nextDirection = BOTTOM;
            } else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && snakeComponent.movingDirection != LEFT) {
                m_nextDirection = RIGHT;
            } else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && snakeComponent.movingDirection != RIGHT) {
                m_nextDirection = LEFT;
            }

        });
    }
private:
    double m_elapsedTime;
    Direction m_nextDirection;
};


#endif // SYSTEMS_H_INCLUDED
