#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <deque>

#include "program.h"
#include "cube.h"

using std::make_pair;
using std::deque;
using std::pair;

class Renderer {
public:
    Renderer() {
        program = new Program("shaders/vertex.glsl", "shaders/fragment.glsl");
        uniformMVP = glGetUniformLocation(program->getProgramID(), "mvp");

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

    void present(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
        glUseProgram(program->getProgramID());

        glm::mat4 vp = projMatrix * viewMatrix;

        for(auto cubeData : m_cubesQueue) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubeData.first);

            glm::mat4 mvp = vp * model;
            glUniformMatrix4fv(uniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        m_cubesQueue.clear();
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
    GLint uniformMVP;
};


#endif // RENDERER_H_INCLUDED
