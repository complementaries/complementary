#include "Font.h"

#include <SDL_image.h>
#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#pragma GCC diagnostic pop

#include "graphics/Buffer.h"
#include "graphics/RenderState.h"
#include "graphics/gl/Shader.h"
#include "graphics/gl/Texture.h"
#include "graphics/gl/VertexBuffer.h"

static GL::Shader shader;
static GL::VertexBuffer buffer;
static GL::Texture texture;

struct Character final {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int originX = 0;
    int originY = 0;
    int advance = 0;
};
static int fontSize = 0;
static int fontWidth = 0;
static int fontHeight = 0;
static int fontMaxOriginY = 0;
std::array<Character, 128> characters;

static float scale(float f) {
    return f / fontSize;
}

static bool hasMember(rapidjson::Document& document, const char* name) {
    if (!document.HasMember(name)) {
        fprintf(stderr, "font json has no field named '%s'\n", name);
        return true;
    }
    return false;
}

static bool checkForField(rapidjson::Document& document, const char* name, rapidjson::Type type) {
    if (hasMember(document, name)) {
        return true;
    } else if (document[name].GetType() != type) {
        fprintf(stderr, "font json field '%s' is not an int\n", name);
        return true;
    }
    return false;
}

static bool checkForField(rapidjson::Document& document, const char* name) {
    if (hasMember(document, name)) {
        return true;
    } else if (!document[name].IsInt()) {
        fprintf(stderr, "font json field '%s' is not an int\n", name);
        return true;
    }
    return false;
}

static bool parseGlobalField(rapidjson::Document& document, const char* name, int& i) {
    if (checkForField(document, name)) {
        return true;
    }
    i = document[name].GetInt();
    return false;
}

static bool readField(rapidjson::Value::ConstObject& o, const char* name, int& i) {
    if (!o.HasMember(name)) {
        fprintf(stderr, "font json character has no field named '%s'\n", name);
        return true;
    } else if (!o[name].IsInt()) {
        fprintf(stderr, "font json character field '%s' is not an int\n", name);
        return true;
    }
    i = o[name].GetInt();
    return false;
}

bool Font::init() {
    if (shader.compile({"assets/shaders/font.vs", "assets/shaders/font.fs"})) {
        return true;
    }
    buffer.init(GL::VertexBuffer::Attributes().addVector2().addVector2().addRGBA());
    texture.init();
    const char* path = "assets/font.png";
    SDL_Surface* font = IMG_Load(path);
    if (font == nullptr) {
        fprintf(stderr, "cannot load font file '%s': %s\n", path, IMG_GetError());
        return true;
    }
    texture.setData(font->w, font->h, font->pixels);
    SDL_FreeSurface(font);

    path = "assets/font.json";
    std::ifstream json;
    json.open(path);
    if (!json.good()) {
        fprintf(stderr, "cannot load font json '%s'\n", path);
        return true;
    }
    rapidjson::BasicIStreamWrapper wrapped(json);
    rapidjson::Document document;
    document.ParseStream(wrapped);
    if (document.HasParseError()) {
        fprintf(stderr, "cannot parse font json: %d\n", document.GetParseError());
        return true;
    } else if (parseGlobalField(document, "size", fontSize) ||
               parseGlobalField(document, "width", fontWidth) ||
               parseGlobalField(document, "height", fontHeight)) {
        return true;
    } else if (checkForField(document, "characters", rapidjson::Type::kObjectType)) {
        return true;
    }
    auto t = document["characters"].GetObject();
    for (const auto& w : t) {
        if (!w.name.IsString() || !w.value.IsObject()) {
            fprintf(stderr, "font json characters has invalid member\n");
            return true;
        }
        const char* name = w.name.GetString();
        if (name == nullptr) {
            fprintf(stderr, "font json character has invalid name\n");
            return true;
        }
        int index = name[0];
        rapidjson::Value::ConstObject intern = w.value.GetObject();
        if (readField(intern, "x", characters[index].x) ||
            readField(intern, "y", characters[index].y) ||
            readField(intern, "width", characters[index].width) ||
            readField(intern, "height", characters[index].height) ||
            readField(intern, "originX", characters[index].originX) ||
            readField(intern, "originY", characters[index].originY) ||
            readField(intern, "advance", characters[index].advance)) {
            return true;
        }
        fontMaxOriginY = std::max(fontMaxOriginY, characters[index].originY);
    }
    return false;
}

void Font::prepare() {
    shader.use();
    RenderState::setViewMatrix(shader);
}

void Font::draw(const Vector& pos, float size, Color color, const char* s) {
    texture.bindTo(0);
    static Buffer data;
    data.clear();
    int index = 0;

    float x = pos.x;
    float y = pos.y;

    while (s[index] != '\0') {
        const Character& c = characters[s[index] & 0x7F];
        float minX = x - size * scale(c.originX);
        float minY = y - size * scale(c.originY - fontMaxOriginY + 1);
        float maxX = minX + size * scale(c.width);
        float maxY = minY + size * scale(c.height);
        float minTexX = static_cast<float>(c.x) / fontWidth;
        float minTexY = static_cast<float>(c.y) / fontHeight;
        float maxTexX = static_cast<float>(c.x + c.width) / fontWidth;
        float maxTexY = static_cast<float>(c.y + c.height) / fontHeight;

        data.add(minX).add(minY).add(minTexX).add(minTexY).add(color);
        data.add(maxX).add(minY).add(maxTexX).add(minTexY).add(color);
        data.add(minX).add(maxY).add(minTexX).add(maxTexY).add(color);
        data.add(maxX).add(maxY).add(maxTexX).add(maxTexY).add(color);
        data.add(maxX).add(minY).add(maxTexX).add(minTexY).add(color);
        data.add(minX).add(maxY).add(minTexX).add(maxTexY).add(color);
        x += size * scale(c.advance);
        index++;
    }

    buffer.setData(data.getData(), data.getSize());
    buffer.drawTriangles(6 * index);
}

float Font::getWidth(float size, const char* s) {
    float width = 0.0f;
    int index = 0;
    while (s[index] != '\0') {
        const Character& c = characters[s[index] & 0x7F];
        width += scale(c.advance);
        index++;
    }
    width *= size;
    return width;
}