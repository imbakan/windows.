
// Gaussian elimination
// https://en.wikipedia.org/wiki/Gaussian_elimination

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include "queue.h"

void Allocate(double*** e, int n, int m);
void Release(double*** e, int n);

bool IsNumber(char ch);
bool IsAllNumber(char* str);
void GetString(char* str1, char* str2, int i1, int i2);
void GetNumbers(char* str, CQueue* que);

void RowReduction(double** e, int n, int m);

int main()
{
    CQueue que;
    int i, j, m, n;
    double** e;
    char str[100];

    printf("\n+-------------------------------------------+\n");
    printf("|            Gauss-Jordan Method            |\n");
    printf("+-------------------------------------------+\n\n");

    printf("Enter number of equation: ");
    gets_s(str, 100);

    if (!IsAllNumber(str)) {
        printf("Not a number.\n");
        return 0;
    }

    GetNumbers(str, &que);

    if (que.GetCount() != 1) {
        printf("Must be one number only.\n");
        return 0;
    }

    que.Remove(str, 100);
    n = atoi(str);
    m = n + 1;

    Allocate(&e, n, m);

    for (i = 0; i < n; i++) {

        printf("Enter coefficient of equation %d :", i + 1);
        gets_s(str, 100);

        if (!IsAllNumber(str)) {
            printf("Not all numbers.\n");
            Release(&e, n);
            return 0;
        }

        GetNumbers(str, &que);

        if (que.GetCount() != (n + 1)) {
            printf("Must be %d numbers.\n", n + 1);
            Release(&e, n);
            return 0;
        }

        j = 0;

        while (!que.IsEmpty()) {
            que.Remove(str, 100);
            e[i][j++] = atof(str);
        }
    }

    // Input
    printf("\nInput :\n\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++)
            printf("%10.2f", e[i][j]);
        printf("\n");
    }

    // Process
    RowReduction(e, n, m);

    // Output
    printf("\nOutput :\n\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++)
            printf("%10.2f", e[i][j]);
        printf("\n");
    }

    Release(&e, n);

    return 0;
}

void Allocate(double*** e, int n, int m)
{
    int i;

    *e = new double* [n];

    for (i = 0; i < n; i++)
        (*e)[i] = new double[m];
}

void Release(double*** e, int n)
{
    int i;

    if (*e == NULL) return;

    for (i = 0; i < n; i++)
        delete[](*e)[i];

    delete[] * e;

    *e = NULL;
}

bool IsNumber(char ch)
{
    char str[13];
    char* p;

    strcpy_s(str, 13, "0123456789.-");
    p = strchr(str, ch);

    return (p != NULL);
}

bool IsAllNumber(char* str)
{
    int i, n;

    n = (int)strlen(str);

    for (i = 0; i < n; i++) {

        if (isblank(str[i])) continue;

        if (!IsNumber(str[i]))
            return false;
    }

    return true;
}

void GetString(char* str1, char* str2, int i1, int i2)
{
    int i, k;

    k = 0;

    for (i = i1; i < i2; i++)
        str1[k++] = str2[i];

    str1[k] = '\0';
}

void GetNumbers(char* str, CQueue* que)
{
    int i, j1, j2, k, n;
    char str1[100];

    que->Clear();

    n = (int)strlen(str);

    i = k = 0;

    while (i < n) {

        if (IsNumber(str[i])) {

            j1 = i;

            for (j2 = i; j2 < n; j2++)
                if (!IsNumber(str[j2]))
                    break;

            GetString(str1, str, j1, j2);

            que->Add(str1);

            i = j2;
        }
        else {

            ++i;
        }
    }
}

void RowReduction(double** e, int n, int m)
{
    int i, j, k;

    for (k = 0; k < n; k++) {

        // prevent divisible by zero
        if (e[k][k] == 0.0) break;

        // elimination
        for (i = 0; i < n; i++) {

            if (i == k) continue;

            for (j = k; j < m; j++) {

                if (j == k) continue;

                e[i][j] = e[i][j] - (e[i][k] * e[k][j]) / e[k][k];
            }
        }

        // do column
        for (j = (m - 1); j > (k - 1); j--)
            e[k][j] /= e[k][k];

        // do row
        for (i = 0; i < n; i++)
            if (i != k)
                e[i][k] = 0.0;
    }
}
