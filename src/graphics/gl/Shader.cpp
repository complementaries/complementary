#include "Shader.h"
#include "Utils.h"
#include <fstream>

#ifndef NDEBUG
GLuint GL::Shader::boundProgram = 0;
#endif

GL::Shader::Shader() : vertex(0), fragment(0), program(0) {
}

GL::Shader::~Shader() {
    if (vertex != 0) {
        glDeleteShader(vertex);
    }
    if (fragment != 0) {
        glDeleteShader(fragment);
    }
    if (program != 0) {
        glDeleteProgram(program);
    }
}

bool GL::Shader::readFile(std::vector<GLchar>& code, const char* path) const {
    std::ifstream in;
    in.open(path);
    if (!in.good()) {
        Utils::printError("cannot open shader file '%s'\n", path);
        return true;
    }
    while (in.good()) {
        GLchar c = in.get();
        if (c == EOF) {
            break;
        }
        code.push_back(c);
    }
    code.push_back('\0');
    return false;
}

bool GL::Shader::compileShader(const char* path, GLenum shaderType, GLuint& shader) {
    std::vector<GLchar> code;
    if (readFile(code, path)) {
        return true;
    }
    shader = glCreateShader(shaderType);
    GLchar* codeP = code.data();
    glShaderSource(shader, 1, &codeP, nullptr);
    glCompileShader(shader);

    GLint error;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &error);
    if (error == GL_FALSE) {
        GLchar buffer[256];
        glGetShaderInfoLog(shader, 256, nullptr, buffer);
        Utils::printError("cannot compile shader %s: %s\n", path, buffer);
        return true;
    }
    return false;
}

bool GL::Shader::compile(const GL::Shader::Options& options) {
    if (compileShader(options.vertexPath, GL_VERTEX_SHADER, vertex)) {
        return true;
    } else if (compileShader(options.fragmentPath, GL_FRAGMENT_SHADER, fragment)) {
        return true;
    }
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    GLint error;
    glGetProgramiv(program, GL_LINK_STATUS, &error);
    if (error == GL_FALSE) {
        GLchar buffer[256];
        glGetProgramInfoLog(program, 256, nullptr, buffer);
        Utils::printError("cannot link shader: %s\n", buffer);
        return true;
    }
#ifndef NDEBUG
    printf("(%s, %s) has program id %d\n", options.vertexPath, options.fragmentPath, program);
#endif
    return false;
}

void GL::Shader::use() {
#ifndef NDEBUG
    boundProgram = program;
#endif
    glUseProgram(program);
}

void GL::Shader::setFloat(const char* name, float f) {
#ifndef NDEBUG
    if (program != boundProgram) {
        fprintf(stderr, "setFloat on invalid shader: %d instead of %d\n", boundProgram, program);
    }
#endif
    glUniform1f(glGetUniformLocation(program, name), f);
}

void GL::Shader::setInt(const char* name, int i) {
#ifndef NDEBUG
    if (program != boundProgram) {
        fprintf(stderr, "setInt on invalid shader: %d instead of %d\n", boundProgram, program);
    }
#endif
    glUniform1i(glGetUniformLocation(program, name), i);
}

void GL::Shader::setVector(const char* name, Vector vec) {
#ifndef NDEBUG
    if (program != boundProgram) {
        fprintf(stderr, "setVector on invalid shader: %d instead of %d\n", boundProgram, program);
    }
#endif
    glUniform2f(glGetUniformLocation(program, name), vec.x, vec.y);
}

void GL::Shader::setMatrix(const char* name, const Matrix& matrix) {
#ifndef NDEBUG
    if (program != boundProgram) {
        fprintf(stderr, "setMatrix on invalid shader: %d instead of %d\n", boundProgram, program);
    }
#endif
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, false, matrix.getData());
}

#ifndef NDEBUG
bool GL::Shader::isBound() const {
    return program == boundProgram;
}
#endif