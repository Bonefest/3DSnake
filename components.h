#ifndef COMPONENTS_H_INCLUDED
#define COMPONENTS_H_INCLUDED

#include "3rdparty/entt.hpp"
#include <glm/glm.hpp>

#include <vector>

using std::vector;

enum Direction {
    LEFT, TOP, RIGHT, BOTTOM
};

glm::vec3 directionToVector(Direction direction) {
    switch(direction) {
    case LEFT: return glm::vec3(1.0f, 0.0f, 0.0f);
    case TOP: return glm::vec3(0.0f, 0.0f, -1.0f);
    case RIGHT: return glm::vec3(-1.0f, 0.0f, 0.0f);
    case BOTTOM: return glm::vec3(0.0f, 0.0f, 1.0f);
    }

    return glm::vec3(0.0f, 0.0f, 0.0f);
}

struct Snake {
    Snake(glm::vec3 position, Direction direction, float spd): movingDirection(direction), speed(spd) {
        parts.push_back(position);
        parts.push_back(position);
    }

    vector<glm::vec3> parts;

    Direction movingDirection;

    float speed;
};

struct Apple {
    explicit Apple(glm::vec3 applePosition): position(applePosition) { }
    glm::vec3 position;
};


#endif // COMPONENTS_H_INCLUDED
