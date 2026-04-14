
// Durand–Kerner method
// https://en.wikipedia.org/wiki/Durand%E2%80%93Kerner_method

#include <stdio.h>
#include <math.h>
#include "complexnumber.h"

CComplexNumber absolute(CComplexNumber c);

CComplexNumber Function(CComplexNumber& x, double* c, int n);
CComplexNumber Polynomial(int i, CComplexNumber* x, int n, double* c);

int main()
{
    CComplexNumber x1[10], x2[10], fx, a, b;
    int i, n;
    double c[10], d;
    bool loop, precise;

    // data entry part

    printf("Enter number of degree: ");
    scanf_s("%d", &n);

    for (i = 0; i < (n + 1); i++) {

        printf("Enter coefficient of x^%d : ", i);
        scanf_s("%lf", &c[i]);

    }

    printf("\n");

    // check if coefficient of x^n equal one

    if (c[n] != 1.0) {

        d = c[n];

        for (i = 0; i < (n + 1); i++)
            c[i] /= d;
    }

    // initialize variable x

    a = CComplexNumber(1.0, 0.0);
    b = CComplexNumber(0.4, 0.9);

    for (i = 0; i < n; i++) {

        x1[i] = a;

        a *= b;
    }

    // iteration

    loop = true;

    while (loop) {

        // compute

        for (i = 0; i < n; i++)
            x2[i] = Polynomial(i, x1, n, c);

        // exit loop when required precision is met

        precise = true;

        for (i = 0; i < n; i++)
            precise = precise && (absolute(x2[i] - x1[i]) < 1.0e-10);

        if (precise) loop = false;

        // set new value for iteration

        for (i = 0; i < n; i++)
            x1[i] = x2[i];
    }

    // value of x

    for (i = 0; i < n; i++) {

        fx = Function(x1[i], c, n);

        printf("[%d]\n", i + 1);
        printf("x    = %80.70f%80.70f\n", x1[i].x, x1[i].y);
        printf("f(x) = %80.70f%80.70f\n\n", fx.x, fx.y);
    }

}

CComplexNumber absolute(CComplexNumber c)
{
    return CComplexNumber(fabs(c.x), fabs(c.y));
}

// f(x)

CComplexNumber Function(CComplexNumber& x, double* c, int n)
{
    CComplexNumber power, sum, term;
    int i;

    power = CComplexNumber(1.0, 0.0);
    sum = CComplexNumber(0.0, 0.0);

    for (i = 0; i < (n + 1); i++) {

        term = power * CComplexNumber(c[i], 0.0);  // 7x^8

        sum += term;          // x^3 - 3x^2 + 3x - 5
        power *= x;
    }

    return sum;
}

// fixed-point iteration

CComplexNumber Polynomial(int k, CComplexNumber* x, int n, double* c)
{
    CComplexNumber polyx, nume, deno;
    int i;

    deno = CComplexNumber(1.0, 0.0);

    for (i = 0; i < n; i++) {

        if (i == k) continue;

        deno *= (x[k] - x[i]);
    }

    nume = Function(x[k], c, n);

    polyx = x[k] - nume / deno;

    return polyx;
}
