#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int id = 0;

    Shader(const char* vertexSource, const char* fragmentSource);
    ~Shader();

    void use() const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec3(const std::string& name, const glm::vec3& vec) const;
};