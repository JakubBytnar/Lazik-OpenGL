#pragma once
#include "Mesh.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

class ObjLoader {
public:
    static std::vector<Mesh> load(const std::string& filepath) {
        std::vector<Mesh> meshes;
        std::vector<glm::vec3> tempPos;
        std::vector<glm::vec3> tempNorm;
        std::vector<glm::vec2> tempUV;

        std::string currentName = "Bryla_0";
        std::vector<Vertex> currentVertices;

        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "[BLAD] Nie znaleziono pliku: " << filepath << "\n";
            return meshes;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "o" || prefix == "g") {
                if (!currentVertices.empty()) {
                    meshes.emplace_back(currentName, std::move(currentVertices));
                    currentVertices.clear();
                }
                std::getline(iss >> std::ws, currentName);
            }
            else if (prefix == "v") {
                glm::vec3 p;
                iss >> p.x >> p.y >> p.z;
                tempPos.push_back(p);
            }
            else if (prefix == "vn") {
                glm::vec3 n;
                iss >> n.x >> n.y >> n.z;
                tempNorm.push_back(n);
            }
            else if (prefix == "vt") {
                glm::vec2 uv;
                iss >> uv.x >> uv.y;
                tempUV.push_back(uv);
            }
            else if (prefix == "f") {
                std::vector<Vertex> faceVerts;
                std::string token;
                while (iss >> token) {
                    faceVerts.push_back(parseFaceVertex(token, tempPos, tempNorm, tempUV));
                }
                // Triangulacja wachlarzowa (gwarantuje same trójkąty)
                for (size_t i = 1; i + 1 < faceVerts.size(); ++i) {
                    currentVertices.push_back(faceVerts[0]);
                    currentVertices.push_back(faceVerts[i]);
                    currentVertices.push_back(faceVerts[i + 1]);
                }
            }
        }

        if (!currentVertices.empty()) {
            meshes.emplace_back(currentName, std::move(currentVertices));
        }

        std::cout << "[OK] Zaimportowano bryl elementarnych: " << meshes.size() << "\n";
        for (size_t i = 0; i < meshes.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << meshes[i].name
                << " (" << meshes[i].vertices.size() / 3 << " trojkatow)\n";
        }

        return meshes;
    }

private:
    static Vertex parseFaceVertex(const std::string& token,
        const std::vector<glm::vec3>& pos,
        const std::vector<glm::vec3>& norm,
        const std::vector<glm::vec2>& uv) {
        Vertex v{};
        std::stringstream ss(token);
        std::string idxStr;
        int indices[3] = { 0, 0, 0 };
        int i = 0;

        while (std::getline(ss, idxStr, '/') && i < 3) {
            if (!idxStr.empty()) indices[i] = std::stoi(idxStr);
            i++;
        }

        if (indices[0] > 0 && indices[0] <= static_cast<int>(pos.size()))
            v.position = pos[indices[0] - 1];
        if (indices[1] > 0 && indices[1] <= static_cast<int>(uv.size()))
            v.texCoords = uv[indices[1] - 1];
        if (indices[2] > 0 && indices[2] <= static_cast<int>(norm.size()))
            v.normal = norm[indices[2] - 1];

        return v;
    }
};