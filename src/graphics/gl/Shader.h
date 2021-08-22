#ifndef SHADER_H
#define SHADER_H

#include <vector>

#include "NonCopyable.h"
#include "graphics/gl/Glew.h"
#include "math/Matrix.h"
#include "math/Vector.h"

namespace GL {
    class Shader final : private NonCopyable {
      public:
        struct Options final {
            const char* vertexPath;
            const char* fragmentPath;
        };

        Shader();
        ~Shader();
        bool compile(const Options& options);

        void use();
        void setFloat(const char* name, float f);
        void setVector(const char* name, Vector v);
        void setMatrix(const char* name, const Matrix& matrix);

      private:
        bool readFile(std::vector<GLchar>& code, const char* path) const;
        bool compileShader(const char* path, GLenum shaderType, GLuint& shader);

        GLuint vertex;
        GLuint fragment;
        GLuint program;
    };
}

#endif
