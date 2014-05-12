

#include <cassert>
#include <cstring>
#include <cstdio>
#include "opengl.h"

bool OpenGLExtensionSupported(const char *pszExtensionName)
{
    static const char *pszGLExtensions = 0;
    static const char *pszWGLExtensions = 0;

    if (!pszGLExtensions)
        pszGLExtensions = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

    if (!pszWGLExtensions)
    {
        // WGL_ARB_extensions_string.

        typedef const char *(WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC);

        PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB =
            reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(
            wglGetProcAddress("wglGetExtensionsStringARB"));

        if (wglGetExtensionsStringARB)
            pszWGLExtensions = wglGetExtensionsStringARB(wglGetCurrentDC());
    }

    if (!strstr(pszGLExtensions, pszExtensionName))
    {
        if (!strstr(pszWGLExtensions, pszExtensionName))
            return false;
    }

    return true;
}

void OpenGLGetGLVersion(int &major, int &minor)
{
    static int majorGL = 0;
    static int minorGL = 0;

    if (!majorGL && !minorGL)
    {
        const char *pszVersion = reinterpret_cast<const char *>(glGetString(GL_VERSION));
        
        if (pszVersion)
            sscanf(pszVersion, "%d.%d", &majorGL, &minorGL);
    }
    
    major = majorGL;
    minor = minorGL;
}

void OpenGLGetGLSLVersion(int &major, int &minor)
{
    static int majorGLSL = 0;
    static int minorGLSL = 0;

    if (!majorGLSL && !minorGLSL)
    {
        int majorGL = 0;
        int minorGL = 0;

        OpenGLGetGLVersion(majorGL, minorGL);

        if (majorGL >= 2)
        {
            const char *pszShaderVersion = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));

            if (pszShaderVersion)
                sscanf(pszShaderVersion, "%d.%d", &majorGLSL, &minorGLSL);
        }
        else
        {
            const char *pszExtension = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

            if (pszExtension)
            {
                if (strstr(pszExtension, "GL_ARB_shading_language_100"))
                {
                    majorGLSL = 1;
                    minorGLSL = 0;
                }
            }
        }
    }

    major = majorGLSL;
    minor = minorGLSL;
}

bool OpenGLSupportsGLVersion(int major, int minor)
{
    static int majorGL = 0;
    static int minorGL = 0;

    if (!majorGL && !minorGL)
        OpenGLGetGLVersion(majorGL, minorGL);

    if (majorGL > major)
        return true;

    if (majorGL == major && minorGL >= minor)
        return true;

    return false;
}

bool OpenGLSupportsGLSLVersion(int major, int minor)
{
    static int majorGLSL = 0;
    static int minorGLSL = 0;

    if (!majorGLSL && !minorGLSL)
        OpenGLGetGLSLVersion(majorGLSL, minorGLSL);

    if (majorGLSL > major)
        return true;

    if (majorGLSL == major && minorGLSL >= minor)
        return true;

    return false;
}

#define LOAD_ENTRYPOINT(name, var, type) \
    if (!var) \
    { \
        var = reinterpret_cast<type>(wglGetProcAddress(name)); \
        assert(var != 0); \
    }

//
// OpenGL 1.2
//

void glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    typedef void (APIENTRY * PFNGLBLENDCOLORPROC) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    static PFNGLBLENDCOLORPROC pfnBlendColor = 0;
    LOAD_ENTRYPOINT("glBlendColor", pfnBlendColor, PFNGLBLENDCOLORPROC);
    pfnBlendColor(red, green, blue, alpha);
}

void glBlendEquation(GLenum mode)
{
    typedef void (APIENTRY * PFNGLBLENDEQUATIONPROC) (GLenum mode);
    static PFNGLBLENDEQUATIONPROC pfnBlendEquation = 0;
    LOAD_ENTRYPOINT("glBlendEquation", pfnBlendEquation, PFNGLBLENDEQUATIONPROC);
    pfnBlendEquation(mode);
}

void glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices)
{
    typedef void (APIENTRY * PFNGLDRAWRANGEELEMENTSPROC) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);
    static PFNGLDRAWRANGEELEMENTSPROC pfnDrawRangeElements = 0;
    LOAD_ENTRYPOINT("glDrawRangeElements", pfnDrawRangeElements, PFNGLDRAWRANGEELEMENTSPROC);
    pfnDrawRangeElements(mode, start, end, count, type, indices);
}

void glColorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table)
{
    typedef void (APIENTRY * PFNGLCOLORTABLEPROC) (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table);
    static PFNGLCOLORTABLEPROC pfnColorTable = 0;
    LOAD_ENTRYPOINT("glColorTable", pfnColorTable, PFNGLCOLORTABLEPROC);
    pfnColorTable(target, internalformat, width, format, type, table);
}

void glColorTableParameterfv(GLenum target, GLenum pname, const GLfloat *params)
{
    typedef void (APIENTRY * PFNGLCOLORTABLEPARAMETERFVPROC) (GLenum target, GLenum pname, const GLfloat *params);
    static PFNGLCOLORTABLEPARAMETERFVPROC pfnColorTableParameterfv = 0;
    LOAD_ENTRYPOINT("glColorTableParameterfv", pfnColorTableParameterfv, PFNGLCOLORTABLEPARAMETERFVPROC);
    pfnColorTableParameterfv(target, pname, params);
}

void glColorTableParameteriv(GLenum target, GLenum pname, const GLint *params)
{
    typedef void (APIENTRY * PFNGLCOLORTABLEPARAMETERIVPROC) (GLenum target, GLenum pname, const GLint *params);
    static PFNGLCOLORTABLEPARAMETERIVPROC pfnColorTableParameteriv = 0;
    LOAD_ENTRYPOINT("glColorTableParameteriv", pfnColorTableParameteriv, PFNGLCOLORTABLEPARAMETERIVPROC);
    pfnColorTableParameteriv(target, pname, params);
}

void glCopyColorTable(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    typedef void (APIENTRY * PFNGLCOPYCOLORTABLEPROC) (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
    static PFNGLCOPYCOLORTABLEPROC pfnCopyColorTable = 0;
    LOAD_ENTRYPOINT("glCopyColorTable", pfnCopyColorTable, PFNGLCOPYCOLORTABLEPROC);
    pfnCopyColorTable(target, internalformat, x, y, width);
}

void glGetColorTable(GLenum target, GLenum format, GLenum type, GLvoid *table)
{
    typedef void (APIENTRY * PFNGLGETCOLORTABLEPROC) (GLenum target, GLenum format, GLenum type, GLvoid *table);
    static PFNGLGETCOLORTABLEPROC pfnGetColorTable = 0;
    LOAD_ENTRYPOINT("glGetColorTable", pfnGetColorTable, PFNGLGETCOLORTABLEPROC);
    pfnGetColorTable(target, format, type, table);
}

void glGetColorTableParameterfv(GLenum target, GLenum pname, GLfloat *params)
{
    typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERFVPROC) (GLenum target, GLenum pname, GLfloat *params);
    static PFNGLGETCOLORTABLEPARAMETERFVPROC pfnGetColorTableParameterfv = 0;
    LOAD_ENTRYPOINT("glGetColorTableParameterfv", pfnGetColorTableParameterfv, PFNGLGETCOLORTABLEPARAMETERFVPROC);
    pfnGetColorTableParameterfv(target, pname, params);
}

void glGetColorTableParameteriv(GLenum target, GLenum pname, GLint *params)
{
    typedef void (APIENTRY * PFNGLGETCOLORTABLEPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
    static PFNGLGETCOLORTABLEPARAMETERIVPROC pfnGetColorTableParameteriv = 0;
    LOAD_ENTRYPOINT("glGetColorTableParameteriv", pfnGetColorTableParameteriv, PFNGLGETCOLORTABLEPARAMETERIVPROC);
    pfnGetColorTableParameteriv(target, pname, params);
}

void glColorSubTable(GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data)
{
    typedef void (APIENTRY * PFNGLCOLORSUBTABLEPROC) (GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data);
    static PFNGLCOLORSUBTABLEPROC pfnColorSubTable = 0;
    LOAD_ENTRYPOINT("glColorSubTable", pfnColorSubTable, PFNGLCOLORSUBTABLEPROC);
    pfnColorSubTable(target, start, count, format, type, data);
}

void glCopyColorSubTable(GLenum target, GLsizei start, GLint x, GLint y, GLsizei width)
{
    typedef void (APIENTRY * PFNGLCOPYCOLORSUBTABLEPROC) (GLenum target, GLsizei start, GLint x, GLint y, GLsizei width);
    static PFNGLCOPYCOLORSUBTABLEPROC pfnCopyColorSubTable = 0;
    LOAD_ENTRYPOINT("glCopyColorSubTable", pfnCopyColorSubTable, PFNGLCOPYCOLORSUBTABLEPROC);
    pfnCopyColorSubTable(target, start, x, y, width);
}

void glConvolutionFilter1D(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image)
{
    typedef void (APIENTRY * PFNGLCONVOLUTIONFILTER1DPROC) (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image);
    static PFNGLCONVOLUTIONFILTER1DPROC pfnConvolutionFilter1D = 0;
    LOAD_ENTRYPOINT("glConvolutionFilter1D", pfnConvolutionFilter1D, PFNGLCONVOLUTIONFILTER1DPROC);
    pfnConvolutionFilter1D(target, internalformat, width, format, type, image);
}

void glConvolutionFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image)
{
    typedef void (APIENTRY * PFNGLCONVOLUTIONFILTER2DPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image);
    static PFNGLCONVOLUTIONFILTER2DPROC pfnConvolutionFilter2D = 0;
    LOAD_ENTRYPOINT("glConvolutionFilter2D", pfnConvolutionFilter2D, PFNGLCONVOLUTIONFILTER2DPROC);
    pfnConvolutionFilter2D(target, internalformat, width, height, format, type, image);
}

void glConvolutionParameterf(GLenum target, GLenum pname, GLfloat params)
{
    typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERFPROC) (GLenum target, GLenum pname, GLfloat params);
    static PFNGLCONVOLUTIONPARAMETERFPROC pfnConvolutionParameterf = 0;
    LOAD_ENTRYPOINT("glConvolutionParameterf", pfnConvolutionParameterf, PFNGLCONVOLUTIONPARAMETERFPROC);
    pfnConvolutionParameterf(target, pname, params);
}

void glConvolutionParameterfv(GLenum target, GLenum pname, const GLfloat *params)
{
    typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERFVPROC) (GLenum target, GLenum pname, const GLfloat *params);
    static PFNGLCONVOLUTIONPARAMETERFVPROC pfnConvolutionParameterfv = 0;
    LOAD_ENTRYPOINT("glConvolutionParameterfv", pfnConvolutionParameterfv, PFNGLCONVOLUTIONPARAMETERFVPROC);
    pfnConvolutionParameterfv(target, pname, params);
}

void glConvolutionParameteri(GLenum target, GLenum pname, GLint params)
{
    typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERIPROC) (GLenum target, GLenum pname, GLint params);
    static PFNGLCONVOLUTIONPARAMETERIPROC pfnConvolutionParameteri = 0;
    LOAD_ENTRYPOINT("glConvolutionParameteri", pfnConvolutionParameteri, PFNGLCONVOLUTIONPARAMETERIPROC);
    pfnConvolutionParameteri(target, pname, params);
}

void glConvolutionParameteriv(GLenum target, GLenum pname, const GLint *params)
{
    typedef void (APIENTRY * PFNGLCONVOLUTIONPARAMETERIVPROC) (GLenum target, GLenum pname, const GLint *params);
    static PFNGLCONVOLUTIONPARAMETERIVPROC pfnConvolutionParameteriv = 0;
    LOAD_ENTRYPOINT("glConvolutionParameteriv", pfnConvolutionParameteriv, PFNGLCONVOLUTIONPARAMETERIVPROC);
    pfnConvolutionParameteriv(target, pname, params);
}

void glCopyConvolutionFilter1D(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    typedef void (APIENTRY * PFNGLCOPYCONVOLUTIONFILTER1DPROC) (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
    static PFNGLCOPYCONVOLUTIONFILTER1DPROC pfnCopyConvolutionFilter1D = 0;
    LOAD_ENTRYPOINT("glCopyConvolutionFilter1D", pfnCopyConvolutionFilter1D, PFNGLCOPYCONVOLUTIONFILTER1DPROC);
    pfnCopyConvolutionFilter1D(target, internalformat, x, y, width);
}

void glCopyConvolutionFilter2D(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)
{
    typedef void (APIENTRY * PFNGLCOPYCONVOLUTIONFILTER2DPROC) (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height);
    static PFNGLCOPYCONVOLUTIONFILTER2DPROC pfnCopyConvolutionFilter2D = 0;
    LOAD_ENTRYPOINT("glCopyConvolutionFilter2D", pfnCopyConvolutionFilter2D, PFNGLCOPYCONVOLUTIONFILTER2DPROC);
    pfnCopyConvolutionFilter2D(target, internalformat, x, y, width, height);
}

void glGetConvolutionFilter(GLenum target, GLenum format, GLenum type, GLvoid *image)
{
    typedef void (APIENTRY * PFNGLGETCONVOLUTIONFILTERPROC) (GLenum target, GLenum format, GLenum type, GLvoid *image);
    static PFNGLGETCONVOLUTIONFILTERPROC pfnGetConvolutionFilter = 0;
    LOAD_ENTRYPOINT("glGetConvolutionFilter", pfnGetConvolutionFilter, PFNGLGETCONVOLUTIONFILTERPROC);
    pfnGetConvolutionFilter(target, format, type, image);
}

void glGetConvolutionParameterfv(GLenum target, GLenum pname, GLfloat *params)
{
    typedef void (APIENTRY * PFNGLGETCONVOLUTIONPARAMETERFVPROC) (GLenum target, GLenum pname, GLfloat *params);
    static PFNGLGETCONVOLUTIONPARAMETERFVPROC pfnGetConvolutionParameterfv = 0;
    LOAD_ENTRYPOINT("glGetConvolutionParameterfv", pfnGetConvolutionParameterfv, PFNGLGETCONVOLUTIONPARAMETERFVPROC);
    pfnGetConvolutionParameterfv(target, pname, params);
}

void glGetConvolutionParameteriv(GLenum target, GLenum pname, GLint *params)
{
    typedef void (APIENTRY * PFNGLGETCONVOLUTIONPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
    static PFNGLGETCONVOLUTIONPARAMETERIVPROC pfnGetConvolutionParameteriv = 0;
    LOAD_ENTRYPOINT("glGetConvolutionParameteriv", pfnGetConvolutionParameteriv, PFNGLGETCONVOLUTIONPARAMETERIVPROC);
    pfnGetConvolutionParameteriv(target, pname, params);
}

void glGetSeparableFilter(GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span)
{
    typedef void (APIENTRY * PFNGLGETSEPARABLEFILTERPROC) (GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span);
    static PFNGLGETSEPARABLEFILTERPROC pfnGetSeparableFilter = 0;
    LOAD_ENTRYPOINT("glGetSeparableFilter", pfnGetSeparableFilter, PFNGLGETSEPARABLEFILTERPROC);
    pfnGetSeparableFilter(target, format, type, row, column, span);
}

void glSeparableFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column)
{
    typedef void (APIENTRY * PFNGLSEPARABLEFILTER2DPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column);
    static PFNGLSEPARABLEFILTER2DPROC pfnSeparableFilter2D = 0;
    LOAD_ENTRYPOINT("glSeparableFilter2D", pfnSeparableFilter2D, PFNGLSEPARABLEFILTER2DPROC);
    pfnSeparableFilter2D(target, internalformat, width, height, format, type, row, column);
}

void glGetHistogram(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values)
{
    typedef void (APIENTRY * PFNGLGETHISTOGRAMPROC) (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);
    static PFNGLGETHISTOGRAMPROC pfnGetHistogram = 0;
    LOAD_ENTRYPOINT("glGetHistogram", pfnGetHistogram, PFNGLGETHISTOGRAMPROC);
    pfnGetHistogram(target, reset, format, type, values);
}

void glGetHistogramParameterfv(GLenum target, GLenum pname, GLfloat *params)
{
    typedef void (APIENTRY * PFNGLGETHISTOGRAMPARAMETERFVPROC) (GLenum target, GLenum pname, GLfloat *params);
    static PFNGLGETHISTOGRAMPARAMETERFVPROC pfnGetHistogramParameterfv = 0;
    LOAD_ENTRYPOINT("glGetHistogramParameterfv", pfnGetHistogramParameterfv, PFNGLGETHISTOGRAMPARAMETERFVPROC);
    pfnGetHistogramParameterfv(target, pname, params);
}

void glGetHistogramParameteriv(GLenum target, GLenum pname, GLint *params)
{
    typedef void (APIENTRY * PFNGLGETHISTOGRAMPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
    static PFNGLGETHISTOGRAMPARAMETERIVPROC pfnGetHistogramParameteriv = 0;
    LOAD_ENTRYPOINT("glGetHistogramParameteriv", pfnGetHistogramParameteriv, PFNGLGETHISTOGRAMPARAMETERIVPROC);
    pfnGetHistogramParameteriv(target, pname, params);
}

void glGetMinmax(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values)
{
    typedef void (APIENTRY * PFNGLGETMINMAXPROC) (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);
    static PFNGLGETMINMAXPROC pfnGetMinmax = 0;
    LOAD_ENTRYPOINT("glGetMinmax", pfnGetMinmax, PFNGLGETMINMAXPROC);
    pfnGetMinmax(target, reset, format, type, values);
}

void glGetMinmaxParameterfv(GLenum target, GLenum pname, GLfloat *params)
{
    typedef void (APIENTRY * PFNGLGETMINMAXPARAMETERFVPROC) (GLenum target, GLenum pname, GLfloat *params);
    static PFNGLGETMINMAXPARAMETERFVPROC pfnGetMinmaxParameterfv = 0;
    LOAD_ENTRYPOINT("glGetMinmaxParameterfv", pfnGetMinmaxParameterfv, PFNGLGETMINMAXPARAMETERFVPROC);
    pfnGetMinmaxParameterfv(target, pname, params);
}

void glGetMinmaxParameteriv(GLenum target, GLenum pname, GLint *params)
{
    typedef void (APIENTRY * PFNGLGETMINMAXPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
    static PFNGLGETMINMAXPARAMETERIVPROC pfnGetMinmaxParameteriv = 0;
    LOAD_ENTRYPOINT("glGetMinmaxParameteriv", pfnGetMinmaxParameteriv, PFNGLGETMINMAXPARAMETERIVPROC);
    pfnGetMinmaxParameteriv(target, pname, params);
}

void glHistogram(GLenum target, GLsizei width, GLenum internalformat, GLboolean sink)
{
    typedef void (APIENTRY * PFNGLHISTOGRAMPROC) (GLenum target, GLsizei width, GLenum internalformat, GLboolean sink);
    static PFNGLHISTOGRAMPROC pfnHistogram = 0;
    LOAD_ENTRYPOINT("glHistogram", pfnHistogram, PFNGLHISTOGRAMPROC);
    pfnHistogram(target, width, internalformat, sink);
}

void glMinmax(GLenum target, GLenum internalformat, GLboolean sink)
{
    typedef void (APIENTRY * PFNGLMINMAXPROC) (GLenum target, GLenum internalformat, GLboolean sink);
    static PFNGLMINMAXPROC pfnMinmax = 0;
    LOAD_ENTRYPOINT("glMinmax", pfnMinmax, PFNGLMINMAXPROC);
    pfnMinmax(target, internalformat, sink);
}

void glResetHistogram(GLenum target)
{
    typedef void (APIENTRY * PFNGLRESETHISTOGRAMPROC) (GLenum target);
    static PFNGLRESETHISTOGRAMPROC pfnResetHistogram = 0;
    LOAD_ENTRYPOINT("glResetHistogram", pfnResetHistogram, PFNGLRESETHISTOGRAMPROC);
    pfnResetHistogram(target);
}

void glResetMinmax(GLenum target)
{
    typedef void (APIENTRY * PFNGLRESETMINMAXPROC) (GLenum target);
    static PFNGLRESETMINMAXPROC pfnResetMinmax = 0;
    LOAD_ENTRYPOINT("glResetMinmax", pfnResetMinmax, PFNGLRESETMINMAXPROC);
    pfnResetMinmax(target);
}

void glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
    typedef void (APIENTRY * PFNGLTEXIMAGE3DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
    static PFNGLTEXIMAGE3DPROC pfnTexImage3D = 0;
    LOAD_ENTRYPOINT("glTexImage3D", pfnTexImage3D, PFNGLTEXIMAGE3DPROC);
    pfnTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
}

void glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels)
{
    typedef void (APIENTRY * PFNGLTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);
    static PFNGLTEXSUBIMAGE3DPROC pfnTexSubImage3D = 0;
    LOAD_ENTRYPOINT("glTexSubImage3D", pfnTexSubImage3D, PFNGLTEXSUBIMAGE3DPROC);
    pfnTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
}

void glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    typedef void (APIENTRY * PFNGLCOPYTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
    static PFNGLCOPYTEXSUBIMAGE3DPROC pfnCopyTexSubImage3D = 0;
    LOAD_ENTRYPOINT("glCopyTexSubImage3D", pfnCopyTexSubImage3D, PFNGLCOPYTEXSUBIMAGE3DPROC);
    pfnCopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height);
}

//
// OpenGL 1.3
//

void glActiveTexture(GLenum texture)
{
    typedef void (APIENTRY * PFNGLACTIVETEXTUREPROC) (GLenum texture);
    static PFNGLACTIVETEXTUREPROC pfnActiveTexture = 0;
    LOAD_ENTRYPOINT("glActiveTexture", pfnActiveTexture, PFNGLACTIVETEXTUREPROC);
    pfnActiveTexture(texture);
}

void glClientActiveTexture(GLenum texture)
{
    typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREPROC) (GLenum texture);
    static PFNGLCLIENTACTIVETEXTUREPROC pfnClientActiveTexture = 0;
    LOAD_ENTRYPOINT("glClientActiveTexture", pfnClientActiveTexture, PFNGLCLIENTACTIVETEXTUREPROC);
    pfnClientActiveTexture(texture);
}

void glMultiTexCoord1d(GLenum target, GLdouble s)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD1DPROC) (GLenum target, GLdouble s);
    static PFNGLMULTITEXCOORD1DPROC pfnMultiTexCoord1d = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord1d", pfnMultiTexCoord1d, PFNGLMULTITEXCOORD1DPROC);
    pfnMultiTexCoord1d(target, s);
}

void glMultiTexCoord1dv(GLenum target, const GLdouble *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD1DVPROC) (GLenum target, const GLdouble *v);
    static PFNGLMULTITEXCOORD1DVPROC pfnMultiTexCoord1dv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord1dv", pfnMultiTexCoord1dv, PFNGLMULTITEXCOORD1DVPROC);
    pfnMultiTexCoord1dv(target, v);
}

void glMultiTexCoord1f(GLenum target, GLfloat s)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD1FPROC) (GLenum target, GLfloat s);
    static PFNGLMULTITEXCOORD1FPROC pfnMultiTexCoord1f = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord1f", pfnMultiTexCoord1f, PFNGLMULTITEXCOORD1FPROC);
    pfnMultiTexCoord1f(target, s);
}

void glMultiTexCoord1fv(GLenum target, const GLfloat *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD1FVPROC) (GLenum target, const GLfloat *v);
    static PFNGLMULTITEXCOORD1FVPROC pfnMultiTexCoord1fv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord1fv", pfnMultiTexCoord1fv, PFNGLMULTITEXCOORD1FVPROC);
    pfnMultiTexCoord1fv(target, v);
}

void glMultiTexCoord1i(GLenum target, GLint s)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD1IPROC) (GLenum target, GLint s);
    static PFNGLMULTITEXCOORD1IPROC pfnMultiTexCoord1i = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord1i", pfnMultiTexCoord1i, PFNGLMULTITEXCOORD1IPROC);
    pfnMultiTexCoord1i(target, s);
}

void glMultiTexCoord1iv(GLenum target, const GLint *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD1IVPROC) (GLenum target, const GLint *v);
    static PFNGLMULTITEXCOORD1IVPROC pfnMultiTexCoord1iv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord1iv", pfnMultiTexCoord1iv, PFNGLMULTITEXCOORD1IVPROC);
    pfnMultiTexCoord1iv(target, v);
}

void glMultiTexCoord1s(GLenum target, GLshort s)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD1SPROC) (GLenum target, GLshort s);
    static PFNGLMULTITEXCOORD1SPROC pfnMultiTexCoord1s = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord1s", pfnMultiTexCoord1s, PFNGLMULTITEXCOORD1SPROC);
    pfnMultiTexCoord1s(target, s);
}

void glMultiTexCoord1sv(GLenum target, const GLshort *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD1SVPROC) (GLenum target, const GLshort *v);
    static PFNGLMULTITEXCOORD1SVPROC pfnMultiTexCoord1sv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord1sv", pfnMultiTexCoord1sv, PFNGLMULTITEXCOORD1SVPROC);
    pfnMultiTexCoord1sv(target, v);
}

void glMultiTexCoord2d(GLenum target, GLdouble s, GLdouble t)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD2DPROC) (GLenum target, GLdouble s, GLdouble t);
    static PFNGLMULTITEXCOORD2DPROC pfnMultiTexCoord2d = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord2d", pfnMultiTexCoord2d, PFNGLMULTITEXCOORD2DPROC);
    pfnMultiTexCoord2d(target, s, t);
}

void glMultiTexCoord2dv(GLenum target, const GLdouble *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD2DVPROC) (GLenum target, const GLdouble *v);
    static PFNGLMULTITEXCOORD2DVPROC pfnMultiTexCoord2dv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord2dv", pfnMultiTexCoord2dv, PFNGLMULTITEXCOORD2DVPROC);
    pfnMultiTexCoord2dv(target, v);
}

void glMultiTexCoord2f(GLenum target, GLfloat s, GLfloat t)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD2FPROC) (GLenum target, GLfloat s, GLfloat t);
    static PFNGLMULTITEXCOORD2FPROC pfnMultiTexCoord2f = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord2f", pfnMultiTexCoord2f, PFNGLMULTITEXCOORD2FPROC);
    pfnMultiTexCoord2f(target, s, t);
}

void glMultiTexCoord2fv(GLenum target, const GLfloat *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD2FVPROC) (GLenum target, const GLfloat *v);
    static PFNGLMULTITEXCOORD2FVPROC pfnMultiTexCoord2fv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord2fv", pfnMultiTexCoord2fv, PFNGLMULTITEXCOORD2FVPROC);
    pfnMultiTexCoord2fv(target, v);
}

void glMultiTexCoord2i(GLenum target, GLint s, GLint t)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD2IPROC) (GLenum target, GLint s, GLint t);
    static PFNGLMULTITEXCOORD2IPROC pfnMultiTexCoord2i = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord2i", pfnMultiTexCoord2i, PFNGLMULTITEXCOORD2IPROC);
    pfnMultiTexCoord2i(target, s, t);
}

void glMultiTexCoord2iv(GLenum target, const GLint *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD2IVPROC) (GLenum target, const GLint *v);
    static PFNGLMULTITEXCOORD2IVPROC pfnMultiTexCoord2iv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord2iv", pfnMultiTexCoord2iv, PFNGLMULTITEXCOORD2IVPROC);
    pfnMultiTexCoord2iv(target, v);
}

void glMultiTexCoord2s(GLenum target, GLshort s, GLshort t)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD2SPROC) (GLenum target, GLshort s, GLshort t);
    static PFNGLMULTITEXCOORD2SPROC pfnMultiTexCoord2s = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord2s", pfnMultiTexCoord2s, PFNGLMULTITEXCOORD2SPROC);
    pfnMultiTexCoord2s(target, s, t);
}

void glMultiTexCoord2sv(GLenum target, const GLshort *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD2SVPROC) (GLenum target, const GLshort *v);
    static PFNGLMULTITEXCOORD2SVPROC pfnMultiTexCoord2sv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord2sv", pfnMultiTexCoord2sv, PFNGLMULTITEXCOORD2SVPROC);
    pfnMultiTexCoord2sv(target, v);
}

void glMultiTexCoord3d(GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD3DPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r);
    static PFNGLMULTITEXCOORD3DPROC pfnMultiTexCoord3d = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord3d", pfnMultiTexCoord3d, PFNGLMULTITEXCOORD3DPROC);
    pfnMultiTexCoord3d(target, s, t, r);
}

void glMultiTexCoord3dv(GLenum target, const GLdouble *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD3DVPROC) (GLenum target, const GLdouble *v);
    static PFNGLMULTITEXCOORD3DVPROC pfnMultiTexCoord3dv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord3dv", pfnMultiTexCoord3dv, PFNGLMULTITEXCOORD3DVPROC);
    pfnMultiTexCoord3dv(target, v);
}

void glMultiTexCoord3f(GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD3FPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
    static PFNGLMULTITEXCOORD3FPROC pfnMultiTexCoord3f = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord3f", pfnMultiTexCoord3f, PFNGLMULTITEXCOORD3FPROC);
    pfnMultiTexCoord3f(target, s, t, r);
}

void glMultiTexCoord3fv(GLenum target, const GLfloat *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD3FVPROC) (GLenum target, const GLfloat *v);
    static PFNGLMULTITEXCOORD3FVPROC pfnMultiTexCoord3fv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord3fv", pfnMultiTexCoord3fv, PFNGLMULTITEXCOORD3FVPROC);
    pfnMultiTexCoord3fv(target, v);
}

void glMultiTexCoord3i(GLenum target, GLint s, GLint t, GLint r)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD3IPROC) (GLenum target, GLint s, GLint t, GLint r);
    static PFNGLMULTITEXCOORD3IPROC pfnMultiTexCoord3i = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord3i", pfnMultiTexCoord3i, PFNGLMULTITEXCOORD3IPROC);
    pfnMultiTexCoord3i(target, s, t, r);
}

void glMultiTexCoord3iv(GLenum target, const GLint *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD3IVPROC) (GLenum target, const GLint *v);
    static PFNGLMULTITEXCOORD3IVPROC pfnMultiTexCoord3iv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord3iv", pfnMultiTexCoord3iv, PFNGLMULTITEXCOORD3IVPROC);
    pfnMultiTexCoord3iv(target, v);
}

void glMultiTexCoord3s(GLenum target, GLshort s, GLshort t, GLshort r)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD3SPROC) (GLenum target, GLshort s, GLshort t, GLshort r);
    static PFNGLMULTITEXCOORD3SPROC pfnMultiTexCoord3s = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord3s", pfnMultiTexCoord3s, PFNGLMULTITEXCOORD3SPROC);
    pfnMultiTexCoord3s(target, s, t, r);
}

void glMultiTexCoord3sv(GLenum target, const GLshort *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD3SVPROC) (GLenum target, const GLshort *v);
    static PFNGLMULTITEXCOORD3SVPROC pfnMultiTexCoord3sv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord3sv", pfnMultiTexCoord3sv, PFNGLMULTITEXCOORD3SVPROC);
    pfnMultiTexCoord3sv(target, v);
}

void glMultiTexCoord4d(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD4DPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
    static PFNGLMULTITEXCOORD4DPROC pfnMultiTexCoord4d = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord4d", pfnMultiTexCoord4d, PFNGLMULTITEXCOORD4DPROC);
    pfnMultiTexCoord4d(target, s, t, r, q);
}

void glMultiTexCoord4dv(GLenum target, const GLdouble *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD4DVPROC) (GLenum target, const GLdouble *v);
    static PFNGLMULTITEXCOORD4DVPROC pfnMultiTexCoord4dv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord4dv", pfnMultiTexCoord4dv, PFNGLMULTITEXCOORD4DVPROC);
    pfnMultiTexCoord4dv(target, v);
}

void glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD4FPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
    static PFNGLMULTITEXCOORD4FPROC pfnMultiTexCoord4f = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord4f", pfnMultiTexCoord4f, PFNGLMULTITEXCOORD4FPROC);
    pfnMultiTexCoord4f(target, s, t, r, q);
}

void glMultiTexCoord4fv(GLenum target, const GLfloat *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD4FVPROC) (GLenum target, const GLfloat *v);
    static PFNGLMULTITEXCOORD4FVPROC pfnMultiTexCoord4fv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord4fv", pfnMultiTexCoord4fv, PFNGLMULTITEXCOORD4FVPROC);
    pfnMultiTexCoord4fv(target, v);
}

void glMultiTexCoord4i(GLenum target, GLint s, GLint t, GLint r, GLint q)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD4IPROC) (GLenum target, GLint s, GLint t, GLint r, GLint q);
    static PFNGLMULTITEXCOORD4IPROC pfnMultiTexCoord4i = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord4i", pfnMultiTexCoord4i, PFNGLMULTITEXCOORD4IPROC);
    pfnMultiTexCoord4i(target, s, t, r, q);
}

void glMultiTexCoord4iv(GLenum target, const GLint *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD4IVPROC) (GLenum target, const GLint *v);
    static PFNGLMULTITEXCOORD4IVPROC pfnMultiTexCoord4iv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord4iv", pfnMultiTexCoord4iv, PFNGLMULTITEXCOORD4IVPROC);
    pfnMultiTexCoord4iv(target, v);
}

void glMultiTexCoord4s(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD4SPROC) (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
    static PFNGLMULTITEXCOORD4SPROC pfnMultiTexCoord4s = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord4s", pfnMultiTexCoord4s, PFNGLMULTITEXCOORD4SPROC);
    pfnMultiTexCoord4s(target, s, t, r, q);
}

void glMultiTexCoord4sv(GLenum target, const GLshort *v)
{
    typedef void (APIENTRY * PFNGLMULTITEXCOORD4SVPROC) (GLenum target, const GLshort *v);
    static PFNGLMULTITEXCOORD4SVPROC pfnMultiTexCoord4sv = 0;
    LOAD_ENTRYPOINT("glMultiTexCoord4sv", pfnMultiTexCoord4sv, PFNGLMULTITEXCOORD4SVPROC);
    pfnMultiTexCoord4sv(target, v);
}

void glLoadTransposeMatrixf(const GLfloat *m)
{
    typedef void (APIENTRY * PFNGLLOADTRANSPOSEMATRIXFPROC) (const GLfloat *m);
    static PFNGLLOADTRANSPOSEMATRIXFPROC pfnLoadTransposeMatrixf = 0;
    LOAD_ENTRYPOINT("glLoadTransposeMatrixf", pfnLoadTransposeMatrixf, PFNGLLOADTRANSPOSEMATRIXFPROC);
    pfnLoadTransposeMatrixf(m);
}

void glLoadTransposeMatrixd(const GLdouble *m)
{
    typedef void (APIENTRY * PFNGLLOADTRANSPOSEMATRIXDPROC) (const GLdouble *m);
    static PFNGLLOADTRANSPOSEMATRIXDPROC pfnLoadTransposeMatrixd = 0;
    LOAD_ENTRYPOINT("glLoadTransposeMatrixd", pfnLoadTransposeMatrixd, PFNGLLOADTRANSPOSEMATRIXDPROC);
    pfnLoadTransposeMatrixd(m);
}

void glMultTransposeMatrixf(const GLfloat *m)
{
    typedef void (APIENTRY * PFNGLMULTTRANSPOSEMATRIXFPROC) (const GLfloat *m);
    static PFNGLMULTTRANSPOSEMATRIXFPROC pfnMultTransposeMatrixf = 0;
    LOAD_ENTRYPOINT("glMultTransposeMatrixf", pfnMultTransposeMatrixf, PFNGLMULTTRANSPOSEMATRIXFPROC);
    pfnMultTransposeMatrixf(m);
}

void glMultTransposeMatrixd(const GLdouble *m)
{
    typedef void (APIENTRY * PFNGLMULTTRANSPOSEMATRIXDPROC) (const GLdouble *m);
    static PFNGLMULTTRANSPOSEMATRIXDPROC pfnMultTransposeMatrixd = 0;
    LOAD_ENTRYPOINT("glMultTransposeMatrixd", pfnMultTransposeMatrixd, PFNGLMULTTRANSPOSEMATRIXDPROC);
    pfnMultTransposeMatrixd(m);
}

void glSampleCoverage(GLclampf value, GLboolean invert)
{
    typedef void (APIENTRY * PFNGLSAMPLECOVERAGEPROC) (GLclampf value, GLboolean invert);
    static PFNGLSAMPLECOVERAGEPROC pfnSampleCoverage = 0;
    LOAD_ENTRYPOINT("glSampleCoverage", pfnSampleCoverage, PFNGLSAMPLECOVERAGEPROC);
    pfnSampleCoverage(value, invert);
}

void glCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data)
{
    typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE3DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
    static PFNGLCOMPRESSEDTEXIMAGE3DPROC pfnCompressedTexImage3D = 0;
    LOAD_ENTRYPOINT("glCompressedTexImage3D", pfnCompressedTexImage3D, PFNGLCOMPRESSEDTEXIMAGE3DPROC);
    pfnCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data);
}

void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
{
    typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
    static PFNGLCOMPRESSEDTEXIMAGE2DPROC pfnCompressedTexImage2D = 0;
    LOAD_ENTRYPOINT("glCompressedTexImage2D", pfnCompressedTexImage2D, PFNGLCOMPRESSEDTEXIMAGE2DPROC);
    pfnCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
}

void glCompressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data)
{
    typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE1DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data);
    static PFNGLCOMPRESSEDTEXIMAGE1DPROC pfnCompressedTexImage1D = 0;
    LOAD_ENTRYPOINT("glCompressedTexImage1D", pfnCompressedTexImage1D, PFNGLCOMPRESSEDTEXIMAGE1DPROC);
    pfnCompressedTexImage1D(target, level, internalformat, width, border, imageSize, data);
}

void glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
    static PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC pfnCompressedTexSubImage3D = 0;
    LOAD_ENTRYPOINT("glCompressedTexSubImage3D", pfnCompressedTexSubImage3D, PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC);
    pfnCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
}

void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
    static PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC pfnCompressedTexSubImage2D = 0;
    LOAD_ENTRYPOINT("glCompressedTexSubImage2D", pfnCompressedTexSubImage2D, PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC);
    pfnCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
}

void glCompressedTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
    static PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC pfnCompressedTexSubImage1D = 0;
    LOAD_ENTRYPOINT("glCompressedTexSubImage1D", pfnCompressedTexSubImage1D, PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC);
    pfnCompressedTexSubImage1D(target, level, xoffset, width, format, imageSize, data);
}

void glGetCompressedTexImage(GLenum target, GLint level, GLvoid *img)
{
    typedef void (APIENTRY * PFNGLGETCOMPRESSEDTEXIMAGEPROC) (GLenum target, GLint level, GLvoid *img);
    static PFNGLGETCOMPRESSEDTEXIMAGEPROC pfnGetCompressedTexImage = 0;
    LOAD_ENTRYPOINT("glGetCompressedTexImage", pfnGetCompressedTexImage, PFNGLGETCOMPRESSEDTEXIMAGEPROC);
    pfnGetCompressedTexImage(target, level, img);
}

//
// OpenGl 1.4
//

void glBlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    typedef void (APIENTRY * PFNGLBLENDFUNCSEPARATEPROC) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
    static PFNGLBLENDFUNCSEPARATEPROC pfnBlendFuncSeparate = 0;
    LOAD_ENTRYPOINT("glBlendFuncSeparate", pfnBlendFuncSeparate, PFNGLBLENDFUNCSEPARATEPROC);
    pfnBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}

void glFogCoordf(GLfloat coord)
{
    typedef void (APIENTRY * PFNGLFOGCOORDFPROC) (GLfloat coord);
    static PFNGLFOGCOORDFPROC pfnFogCoordf = 0;
    LOAD_ENTRYPOINT("glFogCoordf", pfnFogCoordf, PFNGLFOGCOORDFPROC);
    pfnFogCoordf(coord);
}

void glFogCoordfv(const GLfloat *coord)
{
    typedef void (APIENTRY * PFNGLFOGCOORDFVPROC) (const GLfloat *coord);
    static PFNGLFOGCOORDFVPROC pfnFogCoordfv = 0;
    LOAD_ENTRYPOINT("glFogCoordfv", pfnFogCoordfv, PFNGLFOGCOORDFVPROC);
    pfnFogCoordfv(coord);
}

void glFogCoordd(GLdouble coord)
{
    typedef void (APIENTRY * PFNGLFOGCOORDDPROC) (GLdouble coord);
    static PFNGLFOGCOORDDPROC pfnFogCoordd = 0;
    LOAD_ENTRYPOINT("glFogCoordd", pfnFogCoordd, PFNGLFOGCOORDDPROC);
    pfnFogCoordd(coord);
}

void glFogCoorddv(const GLdouble *coord)
{
    typedef void (APIENTRY * PFNGLFOGCOORDDVPROC) (const GLdouble *coord);
    static PFNGLFOGCOORDDVPROC pfnFogCoorddv = 0;
    LOAD_ENTRYPOINT("glFogCoorddv", pfnFogCoorddv, PFNGLFOGCOORDDVPROC);
    pfnFogCoorddv(coord);
}

void glFogCoordPointer(GLenum type, GLsizei stride, const GLvoid *pointer)
{
    typedef void (APIENTRY * PFNGLFOGCOORDPOINTERPROC) (GLenum type, GLsizei stride, const GLvoid *pointer);
    static PFNGLFOGCOORDPOINTERPROC pfnFogCoordPointer = 0;
    LOAD_ENTRYPOINT("glFogCoordPointer", pfnFogCoordPointer, PFNGLFOGCOORDPOINTERPROC);
    pfnFogCoordPointer(type, stride, pointer);
}

void glMultiDrawArrays(GLenum mode, GLint *first, GLsizei *count, GLsizei primcount)
{
    typedef void (APIENTRY * PFNGLMULTIDRAWARRAYSPROC) (GLenum mode, GLint *first, GLsizei *count, GLsizei primcount);
    static PFNGLMULTIDRAWARRAYSPROC pfnMultiDrawArrays = 0;
    LOAD_ENTRYPOINT("glMultiDrawArrays", pfnMultiDrawArrays, PFNGLMULTIDRAWARRAYSPROC);
    pfnMultiDrawArrays(mode, first, count, primcount);
}

void glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const GLvoid* *indices, GLsizei primcount)
{
    typedef void (APIENTRY * PFNGLMULTIDRAWELEMENTSPROC) (GLenum mode, const GLsizei *count, GLenum type, const GLvoid* *indices, GLsizei primcount);
    static PFNGLMULTIDRAWELEMENTSPROC pfnMultiDrawElements = 0;
    LOAD_ENTRYPOINT("glMultiDrawElements", pfnMultiDrawElements, PFNGLMULTIDRAWELEMENTSPROC);
    pfnMultiDrawElements(mode, count, type, indices, primcount);
}

void glPointParameterf(GLenum pname, GLfloat param)
{
    typedef void (APIENTRY * PFNGLPOINTPARAMETERFPROC) (GLenum pname, GLfloat param);
    static PFNGLPOINTPARAMETERFPROC pfnPointParameterf = 0;
    LOAD_ENTRYPOINT("glPointParameterf", pfnPointParameterf, PFNGLPOINTPARAMETERFPROC);
    pfnPointParameterf(pname, param);
}

void glPointParameterfv(GLenum pname, const GLfloat *params)
{
    typedef void (APIENTRY * PFNGLPOINTPARAMETERFVPROC) (GLenum pname, const GLfloat *params);
    static PFNGLPOINTPARAMETERFVPROC pfnPointParameterfv = 0;
    LOAD_ENTRYPOINT("glPointParameterfv", pfnPointParameterfv, PFNGLPOINTPARAMETERFVPROC);
    pfnPointParameterfv(pname, params);
}

void glPointParameteri(GLenum pname, GLint param)
{
    typedef void (APIENTRY * PFNGLPOINTPARAMETERIPROC) (GLenum pname, GLint param);
    static PFNGLPOINTPARAMETERIPROC pfnPointParameteri = 0;
    LOAD_ENTRYPOINT("glPointParameteri", pfnPointParameteri, PFNGLPOINTPARAMETERIPROC);
    pfnPointParameteri(pname, param);
}

void glPointParameteriv(GLenum pname, const GLint *params)
{
    typedef void (APIENTRY * PFNGLPOINTPARAMETERIVPROC) (GLenum pname, const GLint *params);
    static PFNGLPOINTPARAMETERIVPROC pfnPointParameteriv = 0;
    LOAD_ENTRYPOINT("glPointParameteriv", pfnPointParameteriv, PFNGLPOINTPARAMETERIVPROC);
    pfnPointParameteriv(pname, params);
}

void glSecondaryColor3b(GLbyte red, GLbyte green, GLbyte blue)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3BPROC) (GLbyte red, GLbyte green, GLbyte blue);
    static PFNGLSECONDARYCOLOR3BPROC pfnSecondaryColor3b = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3b", pfnSecondaryColor3b, PFNGLSECONDARYCOLOR3BPROC);
    pfnSecondaryColor3b(red, green, blue);
}

void glSecondaryColor3bv(const GLbyte *v)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3BVPROC) (const GLbyte *v);
    static PFNGLSECONDARYCOLOR3BVPROC pfnSecondaryColor3bv = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3bv", pfnSecondaryColor3bv, PFNGLSECONDARYCOLOR3BVPROC);
    pfnSecondaryColor3bv(v);
}

void glSecondaryColor3d(GLdouble red, GLdouble green, GLdouble blue)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3DPROC) (GLdouble red, GLdouble green, GLdouble blue);
    static PFNGLSECONDARYCOLOR3DPROC pfnSecondaryColor3d = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3d", pfnSecondaryColor3d, PFNGLSECONDARYCOLOR3DPROC);
    pfnSecondaryColor3d(red, green, blue);
}

void glSecondaryColor3dv(const GLdouble *v)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3DVPROC) (const GLdouble *v);
    static PFNGLSECONDARYCOLOR3DVPROC pfnSecondaryColor3dv = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3dv", pfnSecondaryColor3dv, PFNGLSECONDARYCOLOR3DVPROC);
    pfnSecondaryColor3dv(v);
}

void glSecondaryColor3f(GLfloat red, GLfloat green, GLfloat blue)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3FPROC) (GLfloat red, GLfloat green, GLfloat blue);
    static PFNGLSECONDARYCOLOR3FPROC pfnSecondaryColor3f = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3f", pfnSecondaryColor3f, PFNGLSECONDARYCOLOR3FPROC);
    pfnSecondaryColor3f(red, green, blue);
}

void glSecondaryColor3fv(const GLfloat *v)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3FVPROC) (const GLfloat *v);
    static PFNGLSECONDARYCOLOR3FVPROC pfnSecondaryColor3fv = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3fv", pfnSecondaryColor3fv, PFNGLSECONDARYCOLOR3FVPROC);
    pfnSecondaryColor3fv(v);
}

void glSecondaryColor3i(GLint red, GLint green, GLint blue)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3IPROC) (GLint red, GLint green, GLint blue);
    static PFNGLSECONDARYCOLOR3IPROC pfnSecondaryColor3i = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3i", pfnSecondaryColor3i, PFNGLSECONDARYCOLOR3IPROC);
    pfnSecondaryColor3i(red, green, blue);
}

void glSecondaryColor3iv(const GLint *v)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3IVPROC) (const GLint *v);
    static PFNGLSECONDARYCOLOR3IVPROC pfnSecondaryColor3iv = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3iv", pfnSecondaryColor3iv, PFNGLSECONDARYCOLOR3IVPROC);
    pfnSecondaryColor3iv(v);
}

void glSecondaryColor3s(GLshort red, GLshort green, GLshort blue)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3SPROC) (GLshort red, GLshort green, GLshort blue);
    static PFNGLSECONDARYCOLOR3SPROC pfnSecondaryColor3s = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3s", pfnSecondaryColor3s, PFNGLSECONDARYCOLOR3SPROC);
    pfnSecondaryColor3s(red, green, blue);
}

void glSecondaryColor3sv(const GLshort *v)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3SVPROC) (const GLshort *v);
    static PFNGLSECONDARYCOLOR3SVPROC pfnSecondaryColor3sv = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3sv", pfnSecondaryColor3sv, PFNGLSECONDARYCOLOR3SVPROC);
    pfnSecondaryColor3sv(v);
}

void glSecondaryColor3ub(GLubyte red, GLubyte green, GLubyte blue)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UBPROC) (GLubyte red, GLubyte green, GLubyte blue);
    static PFNGLSECONDARYCOLOR3UBPROC pfnSecondaryColor3ub = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3ub", pfnSecondaryColor3ub, PFNGLSECONDARYCOLOR3UBPROC);
    pfnSecondaryColor3ub(red, green, blue);
}

void glSecondaryColor3ubv(const GLubyte *v)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UBVPROC) (const GLubyte *v);
    static PFNGLSECONDARYCOLOR3UBVPROC pfnSecondaryColor3ubv = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3ubv", pfnSecondaryColor3ubv, PFNGLSECONDARYCOLOR3UBVPROC);
    pfnSecondaryColor3ubv(v);
}

void glSecondaryColor3ui(GLuint red, GLuint green, GLuint blue)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UIPROC) (GLuint red, GLuint green, GLuint blue);
    static PFNGLSECONDARYCOLOR3UIPROC pfnSecondaryColor3ui = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3ui", pfnSecondaryColor3ui, PFNGLSECONDARYCOLOR3UIPROC);
    pfnSecondaryColor3ui(red, green, blue);
}

void glSecondaryColor3uiv(const GLuint *v)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UIVPROC) (const GLuint *v);
    static PFNGLSECONDARYCOLOR3UIVPROC pfnSecondaryColor3uiv = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3uiv", pfnSecondaryColor3uiv, PFNGLSECONDARYCOLOR3UIVPROC);
    pfnSecondaryColor3uiv(v);
}

void glSecondaryColor3us(GLushort red, GLushort green, GLushort blue)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3USPROC) (GLushort red, GLushort green, GLushort blue);
    static PFNGLSECONDARYCOLOR3USPROC pfnSecondaryColor3us = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3us", pfnSecondaryColor3us, PFNGLSECONDARYCOLOR3USPROC);
    pfnSecondaryColor3us(red, green, blue);
}

void glSecondaryColor3usv(const GLushort *v)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLOR3USVPROC) (const GLushort *v);
    static PFNGLSECONDARYCOLOR3USVPROC pfnSecondaryColor3usv = 0;
    LOAD_ENTRYPOINT("glSecondaryColor3usv", pfnSecondaryColor3usv, PFNGLSECONDARYCOLOR3USVPROC);
    pfnSecondaryColor3usv(v);
}

void glSecondaryColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    typedef void (APIENTRY * PFNGLSECONDARYCOLORPOINTERPROC) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
    static PFNGLSECONDARYCOLORPOINTERPROC pfnSecondaryColorPointer = 0;
    LOAD_ENTRYPOINT("glSecondaryColorPointer", pfnSecondaryColorPointer, PFNGLSECONDARYCOLORPOINTERPROC);
    pfnSecondaryColorPointer(size, type, stride, pointer);
}

void glWindowPos2d(GLdouble x, GLdouble y)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS2DPROC) (GLdouble x, GLdouble y);
    static PFNGLWINDOWPOS2DPROC pfnWindowPos2d = 0;
    LOAD_ENTRYPOINT("glWindowPos2d", pfnWindowPos2d, PFNGLWINDOWPOS2DPROC);
    pfnWindowPos2d(x, y);
}

void glWindowPos2dv(const GLdouble *v)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS2DVPROC) (const GLdouble *v);
    static PFNGLWINDOWPOS2DVPROC pfnWindowPos2dv = 0;
    LOAD_ENTRYPOINT("glWindowPos2dv", pfnWindowPos2dv, PFNGLWINDOWPOS2DVPROC);
    pfnWindowPos2dv(v);
}

void glWindowPos2f(GLfloat x, GLfloat y)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS2FPROC) (GLfloat x, GLfloat y);
    static PFNGLWINDOWPOS2FPROC pfnWindowPos2f = 0;
    LOAD_ENTRYPOINT("glWindowPos2f", pfnWindowPos2f, PFNGLWINDOWPOS2FPROC);
    pfnWindowPos2f(x, y);
}

void glWindowPos2fv(const GLfloat *v)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS2FVPROC) (const GLfloat *v);
    static PFNGLWINDOWPOS2FVPROC pfnWindowPos2fv = 0;
    LOAD_ENTRYPOINT("glWindowPos2fv", pfnWindowPos2fv, PFNGLWINDOWPOS2FVPROC);
    pfnWindowPos2fv(v);
}

void glWindowPos2i(GLint x, GLint y)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS2IPROC) (GLint x, GLint y);
    static PFNGLWINDOWPOS2IPROC pfnWindowPos2i = 0;
    LOAD_ENTRYPOINT("glWindowPos2i", pfnWindowPos2i, PFNGLWINDOWPOS2IPROC);
    pfnWindowPos2i(x, y);
}

void glWindowPos2iv(const GLint *v)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS2IVPROC) (const GLint *v);
    static PFNGLWINDOWPOS2IVPROC pfnWindowPos2iv = 0;
    LOAD_ENTRYPOINT("glWindowPos2iv", pfnWindowPos2iv, PFNGLWINDOWPOS2IVPROC);
    pfnWindowPos2iv(v);
}

void glWindowPos2s(GLshort x, GLshort y)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS2SPROC) (GLshort x, GLshort y);
    static PFNGLWINDOWPOS2SPROC pfnWindowPos2s = 0;
    LOAD_ENTRYPOINT("glWindowPos2s", pfnWindowPos2s, PFNGLWINDOWPOS2SPROC);
    pfnWindowPos2s(x, y);
}

void glWindowPos2sv(const GLshort *v)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS2SVPROC) (const GLshort *v);
    static PFNGLWINDOWPOS2SVPROC pfnWindowPos2sv = 0;
    LOAD_ENTRYPOINT("glWindowPos2sv", pfnWindowPos2sv, PFNGLWINDOWPOS2SVPROC);
    pfnWindowPos2sv(v);
}

void glWindowPos3d(GLdouble x, GLdouble y, GLdouble z)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS3DPROC) (GLdouble x, GLdouble y, GLdouble z);
    static PFNGLWINDOWPOS3DPROC pfnWindowPos3d = 0;
    LOAD_ENTRYPOINT("glWindowPos3d", pfnWindowPos3d, PFNGLWINDOWPOS3DPROC);
    pfnWindowPos3d(x, y, z);
}

void glWindowPos3dv(const GLdouble *v)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS3DVPROC) (const GLdouble *v);
    static PFNGLWINDOWPOS3DVPROC pfnWindowPos3dv = 0;
    LOAD_ENTRYPOINT("glWindowPos3dv", pfnWindowPos3dv, PFNGLWINDOWPOS3DVPROC);
    pfnWindowPos3dv(v);
}

void glWindowPos3f(GLfloat x, GLfloat y, GLfloat z)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS3FPROC) (GLfloat x, GLfloat y, GLfloat z);
    static PFNGLWINDOWPOS3FPROC pfnWindowPos3f = 0;
    LOAD_ENTRYPOINT("glWindowPos3f", pfnWindowPos3f, PFNGLWINDOWPOS3FPROC);
    pfnWindowPos3f(x, y, z);
}

void glWindowPos3fv(const GLfloat *v)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS3FVPROC) (const GLfloat *v);
    static PFNGLWINDOWPOS3FVPROC pfnWindowPos3fv = 0;
    LOAD_ENTRYPOINT("glWindowPos3fv", pfnWindowPos3fv, PFNGLWINDOWPOS3FVPROC);
    pfnWindowPos3fv(v);
}

void glWindowPos3i(GLint x, GLint y, GLint z)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS3IPROC) (GLint x, GLint y, GLint z);
    static PFNGLWINDOWPOS3IPROC pfnWindowPos3i = 0;
    LOAD_ENTRYPOINT("glWindowPos3i", pfnWindowPos3i, PFNGLWINDOWPOS3IPROC);
    pfnWindowPos3i(x, y, z);
}

void glWindowPos3iv(const GLint *v)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS3IVPROC) (const GLint *v);
    static PFNGLWINDOWPOS3IVPROC pfnWindowPos3iv = 0;
    LOAD_ENTRYPOINT("glWindowPos3iv", pfnWindowPos3iv, PFNGLWINDOWPOS3IVPROC);
    pfnWindowPos3iv(v);
}

void glWindowPos3s(GLshort x, GLshort y, GLshort z)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS3SPROC) (GLshort x, GLshort y, GLshort z);
    static PFNGLWINDOWPOS3SPROC pfnWindowPos3s = 0;
    LOAD_ENTRYPOINT("glWindowPos3s", pfnWindowPos3s, PFNGLWINDOWPOS3SPROC);
    pfnWindowPos3s(x, y, z);
}

void glWindowPos3sv(const GLshort *v)
{
    typedef void (APIENTRY * PFNGLWINDOWPOS3SVPROC) (const GLshort *v);
    static PFNGLWINDOWPOS3SVPROC pfnWindowPos3sv = 0;
    LOAD_ENTRYPOINT("glWindowPos3sv", pfnWindowPos3sv, PFNGLWINDOWPOS3SVPROC);
    pfnWindowPos3sv(v);
}

//
// OpenGL 1.5
//

void glGenQueries(GLsizei n, GLuint *ids)
{
    typedef void (APIENTRY * PFNGLGENQUERIESPROC) (GLsizei n, GLuint *ids);
    static PFNGLGENQUERIESPROC pfnGenQueries = 0;
    LOAD_ENTRYPOINT("glGenQueries", pfnGenQueries, PFNGLGENQUERIESPROC);
    pfnGenQueries(n, ids);
}

void glDeleteQueries(GLsizei n, const GLuint *ids)
{
    typedef void (APIENTRY * PFNGLDELETEQUERIESPROC) (GLsizei n, const GLuint *ids);
    static PFNGLDELETEQUERIESPROC pfnDeleteQueries = 0;
    LOAD_ENTRYPOINT("glDeleteQueries", pfnDeleteQueries, PFNGLDELETEQUERIESPROC);
    pfnDeleteQueries(n, ids);
}

GLboolean glIsQuery(GLuint id)
{
    typedef GLboolean (APIENTRY * PFNGLISQUERYPROC) (GLuint id);
    static PFNGLISQUERYPROC pfnIsQuery = 0;
    LOAD_ENTRYPOINT("glIsQuery", pfnIsQuery, PFNGLISQUERYPROC);
    return pfnIsQuery(id);
}

void glBeginQuery(GLenum target, GLuint id)
{
    typedef void (APIENTRY * PFNGLBEGINQUERYPROC) (GLenum target, GLuint id);
    static PFNGLBEGINQUERYPROC pfnBeginQuery = 0;
    LOAD_ENTRYPOINT("glBeginQuery", pfnBeginQuery, PFNGLBEGINQUERYPROC);
    pfnBeginQuery(target, id);
}

void glEndQuery(GLenum target)
{
    typedef void (APIENTRY * PFNGLENDQUERYPROC) (GLenum target);
    static PFNGLENDQUERYPROC pfnEndQuery = 0;
    LOAD_ENTRYPOINT("glEndQuery", pfnEndQuery, PFNGLENDQUERYPROC);
    pfnEndQuery(target);
}

void glGetQueryiv(GLenum target, GLenum pname, GLint *params)
{
    typedef void (APIENTRY * PFNGLGETQUERYIVPROC) (GLenum target, GLenum pname, GLint *params);
    static PFNGLGETQUERYIVPROC pfnGetQueryiv = 0;
    LOAD_ENTRYPOINT("glGetQueryiv", pfnGetQueryiv, PFNGLGETQUERYIVPROC);
    pfnGetQueryiv(target, pname, params);
}

void glGetQueryObjectiv(GLuint id, GLenum pname, GLint *params)
{
    typedef void (APIENTRY * PFNGLGETQUERYOBJECTIVPROC) (GLuint id, GLenum pname, GLint *params);
    static PFNGLGETQUERYOBJECTIVPROC pfnGetQueryObjectiv = 0;
    LOAD_ENTRYPOINT("glGetQueryObjectiv", pfnGetQueryObjectiv, PFNGLGETQUERYOBJECTIVPROC);
    pfnGetQueryObjectiv(id, pname, params);
}

void glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint *params)
{
    typedef void (APIENTRY * PFNGLGETQUERYOBJECTUIVPROC) (GLuint id, GLenum pname, GLuint *params);
    static PFNGLGETQUERYOBJECTUIVPROC pfnGetQueryObjectuiv = 0;
    LOAD_ENTRYPOINT("glGetQueryObjectuiv", pfnGetQueryObjectuiv, PFNGLGETQUERYOBJECTUIVPROC);
    pfnGetQueryObjectuiv(id, pname, params);
}

void glBindBuffer(GLenum target, GLuint buffer)
{
    typedef void (APIENTRY * PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
    static PFNGLBINDBUFFERPROC pfnBindBuffer = 0;
    LOAD_ENTRYPOINT("glBindBuffer", pfnBindBuffer, PFNGLBINDBUFFERPROC);
    pfnBindBuffer(target, buffer);
}

void glDeleteBuffers(GLsizei n, const GLuint *buffers)
{
    typedef void (APIENTRY * PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint *buffers);
    static PFNGLDELETEBUFFERSPROC pfnDeleteBuffers = 0;
    LOAD_ENTRYPOINT("glDeleteBuffers", pfnDeleteBuffers, PFNGLDELETEBUFFERSPROC);
    pfnDeleteBuffers(n, buffers);
}

void glGenBuffers(GLsizei n, GLuint *buffers)
{
    typedef void (APIENTRY * PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
    static PFNGLGENBUFFERSPROC pfnGenBuffers = 0;
    LOAD_ENTRYPOINT("glGenBuffers", pfnGenBuffers, PFNGLGENBUFFERSPROC);
    pfnGenBuffers(n, buffers);
}

GLboolean glIsBuffer(GLuint buffer)
{
    typedef GLboolean (APIENTRY * PFNGLISBUFFERPROC) (GLuint buffer);
    static PFNGLISBUFFERPROC pfnIsBuffer = 0;
    LOAD_ENTRYPOINT("glIsBuffer", pfnIsBuffer, PFNGLISBUFFERPROC);
    return pfnIsBuffer(buffer);
}

void glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
{
    typedef void (APIENTRY * PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
    static PFNGLBUFFERDATAPROC pfnBufferData = 0;
    LOAD_ENTRYPOINT("glBufferData", pfnBufferData, PFNGLBUFFERDATAPROC);
    pfnBufferData(target, size, data, usage);
}

void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
{
    typedef void (APIENTRY * PFNGLBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
    static PFNGLBUFFERSUBDATAPROC pfnBufferSubData = 0;
    LOAD_ENTRYPOINT("glBufferSubData", pfnBufferSubData, PFNGLBUFFERSUBDATAPROC);
    pfnBufferSubData(target, offset, size, data);
}

void glGetBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data)
{
    typedef void (APIENTRY * PFNGLGETBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data);
    static PFNGLGETBUFFERSUBDATAPROC pfnGetBufferSubData = 0;
    LOAD_ENTRYPOINT("glGetBufferSubData", pfnGetBufferSubData, PFNGLGETBUFFERSUBDATAPROC);
    pfnGetBufferSubData(target, offset, size, data);
}

GLvoid* glMapBuffer(GLenum target, GLenum access)
{
    typedef GLvoid* (APIENTRY * PFNGLMAPBUFFERPROC) (GLenum target, GLenum access);
    static PFNGLMAPBUFFERPROC pfnMapBuffer = 0;
    LOAD_ENTRYPOINT("glMapBuffer", pfnMapBuffer, PFNGLMAPBUFFERPROC);
    return pfnMapBuffer(target, access);
}

GLboolean glUnmapBuffer(GLenum target)
{
    typedef GLboolean (APIENTRY * PFNGLUNMAPBUFFERPROC) (GLenum target);
    static PFNGLUNMAPBUFFERPROC pfnUnmapBuffer = 0;
    LOAD_ENTRYPOINT("glUnmapBuffer", pfnUnmapBuffer, PFNGLUNMAPBUFFERPROC);
    return pfnUnmapBuffer(target);
}

void glGetBufferParameteriv(GLenum target, GLenum pname, GLint *params)
{
    typedef void (APIENTRY * PFNGLGETBUFFERPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
    static PFNGLGETBUFFERPARAMETERIVPROC pfnGetBufferParameteriv = 0;
    LOAD_ENTRYPOINT("glGetBufferParameteriv", pfnGetBufferParameteriv, PFNGLGETBUFFERPARAMETERIVPROC);
    pfnGetBufferParameteriv(target, pname, params);
}

void glGetBufferPointerv(GLenum target, GLenum pname, GLvoid* *params)
{
    typedef void (APIENTRY * PFNGLGETBUFFERPOINTERVPROC) (GLenum target, GLenum pname, GLvoid* *params);
    static PFNGLGETBUFFERPOINTERVPROC pfnGetBufferPointerv = 0;
    LOAD_ENTRYPOINT("glGetBufferPointerv", pfnGetBufferPointerv, PFNGLGETBUFFERPOINTERVPROC);
    pfnGetBufferPointerv(target, pname, params);
}

//
// OpenGL 2.0
//

void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
    typedef void (APIENTRY * PFNGLBLENDEQUATIONSEPARATEPROC) (GLenum modeRGB, GLenum modeAlpha);
    static PFNGLBLENDEQUATIONSEPARATEPROC pfnBlendEquationSeparate = 0;
    LOAD_ENTRYPOINT("glBlendEquationSeparate", pfnBlendEquationSeparate, PFNGLBLENDEQUATIONSEPARATEPROC);
    pfnBlendEquationSeparate(modeRGB, modeAlpha);
}

void glDrawBuffers(GLsizei n, const GLenum *bufs)
{
    typedef void (APIENTRY * PFNGLDRAWBUFFERSPROC) (GLsizei n, const GLenum *bufs);
    static PFNGLDRAWBUFFERSPROC pfnDrawBuffers = 0;
    LOAD_ENTRYPOINT("glDrawBuffers", pfnDrawBuffers, PFNGLDRAWBUFFERSPROC);
    pfnDrawBuffers(n, bufs);
}

void glStencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
    typedef void (APIENTRY * PFNGLSTENCILOPSEPARATEPROC) (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
    static PFNGLSTENCILOPSEPARATEPROC pfnStencilOpSeparate = 0;
    LOAD_ENTRYPOINT("glStencilOpSeparate", pfnStencilOpSeparate, PFNGLSTENCILOPSEPARATEPROC);
    pfnStencilOpSeparate(face, sfail, dpfail, dppass);
}

void glStencilFuncSeparate(GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask)
{
    typedef void (APIENTRY * PFNGLSTENCILFUNCSEPARATEPROC) (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask);
    static PFNGLSTENCILFUNCSEPARATEPROC pfnStencilFuncSeparate = 0;
    LOAD_ENTRYPOINT("glStencilFuncSeparate", pfnStencilFuncSeparate, PFNGLSTENCILFUNCSEPARATEPROC);
    pfnStencilFuncSeparate(frontfunc, backfunc, ref, mask);
}

void glStencilMaskSeparate(GLenum face, GLuint mask)
{
    typedef void (APIENTRY * PFNGLSTENCILMASKSEPARATEPROC) (GLenum face, GLuint mask);
    static PFNGLSTENCILMASKSEPARATEPROC pfnStencilMaskSeparate = 0;
    LOAD_ENTRYPOINT("glStencilMaskSeparate", pfnStencilMaskSeparate, PFNGLSTENCILMASKSEPARATEPROC);
    pfnStencilMaskSeparate(face, mask);
}

void glAttachShader(GLuint program, GLuint shader)
{
    typedef void (APIENTRY * PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
    static PFNGLATTACHSHADERPROC pfnAttachShader = 0;
    LOAD_ENTRYPOINT("glAttachShader", pfnAttachShader, PFNGLATTACHSHADERPROC);
    pfnAttachShader(program, shader);
}

void glBindAttribLocation(GLuint program, GLuint index, const GLchar *name)
{
    typedef void (APIENTRY * PFNGLBINDATTRIBLOCATIONPROC) (GLuint program, GLuint index, const GLchar *name);
    static PFNGLBINDATTRIBLOCATIONPROC pfnBindAttribLocation = 0;
    LOAD_ENTRYPOINT("glBindAttribLocation", pfnBindAttribLocation, PFNGLBINDATTRIBLOCATIONPROC);
    pfnBindAttribLocation(program, index, name);
}

void glCompileShader(GLuint shader)
{
    typedef void (APIENTRY * PFNGLCOMPILESHADERPROC) (GLuint shader);
    static PFNGLCOMPILESHADERPROC pfnCompileShader = 0;
    LOAD_ENTRYPOINT("glCompileShader", pfnCompileShader, PFNGLCOMPILESHADERPROC);
    pfnCompileShader(shader);
}

GLuint glCreateProgram()
{
    typedef GLuint (APIENTRY * PFNGLCREATEPROGRAMPROC) ();
    static PFNGLCREATEPROGRAMPROC pfnCreateProgram = 0;
    LOAD_ENTRYPOINT("glCreateProgram", pfnCreateProgram, PFNGLCREATEPROGRAMPROC);
    return pfnCreateProgram();
}

GLuint glCreateShader(GLenum type)
{
    typedef GLuint (APIENTRY * PFNGLCREATESHADERPROC) (GLenum type);
    static PFNGLCREATESHADERPROC pfnCreateShader = 0;
    LOAD_ENTRYPOINT("glCreateShader", pfnCreateShader, PFNGLCREATESHADERPROC);
    return pfnCreateShader(type);
}

void glDeleteProgram(GLuint program)
{
    typedef void (APIENTRY * PFNGLDELETEPROGRAMPROC) (GLuint program);
    static PFNGLDELETEPROGRAMPROC pfnDeleteProgram = 0;
    LOAD_ENTRYPOINT("glDeleteProgram", pfnDeleteProgram, PFNGLDELETEPROGRAMPROC);
    pfnDeleteProgram(program);
}

void glDeleteShader(GLuint shader)
{
    typedef void (APIENTRY * PFNGLDELETESHADERPROC) (GLuint shader);
    static PFNGLDELETESHADERPROC pfnDeleteShader = 0;
    LOAD_ENTRYPOINT("glDeleteShader", pfnDeleteShader, PFNGLDELETESHADERPROC);
    pfnDeleteShader(shader);
}

void glDetachShader(GLuint program, GLuint shader)
{
    typedef void (APIENTRY * PFNGLDETACHSHADERPROC) (GLuint program, GLuint shader);
    static PFNGLDETACHSHADERPROC pfnDetachShader = 0;
    LOAD_ENTRYPOINT("glDetachShader", pfnDetachShader, PFNGLDETACHSHADERPROC);
    pfnDetachShader(program, shader);
}

void glDisableVertexAttribArray(GLuint index)
{
    typedef void (APIENTRY * PFNGLDISABLEVERTEXATTRIBARRAYPROC) (GLuint index);
    static PFNGLDISABLEVERTEXATTRIBARRAYPROC pfnDisableVertexAttribArray = 0;
    LOAD_ENTRYPOINT("glDisableVertexAttribArray", pfnDisableVertexAttribArray, PFNGLDISABLEVERTEXATTRIBARRAYPROC);
    pfnDisableVertexAttribArray(index);
}

void glEnableVertexAttribArray(GLuint index)
{
    typedef void (APIENTRY * PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
    static PFNGLENABLEVERTEXATTRIBARRAYPROC pfnEnableVertexAttribArray = 0;
    LOAD_ENTRYPOINT("glEnableVertexAttribArray", pfnEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC);
    pfnEnableVertexAttribArray(index);
}

void glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
    typedef void (APIENTRY * PFNGLGETACTIVEATTRIBPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
    static PFNGLGETACTIVEATTRIBPROC pfnGetActiveAttrib = 0;
    LOAD_ENTRYPOINT("glGetActiveAttrib", pfnGetActiveAttrib, PFNGLGETACTIVEATTRIBPROC);
    pfnGetActiveAttrib(program, index, bufSize, length, size, type, name);
}

void glGetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
    typedef void (APIENTRY * PFNGLGETACTIVEUNIFORMPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
    static PFNGLGETACTIVEUNIFORMPROC pfnGetActiveUniform = 0;
    LOAD_ENTRYPOINT("glGetActiveUniform", pfnGetActiveUniform, PFNGLGETACTIVEUNIFORMPROC);
    pfnGetActiveUniform(program, index, bufSize, length, size, type, name);
}

void glGetAttachedShaders(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *obj)
{
    typedef void (APIENTRY * PFNGLGETATTACHEDSHADERSPROC) (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *obj);
    static PFNGLGETATTACHEDSHADERSPROC pfnGetAttachedShaders = 0;
    LOAD_ENTRYPOINT("glGetAttachedShaders", pfnGetAttachedShaders, PFNGLGETATTACHEDSHADERSPROC);
    pfnGetAttachedShaders(program, maxCount, count, obj);
}

GLint glGetAttribLocation(GLuint program, const GLchar *name)
{
    typedef GLint (APIENTRY * PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const GLchar *name);
    static PFNGLGETATTRIBLOCATIONPROC pfnGetAttribLocation = 0;
    LOAD_ENTRYPOINT("glGetAttribLocation", pfnGetAttribLocation, PFNGLGETATTRIBLOCATIONPROC);
    return pfnGetAttribLocation(program, name);
}

void glGetProgramiv(GLuint program, GLenum pname, GLint *params)
{
    typedef void (APIENTRY * PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
    static PFNGLGETPROGRAMIVPROC pfnGetProgramiv = 0;
    LOAD_ENTRYPOINT("glGetProgramiv", pfnGetProgramiv, PFNGLGETPROGRAMIVPROC);
    pfnGetProgramiv(program, pname, params);
}

void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    typedef void (APIENTRY * PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
    static PFNGLGETPROGRAMINFOLOGPROC pfnGetProgramInfoLog = 0;
    LOAD_ENTRYPOINT("glGetProgramInfoLog", pfnGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC);
    pfnGetProgramInfoLog(program, bufSize, length, infoLog);
}

void glGetShaderiv(GLuint shader, GLenum pname, GLint *params)
{
    typedef void (APIENTRY * PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
    static PFNGLGETSHADERIVPROC pfnGetShaderiv = 0;
    LOAD_ENTRYPOINT("glGetShaderiv", pfnGetShaderiv, PFNGLGETSHADERIVPROC);
    pfnGetShaderiv(shader, pname, params);
}

void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
    typedef void (APIENTRY * PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
    static PFNGLGETSHADERINFOLOGPROC pfnGetShaderInfoLog = 0;
    LOAD_ENTRYPOINT("glGetShaderInfoLog", pfnGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC);
    pfnGetShaderInfoLog(shader, bufSize, length, infoLog);
}

void glGetShaderSource(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source)
{
    typedef void (APIENTRY * PFNGLGETSHADERSOURCEPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
    static PFNGLGETSHADERSOURCEPROC pfnGetShaderSource = 0;
    LOAD_ENTRYPOINT("glGetShaderSource", pfnGetShaderSource, PFNGLGETSHADERSOURCEPROC);
    pfnGetShaderSource(shader, bufSize, length, source);
}

GLint glGetUniformLocation(GLuint program, const GLchar *name)
{
    typedef GLint (APIENTRY * PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar *name);
    static PFNGLGETUNIFORMLOCATIONPROC pfnGetUniformLocation = 0;
    LOAD_ENTRYPOINT("glGetUniformLocation", pfnGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC);
    return pfnGetUniformLocation(program, name);
}

void glGetUniformfv(GLuint program, GLint location, GLfloat *params)
{
    typedef void (APIENTRY * PFNGLGETUNIFORMFVPROC) (GLuint program, GLint location, GLfloat *params);
    static PFNGLGETUNIFORMFVPROC pfnGetUniformfv = 0;
    LOAD_ENTRYPOINT("glGetUniformfv", pfnGetUniformfv, PFNGLGETUNIFORMFVPROC);
    pfnGetUniformfv(program, location, params);
}

void glGetUniformiv(GLuint program, GLint location, GLint *params)
{
    typedef void (APIENTRY * PFNGLGETUNIFORMIVPROC) (GLuint program, GLint location, GLint *params);
    static PFNGLGETUNIFORMIVPROC pfnGetUniformiv = 0;
    LOAD_ENTRYPOINT("glGetUniformiv", pfnGetUniformiv, PFNGLGETUNIFORMIVPROC);
    pfnGetUniformiv(program, location, params);
}

void glGetVertexAttribdv(GLuint index, GLenum pname, GLdouble *params)
{
    typedef void (APIENTRY * PFNGLGETVERTEXATTRIBDVPROC) (GLuint index, GLenum pname, GLdouble *params);
    static PFNGLGETVERTEXATTRIBDVPROC pfnGetVertexAttribdv = 0;
    LOAD_ENTRYPOINT("glGetVertexAttribdv", pfnGetVertexAttribdv, PFNGLGETVERTEXATTRIBDVPROC);
    pfnGetVertexAttribdv(index, pname, params);
}

void glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat *params)
{
    typedef void (APIENTRY * PFNGLGETVERTEXATTRIBFVPROC) (GLuint index, GLenum pname, GLfloat *params);
    static PFNGLGETVERTEXATTRIBFVPROC pfnGetVertexAttribfv = 0;
    LOAD_ENTRYPOINT("glGetVertexAttribfv", pfnGetVertexAttribfv, PFNGLGETVERTEXATTRIBFVPROC);
    pfnGetVertexAttribfv(index, pname, params);
}

void glGetVertexAttribiv(GLuint index, GLenum pname, GLint *params)
{
    typedef void (APIENTRY * PFNGLGETVERTEXATTRIBIVPROC) (GLuint index, GLenum pname, GLint *params);
    static PFNGLGETVERTEXATTRIBIVPROC pfnGetVertexAttribiv = 0;
    LOAD_ENTRYPOINT("glGetVertexAttribiv", pfnGetVertexAttribiv, PFNGLGETVERTEXATTRIBIVPROC);
    pfnGetVertexAttribiv(index, pname, params);
}

void glGetVertexAttribPointerv(GLuint index, GLenum pname, GLvoid* *pointer)
{
    typedef void (APIENTRY * PFNGLGETVERTEXATTRIBPOINTERVPROC) (GLuint index, GLenum pname, GLvoid* *pointer);
    static PFNGLGETVERTEXATTRIBPOINTERVPROC pfnGetVertexAttribPointerv = 0;
    LOAD_ENTRYPOINT("glGetVertexAttribPointerv", pfnGetVertexAttribPointerv, PFNGLGETVERTEXATTRIBPOINTERVPROC);
    pfnGetVertexAttribPointerv(index, pname, pointer);
}

GLboolean glIsProgram(GLuint program)
{
    typedef GLboolean (APIENTRY * PFNGLISPROGRAMPROC) (GLuint program);
    static PFNGLISPROGRAMPROC pfnIsProgram = 0;
    LOAD_ENTRYPOINT("glIsProgram", pfnIsProgram, PFNGLISPROGRAMPROC);
    return pfnIsProgram(program);
}

GLboolean glIsShader(GLuint shader)
{
    typedef GLboolean (APIENTRY * PFNGLISSHADERPROC) (GLuint shader);
    static PFNGLISSHADERPROC pfnIsShader = 0;
    LOAD_ENTRYPOINT("glIsShader", pfnIsShader, PFNGLISSHADERPROC);
    return pfnIsShader(shader);
}

void glLinkProgram(GLuint program)
{
    typedef void (APIENTRY * PFNGLLINKPROGRAMPROC) (GLuint program);
    static PFNGLLINKPROGRAMPROC pfnLinkProgram = 0;
    LOAD_ENTRYPOINT("glLinkProgram", pfnLinkProgram, PFNGLLINKPROGRAMPROC);
    pfnLinkProgram(program);
}

void glShaderSource(GLuint shader, GLsizei count, const GLchar* *string, const GLint *length)
{
    typedef void (APIENTRY * PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar* *string, const GLint *length);
    static PFNGLSHADERSOURCEPROC pfnShaderSource = 0;
    LOAD_ENTRYPOINT("glShaderSource", pfnShaderSource, PFNGLSHADERSOURCEPROC);
    pfnShaderSource(shader, count, string, length);
}

void glUseProgram(GLuint program)
{
    typedef void (APIENTRY * PFNGLUSEPROGRAMPROC) (GLuint program);
    static PFNGLUSEPROGRAMPROC pfnUseProgram = 0;
    LOAD_ENTRYPOINT("glUseProgram", pfnUseProgram, PFNGLUSEPROGRAMPROC);
    pfnUseProgram(program);
}

void glUniform1f(GLint location, GLfloat v0)
{
    typedef void (APIENTRY * PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
    static PFNGLUNIFORM1FPROC pfnUniform1f = 0;
    LOAD_ENTRYPOINT("glUniform1f", pfnUniform1f, PFNGLUNIFORM1FPROC);
    pfnUniform1f(location, v0);
}

void glUniform2f(GLint location, GLfloat v0, GLfloat v1)
{
    typedef void (APIENTRY * PFNGLUNIFORM2FPROC) (GLint location, GLfloat v0, GLfloat v1);
    static PFNGLUNIFORM2FPROC pfnUniform2f = 0;
    LOAD_ENTRYPOINT("glUniform2f", pfnUniform2f, PFNGLUNIFORM2FPROC);
    pfnUniform2f(location, v0, v1);
}

void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    typedef void (APIENTRY * PFNGLUNIFORM3FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
    static PFNGLUNIFORM3FPROC pfnUniform3f = 0;
    LOAD_ENTRYPOINT("glUniform3f", pfnUniform3f, PFNGLUNIFORM3FPROC);
    pfnUniform3f(location, v0, v1, v2);
}

void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    typedef void (APIENTRY * PFNGLUNIFORM4FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    static PFNGLUNIFORM4FPROC pfnUniform4f = 0;
    LOAD_ENTRYPOINT("glUniform4f", pfnUniform4f, PFNGLUNIFORM4FPROC);
    pfnUniform4f(location, v0, v1, v2, v3);
}

void glUniform1i(GLint location, GLint v0)
{
    typedef void (APIENTRY * PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
    static PFNGLUNIFORM1IPROC pfnUniform1i = 0;
    LOAD_ENTRYPOINT("glUniform1i", pfnUniform1i, PFNGLUNIFORM1IPROC);
    pfnUniform1i(location, v0);
}

void glUniform2i(GLint location, GLint v0, GLint v1)
{
    typedef void (APIENTRY * PFNGLUNIFORM2IPROC) (GLint location, GLint v0, GLint v1);
    static PFNGLUNIFORM2IPROC pfnUniform2i = 0;
    LOAD_ENTRYPOINT("glUniform2i", pfnUniform2i, PFNGLUNIFORM2IPROC);
    pfnUniform2i(location, v0, v1);
}

void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2)
{
    typedef void (APIENTRY * PFNGLUNIFORM3IPROC) (GLint location, GLint v0, GLint v1, GLint v2);
    static PFNGLUNIFORM3IPROC pfnUniform3i = 0;
    LOAD_ENTRYPOINT("glUniform3i", pfnUniform3i, PFNGLUNIFORM3IPROC);
    pfnUniform3i(location, v0, v1, v2);
}

void glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    typedef void (APIENTRY * PFNGLUNIFORM4IPROC) (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
    static PFNGLUNIFORM4IPROC pfnUniform4i = 0;
    LOAD_ENTRYPOINT("glUniform4i", pfnUniform4i, PFNGLUNIFORM4IPROC);
    pfnUniform4i(location, v0, v1, v2, v3);
}

void glUniform1fv(GLint location, GLsizei count, const GLfloat *value)
{
    typedef void (APIENTRY * PFNGLUNIFORM1FVPROC) (GLint location, GLsizei count, const GLfloat *value);
    static PFNGLUNIFORM1FVPROC pfnUniform1fv = 0;
    LOAD_ENTRYPOINT("glUniform1fv", pfnUniform1fv, PFNGLUNIFORM1FVPROC);
    pfnUniform1fv(location, count, value);
}

void glUniform2fv(GLint location, GLsizei count, const GLfloat *value)
{
    typedef void (APIENTRY * PFNGLUNIFORM2FVPROC) (GLint location, GLsizei count, const GLfloat *value);
    static PFNGLUNIFORM2FVPROC pfnUniform2fv = 0;
    LOAD_ENTRYPOINT("glUniform2fv", pfnUniform2fv, PFNGLUNIFORM2FVPROC);
    pfnUniform2fv(location, count, value);
}

void glUniform3fv(GLint location, GLsizei count, const GLfloat *value)
{
    typedef void (APIENTRY * PFNGLUNIFORM3FVPROC) (GLint location, GLsizei count, const GLfloat *value);
    static PFNGLUNIFORM3FVPROC pfnUniform3fv = 0;
    LOAD_ENTRYPOINT("glUniform3fv", pfnUniform3fv, PFNGLUNIFORM3FVPROC);
    pfnUniform3fv(location, count, value);
}

void glUniform4fv(GLint location, GLsizei count, const GLfloat *value)
{
    typedef void (APIENTRY * PFNGLUNIFORM4FVPROC) (GLint location, GLsizei count, const GLfloat *value);
    static PFNGLUNIFORM4FVPROC pfnUniform4fv = 0;
    LOAD_ENTRYPOINT("glUniform4fv", pfnUniform4fv, PFNGLUNIFORM4FVPROC);
    pfnUniform4fv(location, count, value);
}

void glUniform1iv(GLint location, GLsizei count, const GLint *value)
{
    typedef void (APIENTRY * PFNGLUNIFORM1IVPROC) (GLint location, GLsizei count, const GLint *value);
    static PFNGLUNIFORM1IVPROC pfnUniform1iv = 0;
    LOAD_ENTRYPOINT("glUniform1iv", pfnUniform1iv, PFNGLUNIFORM1IVPROC);
    pfnUniform1iv(location, count, value);
}

void glUniform2iv(GLint location, GLsizei count, const GLint *value)
{
    typedef void (APIENTRY * PFNGLUNIFORM2IVPROC) (GLint location, GLsizei count, const GLint *value);
    static PFNGLUNIFORM2IVPROC pfnUniform2iv = 0;
    LOAD_ENTRYPOINT("glUniform2iv", pfnUniform2iv, PFNGLUNIFORM2IVPROC);
    pfnUniform2iv(location, count, value);
}

void glUniform3iv(GLint location, GLsizei count, const GLint *value)
{
    typedef void (APIENTRY * PFNGLUNIFORM3IVPROC) (GLint location, GLsizei count, const GLint *value);
    static PFNGLUNIFORM3IVPROC pfnUniform3iv = 0;
    LOAD_ENTRYPOINT("glUniform3iv", pfnUniform3iv, PFNGLUNIFORM3IVPROC);
    pfnUniform3iv(location, count, value);
}

void glUniform4iv(GLint location, GLsizei count, const GLint *value)
{
    typedef void (APIENTRY * PFNGLUNIFORM4IVPROC) (GLint location, GLsizei count, const GLint *value);
    static PFNGLUNIFORM4IVPROC pfnUniform4iv = 0;
    LOAD_ENTRYPOINT("glUniform4iv", pfnUniform4iv, PFNGLUNIFORM4IVPROC);
    pfnUniform4iv(location, count, value);
}

void glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    typedef void (APIENTRY * PFNGLUNIFORMMATRIX2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    static PFNGLUNIFORMMATRIX2FVPROC pfnUniformMatrix2fv = 0;
    LOAD_ENTRYPOINT("glUniformMatrix2fv", pfnUniformMatrix2fv, PFNGLUNIFORMMATRIX2FVPROC);
    pfnUniformMatrix2fv(location, count, transpose, value);
}

void glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    typedef void (APIENTRY * PFNGLUNIFORMMATRIX3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    static PFNGLUNIFORMMATRIX3FVPROC pfnUniformMatrix3fv = 0;
    LOAD_ENTRYPOINT("glUniformMatrix3fv", pfnUniformMatrix3fv, PFNGLUNIFORMMATRIX3FVPROC);
    pfnUniformMatrix3fv(location, count, transpose, value);
}

void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    typedef void (APIENTRY * PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    static PFNGLUNIFORMMATRIX4FVPROC pfnUniformMatrix4fv = 0;
    LOAD_ENTRYPOINT("glUniformMatrix4fv", pfnUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC);
    pfnUniformMatrix4fv(location, count, transpose, value);
}

void glValidateProgram(GLuint program)
{
    typedef void (APIENTRY * PFNGLVALIDATEPROGRAMPROC) (GLuint program);
    static PFNGLVALIDATEPROGRAMPROC pfnValidateProgram = 0;
    LOAD_ENTRYPOINT("glValidateProgram", pfnValidateProgram, PFNGLVALIDATEPROGRAMPROC);
    pfnValidateProgram(program);
}

void glVertexAttrib1d(GLuint index, GLdouble x)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB1DPROC) (GLuint index, GLdouble x);
    static PFNGLVERTEXATTRIB1DPROC pfnVertexAttrib1d = 0;
    LOAD_ENTRYPOINT("glVertexAttrib1d", pfnVertexAttrib1d, PFNGLVERTEXATTRIB1DPROC);
    pfnVertexAttrib1d(index, x);
}

void glVertexAttrib1dv(GLuint index, const GLdouble *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB1DVPROC) (GLuint index, const GLdouble *v);
    static PFNGLVERTEXATTRIB1DVPROC pfnVertexAttrib1dv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib1dv", pfnVertexAttrib1dv, PFNGLVERTEXATTRIB1DVPROC);
    pfnVertexAttrib1dv(index, v);
}

void glVertexAttrib1f(GLuint index, GLfloat x)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB1FPROC) (GLuint index, GLfloat x);
    static PFNGLVERTEXATTRIB1FPROC pfnVertexAttrib1f = 0;
    LOAD_ENTRYPOINT("glVertexAttrib1f", pfnVertexAttrib1f, PFNGLVERTEXATTRIB1FPROC);
    pfnVertexAttrib1f(index, x);
}

void glVertexAttrib1fv(GLuint index, const GLfloat *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB1FVPROC) (GLuint index, const GLfloat *v);
    static PFNGLVERTEXATTRIB1FVPROC pfnVertexAttrib1fv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib1fv", pfnVertexAttrib1fv, PFNGLVERTEXATTRIB1FVPROC);
    pfnVertexAttrib1fv(index, v);
}

void glVertexAttrib1s(GLuint index, GLshort x)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB1SPROC) (GLuint index, GLshort x);
    static PFNGLVERTEXATTRIB1SPROC pfnVertexAttrib1s = 0;
    LOAD_ENTRYPOINT("glVertexAttrib1s", pfnVertexAttrib1s, PFNGLVERTEXATTRIB1SPROC);
    pfnVertexAttrib1s(index, x);
}

void glVertexAttrib1sv(GLuint index, const GLshort *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB1SVPROC) (GLuint index, const GLshort *v);
    static PFNGLVERTEXATTRIB1SVPROC pfnVertexAttrib1sv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib1sv", pfnVertexAttrib1sv, PFNGLVERTEXATTRIB1SVPROC);
    pfnVertexAttrib1sv(index, v);
}

void glVertexAttrib2d(GLuint index, GLdouble x, GLdouble y)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB2DPROC) (GLuint index, GLdouble x, GLdouble y);
    static PFNGLVERTEXATTRIB2DPROC pfnVertexAttrib2d = 0;
    LOAD_ENTRYPOINT("glVertexAttrib2d", pfnVertexAttrib2d, PFNGLVERTEXATTRIB2DPROC);
    pfnVertexAttrib2d(index, x, y);
}

void glVertexAttrib2dv(GLuint index, const GLdouble *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB2DVPROC) (GLuint index, const GLdouble *v);
    static PFNGLVERTEXATTRIB2DVPROC pfnVertexAttrib2dv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib2dv", pfnVertexAttrib2dv, PFNGLVERTEXATTRIB2DVPROC);
    pfnVertexAttrib2dv(index, v);
}

void glVertexAttrib2f(GLuint index, GLfloat x, GLfloat y)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB2FPROC) (GLuint index, GLfloat x, GLfloat y);
    static PFNGLVERTEXATTRIB2FPROC pfnVertexAttrib2f = 0;
    LOAD_ENTRYPOINT("glVertexAttrib2f", pfnVertexAttrib2f, PFNGLVERTEXATTRIB2FPROC);
    pfnVertexAttrib2f(index, x, y);
}

void glVertexAttrib2fv(GLuint index, const GLfloat *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB2FVPROC) (GLuint index, const GLfloat *v);
    static PFNGLVERTEXATTRIB2FVPROC pfnVertexAttrib2fv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib2fv", pfnVertexAttrib2fv, PFNGLVERTEXATTRIB2FVPROC);
    pfnVertexAttrib2fv(index, v);
}

void glVertexAttrib2s(GLuint index, GLshort x, GLshort y)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB2SPROC) (GLuint index, GLshort x, GLshort y);
    static PFNGLVERTEXATTRIB2SPROC pfnVertexAttrib2s = 0;
    LOAD_ENTRYPOINT("glVertexAttrib2s", pfnVertexAttrib2s, PFNGLVERTEXATTRIB2SPROC);
    pfnVertexAttrib2s(index, x, y);
}

void glVertexAttrib2sv(GLuint index, const GLshort *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB2SVPROC) (GLuint index, const GLshort *v);
    static PFNGLVERTEXATTRIB2SVPROC pfnVertexAttrib2sv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib2sv", pfnVertexAttrib2sv, PFNGLVERTEXATTRIB2SVPROC);
    pfnVertexAttrib2sv(index, v);
}

void glVertexAttrib3d(GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB3DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z);
    static PFNGLVERTEXATTRIB3DPROC pfnVertexAttrib3d = 0;
    LOAD_ENTRYPOINT("glVertexAttrib3d", pfnVertexAttrib3d, PFNGLVERTEXATTRIB3DPROC);
    pfnVertexAttrib3d(index, x, y, z);
}

void glVertexAttrib3dv(GLuint index, const GLdouble *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB3DVPROC) (GLuint index, const GLdouble *v);
    static PFNGLVERTEXATTRIB3DVPROC pfnVertexAttrib3dv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib3dv", pfnVertexAttrib3dv, PFNGLVERTEXATTRIB3DVPROC);
    pfnVertexAttrib3dv(index, v);
}

void glVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB3FPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z);
    static PFNGLVERTEXATTRIB3FPROC pfnVertexAttrib3f = 0;
    LOAD_ENTRYPOINT("glVertexAttrib3f", pfnVertexAttrib3f, PFNGLVERTEXATTRIB3FPROC);
    pfnVertexAttrib3f(index, x, y, z);
}

void glVertexAttrib3fv(GLuint index, const GLfloat *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB3FVPROC) (GLuint index, const GLfloat *v);
    static PFNGLVERTEXATTRIB3FVPROC pfnVertexAttrib3fv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib3fv", pfnVertexAttrib3fv, PFNGLVERTEXATTRIB3FVPROC);
    pfnVertexAttrib3fv(index, v);
}

void glVertexAttrib3s(GLuint index, GLshort x, GLshort y, GLshort z)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB3SPROC) (GLuint index, GLshort x, GLshort y, GLshort z);
    static PFNGLVERTEXATTRIB3SPROC pfnVertexAttrib3s = 0;
    LOAD_ENTRYPOINT("glVertexAttrib3s", pfnVertexAttrib3s, PFNGLVERTEXATTRIB3SPROC);
    pfnVertexAttrib3s(index, x, y, z);
}

void glVertexAttrib3sv(GLuint index, const GLshort *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB3SVPROC) (GLuint index, const GLshort *v);
    static PFNGLVERTEXATTRIB3SVPROC pfnVertexAttrib3sv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib3sv", pfnVertexAttrib3sv, PFNGLVERTEXATTRIB3SVPROC);
    pfnVertexAttrib3sv(index, v);
}

void glVertexAttrib4Nbv(GLuint index, const GLbyte *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4NBVPROC) (GLuint index, const GLbyte *v);
    static PFNGLVERTEXATTRIB4NBVPROC pfnVertexAttrib4Nbv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4Nbv", pfnVertexAttrib4Nbv, PFNGLVERTEXATTRIB4NBVPROC);
    pfnVertexAttrib4Nbv(index, v);
}

void glVertexAttrib4Niv(GLuint index, const GLint *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4NIVPROC) (GLuint index, const GLint *v);
    static PFNGLVERTEXATTRIB4NIVPROC pfnVertexAttrib4Niv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4Niv", pfnVertexAttrib4Niv, PFNGLVERTEXATTRIB4NIVPROC);
    pfnVertexAttrib4Niv(index, v);
}

void glVertexAttrib4Nsv(GLuint index, const GLshort *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4NSVPROC) (GLuint index, const GLshort *v);
    static PFNGLVERTEXATTRIB4NSVPROC pfnVertexAttrib4Nsv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4Nsv", pfnVertexAttrib4Nsv, PFNGLVERTEXATTRIB4NSVPROC);
    pfnVertexAttrib4Nsv(index, v);
}

void glVertexAttrib4Nub(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUBPROC) (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
    static PFNGLVERTEXATTRIB4NUBPROC pfnVertexAttrib4Nub = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4Nub", pfnVertexAttrib4Nub, PFNGLVERTEXATTRIB4NUBPROC);
    pfnVertexAttrib4Nub(index, x, y, z, w);
}

void glVertexAttrib4Nubv(GLuint index, const GLubyte *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUBVPROC) (GLuint index, const GLubyte *v);
    static PFNGLVERTEXATTRIB4NUBVPROC pfnVertexAttrib4Nubv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4Nubv", pfnVertexAttrib4Nubv, PFNGLVERTEXATTRIB4NUBVPROC);
    pfnVertexAttrib4Nubv(index, v);
}

void glVertexAttrib4Nuiv(GLuint index, const GLuint *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUIVPROC) (GLuint index, const GLuint *v);
    static PFNGLVERTEXATTRIB4NUIVPROC pfnVertexAttrib4Nuiv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4Nuiv", pfnVertexAttrib4Nuiv, PFNGLVERTEXATTRIB4NUIVPROC);
    pfnVertexAttrib4Nuiv(index, v);
}

void glVertexAttrib4Nusv(GLuint index, const GLushort *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUSVPROC) (GLuint index, const GLushort *v);
    static PFNGLVERTEXATTRIB4NUSVPROC pfnVertexAttrib4Nusv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4Nusv", pfnVertexAttrib4Nusv, PFNGLVERTEXATTRIB4NUSVPROC);
    pfnVertexAttrib4Nusv(index, v);
}

void glVertexAttrib4bv(GLuint index, const GLbyte *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4BVPROC) (GLuint index, const GLbyte *v);
    static PFNGLVERTEXATTRIB4BVPROC pfnVertexAttrib4bv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4bv", pfnVertexAttrib4bv, PFNGLVERTEXATTRIB4BVPROC);
    pfnVertexAttrib4bv(index, v);
}

void glVertexAttrib4d(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
    static PFNGLVERTEXATTRIB4DPROC pfnVertexAttrib4d = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4d", pfnVertexAttrib4d, PFNGLVERTEXATTRIB4DPROC);
    pfnVertexAttrib4d(index, x, y, z, w);
}

void glVertexAttrib4dv(GLuint index, const GLdouble *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4DVPROC) (GLuint index, const GLdouble *v);
    static PFNGLVERTEXATTRIB4DVPROC pfnVertexAttrib4dv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4dv", pfnVertexAttrib4dv, PFNGLVERTEXATTRIB4DVPROC);
    pfnVertexAttrib4dv(index, v);
}

void glVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4FPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    static PFNGLVERTEXATTRIB4FPROC pfnVertexAttrib4f = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4f", pfnVertexAttrib4f, PFNGLVERTEXATTRIB4FPROC);
    pfnVertexAttrib4f(index, x, y, z, w);
}

void glVertexAttrib4fv(GLuint index, const GLfloat *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4FVPROC) (GLuint index, const GLfloat *v);
    static PFNGLVERTEXATTRIB4FVPROC pfnVertexAttrib4fv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4fv", pfnVertexAttrib4fv, PFNGLVERTEXATTRIB4FVPROC);
    pfnVertexAttrib4fv(index, v);
}

void glVertexAttrib4iv(GLuint index, const GLint *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4IVPROC) (GLuint index, const GLint *v);
    static PFNGLVERTEXATTRIB4IVPROC pfnVertexAttrib4iv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4iv", pfnVertexAttrib4iv, PFNGLVERTEXATTRIB4IVPROC);
    pfnVertexAttrib4iv(index, v);
}

void glVertexAttrib4s(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4SPROC) (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
    static PFNGLVERTEXATTRIB4SPROC pfnVertexAttrib4s = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4s", pfnVertexAttrib4s, PFNGLVERTEXATTRIB4SPROC);
    pfnVertexAttrib4s(index, x, y, z, w);
}

void glVertexAttrib4sv(GLuint index, const GLshort *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4SVPROC) (GLuint index, const GLshort *v);
    static PFNGLVERTEXATTRIB4SVPROC pfnVertexAttrib4sv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4sv", pfnVertexAttrib4sv, PFNGLVERTEXATTRIB4SVPROC);
    pfnVertexAttrib4sv(index, v);
}

void glVertexAttrib4ubv(GLuint index, const GLubyte *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4UBVPROC) (GLuint index, const GLubyte *v);
    static PFNGLVERTEXATTRIB4UBVPROC pfnVertexAttrib4ubv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4ubv", pfnVertexAttrib4ubv, PFNGLVERTEXATTRIB4UBVPROC);
    pfnVertexAttrib4ubv(index, v);
}

void glVertexAttrib4uiv(GLuint index, const GLuint *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4UIVPROC) (GLuint index, const GLuint *v);
    static PFNGLVERTEXATTRIB4UIVPROC pfnVertexAttrib4uiv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4uiv", pfnVertexAttrib4uiv, PFNGLVERTEXATTRIB4UIVPROC);
    pfnVertexAttrib4uiv(index, v);
}

void glVertexAttrib4usv(GLuint index, const GLushort *v)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIB4USVPROC) (GLuint index, const GLushort *v);
    static PFNGLVERTEXATTRIB4USVPROC pfnVertexAttrib4usv = 0;
    LOAD_ENTRYPOINT("glVertexAttrib4usv", pfnVertexAttrib4usv, PFNGLVERTEXATTRIB4USVPROC);
    pfnVertexAttrib4usv(index, v);
}

void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer)
{
    typedef void (APIENTRY * PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
    static PFNGLVERTEXATTRIBPOINTERPROC pfnVertexAttribPointer = 0;
    LOAD_ENTRYPOINT("glVertexAttribPointer", pfnVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC);
    pfnVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

//
// OpenGL 2.1
//

void glUniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    typedef void (APIENTRY * PFNGLUNIFORMMATRIX2X3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    static PFNGLUNIFORMMATRIX2X3FVPROC pfnUniformMatrix2x3fv = 0;
    LOAD_ENTRYPOINT("glUniformMatrix2x3fv", pfnUniformMatrix2x3fv, PFNGLUNIFORMMATRIX2X3FVPROC);
    pfnUniformMatrix2x3fv(location, count, transpose, value);
}

void glUniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    typedef void (APIENTRY * PFNGLUNIFORMMATRIX3X2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    static PFNGLUNIFORMMATRIX3X2FVPROC pfnUniformMatrix3x2fv = 0;
    LOAD_ENTRYPOINT("glUniformMatrix3x2fv", pfnUniformMatrix3x2fv, PFNGLUNIFORMMATRIX3X2FVPROC);
    pfnUniformMatrix3x2fv(location, count, transpose, value);
}

void glUniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    typedef void (APIENTRY * PFNGLUNIFORMMATRIX2X4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    static PFNGLUNIFORMMATRIX2X4FVPROC pfnUniformMatrix2x4fv = 0;
    LOAD_ENTRYPOINT("glUniformMatrix2x4fv", pfnUniformMatrix2x4fv, PFNGLUNIFORMMATRIX2X4FVPROC);
    pfnUniformMatrix2x4fv(location, count, transpose, value);
}

void glUniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    typedef void (APIENTRY * PFNGLUNIFORMMATRIX4X2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    static PFNGLUNIFORMMATRIX4X2FVPROC pfnUniformMatrix4x2fv = 0;
    LOAD_ENTRYPOINT("glUniformMatrix4x2fv", pfnUniformMatrix4x2fv, PFNGLUNIFORMMATRIX4X2FVPROC);
    pfnUniformMatrix4x2fv(location, count, transpose, value);
}

void glUniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    typedef void (APIENTRY * PFNGLUNIFORMMATRIX3X4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    static PFNGLUNIFORMMATRIX3X4FVPROC pfnUniformMatrix3x4fv = 0;
    LOAD_ENTRYPOINT("glUniformMatrix3x4fv", pfnUniformMatrix3x4fv, PFNGLUNIFORMMATRIX3X4FVPROC);
    pfnUniformMatrix3x4fv(location, count, transpose, value);
}

void glUniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    typedef void (APIENTRY * PFNGLUNIFORMMATRIX4X3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    static PFNGLUNIFORMMATRIX4X3FVPROC pfnUniformMatrix4x3fv = 0;
    LOAD_ENTRYPOINT("glUniformMatrix4x3fv", pfnUniformMatrix4x3fv, PFNGLUNIFORMMATRIX4X3FVPROC);
    pfnUniformMatrix4x3fv(location, count, transpose, value);
}