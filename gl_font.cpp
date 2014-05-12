

#include <cstdarg>
#include <vector>

#include "gl_font.h"

// Taken from: wingdi.h
#if !defined(CLEARTYPE_QUALITY)
#define CLEARTYPE_QUALITY 5
#endif

int GLFont::m_logPixelsY = 0;
BYTE GLFont::m_lfQuality = 0;
char GLFont::m_szBuffer[MAX_STR_SIZE] = {0};

GLFont::GLFont()
{
    m_name.clear();
    m_dropShadowOffset = 1;
    m_pointSize = 0;
    m_cellHeight = 0;
    m_cellWidth = 0;
    m_charHeight = 0;
    m_charAvgWidth = 0;
    m_charMaxWidth = 0;
    m_numCharsToDraw = 0;
    m_fontTexture = 0;
    m_vertexBuffer = 0;
    m_drawDropShadows = false;
    m_color[0] = m_color[1] = m_color[2] = m_color[3] = 1.0f;
    m_pVertex = 0;
    m_hFont = 0;

    memset(m_glyphs, 0, sizeof(m_glyphs));

    if (!m_logPixelsY)
    {
        HWND hWndDesktop = GetDesktopWindow();
        HDC hDC = GetDCEx(hWndDesktop, 0, DCX_CACHE | DCX_WINDOW);

        if (hDC)
        {
            m_logPixelsY = GetDeviceCaps(hDC, LOGPIXELSY);
            ReleaseDC(hWndDesktop, hDC);
        }
    }

    if (!m_lfQuality)
    {
        DWORD dwVersion = GetVersion();
        DWORD dwMajorVersion = static_cast<DWORD>((LOBYTE(LOWORD(dwVersion))));
        DWORD dwMinorVersion = static_cast<DWORD>((HIBYTE(LOWORD(dwVersion))));

        // Windows XP and higher will support ClearType quality fonts.
        if (dwMajorVersion >= 6 || (dwMajorVersion == 5 && dwMinorVersion == 1))
            m_lfQuality = CLEARTYPE_QUALITY;
        else
            m_lfQuality = ANTIALIASED_QUALITY;
    }
}

GLFont::~GLFont()
{
    destroy();
}

bool GLFont::create(const char *pszName, int ptSize, Style style)
{
    m_name = pszName;
    m_pointSize = ptSize;

    m_lf.lfHeight = -MulDiv(ptSize, m_logPixelsY, 72);
    m_lf.lfWidth = 0;
    m_lf.lfEscapement = 0;
    m_lf.lfOrientation = 0;
    m_lf.lfWeight = (style == BOLD) ? FW_BOLD : FW_NORMAL;
    m_lf.lfItalic = FALSE;
    m_lf.lfUnderline = FALSE;
    m_lf.lfStrikeOut = FALSE;
    m_lf.lfCharSet = ANSI_CHARSET;
    m_lf.lfOutPrecision = OUT_STROKE_PRECIS;
    m_lf.lfClipPrecision = CLIP_STROKE_PRECIS;
    m_lf.lfQuality = m_lfQuality;
    m_lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

    if (m_name.length() < LF_FACESIZE)
        strcpy(m_lf.lfFaceName, pszName);
    else
        m_lf.lfFaceName[0] = '\0';

    HFONT hNewFont = CreateFontIndirect(&m_lf);

    if (!hNewFont)
        return false;

    if (m_hFont)
    {
        DeleteObject(m_hFont);
        m_hFont = 0;
    }

    m_hFont = hNewFont;

    if (!createFontBitmap())
    {
        destroy();
        return false;
    }

    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_VERTICES, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

void GLFont::destroy()
{
    if (m_hFont)
    {
        DeleteObject(m_hFont);
        m_hFont = 0;
    }

    if (m_fontTexture)
    {
        glDeleteTextures(1, &m_fontTexture);
        m_fontTexture = 0;
    }

    if (m_vertexBuffer)
    {
        glDeleteBuffers(1, &m_vertexBuffer);
        m_vertexBuffer = 0;
    }
}

void GLFont::begin()
{
    HWND hWnd = GetForegroundWindow();
    RECT rcClient;

    GetClientRect(hWnd, &rcClient);

    int w = rcClient.right - rcClient.left;
    int h = rcClient.bottom - rcClient.top;

    glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);

    glDisable(GL_LIGHTING);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_fontTexture);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0f, w, h, 0.0f, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

    drawTextBegin();
}

void GLFont::drawChar(char c, int x, int y)
{
    //  1------4
    //  |      |            1 = (x, y)
    //  |      |            2 = (x, y + charHeight)
    //  |      |            3 = (x + charWidth, y + charHeight)
    //  |      |            4 = (x + charWidth, y)
    //  |      |
    //  |      |
    //  2------3
    //

    const Glyph &glyph = getChar(c);
    int charWidth = glyph.width;
    int charHeight = m_charHeight;

    if (m_drawDropShadows)
    {
        // 1
        m_pVertex->x = x + m_dropShadowOffset;
        m_pVertex->y = y + m_dropShadowOffset;
        m_pVertex->s = glyph.upperLeft[0];
        m_pVertex->t = glyph.upperLeft[1];
        m_pVertex->r = 0.0f;
        m_pVertex->g = 0.0f;
        m_pVertex->b = 0.0f;
        m_pVertex->a = 1.0f;
        ++m_pVertex;

        // 2
        m_pVertex->x = x + m_dropShadowOffset;
        m_pVertex->y = y + charHeight + m_dropShadowOffset;
        m_pVertex->s = glyph.lowerLeft[0];
        m_pVertex->t = glyph.lowerLeft[1];
        m_pVertex->r = 0.0f;
        m_pVertex->g = 0.0f;
        m_pVertex->b = 0.0f;
        m_pVertex->a = 1.0f;
        ++m_pVertex;

        // 3
        m_pVertex->x = x + charWidth + m_dropShadowOffset;
        m_pVertex->y = y + charHeight + m_dropShadowOffset;
        m_pVertex->s = glyph.lowerRight[0];
        m_pVertex->t = glyph.lowerRight[1];
        m_pVertex->r = 0.0f;
        m_pVertex->g = 0.0f;
        m_pVertex->b = 0.0f;
        m_pVertex->a = 1.0f;
        ++m_pVertex;

        // 4
        m_pVertex->x = x + charWidth + m_dropShadowOffset;
        m_pVertex->y = y + m_dropShadowOffset;
        m_pVertex->s = glyph.upperRight[0];
        m_pVertex->t = glyph.upperRight[1];
        m_pVertex->r = 0.0f;
        m_pVertex->g = 0.0f;
        m_pVertex->b = 0.0f;
        m_pVertex->a = 1.0f;
        ++m_pVertex;

        if (++m_numCharsToDraw == MAX_CHARS_PER_BATCH)
        {
            drawTextEnd();
            drawBatchOfChars();
            drawTextBegin();
        }
    }

    // 1
    m_pVertex->x = x;
    m_pVertex->y = y;
    m_pVertex->s = glyph.upperLeft[0];
    m_pVertex->t = glyph.upperLeft[1];
    m_pVertex->r = m_color[0];
    m_pVertex->g = m_color[1];
    m_pVertex->b = m_color[2];
    m_pVertex->a = m_color[3];
    ++m_pVertex;

    // 2
    m_pVertex->x = x;
    m_pVertex->y = y + charHeight;
    m_pVertex->s = glyph.lowerLeft[0];
    m_pVertex->t = glyph.lowerLeft[1];
    m_pVertex->r = m_color[0];
    m_pVertex->g = m_color[1];
    m_pVertex->b = m_color[2];
    m_pVertex->a = m_color[3];
    ++m_pVertex;

    // 3
    m_pVertex->x = x + charWidth;
    m_pVertex->y = y + charHeight;
    m_pVertex->s = glyph.lowerRight[0];
    m_pVertex->t = glyph.lowerRight[1];
    m_pVertex->r = m_color[0];
    m_pVertex->g = m_color[1];
    m_pVertex->b = m_color[2];
    m_pVertex->a = m_color[3];
    ++m_pVertex;

    // 4
    m_pVertex->x = x + charWidth;
    m_pVertex->y = y;
    m_pVertex->s = glyph.upperRight[0];
    m_pVertex->t = glyph.upperRight[1];
    m_pVertex->r = m_color[0];
    m_pVertex->g = m_color[1];
    m_pVertex->b = m_color[2];
    m_pVertex->a = m_color[3];
    ++m_pVertex;

    if (++m_numCharsToDraw == MAX_CHARS_PER_BATCH)
    {
        drawTextEnd();
        drawBatchOfChars();
        drawTextBegin();
    }
}

void GLFont::end()
{
    drawTextEnd();

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glPopAttrib();
}

void GLFont::drawText(int x, int y, const char *pszText)
{
    char prevCh = 0;
    char ch = 0;
    int dx = x;
    int dy = y;
    int charHeight = getCellHeight();
    int whitespaceWidth = getChar(' ').width;

    while (*pszText != '\0')
    {
        prevCh = ch;
        ch = *pszText++;

        if (ch == ' ')
        {
            if (prevCh != '\r')
                dx += whitespaceWidth;
        }
        else if (ch == '\n' || ch == '\r')
        {
            dx = x;
            dy += charHeight;
        }
        else if (ch == '\t')
        {
            dx += whitespaceWidth * TAB_SPACES;
        }
        else if (ch >= CHAR_FIRST && ch <= CHAR_LAST)
        {
            drawChar(ch, dx, dy);
            dx += getChar(ch).width;
        }
    }
}

void GLFont::drawTextFormat(int x, int y, const char *pszFmt, ...)
{
    va_list vlist;

    va_start(vlist, pszFmt);
    _vsnprintf(m_szBuffer, MAX_STR_SIZE, pszFmt, vlist);
    va_end(vlist);

    drawText(x, y, m_szBuffer);
}

int GLFont::getStrWidth(const char *pszText) const
{
    if (!pszText)
        return 0;

    int width = 0;

    while (*pszText != '\0')
        width += getCharWidth(*pszText++);

    return width;
}

int GLFont::getStrWidth(const char *pChar, int length) const
{
    if (!pChar || length <= 0)
        return 0;

    int width = 0;

    for (int i = 0; i < length; ++i)
        width += getCharWidth(pChar[i]);

    return width;
}

void GLFont::setColor(float r, float g, float b)
{
    m_color[0] = r;
    m_color[1] = g;
    m_color[2] = b;
    m_color[3] = 1.0f;
}

void GLFont::setDrawDropShadows(bool enableShadows)
{
    m_drawDropShadows = enableShadows;
}

void GLFont::setDropShadowOffset(int offset)
{
    m_dropShadowOffset = offset;
}

bool GLFont::createTexture(const Bitmap &bitmap)
{
    int w = bitmap.width;
    int h = bitmap.height;

    std::vector<unsigned char> pixels;
    pixels.resize(w * h);

    bitmap.copyBytesAlpha8Bit(&pixels[0]);

    glGenTextures(1, &m_fontTexture);
    glBindTexture(GL_TEXTURE_2D, m_fontTexture);

    // Only use GL_NEAREST filtering for the min and mag filter. Using anything
    // else will cause the font glyphs to be blurred. Using only GL_NEAREST
    // filtering ensures that edges of the font glyphs remain crisp and sharp.

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA,
        GL_UNSIGNED_BYTE, &pixels[0]);

    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

bool GLFont::createFontBitmap()
{
    // The font is drawn as a 10 x 10 grid of characters.

    if (!extractFontMetrics())
        return false;

    int w = 10 * m_cellWidth;
    int h = 10 * m_cellHeight;
    Bitmap bitmap;

    if (!bitmap.create(nextPower2(10 * m_cellWidth), nextPower2(10 * m_cellHeight)))
        return false;

    int x = 0;
    int y = 0;
    int ch = 32;
    HFONT hPrevFont = 0;
    COLORREF prevColor = 0;
    RECT rc = {0, 0, bitmap.width, bitmap.height};

    bitmap.selectObject();
    hPrevFont = reinterpret_cast<HFONT>(SelectObject(bitmap.dc, m_hFont));
    prevColor = SetTextColor(bitmap.dc, RGB(255,255,255));
    SetBkMode(bitmap.dc, TRANSPARENT);
    FillRect(bitmap.dc, &rc, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

    for (int i = 0; i < 10; ++i)
    {
        y = m_cellHeight * i;

        for (int j = 0; j < 10; ++j)
        {
            x = m_cellWidth * j;

            if (ch > 31 && ch < 127)
                TextOut(bitmap.dc, x, y, reinterpret_cast<LPCSTR>(&ch), 1);

            ++ch;
        }
    }

    SetTextColor(bitmap.dc, prevColor);
    SelectObject(bitmap.dc, hPrevFont);
    bitmap.deselectObject();

    generateTexCoords(bitmap);
    return createTexture(bitmap);
}

void GLFont::drawBatchOfChars()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2, GL_INT, sizeof(Vertex), BUFFER_OFFSET(0));
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 2));
    glColorPointer(4, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 4));

    glDrawArrays(GL_QUADS, 0, m_numCharsToDraw * 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void GLFont::drawTextBegin()
{
    m_numCharsToDraw = 0;
    m_pVertex = reinterpret_cast<Vertex *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
}

void GLFont::drawTextEnd()
{
    if (m_pVertex)
    {
        glUnmapBuffer(GL_ARRAY_BUFFER);
        m_pVertex = 0;
    }

    if (m_numCharsToDraw > 0)
        drawBatchOfChars();
}

bool GLFont::extractFontMetrics()
{
    HWND hWndDesktop = GetDesktopWindow();
    HDC hDC = GetDCEx(hWndDesktop, 0, DCX_CACHE | DCX_WINDOW);

    if (!hDC)
        return false;

    HFONT hPrevFont = reinterpret_cast<HFONT>(SelectObject(hDC, m_hFont));
    TEXTMETRIC tm;
    SIZE charSize = {0};
    char szString[2] = {0};
    char szName[128] = {0};

    GetTextFace(hDC, 128, szName);
    m_name = szName;

    GetTextMetrics(hDC, &tm);
    m_charHeight = m_cellHeight = tm.tmHeight + tm.tmExternalLeading;
    m_charMaxWidth = 0;
    m_charAvgWidth = 0;

    for (int c = 32; c < 127; ++c)
    {
        szString[0] = c;
        GetTextExtentPoint32(hDC, szString, 1, &charSize);

        if (charSize.cx > m_charMaxWidth)
            m_charMaxWidth = charSize.cx;

        m_charAvgWidth += charSize.cx;
        m_glyphs[c - 32].width = charSize.cx;
    }

    m_charAvgWidth /= TOTAL_CHARS;
    m_cellWidth = m_charMaxWidth + (m_charAvgWidth / 2);

    SelectObject(hDC, hPrevFont);
    ReleaseDC(hWndDesktop, hDC);
    return true;
}

void GLFont::generateTexCoords(const Bitmap &bitmap)
{
    Glyph *pGlyph = 0;
    int col = 0;
    int row = 0;
    int charWidth = 0;
    float bmpWidth = static_cast<float>(bitmap.width);
    float bmpHeight = static_cast<float>(bitmap.height);

    for (int c = 32; c < 127; ++c)
    {
        col = (c - 32) % 10;
        row = (c - 32) / 10;

        pGlyph = &m_glyphs[c - 32];
        charWidth = pGlyph->width;

        pGlyph->upperLeft[0] = (col * m_cellWidth) / bmpWidth;
        pGlyph->upperLeft[1] = (row * m_cellHeight) / bmpHeight;

        pGlyph->lowerLeft[0] = (col * m_cellWidth) / bmpWidth;
        pGlyph->lowerLeft[1] = ((row + 1) * m_cellHeight) / bmpHeight;

        pGlyph->lowerRight[0] = ((col * m_cellWidth) + charWidth) / bmpWidth;
        pGlyph->lowerRight[1] = ((row + 1) * m_cellHeight) / bmpHeight;

        pGlyph->upperRight[0] = ((col * m_cellWidth) + charWidth) / bmpWidth;
        pGlyph->upperRight[1] = (row * m_cellHeight) / bmpHeight;
    }
}

int GLFont::nextPower2(int x) const
{
    int i = x & (~x + 1);

    while (i < x)
        i <<= 1;

    return i;
}