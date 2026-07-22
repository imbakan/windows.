
// System of Equation

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "queue.h"

void PrintOutput(CQueue* que);
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

void ConvertToArray(char*** var, CQueue* que);
void ConvertToArray(CQueue* que, char** var, int* m, int n, double*** a);

bool GaussJordanMethod(double** a, int m, int n);

int main()
{
    CQueue* que1;
    CQueue que2;
    double** a;
    char** var;
	int i, m, n;
	char str[200];
    bool pass;

	printf("\n");
	printf("System of Equation\n");
	printf("\n");
	printf("Type 'E' or 'e' to enter equaton.\n");
	printf("Type 'Q' or 'q' to exit.\n");
	printf("\n");

	while (true) {

		gets_s(str, 200);

		if (strcmp(str, "Q") == 0 || strcmp(str, "q") == 0) break;

		if (strcmp(str, "E") == 0 || strcmp(str, "e") == 0) {

			printf("Enter number of equation: ");
			gets_s(str, 200);

			n = atoi(str);
            que1 = new CQueue[n];

            pass = true;

			for (i = 0; i < n; i++) {

				printf("Enter equation %d : ", i + 1);
				gets_s(str, 200);

                Parse(str, &que1[i]);

                //PrintOutput(&que1[i]);

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

                ConvertToArray(&var, &que2);
                ConvertToArray(que1, var, &m, n, &a);

                //PrintOutput(a, m, n);

                if (GaussJordanMethod(a, m, n)) {
                    printf("\n");
                    for (i = 0; i < n; i++)
                        printf("%5s = %20.15f\n", var[i], a[i][n]);
                    printf("\n");
                }
                else {
                    printf("no solution\n");
                }

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

		}

	}
}

void PrintOutput(CQueue* que)
{
    NODE* Node;

    que->Reset();

    while (que->Read(&Node))
        printf("%s ", Node->str);

    printf("\n");
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
        else if (str[i] == '+' || str[i] == '-') {

            i1 = i;
            i2 = i + 1;

            GetString(str1, 100, str, i1, i2);

            que->Add(str1);

            i = i2 - 1;
        }
        else if (str[i] == '=') {

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

// 2x - - 3y + z = 5     invalid equation
bool CheckForArrangement(CQueue* que)
{
    NODE* Node;
    char number[12], alpha[53];
    bool validN, validA, validS, validE;

    strcpy_s(number, 12, "1234567890.");
    strcpy_s(alpha, 53, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

    validN = true;      // 12
    validA = true;      // x
    validS = true;      // -
    validE = false;     // =

    que->Reset();

    while (que->Read(&Node)) {

        if (strchr(number, Node->str[0]) != NULL) {

            if (!validN) return true;

            validN = false;     // 12 34
            validA = true;      // 12 x
            validS = true;      // 12 +
            validE = true;      // 12 =

        }
        else if (strchr(alpha, Node->str[0]) != NULL) {

            if (!validA) return true;

            validN = false;     // x 34
            validA = false;     // x x
            validS = true;      // x +
            validE = true;      // x =

        }
        else if (Node->str[0] == '+' || Node->str[0] == '-') {

            if (!validS) return true;

            validN = true;      // - 34
            validA = true;      // - x
            validS = false;     // - +
            validE = false;     // - =

        }
        else if (Node->str[0] == '=') {

            if (!validE) return true;

            validN = true;      // = 34
            validA = true;      // = x
            validS = true;      // = +
            validE = false;     // = =

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

        if (Node->str[0] == '=')
            ++count;
    }

    return (count > 1);
}

// 2x - 3y + x = 5     invalid equation
bool CheckForVariable(CQueue* que)
{
    CQueue que1;
    NODE* Node;
    char alpha[53];
    bool result;

    strcpy_s(alpha, 53, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

    result = false;

    que->Reset();

    while (que->Read(&Node)) {

        if (strchr(alpha, Node->str[0]) != NULL) {
            if (que1.Find(Node->str)) {
                result = true;
                break;
            }
            else {
                que1.Add(Node->str);
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
    char number[12], alpha[53];
    int count;

    strcpy_s(number, 12, "1234567890.");
    strcpy_s(alpha, 53, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

    count = 0;

    que->Reset();

    while (que->Read(&Node1)) {

        if (strchr(number, Node1->str[0]) != NULL) {

            Node2 = Node1->Next;

            if (Node2 != NULL) {

                if (strchr(alpha, Node2->str[0]) != NULL) continue;

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
    char alpha[53];
    int i;

    strcpy_s(alpha, 53, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

    for (i = 0; i < n; i++) {

        que1[i].Reset();

        while (que1[i].Read(&Node)) {

            if (strchr(alpha, Node->str[0]) != NULL) {

                if (!que2->Find(Node->str)) {
                    que2->Add(Node->str);
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
void ConvertToArray(char*** var, CQueue* que)
{
    int i, n;
    size_t size;
    char* str;

    n = que->GetCount();
    *var = new char* [n];

    i = 0;

    while (!que->IsEmpty()) {

        que->Remove(&str);

        size = strlen(str) + 1;
        (*var)[i] = new char[size];

        strcpy_s((*var)[i], size, str);

        ++i;

        delete[] str;
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
void ConvertToArray(CQueue* que, char** var, int* m, int n, double*** a)
{
    int i, j;
    double num, sign, toggle;
    bool numfound;
    char number[12], alpha[53];
    char* str;

    strcpy_s(number, 12, "1234567890.");
    strcpy_s(alpha, 53, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

    // allocate
    *a = new double* [n];
    *m = n + 1;

    for (i = 0; i < n; i++)
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

        while (!que[i].IsEmpty()) {

            que[i].Remove(&str);

            if (strchr(number, str[0]) != NULL) {

                // kung number kunin lang ang number

                num = atof(str);
                numfound = true;

            }
            else if (strchr(alpha, str[0]) != NULL) {

                // kung variable ang sumunod ilagay ang number na nakuha, kung meron, sa matrix sa tamang location

                j = FindIndex(var, n, str);

                if (numfound)
                    (*a)[i][j] = toggle * sign * num;
                else
                    (*a)[i][j] = toggle * sign;

                numfound = false;
                num = sign = 1.0;

            }
            else if (str[0] == '+' || str[0] == '-') {

                // kung sign ang sumunod ilagay ang number na nakuha, kung meron, sa matrix sa tamang location
                // ito ay ang constant sa equation

                if (numfound)
                    (*a)[i][*m - 1] = -toggle * sign * num;

                numfound = false;
                num = 1.0;

                // kung sign kunin ang sign, positive o negative, ang zero ay para lang sa error checking

                if (str[0] == '+')
                    sign = 1.0;
                else if (str[0] == '-')
                    sign = -1.0;
                else
                    sign = 0.0;

            }
            else if (str[0] == '=') {

                // kung equal ang sumunod ilagay ang number na nakuha, kung meron, sa matrix sa tamang location
                // ito ay ang constant sa equation

                if (numfound)
                    (*a)[i][*m - 1] = -toggle * sign * num;

                // kung equal, ito ay nasa kabila ng equation, toggle lang ito

                numfound = false;
                num = sign = 1.0;
                toggle = -1.0;

            }

            delete[] str;
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
