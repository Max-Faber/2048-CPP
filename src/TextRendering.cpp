#include <TextRendering.h>

// This Function Gets The First Power Of 2 >= The Int That We Pass It.
inline int next_p2(int a)
{
    int rval = 1;

    while (rval < a) rval *= 2;
    return rval;
}

void fontData::init(std::string fontPath)
{
    FT_Library library;

    // Allocate Some Memory To Store The Texture Ids.
    textures = new GLuint[128];
    if (FT_Init_FreeType(&library)) throw std::runtime_error("FT_Init_FreeType failed");
    if (FT_New_Face(library, fontPath.c_str(), 0, &face))
        throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)");
    glGenTextures(128, textures);
}

std::tuple<int, int> fontData::setFontSize(std::string text)
{
    std::tuple<int, int> charDimensions = { 0, 0 };
    unsigned int textSize = text.size();
    int fontSize = calcFontSize(text);

    this->h = (float)fontSize;
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    for (int i = 0; i < textSize; i++)
    {
        std::tuple<int, int> t = makeDList(face, text[i], list_base, textures);
        int width;

        width = std::get<0>(t);
        charDimensions = {
                std::get<0>(charDimensions) + width,
                std::max(std::get<1>(charDimensions), std::get<1>(t))
        };
    }
    return charDimensions;
}

int fontData::calcFontSize(const std::string& text)
{
    float calculatedFontSize = (float)fontData::fontSize;
    unsigned int textToRenderSize = text.size();

    if (textToRenderSize >= 3) calculatedFontSize *= 2.75f / (float)textToRenderSize;
    return (int)round(calculatedFontSize);
}

// Create A Display List Corresponding To The Given Character.
std::tuple<int, int> fontData::makeDList(FT_Face face, char ch, GLuint list_base, GLuint* tex_base)
{
    // The First Thing We Do Is Get FreeType To Render Our Character
    // Into A Bitmap.  This Actually Requires A Couple Of FreeType Commands:
    // Load The Glyph For Our Character.
    if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT))
        throw std::runtime_error("FT_Load_Glyph failed");

    // Move The Face's Glyph Into A Glyph Object.
    FT_Glyph glyph;
    if (FT_Get_Glyph(face->glyph, &glyph)) throw std::runtime_error("FT_Get_Glyph failed");

    // Convert The Glyph To A Bitmap.
    FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, nullptr, 1);
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

    // This Reference Will Make Accessing The Bitmap Easier.
    FT_Bitmap& bitmap = bitmap_glyph->bitmap;
    // Use Our Helper Function To Get The Widths Of
    // The Bitmap Data That We Will Need In Order To Create
    // Our Texture.
    int width = next_p2((int)bitmap.width);
    int height = next_p2((int)bitmap.rows);

    // Allocate Memory For The Texture Data.
    GLubyte* expanded_data = new GLubyte[2 * width * height];

    // Here We Fill In The Data For The Expanded Bitmap.
    // Notice That We Are Using A Two Channel Bitmap (One For
    // Channel Luminosity And One For Alpha), But We Assign
    // Both Luminosity And Alpha To The Value That We
    // Find In The FreeType Bitmap.
    // We Use The ?: Operator To Say That Value Which We Use
    // Will Be 0 If We Are In The Padding Zone, And Whatever
    // Is The FreeType Bitmap Otherwise.
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            expanded_data[2 * (i + j * width)] = expanded_data[2 * (i + j * width) + 1] =
                    (i >= bitmap.width || j >= bitmap.rows) ?
                    0 : bitmap.buffer[i + bitmap.width * j];
        }
    }
    // Now We Just Setup Some Texture Parameters.
    glBindTexture(GL_TEXTURE_2D, tex_base[ch]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Here We Actually Create The Texture Itself, Notice
    // That We Are Using GL_LUMINANCE_ALPHA To Indicate That
    // We Are Using 2 Channel Data.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data);

    // With The Texture Created, We Don't Need The Expanded Data Anymore.
    delete[] expanded_data;
    // Now We Create The Display List
    glNewList(list_base + ch, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, tex_base[ch]);
    glPushMatrix();

    // First We Need To Move Over A Little So That
    // The Character Has The Right Amount Of Space
    // Between It And The One Before It.
    glTranslatef((float)bitmap_glyph->left, 0, 0);

    // Now We Move Down A Little In The Case That The
    // Bitmap Extends Past The Bottom Of The Line
    // This Is Only True For Characters Like 'g' Or 'y'.
    glTranslatef(0, (float)bitmap_glyph->top - (float)bitmap.rows, 0);

    // Now We Need To Account For The Fact That Many Of
    // Our Textures Are Filled With Empty Padding Space.
    // We Figure What Portion Of The Texture Is Used By
    // The Actual Character And Store That Information In
    // The x And y Variables, Then When We Draw The
    // Quad, We Will Only Reference The Parts Of The Texture
    // That Contains The Character Itself.
    float x = (float)bitmap.width / (float)width;
    float y = (float)bitmap.rows / (float)height;

    // Here We Draw The Texture mapped Quads.
    // The Bitmap That We Got From FreeType Was Not
    // Oriented Quite Like We Would Like It To Be,
    // But We Link The Texture To The Quad
    // In Such A Way That The Result Will Be Properly Aligned.
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0); glVertex2f(0, (float)bitmap.rows);
    glTexCoord2d(0, y); glVertex2f(0, 0);
    glTexCoord2d(x, y); glVertex2f((float)bitmap.width, 0);
    glTexCoord2d(x, 0); glVertex2f((float)bitmap.width, (float)bitmap.rows);
    glEnd();
    glPopMatrix();
    glTranslatef((float)(face->glyph->advance.x >> 6), 0, 0);

    // Finish The Display List
    glEndList();
    width = (int)glyph->advance.x >> 16, height = (int)bitmap.rows;
    FT_Done_Glyph(glyph);
    return { width, height };
}

// A Fairly Straightforward Function That Pushes
// A Projection Matrix That Will Make Object World
// Coordinates Identical To Window Coordinates.
inline void pushScreenCoordinateMatrix(int curWidth, int curHeight)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, curWidth, 0, curHeight, -1, 1);
}

// Pops The Projection Matrix Without Changing The Current MatrixMode.
inline void pop_projection_matrix()
{
    glPushAttrib(GL_TRANSFORM_BIT);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
}

std::vector<std::string> TextRendering::splitString(const std::string& input, char splitChar)
{
    std::vector<std::string> stringSplitList;
    std::istringstream f(input);
    std::string stringSplit;

    while (getline(f, stringSplit, splitChar)) stringSplitList.push_back(stringSplit);
    return stringSplitList;
}

void TextRendering::renderText(fontData* ft_font, float x, float y, std::string text, int curWidth, int curHeight)
{   // We Want A Coordinate System Where Distance Is Measured In Window Pixels.
    float modelviewMatrix[16];
    GLuint font = ft_font->list_base;
    float h = ft_font->h / .63f; // We Make The Height A Little Bigger. There Will Be Some Space Between Lines.
    std::vector<std::string> lines = splitString(text, '\n');
    glm::vec2 pos = glm::vec2(x, y);

    std::tuple<int, int> charDimensions = ft_font->setFontSize(text);
    pushScreenCoordinateMatrix(curWidth, curHeight);
    glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glListBase(font);
    glGetFloatv(GL_MODELVIEW_MATRIX, modelviewMatrix);

    float curAspectRatio = (float)curWidth / (float)curHeight;
    pos = (pos - glm::vec2(-curAspectRatio, -1)) / (glm::vec2(curAspectRatio, 1) - glm::vec2(-curAspectRatio, -1)); // Normalize
    pos *= glm::vec2(curWidth, curHeight); // Translate to screen coordinates

    for (int i = 0; i < lines.size(); i++)
    {
        glPushMatrix();
        glLoadIdentity();

        glTranslatef(pos.x - ((float)std::get<0>(charDimensions) / 2.f), pos.y - ((float)std::get<1>(charDimensions) / 2.f) - h * (float)i, 0);
        glMultMatrixf(modelviewMatrix);
        glCallLists((int)lines[i].length(), GL_UNSIGNED_BYTE, lines[i].c_str());
        glPopMatrix();
    }
    glPopAttrib();
    pop_projection_matrix();
}
