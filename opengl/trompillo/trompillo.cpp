
#include "framework.h"
#include "trompillo.h"
#include "matrix.h"
#include "shader.h"
#include "sprite.h"
#include "pngfile.h"
#include "anime.h"
#include "spinratedialog.h"

const int MAX_LOADSTRING = 100;

const float RADIUS = 15.0f;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // main window class name

CMatrix matrix1;
CShader shader;
CSprite sprite;
CPngFile file;
CAnime anime;
CSpinRateDialog dlg;
GLuint textures;
LARGE_INTEGER freq;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void OnSpinRate(HWND hWnd, WPARAM wParam, LPARAM lParam);

void OnRButtonDown(HWND hWnd, WORD Key, int x, int y);

void OnSize(HWND hWnd, int width, int height);

void OnPaint(HDC hDC);
void OnCreate(HWND hWnd, HDC* hDC);
void OnDestroy(HWND hWnd, HDC hDC);

void OnToolsStart(HWND hWnd);
void OnToolsStop(HWND hWnd);
void OnToolsSpinRate(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TROMPILLO, szWindowClass, MAX_LOADSTRING);

    // Registers the window class.
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TROMPILLO));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    // Store instance handle in our global variable.
    hInst = hInstance;

    // Creates main window.
    int X, Y, nWidth, nHeight, Cx, Cy;

    // 1080p: 1920 x 1080
    //  720p: 1280 x  720
    //  480p:  854 x  480

    Cx = 1280;
    Cy = 720;

    nWidth = Cx + 16;
    nHeight = Cy + 58;

    X = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2;
    Y = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 4;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        X, Y, nWidth, nHeight, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;

    // main message loop
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HDC hDC;

    switch (message)
    {
    case WM_SPIN_RATE:			OnSpinRate(hWnd, wParam, lParam);   break;
    case WM_GENERATE_SPRITE:	anime.GenerateSprite();             break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_START:		OnToolsStart(hWnd);		break;
        case IDM_STOP:		OnToolsStop(hWnd);		break;
        case IDM_SPIN_RATE:	OnToolsSpinRate(hWnd);	break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_RBUTTONDOWN:    OnRButtonDown(hWnd, WORD(wParam), LOWORD(lParam), HIWORD(lParam));  break;
    case WM_SIZE:           OnSize(hWnd, LOWORD(lParam), HIWORD(lParam));                       break;
    case WM_PAINT:          OnPaint(hDC);                                                       break;
    case WM_CREATE:         OnCreate(hWnd, &hDC);                                               break;
    case WM_DESTROY:        OnDestroy(hWnd, hDC);                                               break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return dlg.Proc(hWnd, message, wParam, lParam);
}

void OnSpinRate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    double spin_rate = *((double*)lParam);
    anime.SetSpinRate(spin_rate);

    //wchar_t str[100];
    //swprintf_s(str, 100, L"%5.2f\n", spin_rate);
    //OutputDebugString(str);
}

void OnRButtonDown(HWND hWnd, WORD Key, int x, int y)
{
    HMENU hMenu;
    HMENU hSubMenu;
    POINT point;
    UINT enable1, enable2;

    point.x = x;
    point.y = y;

    ClientToScreen(hWnd, &point);

    hMenu = LoadMenu(hInst, MAKEINTRESOURCEW(IDC_TROMPILLO));

    enable1 = anime.IsRunning() ? MF_ENABLED : MF_DISABLED;
    enable2 = anime.IsRunning() ? MF_DISABLED : MF_ENABLED;

    EnableMenuItem(hMenu, IDM_START, MF_BYCOMMAND | enable2);
    EnableMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND | enable1);

    hSubMenu = GetSubMenu(hMenu, 0);
    TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, 0, hWnd, NULL);
}

void OnSize(HWND hWnd, int width, int height)
{
    if (width == 0) width = 1;
    if (height == 0) height = 1;

    float left, right, bottom, top, znear, zfar;
    float w, h;

    w = 250.0f;
    h = w * ((float)height / (float)width);

    left = -w / 2.0f;
    right = left + w;
    bottom = -h / 2.0f;
    top = bottom + h;
    znear = 0.0f;
    zfar = 1.0f;

    anime.SetBoundary(left, right, bottom, top);

    matrix1.Orthographic(left, right, bottom, top, znear, zfar);

    glViewport(0, 0, width, height);
}

void OnPaint(HDC hDC)
{
    CMatrix matrix, matrix2;
    static LARGE_INTEGER t1 = { 0 };
    LARGE_INTEGER t2;
    float t, x, y;

    QueryPerformanceCounter(&t2);
    t = (float)(t2.QuadPart - t1.QuadPart) / (float)freq.QuadPart;
    t1.QuadPart = t2.QuadPart;

    anime.DoAnimation(t);

    glClear(GL_COLOR_BUFFER_BIT);

    anime.list.Reset();

    while (anime.list.Read(&x, &y)) {

        matrix2.Translate(x, y, 0.0f);
        matrix = matrix1 * matrix2;

        sprite.Draw(matrix);
    }

    SwapBuffers(hDC);
}

void OnCreate(HWND hWnd, HDC* hDC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int format;
    HGLRC hglrc;

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);  // size ng data structure
    pfd.nVersion = 1;                           // version number
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |          // support window
        PFD_SUPPORT_OPENGL |                    // support OpenGL
        PFD_DOUBLEBUFFER |                      // double buffered
        PFD_GENERIC_ACCELERATED;                // support device driver that accelerates the generic implementation
    pfd.iPixelType = PFD_TYPE_RGBA;             // RGBA pixels
    pfd.cColorBits = 24;                        //  24-bit color
    pfd.cRedBits = 0;                           // color bits ignored
    pfd.cGreenBits = 0;
    pfd.cBlueBits = 0;
    pfd.cAlphaBits = 0;
    pfd.cRedShift = 0;                          // shift bit ignored
    pfd.cGreenShift = 0;
    pfd.cBlueShift = 0;
    pfd.cAlphaShift = 0;
    pfd.cAccumBits = 0;                         // no accumulation buffer
    pfd.cAccumRedBits = 0;
    pfd.cAccumGreenBits = 0;
    pfd.cAccumBlueBits = 0;
    pfd.cAccumAlphaBits = 0;
    pfd.cDepthBits = 32;                        // 32-bit z-buffer
    pfd.cStencilBits = 0;                       // no stencil buffer
    pfd.cAuxBuffers = 0;                        // no auxiliary buffer
    pfd.iLayerType = PFD_MAIN_PLANE;            // main layer
    pfd.bReserved = 0;
    pfd.dwLayerMask = 0;
    pfd.dwVisibleMask = 0;
    pfd.dwDamageMask = 0;

    *hDC = GetDC(hWnd);                         // get the device context for our window
    format = ChoosePixelFormat(*hDC, &pfd); // get the best available match of pixel format for the device context
    SetPixelFormat(*hDC, format, &pfd);     // make that the pixel format of the device context
    hglrc = wglCreateContext(*hDC);             // create an OpenGL rendering context
    wglMakeCurrent(*hDC, hglrc);                // make it the current rendering context

    // Load OpenGL functions.
    LoadOpenGLFunctions();

    //   +-----------------------------------------+
    //   |                TEXTURE                  |
    //   +-----------------------------------------+
    wchar_t pathname[MAX_PATH], filename[MAX_PATH];
    unsigned int width = 1, height = 1;
    unsigned char* buffer = NULL;

    GetCurrentDirectory(MAX_PATH, pathname);

    wcscpy_s(filename, MAX_PATH, pathname);
    wcscat_s(filename, MAX_PATH, L"\\sprite.png");

    glGenTextures(1, &textures);

    if (file.Open(filename)) {

        file.Read(&width, &height, &buffer);

        glBindTexture(GL_TEXTURE_2D, textures);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
        glGenerateMipmap(GL_TEXTURE_2D);

        file.Close();
    }

    //   +-----------------------------------------+
    //   |                  MODEL                  |
    //   +-----------------------------------------+

    char source1[] = "#version 330\n"
        "precision mediump float;\n"
        "in vec3 v_vertex;\n"
        "uniform mat4 m_matrix;\n"
        "void main()\n"
        "{\n"
        "gl_Position =  m_matrix * vec4(v_vertex, 1.0);\n"
        "gl_PointSize = 20.0;\n"
        "}\n";

    char source2[] = "#version 330\n"
        "precision mediump float;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D sampler;\n"
        "void main()\n"
        "{\n"
        "FragColor = vec4(1.0, 1.0, 0.0, 1.0) * texture(sampler, gl_PointCoord);\n"
        "}\n";

    shader.Create(source1, source2);
    shader.Use();
    sprite.Create(shader.Get());

    //   +-----------------------------------------+
    //   |      PARAMETER INITIALIZATION           |
    //   +-----------------------------------------+

    anime.Init(RADIUS);

    QueryPerformanceFrequency(&freq);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    char str[100];
    OutputDebugStringA("-----------------------------------------------------------------------------\n");
    sprintf_s(str, 100, "OpenGL Version :%s\n", glGetString(GL_VERSION));   OutputDebugStringA(str);
    sprintf_s(str, 100, "GLES Version   :%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));  OutputDebugStringA(str);
    sprintf_s(str, 100, "GLU Version    :%s\n", glGetString(GLU_VERSION));  OutputDebugStringA(str);
    sprintf_s(str, 100, "Vendor         :%s\n", glGetString(GL_VENDOR));    OutputDebugStringA(str);
    sprintf_s(str, 100, "Renderer       :%s\n", glGetString(GL_RENDERER));  OutputDebugStringA(str);
    OutputDebugStringA("-----------------------------------------------------------------------------\n");

    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_START, MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND | MF_DISABLED);
}

void OnDestroy(HWND hWnd, HDC hDC)
{
    // Release ojects.
    shader.Destroy();
    sprite.Destroy();

    glDeleteTextures(1, &textures);

    HGLRC hglrc;

    wglMakeCurrent(hDC, NULL);      // get current OpenGL rendering context
    hglrc = wglGetCurrentContext(); // make the rendering context not current
    wglDeleteContext(hglrc);        // delete the rendering context
    ReleaseDC(hWnd, hDC);           // releases a device context

    // close the program.
    PostQuitMessage(0);
}

void OnToolsStart(HWND hWnd)
{
    anime.Play(hWnd);
}

void OnToolsStop(HWND hWnd)
{
    anime.Stop();
}

void OnToolsSpinRate(HWND hWnd)
{
    dlg.Show(hWnd, hInst, DlgProc, anime.GetSpinRate());
}
