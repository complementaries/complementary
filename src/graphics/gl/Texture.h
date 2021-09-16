#ifndef TEXTURE_H
#define TEXTURE_H

#include "NonCopyable.h"
#include "graphics/gl/Glew.h"

namespace GL {
    class Texture final : private NonCopyable {
      public:
        Texture();
        ~Texture();

        void init();
        void setData(int width, int height, const void* data = nullptr);
        void bindTo(int index = 0) const;

      private:
        void bind() const;

        GLuint texture;
    };
}

#endif