
// program para maggenerate ng source code para sa framework.h/.cpp para sa OpenGL
//
// 1. Open glext.h in Notepad.
// 2. Select all.
// 3. Copy.
// 4. Run scrgen_opengl program.
// 5. Select menu Edit > Paste.
// 6. Select menu Tools > Generate Header.
// 7. Result is in Output window of Visual Studio.
// 8. Select menu Tools > Generate Source.
// 9. Result is in Output window of Visual Studio.

#include "framework.h"
#include "scrgen_opengl.h"
#include "queue.h"
#include "queue_s2i.h"
#include "queue_2s.h"

const int MAX_LOADSTRING = 100;

// variable
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

HWND hEdit1;

// function
LRESULT CALLBACK    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

char* ToUpper(char* str);
bool FindString(char* str1, char* str, int start, int* pos);
bool FindSpace(char* str, int start, int* pos);
bool FindCharacters(char* str, int start, int* pos);
bool RFindSpace(char* str, int start, int* pos);
bool FindNonSpace(char* str, int start, int* pos);
void GetString(char* str1, char* str2, int i1, int i2);
void GetGLFunctions(CQueue_2s* que, char* str, int i1, int i2);

void OutputString(char* str, int start);

size_t PasteText(HWND hWnd, char** buffer);
size_t CopyText(HWND hWnd, char* buffer);

void OnTextChanged(HWND hWnd, LPARAM lParam);

void OnSize(HWND hWnd, int width, int height);

void OnPaint(HWND hWnd);
void OnCreate(HWND hWnd);
void OnDestroy(HWND hWnd);

void OnFileExit(HWND hWnd);

void OnEditCopy(HWND hWnd);
void OnEditPaste(HWND hWnd);

void OnToolsGenerateHeader(HWND hWnd);
void OnToolsGenerateSource(HWND hWnd);

// main program
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // initialize ang mga variable
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SCRGENOPENGL, szWindowClass, MAX_LOADSTRING);

    // i register ang window class
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SCRGENOPENGL));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SCRGENOPENGL);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    // i save ito
    hInst = hInstance;

    // gumawa ng window
    int X, Y, nWidth, nHeight, Cx, Cy;

    // 1080p: 1920 x 1080
    //  720p: 1280 x  720
    //  480p:  854 x  480

    Cx = 1280;
    Cy = 720;

    nWidth = Cx + 16;
    nHeight = Cy + 59;

    X = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2;
    Y = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 3;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        X, Y,
        nWidth, nHeight,
        nullptr, nullptr, hInstance, nullptr);

    // kapag may error, wag i run
    if (!hWnd)
        return FALSE;

    // i show ang window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // para sa keyboard shortcut
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SCRGENOPENGL));

    // message loop
    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

// window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:       OnSize(hWnd, LOWORD(lParam), HIWORD(lParam));                                 break;

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDC_EDIT1:

            if (HIWORD(wParam) == EN_CHANGE) {
                //OutputDebugString(L"OnTextChanged 2\n");
            }
            else {
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        case IDM_EXIT:          OnFileExit(hWnd);		        break;
        case IDM_COPY:          OnEditCopy(hWnd);               break;
        case IDM_PASTE:         OnEditPaste(hWnd);              break;
        case IDM_GENERATE_H:    OnToolsGenerateHeader(hWnd);    break;
        case IDM_GENERATE_CPP:  OnToolsGenerateSource(hWnd);    break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;

    case WM_PAINT:   OnPaint(hWnd);									break;
    case WM_CREATE:  OnCreate(hWnd);								break;
    case WM_DESTROY: OnDestroy(hWnd);								break;

    case WM_CTLCOLORSTATIC:

        // white background ng Edit control
        if (GetDlgCtrlID((HWND)lParam) == IDC_EDIT1)
            return (LRESULT)GetStockObject(WHITE_BRUSH);

        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

char* ToUpper(char* str)
{
    size_t i, n;
    static char str1[MAX_STRING];

    n = strlen(str);

    for (i = 0; i < n; i++)
        str1[i] = toupper(str[i]);

    str1[n] = '\0';

    return str1;
}

// find string str1 within string str2
bool FindString(char* str1, char* str2, int start, int* pos)
{
    int i1, i2, k, n1, n2;
    bool found;

    found = false;

    n1 = (int)strlen(str1);
    n2 = (int)strlen(str2) - n1;

    for (i2 = start; i2 < n2; i2++) {

        k = i2;
        found = true;

        for (i1 = 0; i1 < n1; i1++) {

            if (str1[i1] != str2[k]) {
                found = false;
                break;
            }

            ++k;
        }

        if (found) {
            pos[0] = i2;
            pos[1] = i2 + n1;
            break;
        }
    }

    return found;
}

// find space within string str
bool FindSpace(char* str, int start, int* pos)
{
    int i, n;
    bool found;

    found = false;

    n = (int)strlen(str);

    for (i = start; i < n; i++) {
        if (str[i] == ' ' || str[i] == 10 || str[i] == 13) {
            found = true;
            *pos = i;
            break;
        }
    }

    return found;
}

// find character ' ' or '(' within string str
bool FindCharacters(char* str, int start, int* pos)
{
    int i, n;
    bool found;

    found = false;

    n = (int)strlen(str);

    for (i = start; i < n; i++) {
        if (str[i] == ' ' || str[i] == '(') {
            found = true;
            *pos = i;
            break;
        }
    }

    return found;
}

// find space within string str
bool RFindSpace(char* str, int start, int* pos)
{
    int i;
    bool found;

    found = false;

    for (i = start; i > -1; i--) {

        if (str[i] == ' ' || str[i] == 10 || str[i] == 13) {
            found = true;
            *pos = i;
            break;
        }
    }

    return found;
}

// find non space within string str
bool FindNonSpace(char* str, int start, int* pos)
{
    int i, n;
    bool found;

    found = false;

    n = (int)strlen(str);

    for (i = start; i < n; i++) {
        if (str[i] != ' ') {
            found = true;
            *pos = i;
            break;
        }
    }

    return found;
}

void GetString(char* str1, char* str2, int i1, int i2)
{
    int i, k;

    k = 0;

    for (i = i1; i < i2; i++)
        str1[k++] = str2[i];

    str1[k] = '\0';
}

void GetGLFunctions(CQueue_2s* que, char* str, int i1, int i2)
{
    int i, k, pos[2];
    char str1[MAX_STRING], str2[MAX_STRING];
    bool found;

    i = i1;

    while (true) {

        // GLAPI void APIENTRY glDrawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
        //                     ^ ^
        //                     | |
        //                     | pos[1]
        //                     pos[0]
        //
        strcpy_s(str1, 100, "gl");
        found = FindString(str1, str, i, pos);

        // wala ng gl function alis na sa loop
        if (!found) break;

        // ito ay gl sa susunod na version
        if (pos[0] > i2) break;

        // GLAPI void APIENTRY glDrawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
        //                       ^                ^
        //                       |                |
        //                       pos[1]           k
        //
        FindCharacters(str, pos[1], &k);

        GetString(str1, str, pos[0], k);
        sprintf_s(str2, MAX_STRING, "PFN%sPROC", ToUpper(str1));

        que->Add(str2, str1);

        //OutputDebugStringA(str2);
        //OutputDebugStringA("          ");
        //OutputDebugStringA(str1);
        //OutputDebugStringA("\n");

        i = k;
    }

}

void OutputString(char* str, int start)
{
    int i, k;
    char str1[51];

    sprintf_s(str1, 51, "%10d ", start);
    OutputDebugStringA(str1);

    k = start;

    for (i = 0; i < 50; i++)
        str1[i] = str[k++];

    str1[50] = '\0';

    OutputDebugStringA(str1);
    OutputDebugStringA("\n");
}

// i-point ang laman ng clipboard sa buffer
size_t PasteText(HWND hWnd, char** buffer)
{
    HGLOBAL hglb;
    LPSTR lpstr;
    size_t i, n;

    n = 0;

    if (!IsClipboardFormatAvailable(CF_TEXT))
    {
        MessageBox(hWnd, L"Hindi Text ang laman ng clipboard.", L"Clipboard Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    if (!OpenClipboard(hWnd))
    {
        MessageBox(hWnd, L"Di ko ma-open ang clipboard.", L"Clipboard Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    if ((hglb = GetClipboardData(CF_TEXT)) == NULL)
    {
        MessageBox(hWnd, L"Walang laman ang clipboard.", L"Clipboard Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    lpstr = (LPSTR)GlobalLock(hglb);

    n = strlen(lpstr);

    // i-release bago mag-allocate
    if (*buffer != NULL)
    {
        delete[] * buffer;
        *buffer = NULL;
    }

    *buffer = new char[n + 1];

    // kopyahin sa ibang variable (buffer)
    // kasi lpstr ay para sa clipboard
    // pedeng mawala
    for (i = 0; i < n; ++i)
    {
        *(*buffer + i) = lpstr[i];
    }
    *(*buffer + n) = '\0';  // null terminated

    GlobalUnlock(hglb);

    CloseClipboard();

    return n;
}

// icopy ang laman ng buffer sa clipboard
size_t CopyText(HWND hWnd, char* buffer)
{
    HGLOBAL hglb;
    LPSTR lpstr;
    size_t n;

    n = 0;

    if (!OpenClipboard(hWnd))
    {
        MessageBox(hWnd, L"Di ko ma-open ang clipboard.", L"Clipboard Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    if (EmptyClipboard() == 0)
    {
        MessageBox(hWnd, L"Di ko ma-empty ang clipboard.", L"Clipboard Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    n = strlen(buffer) + 1;
    hglb = GlobalAlloc(GMEM_MOVEABLE, n);
    if (hglb == NULL)
    {
        MessageBox(hWnd, L"Di ko makakuha ng memory.", L"Clipboard Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    // Copy the text from buffer. 

    lpstr = (LPSTR)GlobalLock(hglb);
    memcpy(lpstr, buffer, n);
    GlobalUnlock(hglb);

    // Place the handle on the clipboard. 

    SetClipboardData(CF_TEXT, hglb);

    CloseClipboard();

    return n;
}

// idisable ang menu pag walang tinayp
void OnTextChanged(HWND hWnd, LPARAM lParam)
{
    HWND hEdit = (HWND)lParam;
    int n = GetWindowTextLength(hEdit);
    UINT enable = n > 0 ? MF_ENABLED : MF_DISABLED;

    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_GENERATE_H, MF_BYCOMMAND | enable);
    EnableMenuItem(hMenu, IDM_GENERATE_CPP, MF_BYCOMMAND | enable);
}

void OnSize(HWND hWnd, int width, int height)
{
    MoveWindow(hEdit1, 0, 0, width, height, FALSE);
}

void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hDC;
    hDC = BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
}

void OnCreate(HWND hWnd)
{
    /*
        HMENU hMenu = GetMenu(hWnd);
        EnableMenuItem(hMenu, IDM_GENERATE_H, MF_BYCOMMAND | MF_DISABLED);
        EnableMenuItem(hMenu, IDM_GENERATE_CPP, MF_BYCOMMAND | MF_DISABLED);
    */
    // gumawa ng font para sa edit control
    HFONT hFont = CreateFont(16, 0, 0, 0, FW_REGULAR, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, L"Consolas");

    hEdit1 = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE | ES_AUTOHSCROLL | WS_BORDER | WS_VSCROLL | WS_HSCROLL,
        0, 0, 1, 1, hWnd, (HMENU)IDC_EDIT1, hInst, NULL);

    // gamitin ang font na 'to
    SendMessage(hEdit1, WM_SETFONT, (WPARAM)hFont, 0);
}

void OnDestroy(HWND hWnd)
{
    PostQuitMessage(0);
}

void OnFileExit(HWND hWnd)
{
    DestroyWindow(hWnd);
}

void OnEditCopy(HWND hWnd)
{
}

void OnEditPaste(HWND hWnd)
{
    char* buffer = NULL;

    PasteText(hWnd, &buffer);

    SetWindowTextA(hEdit1, buffer);
    SetFocus(hEdit1);

    if (buffer != NULL)
        delete[] buffer;
}

// .h
void OnToolsGenerateHeader(HWND hWnd)
{
    CQueue que;
    CQueue_s2i que1;
    CQueue_2s* que2;
    int pos[2], pos1, pos2, index1, index2, index;
    char str1[100], str2[100], source[MAX_STRING];
    char* strA;
    char* strB;
    bool found;

    char* buffer = NULL;
    int n = GetWindowTextLengthA(hEdit1);

    if (n == 0) return;

    buffer = new char[n + 1];

    GetWindowTextA(hEdit1, buffer, n);
    buffer[n] = '\0';

    index = 0;

    while (true) {

        // hanapin ang string na 'to #ifndef GL_VERSION_#_#
        // ito ang begining string position
        //
        //             #ifndef GL_VERSION_1_2
        //             ^                  ^
        //             pos[0]             pos[1]
        //             index1
        //
        strcpy_s(str1, 100, "#ifndef GL_VERSION_");
        found = FindString(str1, buffer, index, pos);

        // wala ng GL_VERSION_ alis na sa loop
        if (!found) break;

        index1 = pos[0];

        // hanapin ang space pagkatapos ng GL_VERSION_#_#
        //
        //             #ifndef GL_VERSION_1_2
        //                                ^  ^
        //                                |  |
        //                                |  pos2
        //                                |
        //                                pos[1]
        found = FindSpace(buffer, pos[1], &pos2);

        // hanapin ang space bago ang GL_VERSION_#_#
        //
        //             #ifndef GL_VERSION_1_2
        //                    ^             ^
        //                    |             |
        //                    pos1          pos2 - 1
        //
        found = RFindSpace(buffer, pos2 - 1, &pos1);

        // kunin ang string GL_VERSION_#_#
        GetString(str1, buffer, pos1 + 1, pos2);

        // gamit ang string na nakuha sa itaas
        // hanapin ang string na 'to #endif /* GL_VERSION_#_# */
        // ito ang ending string position
        //
        //             #endif /* GL_VERSION_1_2 */
        //             ^                          ^
        //             |                          |
        //             pos[0]                     pos[1]
        //             index2
        //
        sprintf_s(str2, 100, "#endif /* %s */", str1);
        found = FindString(str2, buffer, pos2, pos);
        index2 = pos[1];

        que1.Add(str1, index1, index2);

        index = pos[1];
    }

    // kunin isa-isa ang mga source na define GL_VERSION
    char* version;
    int i1, i2;

    que1.Reset();

    while (que1.Read(&version, &i1, &i2)) {

        que2 = que.Add(version);

        GetGLFunctions(que2 ,buffer, i1, i2);

    }

    delete[] buffer;

    // MAGGENERATE NG SOURCE CODE
    int max_len, len;

    max_len = 0;

    OutputDebugStringA("\n");
    OutputDebugStringA("#pragma once\n");
    OutputDebugStringA("\n");
    OutputDebugStringA("#include \"targetver.h\"\n");
    OutputDebugStringA("\n");
    OutputDebugStringA("#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers\n");
    OutputDebugStringA("#define _USE_MATH_DEFINES               // for M_PI\n");
    OutputDebugStringA("\n");
    OutputDebugStringA("// Windows Header Files\n");
    OutputDebugStringA("#include <windows.h>\n");
    OutputDebugStringA("\n");
    OutputDebugStringA("// C RunTime Header Files\n");
    OutputDebugStringA("#include <stdlib.h>\n");
    OutputDebugStringA("#include <stdio.h>\n");
    OutputDebugStringA("#include <malloc.h>\n");
    OutputDebugStringA("#include <memory.h>\n");
    OutputDebugStringA("#include <tchar.h>\n");
    OutputDebugStringA("#include <math.h>\n");
    OutputDebugStringA("\n");
    OutputDebugStringA("// OpenGL headers\n");
    OutputDebugStringA("#include <gl/gl.h>\n");
    OutputDebugStringA("#include <gl/glu.h>\n");
    OutputDebugStringA("#include <gl/glext.h>\n");
    OutputDebugStringA("\n");
    OutputDebugStringA("/*\n");
    OutputDebugStringA("opengl32.lib\n");
    OutputDebugStringA("glu32.lib\n");
    OutputDebugStringA("*/\n\n");

    que.Reset();

    while (que.Read(&version, &que2)) {

        sprintf_s(source, MAX_STRING, "/* %s */\n\n", version);
        OutputDebugStringA(source);

        que2->Reset();

        while (que2->Read(&strA, &strB)) {

            sprintf_s(source, MAX_STRING, "extern %-52s %s;\n", strA, strB);
            OutputDebugStringA(source);

            len = (int)strlen(strA);
            if (len > max_len) max_len = len;

        }

        OutputDebugStringA("\n");
    }

    OutputDebugStringA("void LoadOpenGLFunctions();\n");
    OutputDebugStringA("bool FindExtension(const char* str);\n");
    OutputDebugStringA("void OutputExtenstions();\n");
    OutputDebugStringA("\n");

    sprintf_s(str1, 100, "MAX LENGTH = %d\n", max_len);
    OutputDebugStringA(str1);
}

// .cpp
void OnToolsGenerateSource(HWND hWnd)
{
    CQueue que;
    CQueue_s2i que1;
    CQueue_2s* que2;
    int pos[2], pos1, pos2, index1, index2, index;
    char str1[100], str2[100], source[MAX_STRING], source2[MAX_STRING];
    char* strA;
    char* strB;
    bool found;

    char* buffer = NULL;
    int n = GetWindowTextLengthA(hEdit1);

    if (n == 0) return;

    buffer = new char[n + 1];

    GetWindowTextA(hEdit1, buffer, n);
    buffer[n] = '\0';

    index = 0;

    while (true) {

        // hanapin ang string na 'to #ifndef GL_VERSION_#_#
        // ito ang begining string position
        //
        //             #ifndef GL_VERSION_1_2
        //             ^                  ^
        //             pos[0]             pos[1]
        //             index1
        //
        strcpy_s(str1, 100, "#ifndef GL_VERSION_");
        found = FindString(str1, buffer, index, pos);

        // wala ng GL_VERSION_ alis na sa loop
        if (!found) break;

        index1 = pos[0];

        // hanapin ang space pagkatapos ng GL_VERSION_#_#
        //
        //             #ifndef GL_VERSION_1_2
        //                                ^  ^
        //                                |  |
        //                                |  pos2
        //                                |
        //                                pos[1]
        found = FindSpace(buffer, pos[1], &pos2);

        // hanapin ang space bago ang GL_VERSION_#_#
        //
        //             #ifndef GL_VERSION_1_2
        //                    ^             ^
        //                    |             |
        //                    pos1          pos2 - 1
        //
        found = RFindSpace(buffer, pos2 - 1, &pos1);

        // kunin ang string GL_VERSION_#_#
        GetString(str1, buffer, pos1 + 1, pos2);

        // gamit ang string na nakuha sa itaas
        // hanapin ang string na 'to #endif /* GL_VERSION_#_# */
        // ito ang ending string position
        //
        //             #endif /* GL_VERSION_1_2 */
        //             ^                          ^
        //             |                          |
        //             pos[0]                     pos[1]
        //             index2
        //
        sprintf_s(str2, 100, "#endif /* %s */", str1);
        found = FindString(str2, buffer, pos2, pos);
        index2 = pos[1];

        que1.Add(str1, index1, index2);

        index = pos[1];
    }

    // kunin isa-isa ang mga source na define GL_VERSION
    char* version;
    int i1, i2;

    que1.Reset();

    while (que1.Read(&version, &i1, &i2)) {

        que2 = que.Add(version);

        GetGLFunctions(que2, buffer, i1, i2);

    }

    delete[] buffer;

    // MAGGENERATE NG SOURCE CODE

    OutputDebugStringA("#include \"framework.h\"\n");
    OutputDebugStringA("\n");

    que.Reset();

    while (que.Read(&version, &que2)) {

        sprintf_s(source, MAX_STRING, "/* %s */\n\n", version);
        OutputDebugStringA(source);

        que2->Reset();

        while (que2->Read(&strA, &strB)) {

            sprintf_s(source, MAX_STRING, "%-52s %-45s = NULL;\n", strA, strB);
            OutputDebugStringA(source);

        }

        OutputDebugStringA("\n");
    }

    OutputDebugStringA("void LoadOpenGLFunctions()\n");
    OutputDebugStringA("{\n");

    que.Reset();

    while (que.Read(&version, &que2)) {

        sprintf_s(source, MAX_STRING, "\t/* %s */\n\n", version);
        OutputDebugStringA(source);

        que2->Reset();

        while (que2->Read(&strA, &strB)) {

            sprintf_s(source2, MAX_STRING, "(%s)", strA);

            sprintf_s(source, MAX_STRING, "\t%-45s = %-47s wglGetProcAddress(\"%s\");\n", strB, source2, strB);
            OutputDebugStringA(source);

        }

        OutputDebugStringA("\n");
    }

    OutputDebugStringA("}\n");

    OutputDebugStringA("\n");
    OutputDebugStringA("bool FindExtension(const char* str)\n");
    OutputDebugStringA("{\n");
    OutputDebugStringA("\tchar* extensions = (char*)glGetString(GL_EXTENSIONS);\n");
    OutputDebugStringA("\treturn (strstr(extensions, str) != NULL);\n");
    OutputDebugStringA("}\n");

    OutputDebugStringA("\n");
    OutputDebugStringA("void OutputExtenstions()\n");
    OutputDebugStringA("{\n");
    OutputDebugStringA("\tchar* str1 = (char*)glGetString(GL_EXTENSIONS);\n");
    OutputDebugStringA("\tchar str2[300];\n");
    OutputDebugStringA("\tsize_t i, j, n;\n");
    OutputDebugStringA("\n");
    OutputDebugStringA("\tn = strlen(str1);\n");
    OutputDebugStringA("\ti = 0;\n");
    OutputDebugStringA("\n");
    OutputDebugStringA("\twhile (i < n) {\n");
    OutputDebugStringA("\n");
    OutputDebugStringA("\t\tj = 0;\n");
    OutputDebugStringA("\n");
    OutputDebugStringA("\t\twhile (str1[i] != ' ') {\n");
    OutputDebugStringA("\n");
    OutputDebugStringA("\t\t\tstr2[j] = str1[i];\n");
    OutputDebugStringA("\n");
    OutputDebugStringA("\t\t\t++i;\n");
    OutputDebugStringA("\t\t\t++j;\n");
    OutputDebugStringA("\t\t}\n");
    OutputDebugStringA("\n");
    OutputDebugStringA("\t\tstr2[j] = '\\0';\n");
    OutputDebugStringA("\n");
    OutputDebugStringA("\t\tOutputDebugStringA(str2);\n");
    OutputDebugStringA("\t\tOutputDebugStringA(\"\\n\");\n");
    OutputDebugStringA("\n");
    OutputDebugStringA("\t\t++i;\n");
    OutputDebugStringA("\t}\n");
    OutputDebugStringA("}\n");
}


