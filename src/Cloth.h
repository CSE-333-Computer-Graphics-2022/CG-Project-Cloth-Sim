#ifndef _CLOTH_H_
#define _CLOTH_H_

#include "utils.h"
#include "Vertex.h"
#include "Triangle.h"
#include "SpringDamper.h"

class Cloth {
    private:
        int width;
        int height;
        glm::vec3 pointWind;
        glm::vec3 translation;
        std::vector<int> indexFixed;

        GLuint VAO;
        GLuint VBO_positions, VBO_normals, EBO;

        glm::mat4 model;
        glm::vec3 color;

        std::vector<Vertex*> vertices;
        std::vector<Triangle*> triangles;
        std::vector<SpringDamper*> springDampers;

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        void updateNormal() {
            for (auto v : vertices) {
                v->normal = glm::vec3(0);
            }

            for (auto t : triangles) {
                t->updateNormal();
            }

            for (auto v : vertices) {
                v->normal = glm::normalize(v->normal);
            }
        }

        void updateAcceleration() {
            for (auto v : vertices) {
                v->resetAcceleration();
                v->addAcceleration(glm::inverse(model) * glm::vec4(0, -9.8, 0, 0));
            }
            for (auto sd : springDampers) {
                sd->updateAcceleration();
            }

            for (auto t : triangles) {
                t->addWind(pointWind);
            }
        }

    public:
        Cloth(int width, int height, glm::vec3 offset)  {
            // model matrix and color
            model = glm::translate(offset) * glm::mat4(1.0f);
            color = glm::vec3(1.0f, 0.1f, 0.1f);
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    glm::vec3 pos = glm::vec3(-0.1 * width / 2, 0.1 * height / 2, 0) + glm::vec3(i) * glm::vec3(0, -0.1, 0) + glm::vec3(j) * glm::vec3(0.1, 0, 0);
                    Vertex* vertex = new Vertex(0.1f, pos, glm::vec3(0.1));
                    vertices.push_back(vertex);
                    positions.push_back(pos);
                    if (i == 0) {
                        vertex->fix();
                        indexFixed.push_back(i * width + j);
                    }
                }
            }
            // create upper triangles
            for (int i = 0; i < height - 1; i++) {
                for (int j = 0; j < width - 1; j++) {
                    int t1 = i * width + j;
                    int t2 = (i + 1) * width + j;
                    int t3 = i * width + j + 1;
                    Vertex* v1 = vertices[t1];
                    Vertex* v2 = vertices[t2];
                    Vertex* v3 = vertices[t3];

                    Triangle* triangle = new Triangle(v1, v2, v3);

                    indices.push_back(t1);
                    indices.push_back(t2);
                    indices.push_back(t3);
                    triangles.push_back(triangle);
                }
            }

            // create lower triangles
            for (int i = 1; i < height; i++) {
                for (int j = 0; j < width - 1; j++) {
                    int t1 = i * width + j;
                    int t2 = i * width + j + 1;
                    int t3 = (i - 1) * width + j + 1;
                    Vertex* v1 = vertices[t1];
                    Vertex* v2 = vertices[t2];
                    Vertex* v3 = vertices[t3];
                    Triangle* triangle = new Triangle(v1, v2, v3);

                    indices.push_back(t1);
                    indices.push_back(t2);
                    indices.push_back(t3);
                    triangles.push_back(triangle);
                }
            }

            // horizontal spring damper
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width - 1; j++) {
                    int t1 = i * width + j;
                    int t2 = i * width + j + 1;
                    Vertex* v1 = vertices[t1];
                    Vertex* v2 = vertices[t2];
                    SpringDamper* springDamper = new SpringDamper(v1, v2, 0.1);

                    springDampers.push_back(springDamper);
                }
            }

            // vertical spring damper
            for (int i = 0; i < height - 1; i++) {
                for (int j = 0; j < width; j++) {
                    int t1 = i * width + j;
                    int t2 = (i + 1) * width + j;
                    Vertex* v1 = vertices[t1];
                    Vertex* v2 = vertices[t2];
                    SpringDamper* springDamper = new SpringDamper(v1, v2, 0.1);

                    springDampers.push_back(springDamper);
                }
            }

            // Upper Left to Lower Right diagonal spring damper
            for (int i = 0; i < height - 1; i++) {
                for (int j = 0; j < width - 1; j++) {
                    int t1 = i * width + j;
                    int t2 = (i + 1) * width + j + 1;
                    Vertex* v1 = vertices[t1];
                    Vertex* v2 = vertices[t2];
                    SpringDamper* springDamper = new SpringDamper(v1, v2, 0.1 * glm::sqrt(2));

                    springDampers.push_back(springDamper);
                }
            }

            // Lower Left to Upper Right diagonal spring damper
            for (int i = 1; i < height; i++) {
                for (int j = 0; j < width - 1; j++) {
                    int t1 = i * width + j;
                    int t2 = (i - 1) * width + j + 1;
                    Vertex* v1 = vertices[t1];
                    Vertex* v2 = vertices[t2];
                    SpringDamper* springDamper = new SpringDamper(v1, v2, 0.1 * glm::sqrt(2));

                    springDampers.push_back(springDamper);
                }
            }

            // horizontal spring damper, large
            for (int i = 0; i < height - 2; i += 2) {
                for (int j = 0; j < width - 2; j += 2) {
                    int t1 = i * width + j;
                    int t2 = i * width + j + 2;
                    Vertex* v1 = vertices[t1];
                    Vertex* v2 = vertices[t2];
                    SpringDamper* springDamper = new SpringDamper(v1, v2, 0.2);

                    springDampers.push_back(springDamper);
                }
            }

            // vertical spring damper, large
            for (int i = 0; i < height - 2; i += 2) {
                for (int j = 0; j < width - 2; j += 2) {
                    int t1 = i * width + j;
                    int t2 = (i + 2) * width + j;
                    Vertex* v1 = vertices[t1];
                    Vertex* v2 = vertices[t2];
                    SpringDamper* springDamper = new SpringDamper(v1, v2, 0.2);

                    springDampers.push_back(springDamper);
                }
            }

            // Upper Left to Lower Right diagonal spring damper, large
            for (int i = 0; i < height - 2; i += 2) {
                for (int j = 0; j < width - 2; j += 2) {
                    int t1 = i * width + j;
                    int t2 = (i + 2) * width + j + 2;
                    Vertex* v1 = vertices[t1];
                    Vertex* v2 = vertices[t2];
                    SpringDamper* springDamper = new SpringDamper(v1, v2, 0.2 * glm::sqrt(2));

                    springDampers.push_back(springDamper);
                }
            }

            // Lower Left to Upper Right diagonal spring damper, large
            for (int i = 2; i < height; i += 2) {
                for (int j = 0; j < width - 2; j += 2) {
                    int t1 = i * width + j;
                    int t2 = (i - 2) * width + j + 2;
                    Vertex* v1 = vertices[t1];
                    Vertex* v2 = vertices[t2];
                    SpringDamper* springDamper = new SpringDamper(v1, v2, 0.2 * glm::sqrt(2));

                    springDampers.push_back(springDamper);
                }
            }

            updateNormal();
            updateAcceleration();

            for (auto vertex : vertices) {
                normals.push_back(vertex->normal);
            }
            
            // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO_positions);
            glGenBuffers(1, &VBO_normals);

            // Bind to the VAO.
            glBindVertexArray(VAO);

            // Bind to the first VBO - We will use it to store the vertices
            glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

            // Bind to the second VBO - We will use it to store the normals
            glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* normals.size(), normals.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

            // Generate EBO, bind the EBO to the bound VAO and send the data
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

            // Unbind the VBOs
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        
        void display(const glm::mat4& viewProjMatrix, GLuint shader) {
            // actiavte the shader program 
            glUseProgram(shader);

            // get the locations and send the uniforms to the shader 
            glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMatrix);
            glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
            glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

            // Bind the VAO
            glBindVertexArray(VAO);

            // display the points using triangles, indexed with the EBO
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

            // Unbind the VAO and shader program
            glBindVertexArray(0);
            glUseProgram(0);
        }

        void update() {
            for (auto v : vertices) {
                v->move(0.001);
            }

            updateNormal();
            updateAcceleration();

            for (int i = 0; i < vertices.size(); i++) {
                positions[i] = vertices[i]->position;
                normals[i] = vertices[i]->normal;
            }

            // Bind to the VAO.
            glBindVertexArray(VAO);

            // Bind to the first VBO - We will use it to store the vertices
            glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

            // Bind to the second VBO - We will use it to store the normals
            glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
        }
        void blowByWind(glm::vec3 wind)  {pointWind = glm::vec3(glm::inverse(model) * glm::vec4(wind, 0));}

        void translate(glm::vec3 t) {
            translation += t;
            glm::vec3 pointT = glm::vec3(glm::inverse(model) * glm::vec4(t, 0));
            for (auto i : indexFixed) {
                vertices[i]->position += pointT;
            }
        }

        void toggleFree() {
            for (auto i : indexFixed) {
                vertices[i]->fixed = !vertices[i]->fixed;
            }
        }
};
#endif