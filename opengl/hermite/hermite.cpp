
// Cubic Hermite spline
// https://en.wikipedia.org/wiki/Cubic_Hermite_spline#Representations

#include "framework.h"
#include "hermite.h"
#include "matrix.h"
#include "shader.h"
#include "circle2.h"
#include "circle1.h"
#include "line1.h"
#include "doublylinkedlist.h"
#include "tensiondialog.h"

const int MAX_LOADSTRING = 100;
const int MAX_SPLINE_VERTEX_COUNT = 100;
const double RADIUS = 0.01;

float color[2][4] = { 0.5f, 0.5f, 0.5f, 1.0f,
                      1.0f, 1.0f, 1.0f, 1.0f };

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // main window class name

CMatrix matrix1;
CShader shader;
CCircle2 circle2;
CCircle1 circle1;
CLine1 tangent;
CDoublyLinkedList control_point;
CTensionDialog dlg;
NODE* selected_node;
double uppx, uppy, offsetx, offsety, scalex, points[2];
int height, index;
bool dragging;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void OutputDebugControlPoint(CDoublyLinkedList* control_point);

double Factorial(int n);
double BinomialCoefficient(int n, int i);
double BernsteinPolynomial(int k, double t);
double H00(double t);
double H10(double t);
double H01(double t);
double H11(double t);
void Interpolate(float** vertices, int count, int* size, double x1, double y1, double m1, double x2, double y2, double m2);

void CreateTangentLines(float** vertices, int* count, int* size, CDoublyLinkedList* control_point);
double CalculateSlope(double x1, double y1, double x2, double y2);
void Reinterpolate(NODE* Node);

void OnTension(HWND hWnd, WPARAM wParam, LPARAM lParam);

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
void OnFileRed(HWND hWnd);
void OnFileGreen(HWND hWnd);
void OnFileBlue(HWND hWnd);
void OnFileExit(HWND hWnd);

void OnEditAdd(HWND hWnd, CDoublyLinkedList* control_point, double* points);
void OnEditEdit(HWND hWnd);
void OnEditDelete(HWND hWnd, CDoublyLinkedList* control_point);

void OnViewControlPoint(HWND hWnd);
void OnViewVerify(HWND hWnd);

void OnToolsFiniteDifference(HWND hWnd);
void OnToolsCardinalSPLine(HWND hWnd, double c);
void OnToolsCatmullRomSPLine(HWND hWnd);
void OnToolsZero(HWND hWnd);
void OnToolsResultant(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HERMITE, szWindowClass, MAX_LOADSTRING);

    // Registers the window class.
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HERMITE));
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
    case WM_TENSION:        OnTension(hWnd, wParam, lParam);    break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_OPEN:	            OnFileOpen(hWnd);		                    break;
        case IDM_SAVE_AS:	        OnFileSaveAs(hWnd);		                    break;
        case IDM_RED:	            OnFileRed(hWnd);		                    break;
        case IDM_GREEN:	            OnFileGreen(hWnd);		                    break;
        case IDM_BLUE:	            OnFileBlue(hWnd);		                    break;
        case IDM_EXIT:	            OnFileExit(hWnd);		                    break;
        case IDM_ADD:	            OnEditAdd(hWnd, &control_point, points);    break;
        case IDM_EDIT:	            OnEditEdit(hWnd);		                    break;
        case IDM_DELETE:	        OnEditDelete(hWnd, &control_point);	        break;
        case IDM_CONTROL_POINT:     OnViewControlPoint(hWnd);		            break;
        case IDM_VERIFY:            OnViewVerify(hWnd);		                    break;
        case IDM_FINITE_DIFFERENCE: OnToolsFiniteDifference(hWnd);		        break;
        case IDM_CARDINAL_SPLINE:   OnToolsCardinalSPLine(hWnd, 0.5);		    break;
        case IDM_CATMULL_ROM_SPLINE:OnToolsCatmullRomSPLine(hWnd);		        break;
        case IDM_ZERO:              OnToolsZero(hWnd);		                    break;
        case IDM_RESULTANT:         OnToolsResultant(hWnd);		                break;
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

    sprintf_s(str, 100, "%10s%20s%20s%20s\n", "i", "x", "y", "m");
    OutputDebugStringA(str);
    i = 0;

    while (control_point->Read(&Node)) {

        sprintf_s(str, 100, "%10d%20.6f%20.6f%20.6f\n", i, Node->x[0], Node->y[0], Node->m);
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

// Bernstein polynomials
//         | 3 |
// b (t) = |   | t^k * (1-t)^(3-k)
//  k      | k |
//
double BernsteinPolynomial(int k, double t)
{
    return BinomialCoefficient(3, k) * pow(t, (double)k) * pow(1.0 - t, (double)(3 - k));
}

// Hermite basis functions
//
// h   = B  (t) + B  (t)
//  00     0       1
double H00(double t)
{
    return BernsteinPolynomial(0, t) + BernsteinPolynomial(1, t);
}

// h      1
//  10 = --- * B (t)
//        3     1
double H10(double t)
{
    return (1.0 / 3.0) * BernsteinPolynomial(1, t);
}

// h   = B  (t) + B  (t)
//  01     3       2
double H01(double t)
{
    return BernsteinPolynomial(3, t) + BernsteinPolynomial(2, t);
}

// h        1
//  11 = - --- * B (t)
//          3     2
double H11(double t)
{
    return -(1.0 / 3.0) * BernsteinPolynomial(2, t);
}


// interpolation polynomial
// p(t) = h  (t)y   + h  (t)m  + h  (t)y  + h  (t)m
//         00    0     10    0    01    1    11    1
void Interpolate(float** vertices, int count, int* size, double x1, double y1, double m1, double x2, double y2, double m2)
{
    const int vertex_size = 3;

    double t, x, y, dt, dx;
    int i, k, n;

    if (*vertices != NULL) delete[] * vertices;

    n = vertex_size * count;
    *vertices = new float[n];
    *size = n * sizeof(float);

    dt = 1.0 / (double)count;
    t = 0.0;

    dx = (x2 - x1) / (double)count;
    x = x1;

    k = 0;

    for (i = 0; i < count; i++) {

        y = H00(t) * y1 + H10(t) * m1 + H01(t) * y2 + H11(t) * m2;

        (*vertices)[k++] = (float)x;
        (*vertices)[k++] = (float)y;
        (*vertices)[k++] = 0.0f;

        t += dt;
        x += dx;
    }
}

void CreateTangentLines(float** vertices, int* count, int* size, CDoublyLinkedList* control_point)
{
    NODE* Node;
    int k, n;

    *count = 2 * control_point->GetCount();
    n = 3 * (*count);
    *vertices = new float[n];
    *size = n * sizeof(float);

    control_point->Reset();

    k = 0;

    while (control_point->Read(&Node)) {

        (*vertices)[k++] = (float)Node->x[0];
        (*vertices)[k++] = (float)Node->y[0];
        (*vertices)[k++] = 0.0f;

        (*vertices)[k++] = (float)Node->x[1];
        (*vertices)[k++] = (float)Node->y[1];
        (*vertices)[k++] = 0.0f;
    }
}

double CalculateSlope(double x1, double y1, double x2, double y2)
{
    double dx, dy;

    dx = x2 - x1;
    dy = y2 - y1;

    return dy / dx;
}

void Reinterpolate(NODE* Node)
{
    NODE* Node1;
    NODE* Node2;
    float* vertices = NULL;
    int size;

    if (Node->Prev != NULL) {

        Node1 = Node->Prev;
        Node2 = Node;

        Interpolate(&vertices, Node1->count, &size, Node1->x[0], Node1->y[0], Node1->m, Node2->x[0], Node2->y[0], Node2->m);
        Node1->spline.Update(vertices, Node1->count, size);
        delete[] vertices;
        vertices = NULL;
    }

    if (Node->Next != NULL) {

        Node1 = Node;
        Node2 = Node->Next;

        Interpolate(&vertices, Node1->count, &size, Node1->x[0], Node1->y[0], Node1->m, Node2->x[0], Node2->y[0], Node2->m);
        Node1->spline.Update(vertices, Node1->count, size);
        delete[] vertices;
        vertices = NULL;
    }
}

void OnTension(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    double* p = (double*)lParam;
    double t = *p;

    OnToolsCardinalSPLine(hWnd, t);
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
    UINT enable1, enable2, enable3, enable4;
    double x1, x2, y1, y2;
    bool found;
    int j;

    found = false;

    while (control_point.Read(&Node)) {

        for (j = 0; j < 2; j++) {

            x1 = Node->x[j] - RADIUS;
            x2 = Node->x[j] + RADIUS;

            y1 = Node->y[j] - RADIUS;
            y2 = Node->y[j] + RADIUS;

            if (x1 < points[0] && points[0] < x2 && y1 < points[1] && points[1] < y2) {
                OutputDebugString(L"found\n");
                found = true;
                break;
            }
        }
    }

    point.x = x;
    point.y = y;

    ClientToScreen(hWnd, &point);

    hMenu = LoadMenu(hInst, MAKEINTRESOURCEW(IDC_HERMITE));

    enable1 = (selected_node != NULL && !found) ? MF_ENABLED : MF_DISABLED;
    enable2 = selected_node == NULL ? MF_DISABLED : MF_ENABLED;
    enable3 = (control_point.GetCount() > 2 && selected_node != NULL) ? MF_ENABLED : MF_DISABLED;
    enable4 = control_point.GetCount() > 3 ? MF_ENABLED : MF_DISABLED;

    EnableMenuItem(hMenu, IDM_ADD, MF_BYCOMMAND | enable1);
    EnableMenuItem(hMenu, IDM_EDIT, MF_BYCOMMAND | enable2);
    EnableMenuItem(hMenu, IDM_DELETE, MF_BYCOMMAND | enable3);

    EnableMenuItem(hMenu, IDM_FINITE_DIFFERENCE, MF_BYCOMMAND | enable4);
    EnableMenuItem(hMenu, IDM_CARDINAL_SPLINE, MF_BYCOMMAND | enable4);
    EnableMenuItem(hMenu, IDM_CATMULL_ROM_SPLINE, MF_BYCOMMAND | enable4);

    EnableMenuItem(hMenu, IDM_ZERO, MF_BYCOMMAND | enable2);
    EnableMenuItem(hMenu, IDM_RESULTANT, MF_BYCOMMAND | enable2);

    hSubMenu = GetSubMenu(hMenu, 0);

    TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, 0, hWnd, NULL);
}

// iselect ang control point (x, y)
void OnLButtonDown(HWND hWnd, WORD Key, int x, int y)
{
    NODE* Node;
    double ux, uy, x1, x2, y1, y2;
    int j, k;

    ux = (double)x * uppx + offsetx;
    uy = (double)(height - y) * uppy + offsety;

    control_point.Reset();

    k = 0;

    while (control_point.Read(&Node)) {

        for (j = 0; j < 2; j++) {

            x1 = Node->x[j] - RADIUS;
            x2 = Node->x[j] + RADIUS;

            y1 = Node->y[j] - RADIUS;
            y2 = Node->y[j] + RADIUS;

            if (x1 < ux && ux < x2 && y1 < uy && uy < y2) {
                selected_node = Node;
                index = j;
                dragging = true;
                break;
            }
        }
    }
}

void OnLButtonUp(HWND hWnd, WORD Key, int x, int y)
{
    dragging = false;
}

// imove ang sinelect na control point
void OnMouseMove(HWND hWnd, WORD Key, int x, int y)
{
    double ux, uy, dx, dy;
    float* vertices;
    int count, size;

    if (dragging) {

        ux = (double)x * uppx + offsetx;
        uy = (double)(height - y) * uppy + offsety;

        if (index == 0) {

            dx = selected_node->x[1] - selected_node->x[0];
            dy = selected_node->y[1] - selected_node->y[0];

            selected_node->x[0] = ux;
            selected_node->y[0] = uy;

            selected_node->x[1] = ux + dx;
            selected_node->y[1] = uy + dy;

        }
        else {

            selected_node->x[1] = ux;
            selected_node->y[1] = uy;

            selected_node->m = CalculateSlope(selected_node->x[0], selected_node->y[0], selected_node->x[1], selected_node->y[1]);
        }

        // iupdate ang tangent line
        CreateTangentLines(&vertices, &count, &size, &control_point);
        tangent.Update(vertices, count, size);
        delete[] vertices;

        // iupdate ang hermite curve
        Reinterpolate(selected_node);
    }
}

void OnSize(HWND hWnd, int width, int height)
{
    double left, right, bottom, top, znear, zfar, w, h;

    ::height = height;

    h = 1.1f;
    w = h * ((float)width / (float)height);

    left = -0.9f;
    right = left + w;
    bottom = -h / 2.0;
    top = bottom + h;
    znear = 0.0f;
    zfar = 1.0f;

    uppx = w / (double)width;
    uppy = h / (double)height;
    offsetx = left;
    offsety = bottom;

    uppx = w / (double)width;
    uppy = h / (double)height;
    offsetx = left;
    offsety = bottom;

    matrix1.Orthographic((float)left, (float)right, (float)bottom, (float)top, (float)znear, (float)zfar);

    char str[100];
    sprintf_s(str, 100, "%20.10f%20.10f%20.10f%20.10f\n", left, right, bottom, top);
    OutputDebugStringA(str);

    glViewport(0, 0, width, height);
}

void OnPaint(HDC hDC)
{
    NODE* Node;
    CMatrix matrix, matrix2;
    int i, j;

    glClear(GL_COLOR_BUFFER_BIT);

    control_point.Reset();

    i = 0;

    while (control_point.Read(&Node)) {

        // idraw ang mga control point
        for (j = 0; j < 2; j++) {

            matrix2.Translate((float)Node->x[j], (float)Node->y[j], 0.0f);
            matrix = matrix1 * matrix2;

            if (selected_node == Node && index == j)
                circle2.Draw(matrix);
            else
                circle1.Draw(matrix);
        }

        // idraw ang hermite curve
        if (i < (control_point.GetCount() - 1))
            Node->spline.Draw(matrix1);

        ++i;
    }

    // idraw ang tangent lines
    tangent.Draw(matrix1);

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
    circle2.Create(shader.Get(), RADIUS, color[0]);
    circle1.Create(shader.Get(), RADIUS, color[0]);

    NODE* Node;
    double x, y, m;
    float* vertices = NULL;
    int count, size;

    // initalize ang control point sa dalawang point
    // control point 1
    m = 0.25;

    x = -0.75;
    y = 0.0;

    count = MAX_SPLINE_VERTEX_COUNT;

    Node = control_point.Add(x, y, m, count);
    Node->spline.Create(shader.Get(), color[1]);

    // control point 2
    m = 0.51;

    x = 0.75;
    y = 0.0;

    count = MAX_SPLINE_VERTEX_COUNT;

    Node = control_point.Add(x, y, m, count);
    Node->spline.Create(shader.Get(), color[1]);

    // hermite curve
    Reinterpolate(Node);

    // tangent lines
    CreateTangentLines(&vertices, &count, &size, &control_point);
    tangent.Create(shader.Get(), color[0]);
    tangent.Update(vertices, count, size);
    delete[] vertices;
    vertices = NULL;

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
    tangent.Destroy();

    NODE* Node;

    control_point.Reset();
    while (control_point.Read(&Node))
        Node->spline.Destroy();

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

// Position = 0.0     Color = (0, 7, 100)
// Position = 0.16    Color = (32, 107, 203)
// Position = 0.42    Color = (237, 255, 255)
// Position = 0.6425  Color = (255, 170, 0)
// Position = 0.8575  Color = (0, 2, 0)
//
void OnFileRed(HWND hWnd)
{
    const int k = 5;

    NODE* Node;
    double x[k], y[k], x1, y1, m1, x2, y2, m2;
    float* vertices = NULL;
    int i, n, count, size;

    x[0] = 0.0;     y[0] = 0.0 / 255.0;
    x[1] = 0.16;    y[1] = 32.0 / 255.0;
    x[2] = 0.42;    y[2] = 237.0 / 255.0;
    x[3] = 0.6425;  y[3] = 255.0 / 255.0;
    x[4] = 0.8575;  y[4] = 0.0 / 255.0;

    // clear control point
    control_point.Reset();
    while (control_point.Read(&Node))
        Node->spline.Destroy();

    control_point.Clear();

    // initialize control point
    for (i = 0; i < k; i++) {
        Node = control_point.Add(x[i], y[i], 0.0, 100);
        Node->spline.Create(shader.Get(), color[1]);
    }

    // tangent lines
    CreateTangentLines(&vertices, &count, &size, &control_point);
    tangent.Update(vertices, count, size);
    delete[] vertices;
    vertices = NULL;

    // spline
    control_point.Reset();

    n = control_point.GetCount();
    i = 0;

    while (control_point.Read(&Node)) {

        x1 = Node->x[0];
        y1 = Node->y[0];
        m1 = Node->m;

        count = Node->count;

        x2 = Node->Next->x[0];
        y2 = Node->Next->y[0];
        m2 = Node->Next->m;

        Interpolate(&vertices, count, &size, x1, y1, m1, x2, y2, m2);
        Node->spline.Update(vertices, count, size);
        delete[] vertices;
        vertices = NULL;

        if (i == (n - 2)) break;

        ++i;
    }
}

// Position = 0.0     Color = (0, 7, 100)
// Position = 0.16    Color = (32, 107, 203)
// Position = 0.42    Color = (237, 255, 255)
// Position = 0.6425  Color = (255, 170, 0)
// Position = 0.8575  Color = (0, 2, 0)
//
void OnFileGreen(HWND hWnd)
{
    const int k = 5;

    NODE* Node;
    double x[k], y[k], x1, y1, m1, x2, y2, m2;
    float* vertices = NULL;
    int i, n, count, size;

    x[0] = 0.0;     y[0] = 7.0 / 255.0;
    x[1] = 0.16;    y[1] = 107.0 / 255.0;
    x[2] = 0.42;    y[2] = 255.0 / 255.0;
    x[3] = 0.6425;  y[3] = 170.0 / 255.0;
    x[4] = 0.8575;  y[4] = 0.0 / 255.0;

    // clear control point
    control_point.Reset();
    while (control_point.Read(&Node))
        Node->spline.Destroy();

    control_point.Clear();

    // initialize control point
    for (i = 0; i < k; i++) {
        Node = control_point.Add(x[i], y[i], 0.0, 100);
        Node->spline.Create(shader.Get(), color[1]);
    }

    // tangent lines
    CreateTangentLines(&vertices, &count, &size, &control_point);
    tangent.Update(vertices, count, size);
    delete[] vertices;
    vertices = NULL;

    // spline
    control_point.Reset();

    n = control_point.GetCount();
    i = 0;

    while (control_point.Read(&Node)) {

        x1 = Node->x[0];
        y1 = Node->y[0];
        m1 = Node->m;

        count = Node->count;

        x2 = Node->Next->x[0];
        y2 = Node->Next->y[0];
        m2 = Node->Next->m;

        Interpolate(&vertices, count, &size, x1, y1, m1, x2, y2, m2);
        Node->spline.Update(vertices, count, size);
        delete[] vertices;
        vertices = NULL;

        if (i == (n - 2)) break;

        ++i;
    }
}

// Position = 0.0     Color = (0, 7, 100)
// Position = 0.16    Color = (32, 107, 203)
// Position = 0.42    Color = (237, 255, 255)
// Position = 0.6425  Color = (255, 170, 0)
// Position = 0.8575  Color = (0, 2, 0)
//
void OnFileBlue(HWND hWnd)
{
    const int k = 5;

    NODE* Node;
    double x[k], y[k], x1, y1, m1, x2, y2, m2;
    float* vertices = NULL;
    int i, n, count, size;

    x[0] = 0.0;     y[0] = 100.0 / 255.0;
    x[1] = 0.16;    y[1] = 203.0 / 255.0;
    x[2] = 0.42;    y[2] = 255.0 / 255.0;
    x[3] = 0.6425;  y[3] = 0.0 / 255.0;
    x[4] = 0.8575;  y[4] = 0.0 / 255.0;

    // clear control point
    control_point.Reset();
    while (control_point.Read(&Node))
        Node->spline.Destroy();

    control_point.Clear();

    // initialize control point
    for (i = 0; i < k; i++) {
        Node = control_point.Add(x[i], y[i], 0.0, 100);
        Node->spline.Create(shader.Get(), color[1]);
    }

    // tangent lines
    CreateTangentLines(&vertices, &count, &size, &control_point);
    tangent.Update(vertices, count, size);
    delete[] vertices;
    vertices = NULL;

    // spline
    control_point.Reset();

    n = control_point.GetCount();
    i = 0;

    while (control_point.Read(&Node)) {

        x1 = Node->x[0];
        y1 = Node->y[0];
        m1 = Node->m;

        count = Node->count;

        x2 = Node->Next->x[0];
        y2 = Node->Next->y[0];
        m2 = Node->Next->m;

        Interpolate(&vertices, count, &size, x1, y1, m1, x2, y2, m2);
        Node->spline.Update(vertices, count, size);
        delete[] vertices;
        vertices = NULL;

        if (i == (n - 2)) break;

        ++i;
    }
}
/*
#set up the control points for your gradient
yR_observed = [0, 0,32,237, 255, 0, 0, 32]
yG_observed = [2, 7, 107, 255, 170, 2, 7, 107]
yB_observed = [0, 100, 203, 255, 0, 0, 100, 203]
*/
void OnFileExit(HWND hWnd)
{
    DestroyWindow(hWnd);
}

// iadd ang control point
void OnEditAdd(HWND hWnd, CDoublyLinkedList* control_point, double* points)
{
    double x, y, m;
    float* vertices = NULL;
    int count, size;

    count = MAX_SPLINE_VERTEX_COUNT;

    m = 0.25;

    x = points[0];
    y = points[1];

    // iupdate ang mga control point at hermite curve
    selected_node = control_point->Add(selected_node, x, y, m, count);
    selected_node->spline.Create(shader.Get(), color[1]);
    Reinterpolate(selected_node);

    // iupdate ang tangent line
    CreateTangentLines(&vertices, &count, &size, control_point);
    tangent.Update(vertices, count, size);
    delete[] vertices;
    vertices = NULL;
}

void OnEditEdit(HWND hWnd)
{
    dlg.Show(hWnd, hInst, DlgProc, selected_node->m);
}

// idelete ang selected control point
void OnEditDelete(HWND hWnd, CDoublyLinkedList* control_point)
{
    NODE* Node1;
    NODE* Node2;
    float* vertices = NULL;
    int count, size;

    Node1 = selected_node->Prev;
    Node2 = selected_node->Next;

    control_point->Remove(selected_node);

    // iupdate ang mga control point at tangent line
    CreateTangentLines(&vertices, &count, &size, control_point);
    tangent.Update(vertices, count, size);
    delete[] vertices;
    vertices = NULL;

    // iupdate ang hermite curve
    if (Node1 != NULL) Reinterpolate(Node1);
    if (Node2 != NULL) Reinterpolate(Node2);
}

// ioutput ang mga control point
void OnViewControlPoint(HWND hWnd)
{
    OutputDebugControlPoint(&control_point);
}

// alamin kung may above one
void OnViewVerify(HWND hWnd)
{
    NODE* Node;
    int i, n;
    char str[100];

    n = control_point.GetCount();

    control_point.Reset();

    i = 0;

    while (control_point.Read(&Node)) {

        sprintf_s(str, 100, "%10d\n", i);
        OutputDebugStringA(str);

        Node->spline.Verify();

        if (i > (n - 3)) break;

        ++i;
    }
}

void OnToolsFiniteDifference(HWND hWnd)
{
    NODE* Node;
    double* x;
    double* y;
    double* m;
    double x1, y1, m1, x2, y2, m2;
    float* vertices = NULL;
    int k, n, count, size;

    n = control_point.GetCount();

    x = new double[n];
    y = new double[n];
    m = new double[n];

    control_point.Reset();

    k = 0;

    while (control_point.Read(&Node)) {

        x[k] = Node->x[0];
        y[k] = Node->y[0];
        m[k] = Node->m;

        ++k;
    }

    for (k = 1; k < (n - 1); k++)
        m[k] = 0.5 * ((y[k + 1] - y[k]) / (x[k + 1] - x[k]) + (y[k] - y[k - 1]) / (x[k] - x[k - 1]));

    control_point.Reset();

    k = 0;

    while (control_point.Read(&Node)) {

        Node->m = m[k++];

        Node->x[1] = Node->x[0] + 0.05;
        Node->y[1] = Node->y[0] + 0.05 * Node->m;
    }

    delete[] x;
    delete[] y;
    delete[] m;

    // tangent lines
    CreateTangentLines(&vertices, &count, &size, &control_point);
    tangent.Update(vertices, count, size);
    delete[] vertices;
    vertices = NULL;

    // spline
    control_point.Reset();

    n = control_point.GetCount();
    k = 0;

    while (control_point.Read(&Node)) {

        //sprintf_s(str, 100, "%10d%20.4f\n", k, Node->m);
        //OutputDebugStringA(str);

        x1 = Node->x[0];
        y1 = Node->y[0];
        m1 = Node->m;

        count = Node->count;

        x2 = Node->Next->x[0];
        y2 = Node->Next->y[0];
        m2 = Node->Next->m;

        Interpolate(&vertices, count, &size, x1, y1, m1, x2, y2, m2);
        Node->spline.Update(vertices, count, size);
        delete[] vertices;
        vertices = NULL;

        if (k == (n - 2)) break;

        ++k;
    }
}

void OnToolsCardinalSPLine(HWND hWnd, double c)
{
    NODE* Node;
    double* x;
    double* y;
    double* m;
    double x1, y1, m1, x2, y2, m2;
    float* vertices = NULL;
    int k, n, count, size;

    n = control_point.GetCount();

    x = new double[n];
    y = new double[n];
    m = new double[n];

    control_point.Reset();

    k = 0;

    while (control_point.Read(&Node)) {

        x[k] = Node->x[0];
        y[k] = Node->y[0];
        m[k] = Node->m;

        ++k;
    }

    for (k = 1; k < (n - 1); k++)
        m[k] = (1.0 - c) * ((y[k + 1] - y[k- 1]) / (x[k + 1] - x[k - 1]));

    control_point.Reset();

    k = 0;

    while (control_point.Read(&Node)) {

        Node->m = m[k++];

        Node->x[1] = Node->x[0] + 0.05;
        Node->y[1] = Node->y[0] + 0.05 * Node->m;
    }

    delete[] x;
    delete[] y;
    delete[] m;

    // tangent lines
    CreateTangentLines(&vertices, &count, &size, &control_point);
    tangent.Update(vertices, count, size);
    delete[] vertices;
    vertices = NULL;

    // spline
    control_point.Reset();

    n = control_point.GetCount();
    k = 0;

    while (control_point.Read(&Node)) {

        //sprintf_s(str, 100, "%10d%20.4f\n", k, Node->m);
        //OutputDebugStringA(str);

        x1 = Node->x[0];
        y1 = Node->y[0];
        m1 = Node->m;

        count = Node->count;

        x2 = Node->Next->x[0];
        y2 = Node->Next->y[0];
        m2 = Node->Next->m;

        Interpolate(&vertices, count, &size, x1, y1, m1, x2, y2, m2);
        Node->spline.Update(vertices, count, size);
        delete[] vertices;
        vertices = NULL;

        if (k == (n - 2)) break;

        ++k;
    }
}

void OnToolsCatmullRomSPLine(HWND hWnd)
{
    NODE* Node;
    double* x;
    double* y;
    double* m;
    double x1, y1, m1, x2, y2, m2;
    float* vertices = NULL;
    int k, n, count, size;

    n = control_point.GetCount();

    x = new double[n];
    y = new double[n];
    m = new double[n];

    control_point.Reset();

    k = 0;

    while (control_point.Read(&Node)) {

        x[k] = Node->x[0];
        y[k] = Node->y[0];
        m[k] = Node->m;

        ++k;
    }

    for (k = 1; k < (n - 1); k++)
        m[k] = (y[k + 1] - y[k - 1]) / 2.0;

    control_point.Reset();

    k = 0;

    while (control_point.Read(&Node)) {

        Node->m = m[k++];

        Node->x[1] = Node->x[0] + 0.05;
        Node->y[1] = Node->y[0] + 0.05 * Node->m;
    }

    delete[] x;
    delete[] y;
    delete[] m;

    // tangent lines
    CreateTangentLines(&vertices, &count, &size, &control_point);
    tangent.Update(vertices, count, size);
    delete[] vertices;
    vertices = NULL;

    // spline
    control_point.Reset();

    n = control_point.GetCount();
    k = 0;

    while (control_point.Read(&Node)) {

        //sprintf_s(str, 100, "%10d%20.4f\n", k, Node->m);
        //OutputDebugStringA(str);

        x1 = Node->x[0];
        y1 = Node->y[0];
        m1 = Node->m;

        count = Node->count;

        x2 = Node->Next->x[0];
        y2 = Node->Next->y[0];
        m2 = Node->Next->m;

        Interpolate(&vertices, count, &size, x1, y1, m1, x2, y2, m2);
        Node->spline.Update(vertices, count, size);
        delete[] vertices;
        vertices = NULL;

        if (k == (n - 2)) break;

        ++k;
    }
}

void OnToolsZero(HWND hWnd)
{
    float* vertices = NULL;
    int count, size;

    control_point.Set(selected_node, 0.0);

    // iupdate ang mga control point at tangent line
    CreateTangentLines(&vertices, &count, &size, &control_point);
    tangent.Update(vertices, count, size);
    delete[] vertices;
    vertices = NULL;

    // iupdate ang hermite curve
    Reinterpolate(selected_node);
}

void OnToolsResultant(HWND hWnd)
{
    OutputDebugString(L"OnToolsResultant\n");

    NODE* Node1;
    NODE* Node2;
    double x1, y1, x2, y2, x3, y3, ax, ay, bx, by, m;
    float* vertices = NULL;
    int count, size;

    Node1 = selected_node->Prev;
    Node2 = selected_node->Next;

    x1 = Node1->x[0];
    y1 = Node1->y[0];

    x2 = selected_node->x[0];
    y2 = selected_node->y[0];

    x3 = Node2->x[0];
    y3 = Node2->y[0];

    ax = x2 - x1;
    ay = y2 - y1;

    bx = x3 - x2;
    by = y3 - y2;

    m = (ay + by) / (ax + bx);

    control_point.Set(selected_node, m);

    // iupdate ang mga control point at tangent line
    CreateTangentLines(&vertices, &count, &size, &control_point);
    tangent.Update(vertices, count, size);
    delete[] vertices;
    vertices = NULL;

    // iupdate ang hermite curve
    Reinterpolate(selected_node);
}
