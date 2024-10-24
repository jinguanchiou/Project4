#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>

class Object3D {
public:
    std::vector<float> vertices;
    unsigned int VAO, VBO;
    glm::mat4 localTransform = glm::mat4(1.0f);
    Object3D* parent = nullptr;
    std::vector<Object3D*> children;

    void addChild(Object3D* child) {
        children.push_back(child);
        child->parent = this;
    }
    
    glm::mat4 getWorldTransform() {
        if (parent) {
            return parent->getWorldTransform() * localTransform;
        }
        return localTransform;
    }

    glm::vec3 calculateCenterPoint() {
        glm::vec3 center(0.0f);
        for (size_t i = 0; i < vertices.size(); i += 3) {
            center += glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
        }
        center /= (vertices.size() / 3);
        return center;
    }
    
    void rotateAroundPoint(float angle, const glm::vec3& axis, const glm::vec3& centerPoint) {
        localTransform = glm::translate(localTransform, centerPoint);
        localTransform = glm::rotate(localTransform, glm::radians(angle), axis);
        localTransform = glm::translate(localTransform, -centerPoint);
    }

    void scale(const glm::vec3& scaleFactor) {
        glm::mat4 parentScaleFactor = glm::mat4(1.0f);
        if (parent) {
            parentScaleFactor = localTransform / parent-> getWorldTransform();
            localTransform = glm::scale(parentScaleFactor, scaleFactor);
        }
        else
        {
            localTransform = glm::scale(localTransform, scaleFactor);
        }
    }

    void translate(const glm::vec3& translation) {
        localTransform = glm::translate(localTransform, translation);
    }

    virtual void render(Shader& shader) = 0;

    void cleanup() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};