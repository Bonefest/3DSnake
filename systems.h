#ifndef SYSTEMS_H_INCLUDED
#define SYSTEMS_H_INCLUDED

#include "common.h"
#include "renderer.h"
#include "components.h"
#include "3rdparty/entt.hpp"

const float LAG_TIME = 0.3f;

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
    MovingSystem(): m_previousDirection(0.0f, 0.0f, 0.0f) { }
    void update(entt::registry& registry, entt::dispatcher& dispatcher, double delta) {
        m_elapsedTime += delta;
        auto snakeView = registry.view<Snake>();
        snakeView.each([&](entt::entity snake, Snake& snakeComponent) {
            auto& parts = snakeComponent.parts;
            if(m_elapsedTime > LAG_TIME) {
                for(std::size_t i = parts.size() - 1; i > 0; i--) {
                    parts[i] = parts[i - 1];
                }

                m_elapsedTime = 0.0f;
            }

            parts[0] += directionToVector(snakeComponent.movingDirection) * float(delta) * (0.95f / LAG_TIME);
            parts[0] = wrapPosition(parts[0]);

            glm::vec3 target = snakeComponent.parts[parts.size() - 2];
            glm::vec3 position = snakeComponent.parts[parts.size() - 1];

            glm::vec3 direction = glm::normalize(target - position);

            if(glm::distance(target, position) > 2.0f) {
                direction = m_previousDirection;
            }


            m_previousDirection = direction;

            parts[parts.size() - 1] += direction * float(delta) * (0.95f / LAG_TIME);
            parts[parts.size() - 1] = wrapPosition(parts[parts.size() - 1]);
        });

    }
private:
    glm::vec3 m_previousDirection;

    glm::vec3 wrapPosition(glm::vec3 position) {
        if(position.x < -5.5f) position.x = 5.5f - (-5.5f - position.x);
        else if(position.x > 5.5f) position.x = -4.5f + (position.x - 5.5f);
//
//        if(position.z < -5.0f) position.z = 5.0f;
//        else if(position.z > 5.0f) position.z = -5.0f;

        return position;
    }

    double m_elapsedTime;
};

class InputProcessingSystem: public ISystem {
public:
    InputProcessingSystem(): m_elapsedTime(0.0), m_nextDirection(LEFT) { }

    void update(entt::registry& registry, entt::dispatcher& dispatcher, double delta) {
        m_elapsedTime += delta;
        if(m_elapsedTime > LAG_TIME) {
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
