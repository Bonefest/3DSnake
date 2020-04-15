#ifndef SYSTEMS_H_INCLUDED
#define SYSTEMS_H_INCLUDED

#include "common.h"
#include "renderer.h"
#include "components.h"
#include "3rdparty/entt.hpp"

#include <stdexcept>

constexpr const float LAG_TIME = 0.3f;
constexpr float SPEED = 1.0f / LAG_TIME;

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
        m_renderer.setViewMatrix(glm::lookAt(glm::vec3(0.0f, 8.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    }
    void draw(entt::registry& registry, entt::dispatcher& dispatcher) {
        auto snakeView = registry.view<Snake>();
        snakeView.each([&](entt::entity snake, Snake& snakeComponent){
            if(!snakeComponent.parts.empty()) {
                for(std::size_t i = 0; i < snakeComponent.parts.size(); i++) {
                    m_renderer.renderCube(snakeComponent.parts[i], glm::vec3(1.0f, 0.7f, 0.0f));
                }

                m_renderer.renderCube(snakeComponent.parts[1] + directionToVector(snakeComponent.movingDirection) * 0.1f, glm::vec3(1.0f, 0.7f, 0.0f));
            }
        });

        registry.view<Apple>().each([&](entt::entity apple, Apple& appleComponent) {
            m_renderer.renderCube(appleComponent.position, glm::vec3(1.0f, 0.0f, 0.0f));
        });

        m_renderer.renderBox(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(2.0f * Constants::BOARD_WIDTH, 1.0f, 2.0f * Constants::BOARD_HEIGHT));
        m_renderer.present();
    }

private:
    Renderer m_renderer;
};

class AppleSpawningSystem: public ISystem {
public:
    void update(entt::registry& registry, entt::dispatcher& dispatcher, double delta) {
        auto appleView = registry.view<Apple>();
        if(appleView.size() < Constants::MAX_APPLES_COUNT) {
            if(drand48() * 100.0f < Constants::APPLE_SPAWN_CHANCE) {
                float randX = std::round( (drand48() - drand48()) * Constants::BOARD_WIDTH);
                float randZ = std::round( (drand48() - drand48()) * Constants::BOARD_HEIGHT);

                auto apple = registry.create();
                registry.assign<Apple>(apple, glm::vec3(randX, 0.0f, randZ));
            }
        }

    }
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

                for(std::size_t i = 1; i < parts.size(); ++i) {
                    if(glm::distance(parts[0], parts[i]) < 0.1f)
                        throw std::logic_error("You lose!");
                }

                for(std::size_t i = parts.size() - 1; i > 0; i--) {
                    parts[i] = glm::round(parts[i - 1]);
                }

                vector<entt::entity> collidedApples;
                registry.view<Apple>().each([&](entt::entity apple, Apple& appleComponent) {
                    if(glm::distance(parts[0], appleComponent.position) < 0.1f) {
                        collidedApples.push_back(apple);
                        parts.push_back(parts.back());
                        //trigger event
                    }
                });

                for(auto apple : collidedApples)
                    registry.destroy(apple);

                m_elapsedTime = 0.0f;
                snakeComponent.parts[0] = glm::round(snakeComponent.parts[0]);
            }


            parts[0] += directionToVector(snakeComponent.movingDirection) * float(delta) * SPEED;
            parts[0] = wrapPosition(parts[0]);

            glm::vec3 target = snakeComponent.parts[parts.size() - 2];
            glm::vec3 position = snakeComponent.parts[parts.size() - 1];

            glm::vec3 direction = glm::normalize(target - position);

            if(glm::distance(target, position) > 2.0f) {
                direction = m_previousDirection;
            }


            m_previousDirection = direction;

            parts[parts.size() - 1] += direction * float(delta) * SPEED;
            parts[parts.size() - 1] = wrapPosition(parts[parts.size() - 1]);
        });

    }
private:
    glm::vec3 m_previousDirection;

    glm::vec3 wrapPosition(glm::vec3 position) {
        float size = Constants::CELL_WIDTH * 0.5f;

        if(position.x < -Constants::BOARD_WIDTH - size) position.x = Constants::BOARD_WIDTH - size - (-Constants::BOARD_WIDTH - size - position.x);
        else if(position.x > Constants::BOARD_WIDTH + size) position.x = -Constants::BOARD_WIDTH + size + (position.x - Constants::BOARD_WIDTH - size);

        if(position.z < -Constants::BOARD_HEIGHT - size) position.z = Constants::BOARD_HEIGHT - size - (-Constants::BOARD_HEIGHT - size - position.z);
        else if(position.z > Constants::BOARD_HEIGHT + size) position.z = -Constants::BOARD_HEIGHT + size + (position.z - Constants::BOARD_HEIGHT - size);

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

            if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
                snakeComponent.parts.push_back(snakeComponent.parts.back());
            }

        });
    }
private:
    double m_elapsedTime;
    Direction m_nextDirection;
};



#endif // SYSTEMS_H_INCLUDED
