#pragma once
#include "ObjLoader.hpp"
#include <cmath>
#include <limits>

class Rover {
public:
    std::vector<Mesh> parts;
    std::vector<bool> isWheel;
    std::vector<bool> isFrontWheel;
    glm::vec3 position{ 0.0f, 0.0f, 0.0f };
    float headingAngle = 0.0f;
    float wheelSpin = 0.0f;
    float wheelSteerAngle = 0.0f;
    float modelForwardOffset = -90.0f;

    explicit Rover(const std::string& objPath) {
        parts = ObjLoader::load(objPath);
        setupGroundAndParts();
    }

    void move(float distance) {
        float rad = glm::radians(headingAngle);
        position.x -= std::sin(rad) * distance;
        position.z -= std::cos(rad) * distance;
        wheelSpin += distance * 80.0f;

        for (size_t i = 0; i < parts.size(); ++i) {
            if (isWheel[i]) {
                parts[i].localRotation.z = wheelSpin;
            }
        }
    }

    void turn(float deltaAngle) {
        headingAngle += deltaAngle;
    }

    // Płynny skręt przednich kół (steerInput: +1 w lewo, -1 w prawo, 0 powrót na wprost)
    void updateSteering(float dt, float steerInput) {
        float maxSteerAngle = 32.0f;   // Maksymalny kąt skrętu kół w stopniach
        float steerSpeed = 180.0f;     // Szybkość skręcania kół
        float targetAngle = steerInput * maxSteerAngle;

        if (wheelSteerAngle < targetAngle) {
            wheelSteerAngle = std::min(targetAngle, wheelSteerAngle + steerSpeed * dt);
        }
        else if (wheelSteerAngle > targetAngle) {
            wheelSteerAngle = std::max(targetAngle, wheelSteerAngle - steerSpeed * dt);
        }

        for (size_t i = 0; i < parts.size(); ++i) {
            if (isFrontWheel[i]) {
                parts[i].localRotation.y = wheelSteerAngle;
            }
        }
    }

    void draw(GLuint modelLoc, GLuint colorLoc) const {
        glm::mat4 roverMatrix = glm::translate(glm::mat4(1.0f), position);
        roverMatrix = glm::rotate(roverMatrix, glm::radians(headingAngle + modelForwardOffset), glm::vec3(0, 1, 0));

        for (const auto& part : parts) {
            part.draw(modelLoc, colorLoc, roverMatrix);
        }
    }

private:
    void setupGroundAndParts() {
        isWheel.resize(parts.size(), false);
        isFrontWheel.resize(parts.size(), false);
        if (parts.empty()) return;

        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        float maxAbsZ = 0.0f;

        for (const auto& part : parts) {
            for (const auto& v : part.vertices) {
                float worldY = part.pivot.y + v.position.y;
                float worldZ = std::abs(part.pivot.z + v.position.z);
                minY = std::min(minY, worldY);
                maxY = std::max(maxY, worldY);
                maxAbsZ = std::max(maxAbsZ, worldZ);
            }
        }

        // Postawienie kół równo na ziemi (y = 0)
        position.y = -minY;
        float totalHeight = maxY - minY;

        float minWheelX = std::numeric_limits<float>::max();
        float maxWheelX = std::numeric_limits<float>::lowest();

        for (size_t i = 0; i < parts.size(); ++i) {
            glm::vec3 minB(0.0f), maxB(0.0f);
            for (const auto& v : parts[i].vertices) {
                minB = glm::min(minB, v.position);
                maxB = glm::max(maxB, v.position);
            }
            glm::vec3 size = maxB - minB;
            float partBottomY = parts[i].pivot.y + minB.y;

            bool onOuterSide = std::abs(parts[i].pivot.z) > maxAbsZ * 0.65f;
            bool touchesGround = partBottomY <= minY + totalHeight * 0.05f;
            bool isBigDisc = size.y > totalHeight * 0.15f && size.z < size.x * 0.8f;

            if (parts[i].name.find("Cylinder") != std::string::npos &&
                onOuterSide && touchesGround && isBigDisc) {
                isWheel[i] = true;
                parts[i].color = glm::vec3(0.16f, 0.16f, 0.18f);
                minWheelX = std::min(minWheelX, parts[i].pivot.x);
                maxWheelX = std::max(maxWheelX, parts[i].pivot.x);
            }
            else {
                float shade = 0.65f + 0.25f * ((i % 4) / 3.0f);
                parts[i].color = glm::vec3(shade * 0.9f, shade * 0.72f, 0.22f);
            }
        }

        // Wykrycie przedniej pary kół (przód modelu w lokalnym układzie to najmniejsze X)
        float axleSpan = maxWheelX - minWheelX;
        float frontThreshold = minWheelX + (axleSpan > 0.01f ? axleSpan * 0.25f : 0.1f);

        for (size_t i = 0; i < parts.size(); ++i) {
            if (isWheel[i] && parts[i].pivot.x <= frontThreshold) {
                isFrontWheel[i] = true;
            }
        }
    }
};