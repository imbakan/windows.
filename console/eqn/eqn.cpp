
// System of Equation

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "queue.h"

void PrintOutput(double** a, int m, int n);

void CapString(char* str);
int FindIndex(char** string1, int n, char* string2);

void GetNumber(char* str, size_t i1, size_t* i2);
void GetAlpha(char* str, size_t i1, size_t* i2);
void GetString(char* str1, size_t n, char* str2, size_t i1, size_t i2);

void Parse(char* str, CQueue* que);
bool CheckForArrangement(CQueue* que);
bool CheckForEqual(CQueue* que);
bool CheckForVariable(CQueue* que);
bool CheckForConstant(CQueue* que);

void CollectVariable(CQueue* que1, int n, CQueue* que2);
void CreateVariable(char*** var, CQueue* que);
void CreateMatrix(CQueue* que, char** var, int* m, int n, double*** a);
bool GaussJordanMethod(double** a, int m, int n);

int main()
{
    CQueue* que1;
    CQueue que2;
    int i, m, n;
    double** a;
    char str[100];
    char** var;
    bool pass;

    printf("\n");

    printf("Enter number of equation: ");
    gets_s(str, 100);

    n = atoi(str);
    que1 = new CQueue[n];

    pass = true;

    for (i = 0; i < n; i++) {

        printf("Enter equation %d : ", i + 1);
        gets_s(str, 100);

        Parse(str, &que1[i]);

        if (CheckForArrangement(&que1[i])) {
            pass = false;
            printf("invalid equation\n");
            break;
        }

        if (CheckForEqual(&que1[i])) {
            pass = false;
            printf("more than one equal in an equation\n");
            break;
        }

        if (CheckForVariable(&que1[i])) {
            pass = false;
            printf("identical variable found in an equation\n");
            break;
        }

        if (CheckForConstant(&que1[i])) {
            pass = false;
            printf("more than one constant in an equation\n");
            break;
        }
    }

    if (pass) {

        CollectVariable(que1, n, &que2);

        m = que2.GetCount();

        if (m != n) {
            pass = false;
            printf("number of equation are not equal number of variable\n");
        }
    }

    // dito pass na sa error checking

    if (pass) {

        CreateVariable(&var, &que2);
        CreateMatrix(que1, var, &m, n, &a);

        //PrintOutput(a, m, n);
        //printf("\n");

        if (GaussJordanMethod(a, m, n)) {
            printf("\n");
            for (i = 0; i < n; i++)
                printf("%5s = %20.15f\n", var[i], a[i][n]);
            printf("\n");
        }
        else {
            printf("no solution\n");
        }

        //PrintOutput(a, m, n);

        // release variable
        for (i = 0; i < n; i++)
            delete[] var[i];

        delete[] var;

        // release coefficient
        for (i = 0; i < n; i++)
            delete[] a[i];

        delete[] a;
    }

    delete[] que1;

    return 0;
}

void PrintOutput(double** a, int m, int n)
{
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++)
            printf("%20.15f", a[i][j]);
        printf("\n");
    }
}

// gawing upper case ang string str
void CapString(char* str)
{
    size_t i, n;

    n = strlen(str);

    for (i = 0; i < n; i++)
        str[i] = toupper(str[i]);
}

// ang string1 ay array of string
// hanapin ang string2 sa array of string
// ireturn ang index of array
int FindIndex(char** string1, int n, char* string2)
{
    int i, k;
    char str1[100], str2[100];

    k = -1;

    strcpy_s(str2, 100, string2);
    CapString(str2);

    for (i = 0; i < n; i++) {

        strcpy_s(str1, 100, string1[i]);
        CapString(str1);

        if (strcmp(str1, str2) == 0) {
            k = i;
            break;
        }
    }

    return k;
}

// kunin ang index i1 at i2 ng string str
// ang index i1 ay ang simula ng numeric character
// ang index i2 ay index ng huling numeric character plus one
// 
// abcd12345efg67890
//     ^    ^
//     |    |
//     i1   i2
//
void GetNumber(char* str, size_t i1, size_t* i2)
{
    size_t i, n;
    char number[12];
    bool found;

    strcpy_s(number, 12, "1234567890.");

    n = strlen(str);
    found = false;

    for (i = i1; i < n; i++) {

        if (strchr(number, str[i]) == NULL) {
            *i2 = i;
            found = true;
            break;
        }
    }

    if (found) return;

    *i2 = n;
}

// kunin ang index i1 at i2 ng string str
// ang index i1 ay ang simula ng alpha character
// ang index i2 ay index ng huling alpha character plus one
// 
// 1234abcde56789
//     ^    ^
//     |    |
//     i1   i2
//
void GetAlpha(char* str, size_t i1, size_t* i2)
{
    size_t i, n;
    char alpha[53];
    bool found;

    strcpy_s(alpha, 53, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

    n = strlen(str);
    found = false;

    for (i = i1; i < n; i++) {

        if (strchr(alpha, str[i]) == NULL) {
            *i2 = i;
            found = true;
            break;
        }
    }

    if (found) return;

    *i2 = n;
}

// kunin ang string str1 sa string str2 sa pagitan ng index i1 at i2
void GetString(char* str1, size_t n, char* str2, size_t i1, size_t i2)
{
    size_t i, k;

    k = 0;

    for (i = i1; i < i2; i++)
        str1[k++] = str2[i];

    str1[k] = '\0';
}

// ilagay sa que ang mga coefficient ng equation str
//
// equation str           2x - y = 34
//
//                        +---+    +---+    +---+    +---+    +---+    +----+
// que                    | 2 |--->| x |--->| - |--->| y |--->| = |--->| 34 |
//                        +---+    +---+    +---+    +---+    +---+    +----+
//
void Parse(char* str, CQueue* que)
{
    size_t i, i1, i2, n;
    char number[12], alpha[53], str1[100];

    strcpy_s(number, 12, "1234567890.");
    strcpy_s(alpha, 53, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

    n = strlen(str);
    i = 0;

    while (i < n) {

        //printf("%5lld\n", i);

        if (strchr(number, str[i]) != NULL) {

            i1 = i;

            GetNumber(str, i1, &i2);
            GetString(str1, 100, str, i1, i2);

            //printf("%10lld%10lld %s\n", i1, i2, str1);
            //printf("%20s\n", str1);

            que->Add(str1, NUMBER);

            i = i2 - 1;

        }
        else if (strchr(alpha, str[i]) != NULL) {

            i1 = i;

            GetAlpha(str, i1, &i2);
            GetString(str1, 100, str, i1, i2);

            //printf("%10lld%10lld %s\n", i1, i2, str1);
            //printf("%20s\n", str1);

            que->Add(str1, ALPHA);

            i = i2 - 1;

        }
        else if (str[i] == '+' || str[i] == '-') {

            i1 = i;
            i2 = i + 1;

            GetString(str1, 100, str, i1, i2);

            //printf("%20s\n", str1);

            que->Add(str1, SIGN);

            i = i2 - 1;
        }
        else if (str[i] == '=') {

            i1 = i;
            i2 = i + 1;

            GetString(str1, 100, str, i1, i2);

            //printf("%20s\n", str1);

            que->Add(str1, EQUAL);

            i = i2 - 1;
        }
        else if (str[i] == ' ') {
            // ignore
        }
        else {

            printf("invalid character\n");
            break;
        }

        ++i;
    }
}

// 2x - - 3y + z = 5     invalid equation
bool CheckForArrangement(CQueue* que)
{
    NODE* Node;
    bool validN, validA, validS, validE;

    validN = true;      // 12
    validA = true;      // x
    validS = true;      // -
    validE = false;     // =

    que->Reset();

    while (que->Read(&Node)) {

        switch (Node->token) {

        case NUMBER:

            if (!validN) return true;

            validN = false;     // 12 34
            validA = true;      // 12 x
            validS = true;      // 12 +
            validE = true;      // 12 =

            break;

        case ALPHA:

            if (!validA) return true;

            validN = false;     // x 34
            validA = false;     // x x
            validS = true;      // x +
            validE = true;      // x =

            break;

        case SIGN:

            if (!validS) return true;

            validN = true;      // - 34
            validA = true;      // - x
            validS = false;     // - +
            validE = false;     // - =

            break;

        case EQUAL:
 
            if (!validE) return true;

            validN = true;      // = 34
            validA = true;      // = x
            validS = true;      // = +
            validE = false;     // = =

            break;

        }
    }

    return false;
}

// 2x = 3y + z = 5     invalid equation
bool CheckForEqual(CQueue* que)
{
    NODE* Node;
    int count;

    count = 0;

    que->Reset();

    while (que->Read(&Node)) {

        if (Node->token == EQUAL)
            ++count;
    }

    return (count > 1);
}

// 2x - 3y + x = 5     invalid equation
bool CheckForVariable(CQueue* que)
{
    CQueue que1;
    NODE* Node;
    bool result;

    result = false;

    que->Reset();

    while (que->Read(&Node)) {

        if (Node->token == ALPHA) {
            if (que1.Find(Node->str)) {
                result = true;
                break;
            } 
            else {
                que1.Add(Node->str, Node->token);
            }
        }
    }

    return result;
}

// 2 - 3y + z = 5     invalid equation
bool CheckForConstant(CQueue* que)
{
    NODE* Node1;
    NODE* Node2;
    int count;

    count = 0;

    que->Reset();

    while (que->Read(&Node1)) {

        if (Node1->token == NUMBER) {

            Node2 = Node1->Next;

            if (Node2 != NULL) {

                if (Node2->token == ALPHA) continue;

                ++count;

            }
            else {

                ++count;

            }
        }
    }

    return (count > 1);
}

// ilagay sa que2 ang mga variable na nasa mga equation que1
// 
//                        +---+    +---+    +---+    +---+    +---+    +----+
// que1[0]                | 2 |--->| x |--->| - |--->| y |--->| = |--->| 34 |
//                        +---+    +---+    +---+    +---+    +---+    +----+
//
//                        +---+    +---+    +---+    +---+    +---+
// que1[1]                | x |--->| + |--->| y |--->| = |--->| 1 |
//                        +---+    +---+    +---+    +---+    +---+
// 
//                        +---+    +---+
// que2                   | x |--->| y |
//                        +---+    +---+
//
void CollectVariable(CQueue* que1, int n, CQueue* que2)
{
    NODE* Node;
    int i;

    for (i = 0; i < n; i++) {

        que1[i].Reset();

        while (que1[i].Read(&Node)) {

            if (Node->token == ALPHA) {

                if (!que2->Find(Node->str)) {
                    que2->Add(Node->str, Node->token);
                }
            }
        }
    }
}

// ilagay sa array of string var ang mga variable na nasa que
// 
//                        +---+    +---+
// que                    | x |--->| y |
//                        +---+    +---+
// 
// var[0]                 x
// var[1]                 y
//
void CreateVariable(char*** var, CQueue* que)
{
    int i, n;
    size_t size;
    char str[100];

    n = que->GetCount();
    *var = new char* [n];

    i = 0;

    while (!que->IsEmpty()) {

        que->Remove(str, 100);

        size = strlen(str) + 1;
        (*var)[i] = new char[size];

        strcpy_s((*var)[i], size, str);

        ++i;
    }
}

// gumawa ng matrix a galing sa mga equation que
// 
//                        +---+    +---+    +---+    +---+    +---+    +----+
// que[0]                 | 2 |--->| x |--->| - |--->| y |--->| = |--->| 34 |
//                        +---+    +---+    +---+    +---+    +---+    +----+
//
//                        +---+    +---+    +---+    +---+    +---+
// que[1]                 | x |--->| + |--->| y |--->| = |--->| 1 |
//                        +---+    +---+    +---+    +---+    +---+
// 
// a                      2   -1   34
//                        1    1    1
//
void CreateMatrix(CQueue* que, char** var, int* m, int n, double*** a)
{
    NODE* Node;
    int i, j;
    double num, sign, toggle;
    bool numfound;

    // allocate
    *a = new double* [n];
    *m = n + 1;

    for (i=0; i<n; i++)
        (*a)[i] = new double[*m];

    // initialize
    for (i = 0; i < n; i++)
        for (j = 0; j < *m; j++)
            (*a)[i][j] = 0.0;

    // gawa matrix
    for (i = 0; i < n; i++) {

        // initialize ang mga ito
        numfound = false;
        num = sign = toggle = 1.0;

        que[i].Reset();

        while (que[i].Read(&Node)) {

            switch (Node->token) {

            case NUMBER:

                // kung number kunin lang ang number

                num = atof(Node->str);
                numfound = true;

                break;

            case ALPHA:

                // kung variable ang sumunod ilagay ang number na nakuha, kung meron, sa matrix sa tamang location

                j = FindIndex(var, n, Node->str);

                if (numfound)
                    (*a)[i][j] = toggle * sign * num;
                else
                    (*a)[i][j] = toggle * sign;

                numfound = false;
                num = sign = 1.0;

                break;

            case SIGN:

                // kung sign ang sumunod ilagay ang number na nakuha, kung meron, sa matrix sa tamang location
                // ito ay ang constant sa equation

                if (numfound)
                    (*a)[i][*m - 1] = -toggle * sign * num;

                numfound = false;
                num = 1.0;

                // kung sign kunin ang sign, positive o negative, ang zero ay para lang sa error checking

                if (Node->str[0] == '+')
                    sign = 1.0;
                else if (Node->str[0] == '-')
                    sign = -1.0;
                else
                    sign = 0.0;

                break;

            case EQUAL:

                // kung equal ang sumunod ilagay ang number na nakuha, kung meron, sa matrix sa tamang location
                // ito ay ang constant sa equation

                if (numfound)
                    (*a)[i][*m - 1] = -toggle * sign * num;

                // kung equal, ito ay nasa kabila ng equation, toggle lang ito

                numfound = false;
                num = sign = 1.0;
                toggle = -1.0;

                break;

            }
        }

        // kung huli na 'to, ilagay ang number na nakuha, kung meron, sa matrix sa tamang location
        // ito ay ang constant sa equation

        if (numfound)
            (*a)[i][*m - 1] = -toggle * sign * num;

    }
}

// Gaussian elimination
// https://en.wikipedia.org/wiki/Gaussian_elimination
bool GaussJordanMethod(double** a, int m, int n)
{
    int i, j, k;
    bool result;

    result = true;

    for (k = 0; k < n; k++) {

        // prevent divisible by zero
        if (a[k][k] == 0.0) {
            result = false;
            break;
        }

        // do gaussian elimination
        for (i = 0; i < n; i++) {

            if (i == k) continue;

            for (j = k; j < m; j++) {

                if (j == k) continue;

                a[i][j] = a[i][j] - (a[i][k] * a[k][j]) / a[k][k];
            }
        }

        // do column
        for (j = (m - 1); j > (k - 1); j--)
            a[k][j] /= a[k][k];

        // do row
        for (i = 0; i < n; i++)
            if (i != k)
                a[i][k] = 0.0;
    }

    return result;
}
