
// Bezier Curve
// https://en.wikipedia.org/wiki/B%C3%A9zier_curve

#include "framework.h"
#include "bezier.h"
#include "matrix.h"
#include "shader.h"
#include "circle2.h"
#include "circle1.h"
#include "line.h"
#include "doublylinkedlist.h"
#include "weightdialog.h"

const int MAX_LOADSTRING = 100;
const int MAX_SPLINE_VERTEX_COUNT = 500;
const double RADIUS = 0.01;

float color[3][4] = { 0.2f, 0.2f, 0.2f, 1.0f,
                      0.5f, 0.5f, 0.5f, 1.0f,
                      1.0f, 1.0f, 1.0f, 1.0f };

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // main window class name

CMatrix matrix1;
CShader shader;
CCircle1 circle1;
CCircle2 circle2;
CLine control_line, spline;
CDoublyLinkedList control_point;
CWeightDialog dlg;
NODE* selected_node;
double uppx, uppy, offsetx, offsety, scalex, points[2];
int height;
bool dragging;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void OutputDebugControlPoint(CDoublyLinkedList* control_point);

double Factorial(int n);
double BinomialCoefficient(int n, int i);
double BernsteinBasisPolynomial(int n, int i, double t);
void Interpolate(float** vertices, int count, int* size, CDoublyLinkedList* control_point);

void CreateControlLine(float** vertices, int* count, int* size, CDoublyLinkedList* control_point);

void OnWeight(HWND hWnd, WPARAM wParam, LPARAM lParam);

void OnRButtonDown(HWND hWnd, WORD Key, int x, int y);

void OnLButtonDown(HWND hWnd, WORD Key, int x, int y);
void OnLButtonUp(HWND hWnd, WORD Key, int x, int y);
void OnMouseMove(HWND hWnd, WORD Key, int x, int y);
void OnSize(HWND hWnd, int width, int height);

void OnPaint(HDC hDC);
void OnCreate(HWND hWnd, HDC* hDC);
void OnDestroy(HWND hWnd, HDC hDC);

void OnFileOpen(HWND hWnd);
void OnFileSaveAs(HWND hWnd);
void OnFileExit(HWND hWnd);

void OnEditAdd(HWND hWnd, CDoublyLinkedList* control_point, double* points);
void OnEditEdit(HWND hWnd);
void OnEditDelete(HWND hWnd, CDoublyLinkedList* control_point);

void OnViewControlPoint(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BEZIER, szWindowClass, MAX_LOADSTRING);

    // Registers the window class.
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BEZIER));
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
    case WM_WEIGHT:        OnWeight(hWnd, wParam, lParam);    break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_OPEN:	        OnFileOpen(hWnd);		                    break;
        case IDM_SAVE_AS:	    OnFileSaveAs(hWnd);		                    break;
        case IDM_EXIT:	        OnFileExit(hWnd);		                    break;
        case IDM_ADD:	        OnEditAdd(hWnd, &control_point, points);    break;
        case IDM_EDIT:	        OnEditEdit(hWnd);		                    break;
        case IDM_DELETE:	    OnEditDelete(hWnd, &control_point);	        break;
        case IDM_CONTROL_POINT: OnViewControlPoint(hWnd);		            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;

    case WM_RBUTTONDOWN:    OnRButtonDown(hWnd, WORD(wParam), LOWORD(lParam), HIWORD(lParam));  break;
    case WM_LBUTTONDOWN:    OnLButtonDown(hWnd, WORD(wParam), LOWORD(lParam), HIWORD(lParam));  break;
    case WM_LBUTTONUP:      OnLButtonUp(hWnd, WORD(wParam), LOWORD(lParam), HIWORD(lParam));    break;
    case WM_MOUSEMOVE:      OnMouseMove(hWnd, WORD(wParam), LOWORD(lParam), HIWORD(lParam));    break;
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

void OutputDebugControlPoint(CDoublyLinkedList* control_point)
{
    NODE* Node;
    int i;
    char str[100];

    control_point->Reset();

    i = 0;

    while (control_point->Read(&Node)) {

        sprintf_s(str, 100, "%10d%20.6f%20.6f%20.6f\n", i, Node->x, Node->y, Node->w);
        OutputDebugStringA(str);

        ++i;
    }

}

// factorial
// n! = n * (n-1) * (n-2) * (n-3) * ... * 3 * 2 * 1
//
double Factorial(int n)
{
    int i, f;

    f = 1;
    for (i = 2; i < (n + 1); ++i)
        f *= i;

    return (double)f;
}

// binomial coefficient
//
//  | n |        n!
//  |   | =  ----------
//  | i |     i!(n-i)!
//
double BinomialCoefficient(int n, int i)
{
    return Factorial(n) / (Factorial(i) * Factorial(n - i));
}

// Bernstein basis polynomials
//          | n |
// b(t)   = |   | t^i (1-t)^(n-i)
//  i,n     | i |
//
double BernsteinBasisPolynomial(int n, int i, double t)
{
    return BinomialCoefficient(n, i) * pow(t, (double)i) * pow(1.0 - t, (double)(n - i));
}

// Rational Bezier Curve
//                       n
//                 +----------
//                  \
//                   \
//                    \
//                     +      b(t) Pi wi
//                    /        i,n
//                   /
//                  /
//                 +----------
//                      i=0
//     B(t) = -----------------------------
//                 +----------
//                  \
//                   \
//                    \
//                     +      b(t) wi
//                    /        i,n
//                   /
//                  /
//                 +----------
//                      i=0
//
void Interpolate(float** vertices, int count, int* size, CDoublyLinkedList* control_point)
{
    const int vertex_size = 3;

    NODE* Node;
    double b, t, x, y, w, dt;
    int i, j, k, m, n;

    if (*vertices != NULL) delete[] *vertices;

    m = vertex_size * count;
    *vertices = new float[m];
    *size = m * sizeof(float);

    n = control_point->GetCount();
    dt = 1.0 / (double)(count - 1);

    t = 0.0;
    k = 0;

    for (j = 0; j < count; ++j) {

        i = 0;
        x = 0.0;
        y = 0.0;
        w = 0.0;

        control_point->Reset();

        while (control_point->Read(&Node)) {

            b = BernsteinBasisPolynomial(n, i, t);

            x += (b * Node->x * Node->w);
            y += (b * Node->y * Node->w);
            w += (b * Node->w);

            ++i;
        }

        (*vertices)[k++] = (float)(x / w);
        (*vertices)[k++] = (float)(y / w);
        (*vertices)[k++] = 0.0f;

        t += dt;
    }
}

void CreateControlLine(float** vertices, int* count, int* size, CDoublyLinkedList* control_point)
{
    NODE* Node;
    int k, n;

    if (*vertices != NULL) delete[] *vertices;

    *count = control_point->GetCount();
    n = 3 * (*count);
    *vertices = new float[n];
    *size = n * sizeof(float);

    control_point->Reset();

    k = 0;

    while (control_point->Read(&Node)) {

        (*vertices)[k++] = (float)Node->x;
        (*vertices)[k++] = (float)Node->y;
        (*vertices)[k++] = 0.0f;
    }
}

void OnWeight(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    double* p = (double*)lParam;
    double w = *p;
    float* vertices = NULL;
    int size;

    //wchar_t str[100];
    //swprintf_s(str, 100, L"%20.10f\n", w);
    //OutputDebugString(str);

    selected_node->w = w;

    Interpolate(&vertices, MAX_SPLINE_VERTEX_COUNT, &size, &control_point);
    spline.Update(vertices, MAX_SPLINE_VERTEX_COUNT, size);
    delete[] vertices;
}

// show menu
void OnRButtonDown(HWND hWnd, WORD Key, int x, int y)
{
    points[0] = (double)x * uppx + offsetx;
    points[1] = (double)(height - y) * uppy + offsety;

    NODE* Node;
    HMENU hMenu;
    HMENU hSubMenu;
    POINT point;
    UINT enable1, enable2, enable3;
    bool already_exist;
    double x1, x2, y1, y2;

    point.x = x;
    point.y = y;

    ClientToScreen(hWnd, &point);

    hMenu = LoadMenu(hInst, MAKEINTRESOURCEW(IDC_BEZIER));

    control_point.Reset();

    already_exist = false;

    while (control_point.Read(&Node)) {

        x1 = Node->x - RADIUS;
        x2 = Node->x + RADIUS;

        y1 = Node->y - RADIUS;
        y2 = Node->y + RADIUS;

        if (x1 < points[0] && points[0] < x2 && y1 < points[1] && points[1] < y2) {
            already_exist = true;
            break;
        }
    }

    enable1 = (selected_node != NULL && !already_exist) ? MF_ENABLED : MF_DISABLED;
    enable2 = selected_node == NULL ? MF_DISABLED : MF_ENABLED;
    enable3 = (control_point.GetCount() > 3 && selected_node != NULL) ? MF_ENABLED : MF_DISABLED;

    EnableMenuItem(hMenu, IDM_ADD, MF_BYCOMMAND | enable1);
    EnableMenuItem(hMenu, IDM_EDIT, MF_BYCOMMAND | enable2);
    EnableMenuItem(hMenu, IDM_DELETE, MF_BYCOMMAND | enable3);

    hSubMenu = GetSubMenu(hMenu, 0);

    TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, 0, hWnd, NULL);
}

// iselect ang control point (x, y)
void OnLButtonDown(HWND hWnd, WORD Key, int x, int y)
{
    NODE* Node;
    double ux, uy, x1, x2, y1, y2;

    ux = (double)x * uppx + offsetx;
    uy = (double)(height - y) * uppy + offsety;

    control_point.Reset();

    while (control_point.Read(&Node)) {

        x1 = Node->x - RADIUS;
        x2 = Node->x + RADIUS;

        y1 = Node->y - RADIUS;
        y2 = Node->y + RADIUS;

        if (x1 < ux && ux < x2 && y1 < uy && uy < y2) {
            selected_node = Node;
            dragging = true;
            break;
        }
    }

    // palitan ang value na nasa weight dialog
    if (selected_node != NULL)
        dlg.Set(selected_node->w);
}

void OnLButtonUp(HWND hWnd, WORD Key, int x, int y)
{
    dragging = false;
}

// imove ang sinelect na control point
void OnMouseMove(HWND hWnd, WORD Key, int x, int y)
{
    double ux, uy;
    float* vertices = NULL;
    int count, size;

    if (dragging) {

        ux = (double)x * uppx + offsetx;
        uy = (double)(height - y) * uppy + offsety;

        selected_node->x = ux;
        selected_node->y = uy;

        // iupdate ang control lines
        CreateControlLine(&vertices, &count, &size, &control_point);
        control_line.Update(vertices, count, size);
        delete[] vertices;
        vertices = NULL;

        // iupdate ang bezier curve
        Interpolate(&vertices, MAX_SPLINE_VERTEX_COUNT, &size, &control_point);
        spline.Update(vertices, MAX_SPLINE_VERTEX_COUNT, size);
        delete[] vertices;
    }
}

void OnSize(HWND hWnd, int width, int height)
{
    double left, right, bottom, top, znear, zfar, w, h;

    ::height = height;

    w = 2.0f;
    h = w * ((float)height / (float)width);

    left = -w / 2.0f;
    right = left + w;
    bottom = -h / 2.0f;
    top = bottom + h;
    znear = 0.0f;
    zfar = 1.0f;

    uppx = w / (double)width;
    uppy = h / (double)height;
    offsetx = left;
    offsety = bottom;

    matrix1.Orthographic((float)left, (float)right, (float)bottom, (float)top, (float)znear, (float)zfar);

    glViewport(0, 0, width, height);
}

void OnPaint(HDC hDC)
{
    NODE* Node;
    CMatrix matrix, matrix2;

    glClear(GL_COLOR_BUFFER_BIT);

    // idraw ang mga control point
    control_point.Reset();

    while (control_point.Read(&Node)) {

        matrix2.Translate((float)Node->x, (float)Node->y, 0.0f);
        matrix = matrix1 * matrix2;

        if (selected_node == Node)
            circle2.Draw(matrix);
        else
            circle1.Draw(matrix);
    }

    // idraw ang control line at bezier curve
    control_line.Draw(matrix1);
    spline.Draw(matrix1);

    SwapBuffers(hDC);
}

void OnCreate(HWND hWnd, HDC* hDC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int format;
    HGLRC hglrc;

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);  // size of structured data
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
    //   |                 SHADER                  |
    //   +-----------------------------------------+

    char source1[] = "#version 330\n"
        "precision mediump float;\n"
        "in vec3 v_vertex;\n"
        "uniform mat4 m_matrix;\n"
        "void main()\n"
        "{\n"
        "gl_Position =  m_matrix * vec4(v_vertex, 1.0);\n"
        "}\n";

    char source2[] = "#version 330\n"
        "precision mediump float;\n"
        "out vec4 FragColor;\n"
        "uniform vec4 v_color;\n"
        "void main()\n"
        "{\n"
        "FragColor = v_color;\n"
        "}\n";

    shader.Create(source1, source2);
    shader.Use();

    //   +-----------------------------------------+
    //   |                  MODEL                  |
    //   +-----------------------------------------+

    // circles
    circle2.Create(shader.Get(), RADIUS, color[1]);
    circle1.Create(shader.Get(), RADIUS, color[1]);

    double x, y, w;
    float* vertices = NULL;
    int count, size;

    // initalize ang control point sa tatlong point
    // control point 1
    x = -0.75;
    y = 0.0;
    w = 1.0;

    control_point.Add(x, y, w);

    // control point 2
    x = 0.0;
    y = 0.4;
    w = 1.0;

    control_point.Add(x, y, w);

    // control point 3
    x = 0.75;
    y = 0.0;
    w = 1.0;

    control_point.Add(x, y, w);

    // control line
    CreateControlLine(&vertices, &count, &size, &control_point);
    control_line.Create(shader.Get(), color[0]);
    control_line.Update(vertices, count, size);
    delete[] vertices;
    vertices = NULL;

    // bezier curve
    Interpolate(&vertices, MAX_SPLINE_VERTEX_COUNT, &size, &control_point);
    spline.Create(shader.Get(), color[2]);
    spline.Update(vertices, MAX_SPLINE_VERTEX_COUNT, size);
    delete[] vertices;

    //   +-----------------------------------------+
    //   |      PARAMETER INITIALIZATION           |
    //   +-----------------------------------------+

    selected_node = NULL;

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    char str[100];
    OutputDebugStringA("-----------------------------------------------------------------------------\n");
    sprintf_s(str, 100, "OpenGL Version :%s\n", glGetString(GL_VERSION));   OutputDebugStringA(str);
    sprintf_s(str, 100, "GLES Version   :%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));  OutputDebugStringA(str);
    sprintf_s(str, 100, "GLU Version    :%s\n", glGetString(GLU_VERSION));  OutputDebugStringA(str);
    sprintf_s(str, 100, "Vendor         :%s\n", glGetString(GL_VENDOR));    OutputDebugStringA(str);
    sprintf_s(str, 100, "Renderer       :%s\n", glGetString(GL_RENDERER));  OutputDebugStringA(str);
    OutputDebugStringA("-----------------------------------------------------------------------------\n");
}

void OnDestroy(HWND hWnd, HDC hDC)
{
    // release all objects
    circle2.Destroy();
    circle1.Destroy();
    control_line.Destroy();
    spline.Destroy();

    control_point.Clear();

    shader.Destroy();

    HGLRC hglrc;

    wglMakeCurrent(hDC, NULL);      // get current OpenGL rendering context
    hglrc = wglGetCurrentContext(); // make the rendering context not current
    wglDeleteContext(hglrc);        // delete the rendering context
    ReleaseDC(hWnd, hDC);           // releases a device context

    // close the program.
    PostQuitMessage(0);
}

void OnFileOpen(HWND hWnd)
{

}

void OnFileSaveAs(HWND hWnd)
{

}

void OnFileExit(HWND hWnd)
{
    DestroyWindow(hWnd);
}

// iadd ang control point
void OnEditAdd(HWND hWnd, CDoublyLinkedList* control_point, double* points)
{
    double x, y;
    float* vertices = NULL;
    int count, size;

    // iselect ang point na inadd
    x = points[0];
    y = points[1];

    selected_node = control_point->Add(selected_node, x, y, 1.0);

    // iupdate ang control line
    CreateControlLine(&vertices, &count, &size, control_point);
    control_line.Update(vertices, count, size);
    delete[] vertices;
    vertices = NULL;

    // iupdate ang bezier curve
    Interpolate(&vertices, MAX_SPLINE_VERTEX_COUNT, &size, control_point);
    spline.Update(vertices, MAX_SPLINE_VERTEX_COUNT, size);
    delete[] vertices;

    dlg.Set(selected_node->w);
}

// baguhin ang parameter weight ng control point
void OnEditEdit(HWND hWnd)
{
    dlg.Show(hWnd, hInst, DlgProc, selected_node->w);
}

// idelete ang selected control point
void OnEditDelete(HWND hWnd, CDoublyLinkedList* control_point)
{ 
    float* vertices = NULL;
    int count, size;

    // walang iselect
    control_point->Remove(selected_node);
    selected_node = NULL;

    // iupdate ang control line
    CreateControlLine(&vertices, &count, &size, control_point);
    control_line.Update(vertices, count, size);
    delete[] vertices;
    vertices = NULL;

    // iupdate ang bezier curve
    Interpolate(&vertices, MAX_SPLINE_VERTEX_COUNT, &size, control_point);
    spline.Update(vertices, MAX_SPLINE_VERTEX_COUNT, size);
    delete[] vertices;

    dlg.CloseWindow();

}

// ioutput ang mga control point
void OnViewControlPoint(HWND hWnd)
{
    OutputDebugControlPoint(&control_point);
}
