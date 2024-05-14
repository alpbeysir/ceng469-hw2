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

#define BUFFER_OFFSET(i) ((char*)NULL + (i))
const int MAX_SUBDIVIDE = 5;

struct DrawData {
    glm::mat4 transformation;
    glm::vec3 color;
};

struct ModelData
{
    unsigned int glVao;
    unsigned int glVertexData;
    unsigned int glSolidIndexData;
    size_t solidDataLength;
    glm::vec3 offset;
    glm::vec3 size;
};

void draw_model(const ModelData& model, Shader& s, const DrawData& draw_data, const glm::vec3& light_pos, const glm::vec3& camera_pos);
ModelData load_model(const Mesh& mesh);