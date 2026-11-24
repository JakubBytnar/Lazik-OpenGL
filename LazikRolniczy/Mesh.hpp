#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <algorithm>

struct Vertex {
    glm::vec3 position{ 0.0f };
    glm::vec3 normal{ 0.0f, 1.0f, 0.0f };
    glm::vec2 texCoords{ 0.0f };
};

class Mesh {
public:
    std::string name;
    std::vector<Vertex> vertices;
    glm::vec3 pivot{ 0.0f };
    glm::vec3 localRotation{ 0.0f };
    glm::vec3 color{ 0.75f, 0.78f, 0.80f };

    Mesh(const std::string& meshName, std::vector<Vertex> verts)
        : name(meshName), vertices(std::move(verts)) {
        calculatePivotAndCenter();
        setupMesh();
    }

    ~Mesh() {
        if (VAO != 0) glDeleteVertexArrays(1, &VAO);
        if (VBO != 0) glDeleteBuffers(1, &VBO);
    }

    // Obsługa przenoszenia (zapobiega usunięciu VAO przy relokacji wektora)
    Mesh(Mesh&& other) noexcept
        : name(std::move(other.name)),
        vertices(std::move(other.vertices)),
        pivot(other.pivot),
        localRotation(other.localRotation),
        color(other.color),
        VAO(other.VAO),
        VBO(other.VBO) {
        other.VAO = 0;
        other.VBO = 0;
    }

    Mesh& operator=(Mesh&& other) noexcept {
        if (this != &other) {
            if (VAO != 0) glDeleteVertexArrays(1, &VAO);
            if (VBO != 0) glDeleteBuffers(1, &VBO);
            name = std::move(other.name);
            vertices = std::move(other.vertices);
            pivot = other.pivot;
            localRotation = other.localRotation;
            color = other.color;
            VAO = other.VAO;
            VBO = other.VBO;
            other.VAO = 0;
            other.VBO = 0;
        }
        return *this;
    }

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw(GLuint modelLoc, GLuint colorLoc, const glm::mat4& parentTransform) const {
        if (vertices.empty()) return;

        glm::mat4 localModel = glm::translate(glm::mat4(1.0f), pivot);
        localModel = glm::rotate(localModel, glm::radians(localRotation.x), glm::vec3(1, 0, 0));
        localModel = glm::rotate(localModel, glm::radians(localRotation.y), glm::vec3(0, 1, 0));
        localModel = glm::rotate(localModel, glm::radians(localRotation.z), glm::vec3(0, 0, 1));

        glm::mat4 finalModel = parentTransform * localModel;
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &finalModel[0][0]);
        glUniform3fv(colorLoc, 1, &color[0]);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
        glBindVertexArray(0);
    }

private:
    GLuint VAO = 0;
    GLuint VBO = 0;

    void calculatePivotAndCenter() {
        if (vertices.empty()) return;
        glm::vec3 minB = vertices[0].position;
        glm::vec3 maxB = vertices[0].position;
        for (const auto& v : vertices) {
            minB = glm::min(minB, v.position);
            maxB = glm::max(maxB, v.position);
        }
        pivot = (minB + maxB) * 0.5f;
        for (auto& v : vertices) {
            v.position -= pivot;
        }
    }

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

        glBindVertexArray(0);
    }
};