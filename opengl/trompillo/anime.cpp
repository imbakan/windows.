
#include "framework.h"
#include "anime.h"
#include "matrix.h"
#include "queue.h"

CAnime::CAnime()
{
    spin_rate = 0.0;

    thread = NULL;
    stopevent = NULL;
    id = 0;

    stopevent = CreateEvent(NULL, TRUE, FALSE, L"FireworksStopEvent");
    if (stopevent == NULL) {
        OutputDebugString(L"Create event failed.\n");
    }
}

CAnime::~CAnime()
{
    if (thread != NULL)    CloseHandle(thread);
    if (stopevent != NULL) CloseHandle(stopevent);

    delete[] X;
    delete[] Y;

    delete[] Rx;
    delete[] Ry;
}

double* CAnime::RotateZ(double degree, double x, double y)
{
    double r, a, b, a1, b1, a2, b2, radian;
    static double p[2];

    r = sqrt(x * x + y * y);

    a1 = x / r;
    b1 = y / r;

    radian = (degree / 180.0) * M_PI;

    a2 = cos(radian);
    b2 = sin(radian);

    a = a1 * a2 - b1 * b2;
    b = a2 * b1 + a1 * b2;

    r = sqrt(a * a + b * b);
    
    p[0] = a / r;
    p[1] = b / r;

    return p;
}

// Initialize position (X, Y) and vector (Rx, Ry).
void CAnime::Init(double radius)
{
    int i;
    double x, y, rx, ry, a, *u;

    this->radius = radius;

    X = new double[N];
    Y = new double[N];

    Rx = new double[N];
    Ry = new double[N];

    x = radius;
    y = 0.0;

    rx = 0.0;
    ry = -1.0;

    a = 360.0 / (double)N;

    for (i = 0; i < N; i++) {

        u = RotateZ(a, x, y);

        x = radius * u[0];
        y = radius * u[1];

        X[i] = x;
        Y[i] = y;

        u = RotateZ(a, rx, ry);

        Rx[i] = rx = u[0];
        Ry[i] = ry = u[1];
    }
}

void CAnime::Play(HWND hWnd)
{
    this->hWnd = hWnd;

    ResetEvent(stopevent);

    if (thread != NULL) CloseHandle(thread);
    thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Function, this, 0, &id);
}

void CAnime::Stop()
{
    SetEvent(stopevent);
}

void CAnime::DoAnimation(float t)
{
    CQueue que;
    NODE* Node;
    double vx, vy, x, y;

    // Do the projectile computation for every sprite.
    list.Reset();

    while (list.Read(&Node)) {

        vx = Node->vx;
        vy = Node->vy + g * t;

        x = Node->x + Node->vx * t;
        y = Node->y + Node->vy * t + 2.0f * g * t * t;

        Node->vx = vx;
        Node->vy = vy;

        Node->x = x;
        Node->y = y;

        if (Node->x < left ||  right < Node->x || Node->y < bottom || top < Node->y)
            que.Add(Node);
    }

    // Remove sprites that are off screen.
    while (!que.IsEmpty()) {
        que.Remove(&Node);
        list.Remove(Node);
    }

    //wchar_t str[100];
    //swprintf_s(str, 100, L"%10d\n", list.GetCount());
    //OutputDebugString(str);
}

// The function GenerateSprite can be put here in place of SendMessage(p->hWnd, WM_GENERATE_SPRITE, 0, 0)
// but there an access error so I do this instead.
DWORD WINAPI CAnime::Function(LPVOID lpParam)
{
    CAnime* p = (CAnime*)lpParam;
    DWORD result;

    //wchar_t str[100];
    //swprintf_s(str, 100, L"THE THREAD 0x%x HAS STARTED.\n", p->id);
    //OutputDebugString(str);

    while (true) {

        result = WaitForSingleObject(p->stopevent, 1);
        if (result == WAIT_OBJECT_0) break;

        PostMessage(p->hWnd, WM_GENERATE_SPRITE, 0, 0);
    }

    //swprintf_s(str, 100, L"THE THREAD 0x%x HAS EXITED.\n", p->id);
    //OutputDebugString(str);

    return 0;
}

void CAnime::SetBoundary(double left, double right, double bottom, double top)
{
    this->left = left;
    this->right = right;
    this->bottom = bottom;
    this->top = top;
}

void CAnime::SetSpinRate(double spin_rate)
{
    this->spin_rate = spin_rate;
}

double CAnime::GetSpinRate()
{
    return spin_rate;
}

void CAnime::GenerateSprite()
{
    double x, y, v, vx, vy, *u, a;
    int i, j;

    // Rotate position (X, Y) and vector (Rx, Ry).
    for (i = 0; i < N; i++) {

        u = RotateZ(spin_rate, X[i], Y[i]);

        X[i] = radius * u[0];
        Y[i] = radius * u[1];

        u = RotateZ(spin_rate, Rx[i], Ry[i]);

        Rx[i] = u[0];
        Ry[i] = u[1];
    }

    // For every position (X, Y), create more vectors around vector (Rx, Ry).
    for (i = 0; i < N; i++) {

        x = X[i];
        y = Y[i];

        for (j = 0; j < 8; j++) {

            v = 98.0f + 30.0f * ((double)rand() / (double)RAND_MAX);

            a = 30.0 * ((double)rand() / (double)RAND_MAX) - 15.0;
            u = RotateZ(a, Rx[i], Ry[i]);

            vx = v * u[0];
            vy = v * u[1];

            list.Add(x, y, vx, vy);
        }
    }
}

bool CAnime::IsRunning()
{
    return !list.IsEmpty();
}
