#include "Shader.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Shader::Shader(const char* vSource, const char* fSource) {
    auto compile = [](unsigned int type, const char* src) {
        unsigned int sId = glCreateShader(type);
        glShaderSource(sId, 1, &src, nullptr);
        glCompileShader(sId);
        int success;
        glGetShaderiv(sId, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512];
            glGetShaderInfoLog(sId, 512, nullptr, log);
            std::cerr << "Shader compile error: " << log << std::endl;
        }
        return sId;
    };

    unsigned int vs = compile(GL_VERTEX_SHADER, vSource);
    unsigned int fs = compile(GL_FRAGMENT_SHADER, fSource);

    id = glCreateProgram();
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader() {
    if (id) glDeleteProgram(id);
}

void Shader::use() const {
    glUseProgram(id);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec3(const std::string& name, const glm::vec3& vec) const {
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(vec));
}