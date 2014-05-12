//-----------------------------------------------------------------------------
//
// Play with the code to get a better understanding of it.


// This demo implements procedural terrain texturing on the GPU using a series
// of OpenGL Shading Language (GLSL) shader programs. The demo renders a
// terrain generated from a height map texture. The terrain is lit using a per
// fragment Lambert shader. The terrain texture is generated in the fragment
// shader using the terrain height map and 4 tileable terrain textures. The
// fragment shader mixes and blends these tileable terrain textures together to
// paint the terrain to give it the appearance of a very high resolution
// terrain texture map draped over the terrain mesh. The benefit of this
// approach is that a conventional terrain texture is limited by the maximum
// texture size supported by OpenGL. This is typically around 4096 x 4096 for
// most mid to high end video cards. Generating the terrain texture on the GPU
// overcomes this limit.
// 
// The demo allows the player to walk along the terrain using typical first
// person style controls. The terrain is randomly generated using a diamond
// square (mid point displacement) fractal algorithm. The terrain texture is
// created from 4 tileable textures: dirt, grass, rock, and snow.
//
//-----------------------------------------------------------------------------

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#if defined(_DEBUG)
#include <crtdbg.h>
#endif

#include "bitmap.h"
#include "camera.h"
#include "gl_font.h"
#include "input.h"
#include "mathlib.h"
#include "opengl.h"
#include "terrain.h"
#include "WGL_ARB_multisample.h"

//-----------------------------------------------------------------------------
// Constants.
//-----------------------------------------------------------------------------

#define APP_TITLE "GLSL Terrain Texturing Demo"

// Windows Vista compositing support.
#if !defined(PFD_SUPPORT_COMPOSITION)
#define PFD_SUPPORT_COMPOSITION 0x00008000
#endif

// GL_EXT_texture_filter_anisotropic
#define GL_TEXTURE_MAX_ANISOTROPY_EXT     0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

const int       TERRAIN_REGIONS_COUNT = 4;

const float     HEIGHTMAP_ROUGHNESS = 1.2f; //  float > 0 Roughness Increases. Determines smoothness of terrain
const float     HEIGHTMAP_SCALE = 2.0f;
const float     HEIGHTMAP_TILING_FACTOR = 12.0f;
const int       HEIGHTMAP_SIZE = 128; // SIZE OF MAP
const int       HEIGHTMAP_GRID_SPACING = 16;

const float     CAMERA_FOVX = 90.0f;
const float     CAMERA_ZFAR = HEIGHTMAP_SIZE * HEIGHTMAP_GRID_SPACING * 2.0f;
const float     CAMERA_ZNEAR = 1.0f;
const float     CAMERA_Y_OFFSET = 20.0f; //CAMERA OFFSET TO GROUND
const Vector3   CAMERA_ACCELERATION(400.0f, 400.0f, 400.0f);
const Vector3   CAMERA_VELOCITY(100.0f, 100.0f, 100.0f);

//-----------------------------------------------------------------------------
// Types.
//-----------------------------------------------------------------------------

struct TerrainRegion
{
    float min;
    float max;
    GLuint texture;
    std::string filename;
};

//-----------------------------------------------------------------------------
// Globals.
//-----------------------------------------------------------------------------

HWND                g_hWnd;
HDC                 g_hDC;
HGLRC               g_hRC;
HINSTANCE           g_hInstance;
int                 g_framesPerSecond;
int                 g_windowWidth;
int                 g_windowHeight;
int                 g_maxAnisotrophy;
bool                g_isFullScreen;
bool                g_hasFocus;
bool                g_enableVerticalSync;
bool                g_displayHelp;
bool                g_disableColorMaps;
float               g_lightDir[4] = {0.0f, 1.0f, 0.0f, 0.0f};
GLuint              g_nullTexture;
GLuint              g_terrainShader;
GLFont              g_font;
Terrain             g_terrain;
Camera              g_camera;
Vector3             g_cameraBoundsMax;
Vector3             g_cameraBoundsMin;

TerrainRegion g_regions[TERRAIN_REGIONS_COUNT] =
{
    // Terrain region 1.
    0.0f, 50.0f * HEIGHTMAP_SCALE, 0, "content/textures/dirt.jpg",

    // Terrain region 2.
    51.0f * HEIGHTMAP_SCALE, 101.0f * HEIGHTMAP_SCALE, 0, "content/textures/grass.jpg",

    // Terrain region 3.
    102.0f * HEIGHTMAP_SCALE, 203.0f * HEIGHTMAP_SCALE, 0, "content/textures/rock.jpg",

    // Terrain region 4.
    204.0f * HEIGHTMAP_SCALE, 255.0f * HEIGHTMAP_SCALE, 0, "content/textures/snow.jpg"
};

//-----------------------------------------------------------------------------
// Functions Prototypes. Declaration but not a Definition (doesnt include return type so doesnt create the function object)
//-----------------------------------------------------------------------------

void    BindTexture(GLuint texture, GLuint unit);
void    Cleanup();
void    CleanupApp();
GLuint  CompileShader(GLenum type, const GLchar *pszSource, GLint length);
HWND    CreateAppWindow(const WNDCLASSEX &wcl, const char *pszTitle);
GLuint  CreateNullTexture(int width, int height);
void    EnableVerticalSync(bool enableVerticalSync);
void    GenerateTerrain();
float   GetElapsedTimeInSeconds();
Vector3 GetMovementDirection();
bool    Init();
void    InitApp();
void    InitGL();
GLuint  LinkShaders(GLuint vertShader, GLuint fragShader);
GLuint  LoadShaderProgram(const char *pszFilename, std::string &infoLog);
GLuint  LoadTexture(const char *pszFilename);
GLuint  LoadTexture(const char *pszFilename, GLint magFilter, GLint minFilter,
                    GLint wrapS, GLint wrapT);
void    PerformCameraCollisionDetection();
void    ProcessUserInput();
void    ReadTextFile(const char *pszFilename, std::string &buffer);
void    RenderFrame();
void    RenderTerrain();
void    RenderText();
void    SetProcessorAffinity();
void    ToggleFullScreen();
void    UpdateCamera(float elapsedTimeSec);
void    UpdateFrame(float elapsedTimeSec);
void    UpdateFrameRate(float elapsedTimeSec);
void    UpdateTerrainShaderParameters();
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//-----------------------------------------------------------------------------
// Functions.
//-----------------------------------------------------------------------------


/*To start a Win32-based application - c compilier first looks here (main method almost)
Just as every C application and C++ application must have a main function as its starting point,
 every Win32-based application must have a WinMain function. WinMain has the following syntax.
*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#if defined _DEBUG
    _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif

    MSG msg = {0};
    WNDCLASSEX wcl = {0};

    wcl.cbSize = sizeof(wcl);
    wcl.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wcl.lpfnWndProc = WindowProc;
    wcl.cbClsExtra = 0;
    wcl.cbWndExtra = 0;
    wcl.hInstance = g_hInstance = hInstance;
    wcl.hIcon = LoadIcon(0, IDI_APPLICATION);
    wcl.hCursor = LoadCursor(0, IDC_ARROW);
    wcl.hbrBackground = 0;
    wcl.lpszMenuName = 0;
    wcl.lpszClassName = "GL2WindowClass";
    wcl.hIconSm = 0;

    if (!RegisterClassEx(&wcl))
        return 0;

    g_hWnd = CreateAppWindow(wcl, APP_TITLE);

    if (g_hWnd)
    {
        SetProcessorAffinity();

        if (Init())
        {
            ShowWindow(g_hWnd, nShowCmd);
            UpdateWindow(g_hWnd);

            while (true)
            {
                while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
                {
                    if (msg.message == WM_QUIT)
                        break;

                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                if (msg.message == WM_QUIT)
                    break;

                if (g_hasFocus)
                {
                    UpdateFrame(GetElapsedTimeInSeconds());
                    RenderFrame();
                    SwapBuffers(g_hDC);
                }
                else
                {
                    WaitMessage();
                }
            }
        }

        Cleanup();
        UnregisterClass(wcl.lpszClassName, hInstance);
    }

    return static_cast<int>(msg.wParam);
}
// "callback" is a function pointer that is called everytime a certain condition is met. 
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_ACTIVATE:
        switch (wParam)
        {
        default:
            break;

        case WA_ACTIVE:
        case WA_CLICKACTIVE:
            Mouse::instance().attach(hWnd);
            g_hasFocus = true;
            break;

        case WA_INACTIVE:
            if (g_isFullScreen)
                ShowWindow(hWnd, SW_MINIMIZE);
            Mouse::instance().detach();
            g_hasFocus = false;
            break;
        }
        break;
    
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_SIZE:
        g_windowWidth = static_cast<int>(LOWORD(lParam));
        g_windowHeight = static_cast<int>(HIWORD(lParam));
        break;

    default:
        Mouse::instance().handleMsg(hWnd, msg, wParam, lParam);
        Keyboard::instance().handleMsg(hWnd, msg, wParam, lParam);
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

//----------------------------------------------------------------------------------------------------// enough with the windows crap

void BindTexture(GLuint texture, GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Cleanup()
{
    CleanupApp();

    if (g_hDC)
    {
        if (g_hRC)
        {
            wglMakeCurrent(g_hDC, 0);
            wglDeleteContext(g_hRC);
            g_hRC = 0;
        }

        ReleaseDC(g_hWnd, g_hDC);
        g_hDC = 0;
    }
}

void CleanupApp()
{
    for (int i = 0; i < TERRAIN_REGIONS_COUNT; ++i)
    {
        if (g_regions[i].texture)
        {
            glDeleteTextures(1, &g_regions[i].texture);
            g_regions[i].texture = 0;
        }
    }

    if (g_nullTexture)
    {
        glDeleteTextures(1, &g_nullTexture);
        g_nullTexture = 0;
    }

    if (g_terrainShader)
    {
        glUseProgram(0);
        glDeleteProgram(g_terrainShader);
        g_terrainShader = 0;
    }

    g_terrain.destroy();
    g_font.destroy();
}

GLuint CompileShader(GLenum type, const GLchar *pszSource, GLint length)
{
    // Compiles the shader given it's source code. Returns the shader object.
    // A std::string object containing the shader's info log is thrown if the
    // shader failed to compile.
    //
    // 'type' is either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
    // 'pszSource' is a C style string containing the shader's source code.
    // 'length' is the length of 'pszSource'.

    GLuint shader = glCreateShader(type);

    if (shader)
    {
        GLint compiled = 0;

        glShaderSource(shader, 1, &pszSource, &length);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled)
        {
            GLsizei infoLogSize = 0;
            std::string infoLog;

            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogSize);
            infoLog.resize(infoLogSize);
            glGetShaderInfoLog(shader, infoLogSize, &infoLogSize, &infoLog[0]);

            throw infoLog;
        }
    }

    return shader;
}

HWND CreateAppWindow(const WNDCLASSEX &wcl, const char *pszTitle)
{
    // Create a window that is centered on the desktop. It's exactly 1/4 the
    // size of the desktop. Don't allow it to be resized.

    DWORD wndExStyle = WS_EX_OVERLAPPEDWINDOW;
    DWORD wndStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
        WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

    HWND hWnd = CreateWindowEx(wndExStyle, wcl.lpszClassName, pszTitle,
        wndStyle, 0, 0, 0, 0, 0, 0, wcl.hInstance, 0);

    if (hWnd)
    {
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        int halfScreenWidth = screenWidth / 2;
        int halfScreenHeight = screenHeight / 2;
        int left = (screenWidth - halfScreenWidth) / 2;
        int top = (screenHeight - halfScreenHeight) / 2;
        RECT rc = {0};

        SetRect(&rc, left, top, left + halfScreenWidth, top + halfScreenHeight);
        AdjustWindowRectEx(&rc, wndStyle, FALSE, wndExStyle);
        MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);

        GetClientRect(hWnd, &rc);
        g_windowWidth = rc.right - rc.left;
        g_windowHeight = rc.bottom - rc.top;
    }

    return hWnd;
}

GLuint CreateNullTexture(int width, int height)
{
    // Create an empty white texture. This texture is applied to models
    // that don't have any texture maps. This trick allows the same shader to
    // be used to draw the model with and without textures applied.

    int pitch = ((width * 32 + 31) & ~31) >> 3; // align to 4-byte boundaries
    std::vector<GLubyte> pixels(pitch * height, 255);
    GLuint texture = 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA,
        GL_UNSIGNED_BYTE, &pixels[0]);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

void EnableVerticalSync(bool enableVerticalSync)
{
    // WGL_EXT_swap_control.

    typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC)(GLint);

    static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
        reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(
        wglGetProcAddress("wglSwapIntervalEXT"));

    if (wglSwapIntervalEXT)
    {
        wglSwapIntervalEXT(enableVerticalSync ? 1 : 0);
        g_enableVerticalSync = enableVerticalSync;
    }
}

void GenerateTerrain()
{
    if (!g_terrain.generateUsingDiamondSquareFractal(HEIGHTMAP_ROUGHNESS))
        throw std::runtime_error("Failed to generate terrain.");
}

float GetElapsedTimeInSeconds()
{
    // Returns the elapsed time (in seconds) since the last time this function
    // was called. This elaborate setup is to guard against large spikes in
    // the time returned by QueryPerformanceCounter().

    static const int MAX_SAMPLE_COUNT = 50;

    static float frameTimes[MAX_SAMPLE_COUNT];
    static float timeScale = 0.0f;
    static float actualElapsedTimeSec = 0.0f;
    static INT64 freq = 0;
    static INT64 lastTime = 0;
    static int sampleCount = 0;
    static bool initialized = false;

    INT64 time = 0;
    float elapsedTimeSec = 0.0f;

    if (!initialized)
    {
        initialized = true;
        QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&freq));
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&lastTime));
        timeScale = 1.0f / freq;
    }

    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&time));
    elapsedTimeSec = (time - lastTime) * timeScale;
    lastTime = time;

    if (fabsf(elapsedTimeSec - actualElapsedTimeSec) < 1.0f)
    {
        memmove(&frameTimes[1], frameTimes, sizeof(frameTimes) - sizeof(frameTimes[0]));
        frameTimes[0] = elapsedTimeSec;

        if (sampleCount < MAX_SAMPLE_COUNT)
            ++sampleCount;
    }

    actualElapsedTimeSec = 0.0f;

    for (int i = 0; i < sampleCount; ++i)
        actualElapsedTimeSec += frameTimes[i];

    if (sampleCount > 0)
        actualElapsedTimeSec /= sampleCount;

    return actualElapsedTimeSec;
}

Vector3 GetMovementDirection()
{
    static bool moveForwardsPressed = false;
    static bool moveBackwardsPressed = false;
    static bool moveRightPressed = false;
    static bool moveLeftPressed = false;
    static bool moveUpPressed = false;
    static bool moveDownPressed = false;

    Vector3 direction(0.0f, 0.0f, 0.0f);
    Vector3 velocity = g_camera.getCurrentVelocity();
    Keyboard &keyboard = Keyboard::instance();

    if (keyboard.keyDown(Keyboard::KEY_UP) || keyboard.keyDown(Keyboard::KEY_W))
    {
        if (!moveForwardsPressed)
        {
            moveForwardsPressed = true;
            g_camera.setCurrentVelocity(velocity.x, velocity.y, 0.0f);
        }

        direction.z += 1.0f;
    }
    else
    {
        moveForwardsPressed = false;
    }

    if (keyboard.keyDown(Keyboard::KEY_DOWN) || keyboard.keyDown(Keyboard::KEY_S))
    {
        if (!moveBackwardsPressed)
        {
            moveBackwardsPressed = true;
            g_camera.setCurrentVelocity(velocity.x, velocity.y, 0.0f);
        }

        direction.z -= 1.0f;
    }
    else
    {
        moveBackwardsPressed = false;
    }

    if (keyboard.keyDown(Keyboard::KEY_RIGHT) || keyboard.keyDown(Keyboard::KEY_D))
    {
        if (!moveRightPressed)
        {
            moveRightPressed = true;
            g_camera.setCurrentVelocity(0.0f, velocity.y, velocity.z);
        }

        direction.x += 1.0f;
    }
    else
    {
        moveRightPressed = false;
    }

    if (keyboard.keyDown(Keyboard::KEY_LEFT) || keyboard.keyDown(Keyboard::KEY_A))
    {
        if (!moveLeftPressed)
        {
            moveLeftPressed = true;
            g_camera.setCurrentVelocity(0.0f, velocity.y, velocity.z);
        }

        direction.x -= 1.0f;
    }
    else
    {
        moveLeftPressed = false;
    }

    if (keyboard.keyDown(Keyboard::KEY_E) || keyboard.keyDown(Keyboard::KEY_PAGEUP))
    {
        if (!moveUpPressed)
        {
            moveUpPressed = true;
            g_camera.setCurrentVelocity(velocity.x, 0.0f, velocity.z);
        }

        direction.y += 1.0f;
    }
    else
    {
        moveUpPressed = false;
    }

    if (keyboard.keyDown(Keyboard::KEY_Q) || keyboard.keyDown(Keyboard::KEY_PAGEDOWN))
    {
        if (!moveDownPressed)
        {
            moveDownPressed = true;
            g_camera.setCurrentVelocity(velocity.x, 0.0f, velocity.z);
        }

        direction.y -= 1.0f;
    }
    else
    {
        moveDownPressed = false;
    }

    return direction;
}

bool Init()
{
    try
    {
        InitGL();

        if (!OpenGLSupportsGLVersion(2, 0))
            throw std::runtime_error("This application requires OpenGL 2.0 support.");

        InitApp();
        return true;
    }
    catch (const std::exception &e)
    {
        std::ostringstream msg;

        msg << "Application initialization failed!" << std::endl << std::endl;
        msg << e.what();

        MessageBox(g_hWnd, msg.str().c_str(), "Error", MB_ICONSTOP);
        return false;
    }
}

void InitApp()
{
    // Setup fonts.

    if (!g_font.create("Arial", 10, GLFont::BOLD))
        throw std::runtime_error("Failed to create font.");

    // Setup textures.

    if (!(g_nullTexture = CreateNullTexture(2, 2)))
        throw std::runtime_error("failed to create null texture.");

    for (int i = 0; i < TERRAIN_REGIONS_COUNT; ++i)
    {
        if (!(g_regions[i].texture = LoadTexture(g_regions[i].filename.c_str())))
            throw std::runtime_error("Failed to load texture: " + g_regions[i].filename);
    }

    // Setup shaders.

    std::string infoLog;

    if (!(g_terrainShader = LoadShaderProgram("content/shaders/terrain.glsl", infoLog)))
        throw std::runtime_error("Failed to load shader: terrain.glsl.\n" + infoLog);

    // Setup terrain.

    if (!g_terrain.create(HEIGHTMAP_SIZE, HEIGHTMAP_GRID_SPACING, HEIGHTMAP_SCALE))
        throw std::runtime_error("Failed to create terrain.");

    GenerateTerrain();
            
    // Setup camera.

    Vector3 pos;

    pos.x = HEIGHTMAP_SIZE * HEIGHTMAP_GRID_SPACING * 0.5f;
    pos.z = HEIGHTMAP_SIZE * HEIGHTMAP_GRID_SPACING * 0.5f;
    pos.y = g_terrain.getHeightMap().heightAt(pos.x, pos.z) + CAMERA_Y_OFFSET;

    g_camera.setBehavior(Camera::CAMERA_BEHAVIOR_FIRST_PERSON);
    g_camera.setPosition(pos);
    g_camera.setAcceleration(CAMERA_ACCELERATION);
    g_camera.setVelocity(CAMERA_VELOCITY);

    g_camera.perspective(CAMERA_FOVX,
        static_cast<float>(g_windowWidth) / static_cast<float>(g_windowHeight),
        CAMERA_ZNEAR, CAMERA_ZFAR);

    float upperBounds = (HEIGHTMAP_SIZE * HEIGHTMAP_GRID_SPACING - (2.0f * HEIGHTMAP_GRID_SPACING));
    float lowerBounds = static_cast<float>(HEIGHTMAP_GRID_SPACING);

    g_cameraBoundsMax.x = upperBounds;
    g_cameraBoundsMax.y = CAMERA_ZFAR;
    g_cameraBoundsMax.z = upperBounds;

    g_cameraBoundsMin.x = lowerBounds;
    g_cameraBoundsMin.y = 0.0f;
    g_cameraBoundsMin.z = lowerBounds;

    // Setup input.

    Mouse::instance().hideCursor(true);
    Mouse::instance().setPosition(g_windowWidth / 2, g_windowHeight / 2);
}

void InitGL()
{
    if (!(g_hDC = GetDC(g_hWnd)))
        throw std::runtime_error("GetDC() failed.");

    int pf = 0;
    PIXELFORMATDESCRIPTOR pfd = {0};
    OSVERSIONINFO osvi = {0};

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if (!GetVersionEx(&osvi))
        throw std::runtime_error("GetVersionEx() failed.");

    // When running under Windows Vista or later support desktop composition.
    if (osvi.dwMajorVersion > 6 || (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion >= 0))
        pfd.dwFlags |=  PFD_SUPPORT_COMPOSITION;

    ChooseBestAntiAliasingPixelFormat(pf);

    if (!pf)
        pf = ChoosePixelFormat(g_hDC, &pfd);

    if (!SetPixelFormat(g_hDC, pf, &pfd))
        throw std::runtime_error("SetPixelFormat() failed.");

    if (!(g_hRC = wglCreateContext(g_hDC)))
        throw std::runtime_error("wglCreateContext() failed.");

    if (!wglMakeCurrent(g_hDC, g_hRC))
        throw std::runtime_error("wglMakeCurrent() failed.");

    EnableVerticalSync(false);

    // Check for GL_EXT_texture_filter_anisotropic support.
    if (OpenGLExtensionSupported("GL_EXT_texture_filter_anisotropic"))
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &g_maxAnisotrophy);
    else
        g_maxAnisotrophy = 1;
}

GLuint LinkShaders(GLuint vertShader, GLuint fragShader)
{
    // Links the compiled vertex and/or fragment shaders into an executable
    // shader program. Returns the executable shader object. If the shaders
    // failed to link into an executable shader program, then a std::string
    // object is thrown containing the info log.

    GLuint program = glCreateProgram();

    if (program)
    {
        GLint linked = 0;

        if (vertShader)
            glAttachShader(program, vertShader);

        if (fragShader)
            glAttachShader(program, fragShader);

        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &linked);

        if (!linked)
        {
            GLsizei infoLogSize = 0;
            std::string infoLog;

            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogSize);
            infoLog.resize(infoLogSize);
            glGetProgramInfoLog(program, infoLogSize, &infoLogSize, &infoLog[0]);

            throw infoLog;
        }

        // Mark the two attached shaders for deletion. These two shaders aren't
        // deleted right now because both are already attached to a shader
        // program. When the shader program is deleted these two shaders will
        // be automatically detached and deleted.

        if (vertShader)
            glDeleteShader(vertShader);

        if (fragShader)
            glDeleteShader(fragShader);
    }

    return program;
}

GLuint LoadShaderProgram(const char *pszFilename, std::string &infoLog)
{
    infoLog.clear();

    GLuint program = 0;
    std::string buffer;

    // Read the text file containing the GLSL shader program.
    // This file contains 1 vertex shader and 1 fragment shader.
    ReadTextFile(pszFilename, buffer);

    // Compile and link the vertex and fragment shaders.
    if (buffer.length() > 0)
    {
        const GLchar *pSource = 0;
        GLint length = 0;
        GLuint vertShader = 0;
        GLuint fragShader = 0;

        std::string::size_type vertOffset = buffer.find("[vert]");
        std::string::size_type fragOffset = buffer.find("[frag]");

        try
        {
            // Get the vertex shader source and compile it.
            // The source is between the [vert] and [frag] tags.
            if (vertOffset != std::string::npos)
            {
                vertOffset += 6;        // skip over the [vert] tag
                pSource = reinterpret_cast<const GLchar *>(&buffer[vertOffset]);
                length = static_cast<GLint>(fragOffset - vertOffset);
                vertShader = CompileShader(GL_VERTEX_SHADER, pSource, length);
            }

            // Get the fragment shader source and compile it.
            // The source is between the [frag] tag and the end of the file.
            if (fragOffset != std::string::npos)
            {
                fragOffset += 6;        // skip over the [frag] tag
                pSource = reinterpret_cast<const GLchar *>(&buffer[fragOffset]);
                length = static_cast<GLint>(buffer.length() - fragOffset - 1);
                fragShader = CompileShader(GL_FRAGMENT_SHADER, pSource, length);
            }

            // Now link the vertex and fragment shaders into a shader program.
            program = LinkShaders(vertShader, fragShader);
        }
        catch (const std::string &errors)
        {
            infoLog = errors;
        }
    }

    return program;
}

GLuint LoadTexture(const char *pszFilename)
{
    return LoadTexture(pszFilename, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR,
        GL_REPEAT, GL_REPEAT);
}

GLuint LoadTexture(const char *pszFilename, GLint magFilter, GLint minFilter,
                   GLint wrapS, GLint wrapT)
{
    GLuint id = 0;
    Bitmap bitmap;

    if (bitmap.loadPicture(pszFilename))
    {
        // The Bitmap class loads images and orients them top-down.
        // OpenGL expects bitmap images to be oriented bottom-up.
        bitmap.flipVertical();

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

        if (g_maxAnisotrophy > 1)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                g_maxAnisotrophy);
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bitmap.width, bitmap.height,
            0, GL_BGRA, GL_UNSIGNED_BYTE, bitmap.getPixels());

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    return id;
}

void PerformCameraCollisionDetection()
{
    const Vector3 &pos = g_camera.getPosition();
    Vector3 newPos(pos);

    if (pos.x > g_cameraBoundsMax.x)
        newPos.x = g_cameraBoundsMax.x;

    if (pos.x < g_cameraBoundsMin.x)
        newPos.x = g_cameraBoundsMin.x;

    if (pos.z > g_cameraBoundsMax.z)
        newPos.z = g_cameraBoundsMax.z;

    if (pos.z < g_cameraBoundsMin.z)
        newPos.z = g_cameraBoundsMin.z;

    newPos.y = g_terrain.getHeightMap().heightAt(newPos.x, newPos.z) + CAMERA_Y_OFFSET;

    g_camera.setPosition(newPos);
}

void ProcessUserInput()
{
    Keyboard &keyboard = Keyboard::instance();

    if (keyboard.keyPressed(Keyboard::KEY_ESCAPE))
        PostMessage(g_hWnd, WM_CLOSE, 0, 0);

    if (keyboard.keyDown(Keyboard::KEY_LALT) || keyboard.keyDown(Keyboard::KEY_RALT))
    {
        if (keyboard.keyPressed(Keyboard::KEY_ENTER))
            ToggleFullScreen();
    }

    if (keyboard.keyPressed(Keyboard::KEY_H))
        g_displayHelp = !g_displayHelp;

    if (keyboard.keyPressed(Keyboard::KEY_ADD) || keyboard.keyPressed(Keyboard::KEY_NUMPAD_ADD))
    {
        g_camera.setRotationSpeed(g_camera.getRotationSpeed() + 0.01f);

        if (g_camera.getRotationSpeed() > 1.0f)
            g_camera.setRotationSpeed(1.0f);
    }     

    if (keyboard.keyPressed(Keyboard::KEY_SUBTRACT) || keyboard.keyPressed(Keyboard::KEY_NUMPAD_SUBTRACT))
    {
        g_camera.setRotationSpeed(g_camera.getRotationSpeed() - 0.01f);

        if (g_camera.getRotationSpeed() <= 0.0f)
            g_camera.setRotationSpeed(0.01f);
    }

    if (keyboard.keyPressed(Keyboard::KEY_V))
        EnableVerticalSync(!g_enableVerticalSync);

    if (keyboard.keyPressed(Keyboard::KEY_SPACE))
        GenerateTerrain();

    if (keyboard.keyPressed(Keyboard::KEY_M))
        Mouse::instance().smoothMouse(!Mouse::instance().mouseSmoothingIsEnabled());

    if (keyboard.keyPressed(Keyboard::KEY_T))
        g_disableColorMaps = !g_disableColorMaps;
}

void ReadTextFile(const char *pszFilename, std::string &buffer)
{
    std::ifstream file(pszFilename, std::ios::binary);

    if (file.is_open())
    {
        file.seekg(0, std::ios::end);

        std::ifstream::pos_type fileSize = file.tellg();

        buffer.resize(static_cast<unsigned int>(fileSize));
        file.seekg(0, std::ios::beg);
        file.read(&buffer[0], fileSize);
    }
}

void RenderFrame()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glViewport(0, 0, g_windowWidth, g_windowHeight);
    glClearColor(0.3f, 0.5f, 0.9f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(&g_camera.getProjectionMatrix()[0][0]);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf(&g_camera.getViewMatrix()[0][0]);

    RenderTerrain();
    RenderText();
}

void RenderTerrain()
{
    glUseProgram(g_terrainShader);
    UpdateTerrainShaderParameters();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, g_lightDir);
  
    if (g_disableColorMaps)
    {
        BindTexture(g_nullTexture, 0);
        BindTexture(g_nullTexture, 1);
        BindTexture(g_nullTexture, 2);
        BindTexture(g_nullTexture, 3);
    }
    else
    {
        BindTexture(g_regions[0].texture, 0);
        BindTexture(g_regions[1].texture, 1);
        BindTexture(g_regions[2].texture, 2);
        BindTexture(g_regions[3].texture, 3);
    }
    
    g_terrain.draw();
    
    for (int i = 3; i >= 0; --i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);

    glUseProgram(0);
}

void RenderText()
{
    std::ostringstream output;

    if (g_displayHelp)
    {
        output
            << "Press W and S to move forwards and backwards" << std::endl
            << "Press A and D to strafe left and right" << std::endl
            << "Move mouse to free look" << std::endl
            << std::endl
            << "Press M to enable/disable mouse smoothing" << std::endl
            << "Press T to enable/disable textures" << std::endl
            << "Press V to enable/disable vertical sync" << std::endl
            << "Press SPACE to generate a new random terrain" << std::endl
            << "Press +/- to change camera rotation speed" << std::endl
            << "Press ALT + ENTER to toggle full screen" << std::endl
            << "Press ESC to exit" << std::endl
            << std::endl
            << "Press H to hide help";
    }
    else
    {
        output.setf(std::ios::fixed, std::ios::floatfield);
        output << std::setprecision(2);

        output
            << "FPS: " << g_framesPerSecond << std::endl
            << "Anti-aliasing: " << GetAntiAliasingPixelFormatString() << std::endl
            << "Anisotropic filtering: " << g_maxAnisotrophy << "x" << std::endl
            << "Mouse smoothing: " << (Mouse::instance().mouseSmoothingIsEnabled() ? "on" : "off") << std::endl
            << std::endl
            << "Camera:" << std::endl
            << "  Position:"
            << " x:" << g_camera.getPosition().x
            << " y:" << g_camera.getPosition().y
            << " z:" << g_camera.getPosition().z << std::endl
            << "  Velocity:"
            << " x:" << g_camera.getCurrentVelocity().x
            << " y:" << g_camera.getCurrentVelocity().y
            << " z:" << g_camera.getCurrentVelocity().z << std::endl
            << "  Rotation speed: " << g_camera.getRotationSpeed() << std::endl
            << std::endl
            << "Press H to display help";
    }

    g_font.begin();
    g_font.setColor(1.0f, 1.0f, 0.0f);
    g_font.drawText(1, 1, output.str().c_str());
    g_font.end();
}

void SetProcessorAffinity()
{
    // Assign the current thread to one processor. This ensures that timing
    // code runs on only one processor, and will not suffer any ill effects
    // from power management.
    //
    // Based on DXUTSetProcessorAffinity() function from the DXUT framework.

    DWORD_PTR dwProcessAffinityMask = 0;
    DWORD_PTR dwSystemAffinityMask = 0;
    HANDLE hCurrentProcess = GetCurrentProcess();

    if (!GetProcessAffinityMask(hCurrentProcess, &dwProcessAffinityMask, &dwSystemAffinityMask))
        return;

    if (dwProcessAffinityMask)
    {
        // Find the lowest processor that our process is allowed to run against.

        DWORD_PTR dwAffinityMask = (dwProcessAffinityMask & ((~dwProcessAffinityMask) + 1));

        // Set this as the processor that our thread must always run against.
        // This must be a subset of the process affinity mask.

        HANDLE hCurrentThread = GetCurrentThread();

        if (hCurrentThread != INVALID_HANDLE_VALUE)
        {
            SetThreadAffinityMask(hCurrentThread, dwAffinityMask);
            CloseHandle(hCurrentThread);
        }
    }

    CloseHandle(hCurrentProcess);
}

void ToggleFullScreen()
{
    static DWORD savedExStyle;
    static DWORD savedStyle;
    static RECT rcSaved;

    g_isFullScreen = !g_isFullScreen;

    if (g_isFullScreen)
    {
        // Moving to full screen mode.

        savedExStyle = GetWindowLong(g_hWnd, GWL_EXSTYLE);
        savedStyle = GetWindowLong(g_hWnd, GWL_STYLE);
        GetWindowRect(g_hWnd, &rcSaved);

        SetWindowLong(g_hWnd, GWL_EXSTYLE, 0);
        SetWindowLong(g_hWnd, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
        SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

        g_windowWidth = GetSystemMetrics(SM_CXSCREEN);
        g_windowHeight = GetSystemMetrics(SM_CYSCREEN);

        SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0,
            g_windowWidth, g_windowHeight, SWP_SHOWWINDOW);
    }
    else
    {
        // Moving back to windowed mode.

        SetWindowLong(g_hWnd, GWL_EXSTYLE, savedExStyle);
        SetWindowLong(g_hWnd, GWL_STYLE, savedStyle);
        SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

        g_windowWidth = rcSaved.right - rcSaved.left;
        g_windowHeight = rcSaved.bottom - rcSaved.top;

        SetWindowPos(g_hWnd, HWND_NOTOPMOST, rcSaved.left, rcSaved.top,
            g_windowWidth, g_windowHeight, SWP_SHOWWINDOW);
    }

    g_camera.perspective(CAMERA_FOVX,
        static_cast<float>(g_windowWidth) / static_cast<float>(g_windowHeight),
        CAMERA_ZNEAR, CAMERA_ZFAR);
}

void UpdateCamera(float elapsedTimeSec)
{
    Mouse &mouse = Mouse::instance();
    float dx = -mouse.xPosRelative();
    float dy = -mouse.yPosRelative();

    g_camera.rotateSmoothly(dx, dy, 0.0f);
    g_camera.updatePosition(GetMovementDirection(), elapsedTimeSec);

    mouse.setPosition(g_windowWidth / 2, g_windowHeight / 2);

    PerformCameraCollisionDetection();
}

void UpdateFrame(float elapsedTimeSec)
{
    UpdateFrameRate(elapsedTimeSec);

    Mouse::instance().update();
    Keyboard::instance().update();

    ProcessUserInput();
    UpdateCamera(elapsedTimeSec);
}

void UpdateFrameRate(float elapsedTimeSec)
{
    static float accumTimeSec = 0.0f;
    static int frames = 0;

    accumTimeSec += elapsedTimeSec;

    if (accumTimeSec > 1.0f)
    {
        g_framesPerSecond = frames;

        frames = 0;
        accumTimeSec = 0.0f;
    }
    else
    {
        ++frames;
    }
}

void UpdateTerrainShaderParameters()
{
    GLint handle = -1;

    // Update the terrain tiling factor.

    handle = glGetUniformLocation(g_terrainShader, "tilingFactor");
    glUniform1f(handle, HEIGHTMAP_TILING_FACTOR);

    // Update terrain region 1.

    handle = glGetUniformLocation(g_terrainShader, "region1.max");
    glUniform1f(handle, g_regions[0].max);

    handle = glGetUniformLocation(g_terrainShader, "region1.min");
    glUniform1f(handle, g_regions[0].min);    

    // Update terrain region 2.

    handle = glGetUniformLocation(g_terrainShader, "region2.max");
    glUniform1f(handle, g_regions[1].max);

    handle = glGetUniformLocation(g_terrainShader, "region2.min");
    glUniform1f(handle, g_regions[1].min);

    // Update terrain region 3.

    handle = glGetUniformLocation(g_terrainShader, "region3.max");
    glUniform1f(handle, g_regions[2].max);

    handle = glGetUniformLocation(g_terrainShader, "region3.min");
    glUniform1f(handle, g_regions[2].min);

    // Update terrain region 4.

    handle = glGetUniformLocation(g_terrainShader, "region4.max");
    glUniform1f(handle, g_regions[3].max);

    handle = glGetUniformLocation(g_terrainShader, "region4.min");
    glUniform1f(handle, g_regions[3].min);

    // Bind textures.

    glUniform1i(glGetUniformLocation(g_terrainShader, "region1ColorMap"), 0);
    glUniform1i(glGetUniformLocation(g_terrainShader, "region2ColorMap"), 1);
    glUniform1i(glGetUniformLocation(g_terrainShader, "region3ColorMap"), 2);
    glUniform1i(glGetUniformLocation(g_terrainShader, "region4ColorMap"), 3);
}