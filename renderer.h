#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include <glm/glm.hpp>
#include <deque>

#include "program.h"
#include "cube.h"

using std::deque;

class Renderer {
public:
    Renderer() {
        program = new Program("shaders/vertex.glsl", "shaders/fragment.glsl");

        initBuffers();
    }

    ~Renderer() {
        delete program;

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    void renderCube(glm::vec3 position, glm::vec3 color) {
        m_cubesQueue.push_back(make_pair(position, color));
    }

    void present() {
        for(auto cubeData : m_cubesQueue) {

        }
    }

private:
    void initBuffers() {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTICIES), CUBE_VERTICIES, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    Program* program;
    deque<pair<glm::vec3, glm::vec3>> m_cubesQueue;

    unsigned int VAO, VBO;
};


#endif // RENDERER_H_INCLUDED
