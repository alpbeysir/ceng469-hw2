#pragma once

#include <glm/glm.hpp>
#include <string>

glm::vec3 qn(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& v4);

glm::vec3 tn(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);

struct Triangle {
    int vi1, vi2, vi3;

    Triangle(int vi1, int vi2, int vi3) : vi1(vi1), vi2(vi2), vi3(vi3) {

    }
};

struct Mesh {
    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> normals;
    std::vector<Triangle> tris;
};


std::vector<std::string> splitString(const std::string& input, char delimiter);
int vi(const std::string& input);
Mesh load_mesh(const std::string& fileName);
void save_obj_debug(const Mesh& mesh, const std::string& filename);