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
        uniformColor = glGetUniformLocation(program->getProgramID(), "color");

        initBuffers();
    }

    ~Renderer() {
        delete program;

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    void renderCube(glm::vec3 position, glm::vec3 color) {
        if(position.x > Constants::BOARD_WIDTH - Constants::CELL_WIDTH * 0.5f) {
            float sizex = std::max(Constants::BOARD_WIDTH - (position.x - Constants::CELL_WIDTH * 0.5f), 0.0f);
            glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(Constants::BOARD_WIDTH - sizex*0.5f, position.y, position.z));
            mat = glm::scale(mat, glm::vec3(sizex, Constants::CELL_WIDTH, Constants::CELL_WIDTH));
            mat = glm::translate(mat, glm::vec3(0.0f, 0.0f, 0.0f));


            m_cubesQueue.push_back(make_pair(mat, color));

            mat = glm::translate(glm::mat4(1.0f), glm::vec3(-Constants::BOARD_WIDTH + (Constants::CELL_WIDTH - sizex)*0.5f, position.y, position.z));
            mat = glm::scale(mat, glm::vec3((Constants::CELL_WIDTH - sizex), Constants::CELL_WIDTH, Constants::CELL_WIDTH));
            mat = glm::translate(mat, glm::vec3(0.0f, 0.0f, 0.0f));

            m_cubesQueue.push_back(make_pair(mat, color));
        } else if(position.x < -Constants::BOARD_WIDTH + Constants::CELL_WIDTH * 0.5f) {
            float sizex = std::max((position.x + Constants::CELL_WIDTH * 0.5f) + Constants::BOARD_WIDTH, 0.0f);
            glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(-Constants::BOARD_WIDTH + sizex*0.5f, position.y, position.z));
            mat = glm::scale(mat, glm::vec3(sizex, Constants::CELL_WIDTH, Constants::CELL_WIDTH));
            mat = glm::translate(mat, glm::vec3(0.0f, 0.0f, 0.0f));


            m_cubesQueue.push_back(make_pair(mat, color));

            mat = glm::translate(glm::mat4(1.0f), glm::vec3(Constants::BOARD_WIDTH - (Constants::CELL_WIDTH - sizex)*0.5f, position.y, position.z));
            mat = glm::scale(mat, glm::vec3((Constants::CELL_WIDTH - sizex), Constants::CELL_WIDTH, Constants::CELL_WIDTH));
            mat = glm::translate(mat, glm::vec3(0.0f, 0.0f, 0.0f));

            m_cubesQueue.push_back(make_pair(mat, color));
        } else if(position.z > Constants::BOARD_HEIGHT - Constants::CELL_WIDTH * 0.5f) {
            float sizez = std::max(Constants::BOARD_HEIGHT - (position.z - Constants::CELL_WIDTH * 0.5f), 0.0f);
            glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, Constants::BOARD_HEIGHT - sizez*0.5f));
            mat = glm::scale(mat, glm::vec3(Constants::CELL_WIDTH, Constants::CELL_WIDTH, sizez));
            mat = glm::translate(mat, glm::vec3(0.0f, 0.0f, 0.0f));


            m_cubesQueue.push_back(make_pair(mat, color));

            mat = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, -Constants::BOARD_HEIGHT + (Constants::CELL_WIDTH - sizez)*0.5f));
            mat = glm::scale(mat, glm::vec3(Constants::CELL_WIDTH, Constants::CELL_WIDTH, (Constants::CELL_WIDTH - sizez)));
            mat = glm::translate(mat, glm::vec3(0.0f, 0.0f, 0.0f));

            m_cubesQueue.push_back(make_pair(mat, color));
        } else if(position.z < -Constants::BOARD_HEIGHT + Constants::CELL_WIDTH * 0.5f) {
            float sizez = std::max((position.z + Constants::CELL_WIDTH * 0.5f) + Constants::BOARD_HEIGHT, 0.0f);
            glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, -Constants::BOARD_HEIGHT + sizez*0.5f));
            mat = glm::scale(mat, glm::vec3(Constants::CELL_WIDTH, Constants::CELL_WIDTH, sizez));
            mat = glm::translate(mat, glm::vec3(0.0f, 0.0f, 0.0f));


            m_cubesQueue.push_back(make_pair(mat, color));

            mat = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, Constants::BOARD_HEIGHT - (Constants::CELL_WIDTH - sizez)*0.5f));
            mat = glm::scale(mat, glm::vec3(Constants::CELL_WIDTH, Constants::CELL_WIDTH, (Constants::CELL_WIDTH - sizez)));
            mat = glm::translate(mat, glm::vec3(0.0f, 0.0f, 0.0f));

            m_cubesQueue.push_back(make_pair(mat, color));
        } else {
            glm::mat4 mat = glm::translate(glm::mat4(1.0f), position);
            mat = glm::scale(mat, glm::vec3(Constants::CELL_WIDTH, Constants::CELL_WIDTH, Constants::CELL_WIDTH));
            m_cubesQueue.push_back(make_pair(mat, color));
        }

    }

    void renderBox(glm::vec3 position, glm::vec3 color, glm::vec3 size) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model = glm::scale(model, size);

        m_cubesQueue.push_back(make_pair(model, color));
    }

    void present() {
        glUseProgram(program->getProgramID());

        glm::mat4 vp = m_projection * m_view;

        for(auto cubeData : m_cubesQueue) {
            glm::mat4 mvp = vp * cubeData.first;
            glUniformMatrix4fv(uniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));
            glUniform4f(uniformColor, cubeData.second.x, cubeData.second.y, cubeData.second.z, 1.0f);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        m_cubesQueue.clear();
    }

    void setProjectionMatrix(glm::mat4 projection) {
        m_projection = projection;
    }

    void setViewMatrix(glm::mat4 view) {
        m_view = view;
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
    deque<pair<glm::mat4, glm::vec3>> m_cubesQueue;

    unsigned int VAO, VBO;
    GLint uniformMVP;
    GLint uniformColor;

    glm::mat4 m_projection, m_view;
};


#endif // RENDERER_H_INCLUDED
