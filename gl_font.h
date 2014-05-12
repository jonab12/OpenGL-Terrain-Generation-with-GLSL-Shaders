

#if !defined(GL_FONT_H)
#define GL_FONT_H

#include <windows.h>
#include <GL/gl.h>
#include <cassert>
#include <string>

#include "bitmap.h"
#include "opengl.h"

//-----------------------------------------------------------------------------
// This GLFont class draws text as a bunch of textured quads. The GLFont class
// only stores the displayable ASCII characters in the range ASCII 32 to ASCII
// 126 inclusive. This range represents the displayable ASCII characters.
// ClearType text smoothing is used if the code is running under WinXP or
// higher. Otherwise standard text anti-aliasing is used.
//
// To use the GLFont class:
//  GLFont font;
//  font.create("Arial", 12, GLFont::NORMAL);
//  font.begin();
//  font.setColor(1.0f, 0.0f, 0.0f);
//  font.drawTextFormat(1, 1, "%s", "Hello, World!");
//  font.end();
//  font.destroy();
//-----------------------------------------------------------------------------

class GLFont
{
public:
    enum Style
    {
        NORMAL,
        BOLD
    };

    struct Glyph
    {
        int width;
        float upperLeft[2];
        float lowerLeft[2];
        float upperRight[2];
        float lowerRight[2];
    };

    GLFont();
    ~GLFont();

    bool create(const char *pszName, int ptSize, Style style = NORMAL);
    void destroy();

    void begin();
    void end();

    void drawChar(char c, int x, int y);
    void drawText(int x, int y, const char *pszText);
    void drawTextFormat(int x, int y, const char *pszFmt, ...);

    int getCellHeight() const;
    int getCellWidth() const;
    const Glyph &getChar(char ch) const;
    int getCharHeight() const;
    int getCharWidthAvg() const;
    int getCharWidthMax() const;
    int getCharWidth(char ch) const;
    const char *getName() const;
    int getPointSize() const;
    int getStrWidth(const char *pszText) const;
    int getStrWidth(const char *pChar, int length) const;

    bool isNull() const;

    void setColor(float r, float g, float b);
    void setDrawDropShadows(bool enableShadows);
    void setDropShadowOffset(int offset);

private:
    GLFont(const GLFont &);
    GLFont &operator=(const GLFont &);

    struct Vertex
    {
        int x, y;
        float s, t;
        float r, g, b, a;
    };

    bool createTexture(const Bitmap &bitmap);
    bool createFontBitmap();
    void drawBatchOfChars();
    void drawTextBegin();
    void drawTextEnd();
    bool extractFontMetrics();
    void generateTexCoords(const Bitmap &bitmap);
    int nextPower2(int x) const;

    static const int CHAR_FIRST = 32;
    static const int CHAR_LAST = 126;
    static const int TAB_SPACES = 4;
    static const int TOTAL_CHARS = 95;

    static const int MAX_CHARS_PER_BATCH = 256;
    static const int MAX_STR_SIZE = 1024;
    static const int MAX_VERTICES = MAX_CHARS_PER_BATCH * 4;

    static int m_logPixelsY;
    static BYTE m_lfQuality;
    static char m_szBuffer[MAX_STR_SIZE];

    std::string m_name;
    int m_dropShadowOffset;
    int m_pointSize;
    int m_cellHeight;
    int m_cellWidth;
    int m_charHeight;
    int m_charAvgWidth;
    int m_charMaxWidth;
    int m_numCharsToDraw;
    GLuint m_fontTexture;
    GLuint m_vertexBuffer;
    bool m_drawDropShadows;
    float m_color[4];
    Vertex *m_pVertex;
    HFONT m_hFont;
    LOGFONT m_lf;
    Glyph m_glyphs[TOTAL_CHARS];
};

//-----------------------------------------------------------------------------

inline int GLFont::getCellHeight() const
{ return m_cellHeight; }

inline int GLFont::getCellWidth() const
{ return m_cellWidth; }

inline const GLFont::Glyph &GLFont::getChar(char ch) const
{
    assert(ch >= CHAR_FIRST && ch <= CHAR_LAST);
    return m_glyphs[ch - 32];
}

inline int GLFont::getCharHeight() const
{ return m_charHeight; }

inline int GLFont::getCharWidthAvg() const
{ return m_charAvgWidth; }

inline int GLFont::getCharWidthMax() const
{ return m_charMaxWidth; }

inline int GLFont::getCharWidth(char ch) const
{
    assert(ch >= CHAR_FIRST && ch <= CHAR_LAST);
    return m_glyphs[ch - 32].width;
}

inline const char *GLFont::getName() const
{ return m_name.c_str(); }

inline int GLFont::getPointSize() const
{ return m_pointSize; }

inline bool GLFont::isNull() const
{ return !m_hFont; }

#endif