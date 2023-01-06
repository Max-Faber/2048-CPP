#pragma once

// FreeType Headers
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <glm/vec2.hpp>


// OpenGL Headers
//#ifdef __APPLE_CC__
//    #define GL_SILENCE_DEPRECATION
//    #include <GLUT/glut.h>
//#else
//    #include <GL/glut.h>
//#endif

//#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Some STL Headers
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>

// Wrap Everything In A Namespace, That Way We Can Use A Common
// Function Name Like "renderText" Without Worrying About
// Overlapping With Anyone Else's Code.
namespace freetype
{
    // This Holds All Of The Information Related To Any
    // FreeType Font That We Want To Create.
    struct font_data
    {
        FT_Face face;
        float h; // Holds The Height Of The Font.
        GLuint* textures; // Holds The Texture Id's
        GLuint list_base;  // Holds The First Display List Id

        // The Init Function Will Create A Font With
        // The Height fontSize From The File fontPath.
        void init(const char* fontPath);
        std::tuple<int, int> setFontSize(const char* text, unsigned int fontSize);
        // Free All The Resources Associated With The Font.
        void clean();
    };
    std::vector<std::string> splitString(const std::string& input, char splitChar);
    void renderText(font_data* ft_font, unsigned int fontSize, float x, float y, const char* text, int curWidth, int curHeight);
}
