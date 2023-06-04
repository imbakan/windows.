//
// Pag-compute ng trigonometric function gamit ang Taylor's Series.
// 

#define _USE_MATH_DEFINES // para M_PI

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>

#define ULC 0xc9
#define URC 0xbb
#define LLC 0xc8
#define LRC 0xbc

#define VER1 0xf5

#define HOR2 0xcd
#define VER2 0xba

#define LFD 0xcc
#define RTD 0xb9

#define CRS 0xd8

#define UPD 0xd1
#define LOD 0xcf

#define INF 0xec

double factorial(long long n);
double cosine(double x);
double sine(double x);

int main()
{
    int i, ch, a;
    double x, c, s, t;

    // iset ang title ng console
    SetConsoleTitle(L"Trigonometric Table");

    printf("\n%54s\n\n", "TRIGONOMETRIC TABLE");

    // upper border
    printf("%10c", ULC);
    for (i = 0; i < 7; i++)
        printf("%c", HOR2);
    printf("%c", UPD);
    for (i = 0; i < 22; i++)
        printf("%c", HOR2);
    printf("%c", UPD);
    for (i = 0; i < 22; i++)
        printf("%c", HOR2);
    printf("%c", UPD);
    for (i = 0; i < 22; i++)
        printf("%c", HOR2);
    printf("%c\n", URC);

    // header
    printf("%10c", VER2);
    printf("%3c%-2s", ' ', "x");
    printf("%3c", VER1);
    printf("%3c%-17s", ' ', "Cos(x)");
    printf("%3c", VER1);
    printf("%3c%-17s", ' ', "Sin(x)");
    printf("%3c", VER1);
    printf("%3c%-17s", ' ', "Tan(x)");
    printf("%3c\n", VER2);

    // divider
    printf("%10c", LFD);
    for (i = 0; i < 7; i++)
        printf("%c", HOR2);
    printf("%c", CRS);
    for (i = 0; i < 22; i++)
        printf("%c", HOR2);
    printf("%c", CRS);
    for (i = 0; i < 22; i++)
        printf("%c", HOR2);
    printf("%c", CRS);
    for (i = 0; i < 22; i++)
        printf("%c", HOR2);
    printf("%c\n", RTD);

    // cell
    for (a = 0; a < 360; a++)
    {
        x = ((double)a / 180.0) * M_PI;

        c = cosine(x);
        s = sine(x);
        t = s / c;

        printf("%10c", VER2);
        printf("%5d", a);
        printf("%3c", VER1);
        printf("%20.15f", c);
        printf("%3c", VER1);
        printf("%20.15f", s);
        printf("%3c", VER1);

        if (fabs(c) <= 0.0000001)
            printf("   %-17c", INF);
        else
            printf("%20.15f", t);

        printf("%3c\n", VER2);
    }

    // lower border
    printf("%10c", LLC);
    for (i = 0; i < 7; i++)
        printf("%c", HOR2);
    printf("%c", LOD);
    for (i = 0; i < 22; i++)
        printf("%c", HOR2);
    printf("%c", LOD);
    for (i = 0; i < 22; i++)
        printf("%c", HOR2);
    printf("%c", LOD);
    for (i = 0; i < 22; i++)
        printf("%c", HOR2);
    printf("%c\n", LRC);

    // para de mag-exit agad.
    printf("\n          Press any key to exit.\n");
    ch = _getch();
}

// n! = n(n-1)(n-2)(x-3) ... (n-n+1)

double factorial(long long n)
{
    long long i;
    double val;

    val = 1.0;

    for (i = 1; i < (n + 1); i++)
        val *= (double)i;

    return val;
}

//           oo
//          -----
//          \         (-1)^n                 x^2     x^4     x^6
// cos(x) =  >     ----------- x^(2n) = 1 + ----- + ----- + ----- + ...
//          /         (2n)!                   2!      4!      6!
//          -----
//          n = 0

double cosine(double x)
{
    long long n, y, s;
    double nume, dena, val1, val2;

    n = 0LL;
    s = 1LL;
    val2 = 1.0;

    do
    {
        val1 = val2;

        s *= -1;
        ++n;
        y = 2LL * n;
        nume = pow(x, (double)y);
        dena = factorial(y);

        val2 += ((nume / dena) * s);

    } while (fabs(val2 - val1) > 1.0e-20);

    return val2;
}

//           oo
//          -----
//          \         (-1)^n                      x^3     x^5     x^7
// sin(x) =  >     ----------- x^(2n + 1) = x + ----- + ----- + ----- + ...
//          /       (2n + 1)!                      3!      5!      7!
//          -----
//          n = 0

double sine(double x)
{
    long long n, y, s;
    double nume, dena, val1, val2;

    n = 0LL;
    s = 1LL;
    val2 = x;

    do
    {
        val1 = val2;

        s *= -1;
        ++n;
        y = 2LL * n + 1;
        nume = pow(x, (double)y);
        dena = factorial(y);

        val2 += ((nume / dena) * s);

    } while (fabs(val2 - val1) > 1.0e-20);

    return val2;
}
