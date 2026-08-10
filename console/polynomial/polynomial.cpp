
// Polynomials Algebra

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "queue1.h"
#include "queue2.h"
#include "complexnumber.h"

void PrintOutput(CQueue1* que);
void PrintOutput(CQueue2* que);
void PrintOutput(double* a, int n);
void PrintRoot(CComplexNumber* x, double* a, int n);
void PrintRoot2(char* str, CComplexNumber* x, double* a, int n);

void GetNumber(char* str, size_t i1, size_t* i2);
void GetAlpha(char* str, size_t i1, size_t* i2);
void GetString(char* str1, size_t n, char* str2, size_t i1, size_t i2);

void Parse(char* str, CQueue1* que);
bool CheckForArrangement(CQueue1* que);
bool CheckForVariable(CQueue1* que, char** str);

bool GetAllTerms(CQueue1* que1, CQueue2* que2);

CComplexNumber Function(CComplexNumber& x, double* c, int n);
CComplexNumber Polynomial(int i, CComplexNumber* x, int n, double* c);
CComplexNumber absolute(CComplexNumber c);

void FindTheRoot(CComplexNumber* x1, double* c, int n);

int main()
{
    CComplexNumber* x;
    CQueue1 que1;
    CQueue2 que2;
    char str[200];
    char* var;
    bool pass;
    int i, n, index;
    double* a;
    double value;

    SetConsoleTitle(L"Polynomial");

    printf("\n");
    printf("Polynomials Algebra\n");
    printf("\n");
    printf("Type 'P' or 'P' to enter polynomial.\n");
    printf("Type 'Q' or 'q' to exit.\n");
    printf("\n");

    var = NULL;

    while (true) {

        gets_s(str, 200);

        if (strcmp(str, "Q") == 0 || strcmp(str, "q") == 0) break;

        if (strcmp(str, "P") == 0 || strcmp(str, "p") == 0) {

            printf("Enter polynomial : ");
            gets_s(str, 200);

            Parse(str, &que1);

            pass = true;

            //PrintOutput(&que1);

            if (CheckForArrangement(&que1)) {
                pass = false;
                printf("Invalid polynomial arrangement.\n\n");
            }
            else if (CheckForVariable(&que1, &var)) {
                pass = false;
                printf("More than one variable in the polynomial.\n\n");
            }
            else if (GetAllTerms(&que1, &que2)) {
                pass = false;
                printf("Identical exponent in the polynomial.\n\n");
            }

            if (pass) {

                //printf("\n");
                //PrintOutput(&que2);

                n = que2.GetMax();

                //printf("\nMaximum Degree of a polynomial : %d\n", n);

                a = new double[n + 1];

                for (i = 0; i < n + 1; i++)
                    a[i] = 0.0;

                while (!que2.IsEmpty()) {

                    que2.Remove(&index, &value);

                    a[index] = value;
                }

                //printf("\n");
                //PrintOutput(a, n + 1);

                x = new CComplexNumber[n];

                FindTheRoot(x, a, n);

                printf("\n");
                PrintRoot2(var, x, a ,n);

                delete[] x;
                delete[] a;
            }

        }
    }

    if (var != NULL)
        delete[] var;
}

void PrintOutput(CQueue1* que)
{
    NODE1* Node;

    que->Reset();

    while (que->Read(&Node))
        printf("%s ", Node->str);
}

void PrintOutput(CQueue2* que)
{
    NODE2* Node;

    que->Reset();

    while (que->Read(&Node))
        printf("%10d%10.2f\n", Node->index, Node->value);
}

void PrintOutput(double* a, int n)
{
    int i;

    for (i = n - 1; i > -1; i--) {
        printf("%10d%10.2f\n", i, a[i]);
    }
}

void PrintRoot(CComplexNumber* x, double* c, int n)
{
    CComplexNumber fx;
    int i;

    for (i = 0; i < n; i++) {

        fx = Function(x[i], c, n);

        printf("[Root %d]\n", i + 1);
        printf("x    = %80.70f%80.70f\n", x[i].x, x[i].y);
        printf("f(x) = %80.70f%80.70f\n\n", fx.x, fx.y);
    }
}

void PrintRoot2(char* str, CComplexNumber* x, double* c, int n)
{
    CComplexNumber fx;
    int i;

    for (i = 0; i < n; i++) {

        fx = Function(x[i], c, n);

        printf("[Root %d]\n", i + 1);
        printf("%s    = %20.7f%20.7fi\n", str, x[i].x, x[i].y);
        printf("f(%s) = %20.7f%20.7fi\n\n", str, fx.x, fx.y);
    }
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
// polynomial  x^4 - 2x^3 - 13x^2 + 14x + 24
//
//             +---+    +---+    +---+    +---+    +---+    +---+    +---+    +---+
// que         | x |--->| ^ |--->| 4 |--->| - |--->| 2 |--->| x |--->| ^ |--->| 3 |
//             +---+    +---+    +---+    +---+    +---+    +---+    +---+    +---+
//
void Parse(char* str, CQueue1* que)
{
    size_t i, i1, i2, n;
    char number[12], alpha[53], str1[100];

    strcpy_s(number, 12, "1234567890.");
    strcpy_s(alpha, 53, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

    que->Clear();

    n = strlen(str);
    i = 0;

    while (i < n) {

        if (strchr(number, str[i]) != NULL) {

            i1 = i;

            GetNumber(str, i1, &i2);
            GetString(str1, 100, str, i1, i2);

            que->Add(str1);

            i = i2 - 1;

        }
        else if (strchr(alpha, str[i]) != NULL) {

            i1 = i;

            GetAlpha(str, i1, &i2);
            GetString(str1, 100, str, i1, i2);

            que->Add(str1);

            i = i2 - 1;

        }
        else if (str[i] == '+' || str[i] == '-' || str[i] == '^') {

            i1 = i;
            i2 = i + 1;

            GetString(str1, 100, str, i1, i2);

            que->Add(str1);

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

// x^4 - 2x^3 - 13x^2 + 14x + 24
bool CheckForArrangement(CQueue1* que)
{
    NODE1* Node;
    char number[12], alpha[53];
    bool NextTermNumber, NextTermAlpha, NextTermSign, NextTermExp;

    strcpy_s(number, 12, "1234567890.");
    strcpy_s(alpha, 53, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

    NextTermNumber = true;   // 12
    NextTermAlpha = true;    // x
    NextTermSign = true;     // -
    NextTermExp = false;     // ^

    que->Reset();

    while (que->Read(&Node)) {

        if (strchr(number, Node->str[0]) != NULL) {

            if (!NextTermNumber) return true;

            NextTermNumber = false;  // 12 34
            NextTermAlpha = true;    // 12 x
            NextTermSign = true;     // 12 +
            NextTermExp = false;     // 12 ^

        }
        else if (strchr(alpha, Node->str[0]) != NULL) {

            if (!NextTermAlpha) return true;

            NextTermNumber = false;  // x 34
            NextTermAlpha = false;   // x x
            NextTermSign = true;     // x +
            NextTermExp = true;      // x ^

        }
        else if (Node->str[0] == '+' || Node->str[0] == '-') {

            if (!NextTermSign) return true;

            NextTermNumber = true;   // - 34
            NextTermAlpha = true;    // - x
            NextTermSign = false;    // - +
            NextTermExp = false;     // - ^

        }
        else if (Node->str[0] == '^') {

            if (!NextTermExp) return true;

            NextTermNumber = true;    // ^ 34
            NextTermAlpha = false;    // ^ x
            NextTermSign = false;     // ^ +
            NextTermExp = false;      // ^ ^

        }
    }

    return false;
}

// x^4 - 2x^3 - 13x^2 + 14y + 24
bool CheckForVariable(CQueue1* que, char** str)
{
    CQueue1 que1;
    NODE1* Node;
    char alpha[53];
    int count;
    size_t n;

    strcpy_s(alpha, 53, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

    que->Reset();

    while (que->Read(&Node)) {

        if (strchr(alpha, Node->str[0]) != NULL) {
            if (!que1.Find(Node->str)) {
                que1.Add(Node->str);
            }
        }
    }

    count = que1.GetCount();

    if (count == 1) {

        que->Reset();

        while (que->Read(&Node)) {

            if (strchr(alpha, Node->str[0]) != NULL) {

                if (*str != NULL)
                    delete[] * str;

                n = strlen(Node->str) + 1;
                *str = new char[n];
                strcpy_s(*str, n, Node->str);

                break;
            }
        }
    }

    return count > 1;
}

bool GetAllTerms(CQueue1* que1, CQueue2* que2)
{
    NODE1* next_term;
    char alpha[53], number[12];
    char* str1;
    char* str2;
    int exp;
    double num, sign, a;
    bool IsPrevTermNumber;

    strcpy_s(alpha, 53, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    strcpy_s(number, 12, "1234567890.");

    // isa-isahin ang mga term ng polynomial

    que2->Clear();

    num = sign = 1.0;
    IsPrevTermNumber = false;

    while (!que1->IsEmpty()) {

        que1->Remove(&str1);

        if (strchr(number, str1[0]) != NULL) {

            // kung ang current term ay number
            // ito ay coefficient ng polynomial
            // ilagay ito sa num at itrue ang IsPrevTermNumber

            num = atof(str1);
            IsPrevTermNumber = true;
        }
        else if (str1[0] == '+' || str1[0] == '-')  {

            if (IsPrevTermNumber) {

                // kung ang current term ay '+' o '-' at ang IsPrevTermNumber ay true
                // ang coefficient na nakuha sa itaas ay constant
                // ilagay ito sa a at ifalse ang IsPrevTermNumber

                a = sign * num;

                //printf("%10d%10.2f\n", 0, a);

                if (que2->Find(0)) {
                    delete[] str1;
                    return true;
                }
                else {
                    que2->Add(0, a);
                }

                num = sign = 1.0;
                IsPrevTermNumber = false;

            }
            else {

                // kung ang current term ay '+' o '-' at ang IsPrevTermNumber ay false
                // kunin lang ang sign

                sign = str1[0] == '+' ? 1.0 : -1.0;
            }
        }
        else if (str1[0] == '^') {

            // kung ang current term ay '^'
            // ang susunod na term ay exponent
            // ang coefficient na nakuha sa itaas ay coefficient ng degree exponent

            que1->Remove(&str2);

            exp = atoi(str2);
            a = sign * num;

            delete[] str2;

            //printf("%10d%10.2f\n", exp, a);

            if (que2->Find(exp)) {
                delete[] str1;
                return true;
            }
            else {
                que2->Add(exp, a);
            }

            num = sign = 1.0;
            IsPrevTermNumber = false;
        }
        else if (strchr(alpha, str1[0]) != NULL) {

            que1->Peek(&next_term);

            if (next_term == NULL) {

                // kung ang current term ay variable at ito ang huling term ng polynomial
                // ang coefficient na nakuha sa itaas ay coefficient ng degree one

                a = sign * num;

                //printf("%10d%10.2f\n", 1, a);

                if (que2->Find(1)) {
                    delete[] str1;
                    return true;
                }
                else {
                    que2->Add(1, a);
                }

                num = sign = 1.0;
                IsPrevTermNumber = false;
            }
            else {

                // kung ang current term ay variable at ang susunod na term ay hindi '^'
                // ang coefficient na nakuha sa itaas ay coefficient ng degree one

                if (next_term->str[0] != '^') {

                    a = sign * num;

                    //printf("%10d%10.2f\n", 1, a);

                    if (que2->Find(1)) {
                        delete[] str1;
                        return true;
                    }
                    else {
                        que2->Add(1, a);
                    }

                    num = sign = 1.0;
                    IsPrevTermNumber = false;
                }
            }

        }

        delete[] str1;

    }

    if (IsPrevTermNumber) {

        // kung ang IsPrevTermNumber ay true
        // ang coefficient na nakuha sa itaas ay constant

        a = sign * num;

        //printf("%10d%10.2f\n", 0, a);

        if (que2->Find(0)) {
            return true;
        }
        else {
            que2->Add(0, a);
        }

    }

    return false;
}

// f(x)

CComplexNumber Function(CComplexNumber& x, double* c, int n)
{
    CComplexNumber power, sum, term;
    int i;

    power = CComplexNumber(1.0, 0.0);
    sum = CComplexNumber(0.0, 0.0);

    for (i = 0; i < (n + 1); i++) {

        term = power * CComplexNumber(c[i], 0.0);

        sum += term;
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

CComplexNumber absolute(CComplexNumber c)
{
    return CComplexNumber(fabs(c.x), fabs(c.y));
}

void FindTheRoot(CComplexNumber* x1, double* c, int n)
{
    CComplexNumber a, b;
    CComplexNumber* x2;
    int i;
    double d;
    bool loop, precise;

    x2 = new CComplexNumber[n];

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

    delete[] x2;
}
