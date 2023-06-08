//
// Sovling system of linear equations gamit ang Gauss Jordan Method.
// 

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>

const int EQN_COUNT = 10;
const int EQN_LEN = 100;
const int VAR_LEN = 20;

bool IsNumber(char ch);
bool IsAlpha(char ch);

void GetNumber(double* number, char* str, int* i);
void GetAlpha(char* alpha, char* str, int* i);

void CollectVariable(char var[][VAR_LEN], char* str, int* k, int* n);

void ParseEquation(char str[EQN_COUNT][EQN_LEN], char var[EQN_COUNT][VAR_LEN], double a[EQN_COUNT][EQN_COUNT + 1], int count);
bool FindUnknown(double a[EQN_COUNT][EQN_COUNT + 1], int count);

int main()
{
    char ch, eqn[EQN_COUNT][EQN_LEN], var[EQN_COUNT][VAR_LEN];
    int i, n;
    double a[EQN_COUNT][EQN_COUNT + 1];
    bool nosoln;

    // iset ang title ng console
    SetConsoleTitle(L"System of Equations");

    printf("System of Equations\n\n");

    ch = 'Y';

    while (ch == 'Y' || ch == 'y')
    {
        // kunin ang mga equation
        printf("Enter number of equation: ");
        gets_s(eqn[0], EQN_LEN);

        n = atoi(eqn[0]);

        printf("\n");
        for (i = 0; i < n; i++)
        {
            printf("Enter equation %d :", i + 1);
            gets_s(eqn[i], EQN_LEN);
        }

        // hanapin ang solusyon
        ParseEquation(eqn, var, a, n);
        nosoln = FindUnknown(a, n);

        // i-display ang mga sagot
        printf("\n");
        if (nosoln)
            printf("No Solution\n");
        else
            for (i = 0; i < n; i++)
                printf("%s = %f\n", var[i], a[i][n]);

        // ulitin
        printf("\nContinue (y/n)?");

        do
        {
            ch = _getch();
        } while (!(ch == 'Y' || ch == 'y' || ch == 'N' || ch == 'n'));

        printf((ch == 'Y' || ch == 'y') ? "y\n\n" : "n\n");
    }
}

// alamin kung number
bool IsNumber(char ch)
{
    char str[12];
    char* p;

    strcpy_s(str, 12, "0123456789.");
    p = strchr(str, ch);

    return (p != NULL);
}

// alamin kung letter
bool IsAlpha(char ch)
{
    char str[53];
    char* p;

    strcpy_s(str, 53, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    p = strchr(str, ch);

    return (p != NULL);
}

// kunin ang numeric character sa string str simula sa index i
// ilagay ang nakuha sa variable number
void GetNumber(double* number, char* str, int* i)
{
    int k, n;
    char s[VAR_LEN];

    k = 0;

    n = strlen(str);

    while (IsNumber(str[*i]) && *i < n)
    {
        s[k] = str[*i];
        ++k;
        ++(*i);
    }

    --(*i);

    s[k] = '\0';

    *number = atof(s);
}

// kunin ang alpha character sa string str simula sa index i
// ilagay ang nakuha sa variable alpha
void GetAlpha(char* alpha, char* str, int* i)
{
    int k, n;

    k = 0;

    n = strlen(str);

    while (IsAlpha(str[*i]) && *i < n)
    {
        alpha[k] = str[*i];
        ++k;
        ++(*i);
    }

    --(*i);

    alpha[k] = '\0';
}


// kolektahin ang mga alpha character o variable na nakuha sa string str
// ilagay sa array var ang mga nakuha
void CollectVariable(char var[][VAR_LEN], char* str, int* k, int* n)
{
    int j;
    bool found;

    if (*n == 0)
    {
        // kung ito ay ang unang nakuha
        // ilagay 'to sa array var
        strcpy_s(var[*n], VAR_LEN, str);
        *k = *n;
        ++(*n);
    }
    else
    {
        // kung hindi, alamin kung may kapareho
        found = false;

        for (j = 0; j < (*n); j++)
        {
            found = (strcmp(str, var[j]) == 0);
            if (found)
            {
                *k = j;
                break;
            }
        }

        // kung walang kapareho,
        // ilagay 'to sa array var
        if (!found)
        {
            strcpy_s(var[*n], VAR_LEN, str);
            *k = *n;
            ++(*n);
        }
    }
}

// kunin ang mga variable at mga coefficient ng mga equation
void ParseEquation(char str[EQN_COUNT][EQN_LEN], char var[EQN_COUNT][VAR_LEN], double a[EQN_COUNT][EQN_COUNT + 1], int count)
{
    int i, j, k, m, n, c, l;
    double num, sign, toogle, numfound;
    char v[VAR_LEN];

    // i-zero ang two-dimensional array a
    n = count;
    m = n + 1;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            a[i][j] = 0.0;

    c = 0;

    // isa isahin ang equation str
    for (k = 0; k < n; k++)
    {
        i = 0;
        l = strlen(str[k]);
        numfound = false;
        num = sign = toogle = 1.0;

        // isa isahin ang character ng equation str
        while (i < l)
        {
            // kung ang character ay space, laktawan 'to
            if (str[k][i] == ' ') goto DITO;

            // kung ang character ay equal at may number na nakuha (numfound = true),
            // ilagay ang number sa array a, tapos i-set ang numfound sa false
            if (str[k][i] == '=')
            {
                if (numfound)
                {
                    a[k][n] = -toogle * sign * num;

                    numfound = false;
                }

                // ito ay nasa kabilang side ng equation, baliktarin ang sign
                toogle = -1.0;

                // iset sa plus sign
                num = sign = 1.0;
            }

            // kung ang character ay plus at may number na nakuha (numfound = true),
            // ilagay ang number sa array a, tapos i-set ang numfound sa false
            if (str[k][i] == '+')
            {
                if (numfound)
                {
                    a[k][n] = -toogle * sign * num;

                    numfound = false;
                }

                // iset sa plus sign
                num = sign = 1.0;
            }

            // kung ang character ay minus at may number na nakuha (numfound = true),
            // ilagay ang number sa array a, tapos i-set ang numfound sa false
            if (str[k][i] == '-')
            {
                if (numfound)
                {
                    a[k][n] = -toogle * sign * num;

                    numfound = false;
                }

                // iset sa minus sign
                sign = -1.0;
                num = 1.0;
            }

            // kung ang character ay number, kunin ang number
            // i-set sa true ang numfound para malaman na may number na nakuha
            if (IsNumber(str[k][i]))
            {
                GetNumber(&num, str[k], &i);

                numfound = true;
            }

            // kung ang character ay letter, ito ay variable, kunin 'to at
            // ilagay sa array var at ang number sa array a
            // tapos i-set ang numfound sa false
            if (IsAlpha(str[k][i]))
            {
                GetAlpha(v, str[k], &i);
                CollectVariable(var, v, &j, &c);

                a[k][j] = toogle * sign * num;

                numfound = false;
                num = 1.0;
            }

        DITO:

            ++i;
        }

        // kung may number pang nakuha (numfound = true), ilagay to sa array a
        if (numfound)
            a[k][n] = -toogle * sign * num;
    }
}

// find unknown by gauss jordan method
bool FindUnknown(double a[EQN_COUNT][EQN_COUNT + 1], int count)
{
    int i, j, k, m, n;
    bool nosoln;

    nosoln = false;
    n = count;
    m = n + 1;

    for (k = 0; k < n; k++)
    {
        if (a[k][k] == 0.0)
        {
            nosoln = true;
            break;
        }

        for (j = m - 1; j > (k - 1); j--)
            a[k][j] = a[k][j] / a[k][k];

        for (i = 0; i < n; i++)
            if (i != k)
                for (j = 0; j < m; j++)
                    if (j != k)
                        a[i][j] = a[i][j] - a[i][k] * a[k][j];

        j = k;
        for (i = 0; i < n; i++)
            if (i != k)
                a[i][j] = a[i][j] - a[i][k] * a[k][j];
    }

    return nosoln;
}
