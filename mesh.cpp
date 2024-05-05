#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>
#include "mesh.h"

std::vector<std::string> splitString(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;

    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

int vi(const std::string& input) {
    size_t delimiterPos = input.find("//");
    std::string firstNumberStr = input.substr(0, delimiterPos);
    int firstNumber = std::stoi(firstNumberStr);
    return firstNumber - 1;
}

// ChatGPT get quad normal
glm::vec3 qn(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& v4) {
    glm::vec3 edge1 = v2 - v1;
    glm::vec3 edge2 = v3 - v1;
    glm::vec3 normal = glm::cross(edge1, edge2);
    normal = glm::normalize(normal);
    return normal;
}

glm::vec3 tn(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    return glm::triangleNormal(v1, v2, v3);
}

Mesh load_mesh(const std::string& fileName)
{
    std::fstream myfile;
    myfile.open(fileName.c_str(), std::ios::in);

    std::cout << "Loading model " << fileName << std::endl;

    Mesh mesh;

    if (myfile.is_open())
    {
        std::string curLine;

        while (getline(myfile, curLine))
        {
            if (curLine.length() >= 2)
            {
                auto split = splitString(curLine, ' ');
                if (split[0] == "v") {
                    glm::vec3 v(stof(split[1]), stof(split[2]), stof(split[3]));
                    mesh.verts.push_back(v);
                }
                else if (split[0] == "f") {
                    bool is_quad = split.size() == 5;
                    bool has_vertex_normals = split[1].find('//') != std::string::npos;
                    if (is_quad) {
                        Quad q{ vi(split[1]), vi(split[2]), vi(split[3]), vi(split[4]) };
                        mesh.quads.push_back(q);
                    }
                    else {
                        Triangle t{ vi(split[1]), vi(split[2]), vi(split[3]) };
                        mesh.tris.push_back(t);
                    }
                }
                else
                {
                    std::cout << "Ignoring unidentified line in obj file: " << curLine << std::endl;
                }
            }
        }

        myfile.close();
    }
    else
    {
        exit(-1);
    }

    assert(mesh.tris.size() == 0 ^ mesh.quads.size() == 0);

    if (mesh.tris.size() != 0) {
        // triangle mesh
    }
    else if (mesh.quads.size() != 0) {
        // quad mesh
    }

    return mesh;
}

void save_obj_debug(const Mesh& mesh, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // Write vertices
    for (const auto& vertex : mesh.verts) {
        file << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
    }

    // Write normals
    for (const auto& normal : mesh.normals) {
        file << "vn " << normal.x << " " << normal.y << " " << normal.z << "\n";
    }

    if (mesh.tris.size() > 0) {
        // Write faces (triangles)
        for (const auto& triangle : mesh.tris) {
            file << "f " << (triangle.vi1 + 1) << "//" << " " << (triangle.vi2 + 1) << "//" << " " << (triangle.vi3 + 1) << "//" << "\n";
        }
    }
    else {
        // Write faces (quads)
        for (const auto& quad : mesh.quads) {
            file << "f " << (quad.vi1 + 1) << "//" << " " << (quad.vi2 + 1) << "//" << " " << (quad.vi3 + 1) << "//" << " " << (quad.vi4 + 1) << "//" << "\n";
        }
    }


    file.close();
    std::cout << "OBJ file saved successfully: " << filename << std::endl;
}