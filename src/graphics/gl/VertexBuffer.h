#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <vector>

#include "NonCopyable.h"
#include "graphics/gl/Glew.h"

namespace GL {
    class VertexBuffer final : private NonCopyable {
      public:
        class Attributes final {
            friend class VertexBuffer;

          public:
            Attributes& addFloat();
            Attributes& addVector2();
            Attributes& addVector3();
            Attributes& addVector4();
            Attributes& addRGBA();

          private:
            Attributes& addFloat(int count);

            struct Data final {
                GLenum type;
                GLboolean normalized;
                int count;
                int size;
            };
            std::vector<Data> data;
        };

        VertexBuffer();
        ~VertexBuffer();

        void init(const Attributes& a);

        void setData(const void* data, int length);
        void drawTriangles(int vertices) const;

      private:
        void bindBuffer() const;
        void bindArray() const;

        GLuint vertexArray;
        GLuint vertexBuffer;
    };
}

#endif
