#define _USE_MATH_DEFINES
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "shader_s.h"
#include <glm/gtc/matrix_transform.hpp>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include <cmath>
#include <vector>
#include <iostream>

typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;
class Cube : public Object3D {
public:
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO, normalVBO;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 worldTransform;

    Cube(const glm::mat4& transform) {

        vertices = {

            -1.0f, -1.0f, -1.0f,  // 0
             1.0f, -1.0f, -1.0f,  // 1
             1.0f,  1.0f, -1.0f,  // 2
            -1.0f,  1.0f, -1.0f,  // 3
            -1.0f, -1.0f,  1.0f,  // 4
             1.0f, -1.0f,  1.0f,  // 5
             1.0f,  1.0f,  1.0f,  // 6
            -1.0f,  1.0f,  1.0f   // 7
        };
        indices = {
            0, 1, 2, 2, 3, 0,  // 後面
            4, 5, 6, 6, 7, 4,  // 前面
            4, 5, 1, 1, 0, 4,  // 底面
            7, 6, 2, 2, 3, 7,  // 頂面
            3, 0, 4, 4, 7, 3,  // 左面
            1, 5, 6, 6, 2, 1   // 右面
        };
        applyTransform(transform);
        worldTransform = transform;

        initBuffers();
    }
    void applyTransform(const glm::mat4& transform) {
        for (size_t i = 0; i < vertices.size(); i += 3) {
            glm::vec4 localPos(vertices[i], vertices[i + 1], vertices[i + 2], 1.0f);
            glm::vec4 worldPos = transform * localPos;
            vertices[i] = worldPos.x;
            vertices[i + 1] = worldPos.y;
            vertices[i + 2] = worldPos.z;
        }
    }
    void initBuffers() {
        MyMesh mesh;
        std::vector<MyMesh::VertexHandle> vhandle;

        for (size_t i = 0; i < vertices.size(); i += 3) {
            vhandle.push_back(mesh.add_vertex(MyMesh::Point(vertices[i], vertices[i + 1], vertices[i + 2])));
        }

        for (size_t i = 0; i < indices.size(); i += 3) {
            std::vector<MyMesh::VertexHandle> face_vhandles;
            face_vhandles.push_back(vhandle[indices[i]]);
            face_vhandles.push_back(vhandle[indices[i + 1]]);
            face_vhandles.push_back(vhandle[indices[i + 2]]);
            mesh.add_face(face_vhandles);
        }

        mesh.request_face_normals();
        mesh.request_vertex_normals();
        mesh.update_normals();

        std::vector<float> normals;
        for (const auto& vh : mesh.vertices()) {
            MyMesh::Normal normal = mesh.normal(vh);
            normals.push_back(normal[0]);
            normals.push_back(normal[1]);
            normals.push_back(normal[2]);
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &normalVBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
    

    // 平移立方體的函數
    

    void render(Shader& shader) {
        shader.setMat4("model", getWorldTransform());
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    void cleanup() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};