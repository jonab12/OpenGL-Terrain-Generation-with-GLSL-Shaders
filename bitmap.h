

#if !defined(BITMAP_H)
#define BITMAP_H

#include <windows.h>
#include <tchar.h>

//-----------------------------------------------------------------------------
// 32-bit BGRA WIN32 device independent bitmap (DIB) class.
//
// Supports the loading of BMP, EMF, GIF, ICO, JPG, and WMF files using the
// WIN32 IPicture COM object.
//
// Also supports the loading of true color 24-bit and 32-bit TGA files, and
// grayscale 8-bit TGA files.
//
// Support is also provided for capturing a screen shot of the current Windows
// desktop and loading that as an image into the Bitmap class.
//
// This class stores the DIB in a top-down orientation. The pixel bytes are
// stored in the standard Windows DIB order of BGRA.
//
// All Windows DIBs are aligned to 4-byte (DWORD) memory boundaries. This means
// that each scan line is padded with extra bytes to ensure that the next scan
// line starts on a 4-byte memory boundary.
//
// To get a copy of the DIB that is BYTE (1-byte) aligned with all the extra
// padding bytes removed use the copyBytes() methods.
//-----------------------------------------------------------------------------
class Bitmap
{
public:
    HDC dc;
    HBITMAP hBitmap;
    int width;
    int height;
    int pitch;
    BITMAPINFO info;

    Bitmap();
    Bitmap(const Bitmap &bitmap);
    ~Bitmap();

    Bitmap &operator=(const Bitmap &bitmap);

    BYTE *operator[](int row) const
    { return &m_pBits[pitch * row]; }

    void blt(HDC hdcDest);
    void blt(HDC hdcDest, int x, int y);
    void blt(HDC hdcDest, int x, int y, int w, int h);
    void blt(HDC hdcDest, const RECT &rcDest, const RECT &rcSrc);

    bool clone(const Bitmap &bitmap);
    bool create(int widthPixels, int heightPixels);
    void destroy();

    void fill(int r, int g, int b, int a);
    void fill(float r, float g, float b, float a);
    
    BYTE *getPixel(int x, int y) const
    { return &m_pBits[(y * pitch) + (x * 4)]; }

    BYTE *getPixels() const
    { return m_pBits; }

    bool loadDesktop();
    bool loadBitmap(LPCTSTR pszFilename);
    bool loadPicture(LPCTSTR pszFilename);
    bool loadTarga(LPCTSTR pszFilename);
    
    bool saveBitmap(LPCTSTR pszFilename) const;
    bool saveTarga(LPCTSTR pszFilename) const;

    void selectObject();
    void deselectObject();

    void copyBytes24Bit(BYTE *pDest) const;
    void copyBytes32Bit(BYTE *pDest) const;

    void copyBytesAlpha8Bit(BYTE *pDest) const;
    void copyBytesAlpha32Bit(BYTE *pDest) const;
    
    void setPixels(const BYTE *pPixels, int w, int h, int bytesPerPixel);

    void flipHorizontal();
    void flipVertical();
    
    void resize(int newWidth, int newHeight);

private:
    DWORD createPixel(int r, int g, int b, int a) const;
    DWORD createPixel(float r, float g, float b, float a) const;
    
    static const int HIMETRIC_INCH = 2540; // matches constant in MFC CDC class

    static int m_logpixelsx;
    static int m_logpixelsy;

    HGDIOBJ m_hPrevObj;
    BYTE *m_pBits;
};

#endif