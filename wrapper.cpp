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

void draw_model(const ModelData& model, Shader& s, const DrawData& draw_data, const glm::vec3& light_pos, const glm::vec3& camera_pos, const DrawMode& draw_mode)
{
    glBindVertexArray(model.glVao);
    glBindBuffer(GL_ARRAY_BUFFER, model.glVertexData);
    //glPolygonOffset(500.0f, 1.0f);

    s.Bind();
    s.SetUniformMat4f("u_Transformation", draw_data.transformation);
    s.SetUniformVec3f("u_LightPosition", light_pos);
    s.SetUniformVec3f("u_CameraPosition", camera_pos);
    s.SetUniformVec3f("u_Color", draw_data.color);

    if (draw_mode == Solid) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.glSolidIndexData);
        glDrawElements(GL_TRIANGLES, model.solidDataLength, GL_UNSIGNED_INT, 0);
    }
    else if (draw_mode == Line) {
        glLineWidth(2.0f);
        glColorMask(false, false, false, false);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.glSolidIndexData);
        glDrawElements(GL_TRIANGLES, model.solidDataLength, GL_UNSIGNED_INT, 0);
        glColorMask(true, true, true, true);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.glLineIndexData);
        glDrawElements(GL_LINES, model.lineDataLength, GL_UNSIGNED_INT, 0);
    }
    else {
        // wire
        glDisable(GL_CULL_FACE);
        glLineWidth(1.0f);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.glLineIndexData);
        glDrawElements(GL_LINES, model.lineDataLength, GL_UNSIGNED_INT, 0);
    }


}

ModelData load_model(const Mesh& input)
{
    Mesh mesh;
    std::vector<int> line_indexes;
    if (input.tris.size() > 0) {
        for (const auto& tri : input.tris) {
            auto normal = tn(input.verts[tri.vi1], input.verts[tri.vi2], input.verts[tri.vi3]);
            mesh.verts.push_back(input.verts[tri.vi1]);
            mesh.verts.push_back(input.verts[tri.vi2]);
            mesh.verts.push_back(input.verts[tri.vi3]);
            mesh.normals.push_back(normal);
            mesh.normals.push_back(normal);
            mesh.normals.push_back(normal);
            auto s = mesh.verts.size();
            mesh.tris.push_back(Triangle(s - 1, s - 2, s - 3));
            line_indexes.push_back(s - 1);
            line_indexes.push_back(s - 2);
            line_indexes.push_back(s - 2);
            line_indexes.push_back(s - 3);
            line_indexes.push_back(s - 3);
            line_indexes.push_back(s - 1);
        }
    }
    else {
        // for quad also triangulate
        for (const auto& q : input.quads) {
            auto normal = qn(input.verts[q.vi1], input.verts[q.vi2], input.verts[q.vi3], input.verts[q.vi4]);
            mesh.verts.push_back(input.verts[q.vi1]);
            mesh.verts.push_back(input.verts[q.vi2]);
            mesh.verts.push_back(input.verts[q.vi4]);
            auto s = mesh.verts.size();
            mesh.tris.push_back(Triangle(s - 1, s - 2, s - 3));
            mesh.normals.push_back(normal);
            mesh.normals.push_back(normal);
            mesh.normals.push_back(normal);

            mesh.verts.push_back(input.verts[q.vi3]);
            mesh.verts.push_back(input.verts[q.vi4]);
            mesh.verts.push_back(input.verts[q.vi2]);
            s = mesh.verts.size();
            mesh.tris.push_back(Triangle(s - 1, s - 2, s - 3));
            mesh.normals.push_back(normal);
            mesh.normals.push_back(normal);
            mesh.normals.push_back(normal);

            s = mesh.verts.size();
            line_indexes.push_back(s - 6);
            line_indexes.push_back(s - 5);
            line_indexes.push_back(s - 5);
            line_indexes.push_back(s - 3);
            line_indexes.push_back(s - 4);
            line_indexes.push_back(s - 3);
            line_indexes.push_back(s - 6);
            line_indexes.push_back(s - 4);
        }
    }

    ModelData model{};

    int gVertexDataSizeInBytes = mesh.verts.size() * 3 * sizeof(float);
    int gNormalDataSizeInBytes = mesh.normals.size() * 3 * sizeof(float);

    glGenVertexArrays(1, &model.glVao);
    glBindVertexArray(model.glVao);
    glGenBuffers(1, &model.glVertexData);
    glGenBuffers(1, &model.glSolidIndexData);
    glGenBuffers(1, &model.glLineIndexData);

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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.glLineIndexData);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, line_indexes.size() * sizeof(int), line_indexes.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));
    glEnableVertexAttribArray(1);

    assert(glGetError() == GL_NONE);

    model.solidDataLength = mesh.tris.size() * 3;
    model.lineDataLength = line_indexes.size();
    return model;
}