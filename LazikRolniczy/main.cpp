#pragma comment(lib, "opengl32.lib")
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "Rover.hpp"

const char* vertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;
out vec4 FragColor;

void main() {
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.4));
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.25);
    vec3 result = diff * objectColor;
    FragColor = vec4(result, 1.0);
}
)";

GLuint createShaderProgram() {
    auto compile = [](GLenum type, const char* src) -> GLuint {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        return s;
        };
    GLuint vs = compile(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fragmentShaderSrc);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

// Tworzy prostą płytę podłoża złożoną z 2 trójkątów (GL_TRIANGLES)
Mesh createGroundMesh() {
    float s = 40.0f;
    glm::vec3 n(0.0f, 1.0f, 0.0f);
    std::vector<Vertex> verts = {
        { { -s, 0.0f, -s }, n,{ 0, 0 } },
        { { -s, 0.0f,  s }, n,{ 0, 1 } },
        { { s, 0.0f,  s }, n,{ 1, 1 } },
        { { -s, 0.0f, -s }, n,{ 0, 0 } },
        { { s, 0.0f,  s }, n,{ 1, 1 } },
        { { s, 0.0f, -s }, n,{ 1, 0 } }
    };
    Mesh ground("Podloze", std::move(verts));
    ground.color = glm::vec3(0.32f, 0.42f, 0.28f);
    return ground;
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Robot Rolniczy (Lazik) - OpenGL", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Blad inicjalizacji GLEW\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    GLuint shader = createShaderProgram();
    GLuint modelLoc = glGetUniformLocation(shader, "model");
    GLuint viewLoc = glGetUniformLocation(shader, "view");
    GLuint projLoc = glGetUniformLocation(shader, "projection");
    GLuint colorLoc = glGetUniformLocation(shader, "objectColor");

    // Wczytanie łazika wyeksportowanego z Blendera
    Rover rover("lazik.obj");
    Mesh ground = createGroundMesh();

    float lastTime = static_cast<float>(glfwGetTime());
    float camYaw = 0.0f; // Kąt kamery na sztywno za łazikiem

    while (!glfwWindowShouldClose(window)) {
        float currentTime = static_cast<float>(glfwGetTime());
        float dt = currentTime - lastTime;
        lastTime = currentTime;

        // Sterowanie łazikiem: W/S (przód/tył), A/D (skręt)
        float moveSpeed = 4.0f * dt;
        float turnSpeed = 90.0f * dt;
        float steerInput = 0.0f;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) rover.move(moveSpeed);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) rover.move(-moveSpeed);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            rover.turn(turnSpeed);
            camYaw += turnSpeed;
            steerInput += 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            rover.turn(-turnSpeed);
            camYaw -= turnSpeed;
            steerInput -= 1.0f;
        }
        rover.updateSteering(dt, steerInput);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

        glClearColor(0.55f, 0.72f, 0.88f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);

        // Kamera na sztywno za łazikiem
        float rad = glm::radians(camYaw);
        glm::vec3 camOffset(sin(rad) * 8.5f, 4.5f, cos(rad) * 8.5f);
        glm::vec3 camPos = rover.position + camOffset;
        glm::mat4 view = glm::lookAt(camPos, rover.position + glm::vec3(0.0f, 0.8f, 0.0f), glm::vec3(0, 1, 0));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 200.0f);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

        ground.draw(modelLoc, colorLoc, glm::mat4(1.0f));
        rover.draw(modelLoc, colorLoc);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}