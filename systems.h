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
            for(auto part : snakeComponent.parts) {
                m_renderer.renderCube(part.position, glm::vec3(0.0f, 0.0f, 0.0f));
            }
        });

        m_renderer.present();
    }

private:
    Renderer m_renderer;
};

class MovingSystem: public ISystem {
public:
    void update(entt::registry& registry, entt::dispatcher& dispatcher, double delta) {
        auto snakeView = registry.view<Snake>();
        snakeView.each([&](entt::entity snake, Snake& snakeComponent) {
            if(!snakeComponent.parts.empty()) {
                if(snakeComponent.parts.size() > 1) {
                    for(std::size_t i = snakeComponent.parts.size() - 1; i > 0; --i) {
                        snakeComponent.parts[i].position = snakeComponent.parts[i - 1].position;
                        directionToVector(snakeComponent.parts[i - 1].previousDirection)
                    }
                }

                snakeComponent.parts[0].position += directionToVector(snakeComponent.movingDirection) * float(snakeComponent.speed * delta);
            }

        });
    }
};

class InputProcessingSystem: public ISystem {
public:
    void processInput(entt::registry& registry, entt::dispatcher& dispatcher, GLFWwindow* window) {
        auto snakeView = registry.view<Snake>(); //registry.view<Snake, PlayerConfiguration>();
        snakeView.each([&](entt::entity snake, Snake& snakeComponent) {
            if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && snakeComponent.movingDirection != BOTTOM) {
                snakeComponent.movingDirection = TOP;
            } else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && snakeComponent.movingDirection != TOP) {
                snakeComponent.movingDirection = BOTTOM;
            } else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && snakeComponent.movingDirection != LEFT) {
                snakeComponent.movingDirection = RIGHT;
            } else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && snakeComponent.movingDirection != RIGHT) {
                snakeComponent.movingDirection = LEFT;
            }
        });
    }
};


#endif // SYSTEMS_H_INCLUDED
