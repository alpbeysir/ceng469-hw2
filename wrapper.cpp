#include "wrapper.h"

#include <iostream>

std::ostream& operator<<(std::ostream& os, const glm::mat4& m) {
    for(int i = 0; i < 4; i++) {
        os << "[" << m[i].x << "," << m[i].y << "," << m[i].z << "," << m[i].w << "]\n";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
    return os << "[" << v.x << "," << v.y << "," << v.z << "]";
}

void draw_model(const ModelData& model, Shader& s, const DrawData& draw_data, const glm::vec3& light_pos, const glm::vec3& camera_pos)
{
    glBindVertexArray(model.glVao);
    glBindBuffer(GL_ARRAY_BUFFER, model.glVertexData);

    s.Bind();
    s.SetUniformMat4f("u_Transformation", draw_data.transformation);
    s.SetUniformVec3f("u_LightPosition", light_pos);
    s.SetUniformVec3f("u_CameraPosition", camera_pos);
    s.SetUniformVec3f("u_Color", draw_data.color);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.glSolidIndexData);
    glDrawElements(GL_TRIANGLES, model.solidDataLength, GL_UNSIGNED_INT, 0);
}

ModelData load_model(const Mesh& input)
{
    Mesh mesh;  
    ModelData model{};

    int gVertexDataSizeInBytes = mesh.verts.size() * 3 * sizeof(float);
    int gNormalDataSizeInBytes = mesh.normals.size() * 3 * sizeof(float);

    glGenVertexArrays(1, &model.glVao);
    glBindVertexArray(model.glVao);
    glGenBuffers(1, &model.glVertexData);
    glGenBuffers(1, &model.glSolidIndexData);

    glBindBuffer(GL_ARRAY_BUFFER, model.glVertexData);
    glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, mesh.verts.data());
    glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, mesh.normals.data());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.glSolidIndexData);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.tris.size() * 3 * sizeof(int), mesh.tris.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));
    glEnableVertexAttribArray(1);

    assert(glGetError() == GL_NONE);

    model.solidDataLength = mesh.tris.size() * 3;
    return model;
}