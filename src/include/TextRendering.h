#pragma once

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <cmath>

struct fontData
{
    FT_Face face;
    float h; // Holds The Height Of The Font.
    GLuint* textures; // Holds The Texture Id's
    GLuint list_base;  // Holds The First Display List Id
    unsigned int fontSize;

    // The Init Function Will Create A Font With
    // The Height fontSize From The File fontPath.
    void init(const char* fontPath);
    std::tuple<int, int> setFontSize(std::string text);
    int calcFontSize(const std::string& text);
    static std::tuple<int, int> makeDList(FT_Face face, char ch, GLuint list_base, GLuint* tex_base);
};

class TextRendering
{
private:
    static std::vector<std::string> splitString(const std::string& input, char splitChar);
public:
    static void renderText(fontData* ft_font, float x, float y, std::string text, int curWidth, int curHeight);
};
