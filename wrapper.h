#pragma once
#include <cassert>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "Shader.h"
#include "mesh.h"
#include "subdivision.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))
const int MAX_SUBDIVIDE = 5;

enum DrawMode {
    Solid, Line, Wire
};

struct DrawData {
    glm::mat4 transformation;
    glm::vec3 color;
};

struct ModelData
{
    unsigned int glVao;
    unsigned int glVertexData;
    unsigned int glSolidIndexData;
    unsigned int glLineIndexData;
    size_t solidDataLength;
    size_t lineDataLength;
    glm::vec3 offset;
    glm::vec3 size;
};

void draw_model(const ModelData& model, Shader& s, const DrawData& draw_data, const glm::vec3& light_pos, const glm::vec3& camera_pos, const DrawMode& draw_mode);
ModelData load_model(const Mesh& mesh);

struct SceneObject {
    DrawData draw_data;
    std::vector<ModelData> levels;
    int cur_level = 0;

    void precompute(const Mesh& mesh) {
        Mesh tmp = mesh;
        for (int i = 0; i < MAX_SUBDIVIDE; i++) {
            levels.push_back(load_model(tmp));
            tmp = subdivide(tmp);
        }
    }
};