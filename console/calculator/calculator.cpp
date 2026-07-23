
// calculator

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "queue.h"
#include "stack.h"
#include "stack2.h"

const int LEFT_TO_RIGHT = 1001;
const int RIGHT_TO_LEFT = 1002;

void PrintOutput(CQueue* que);
void PrintOutput(CStack* stc, CQueue* que);

int precedence(char ch);
int associativity(char ch);

void GetNumber(char* str, size_t i1, size_t* i2);
void GetString(char* str1, size_t n, char* str2, size_t i1, size_t i2);

void Parse(char* str, CQueue* que);
bool CheckForArrangement(CQueue* que);
void InfixToPosfix(CQueue* que, CQueue* que2);

double DoTheMath(CQueue* queue);

int main()
{
    CQueue que1, que2;
    char str[200];
    double a;

    printf("\n");
    printf("Calculator\n");
    printf("\n");
    printf("Type mathematical operation to perform calculation.\n");
    printf("Type 'Q' or 'q' to exit.\n");
    printf("\n");

    while (true) {

        gets_s(str, 200);

        if (strcmp(str, "Q") == 0 || strcmp(str, "q") == 0) break;

        Parse(str, &que1);

        if (CheckForArrangement(&que1)) {
            printf("Invalid mathematical operations.\n");
        }
        else {

            InfixToPosfix(&que1, &que2);

            //PrintOutput(&que2);

            a = DoTheMath(&que2);

            printf("%25.15f\n", a);
        }
    }
}

void PrintOutput(CQueue* que)
{
    QUEUE* Node;

    que->Reset();

    while (que->Read(&Node))
        printf("%s ", Node->str);

    printf("\n");
}

void PrintOutput(CStack* stc, CQueue* que)
{
    STACK* Node1;
    QUEUE* Node2;
    int i, n;

    n = 20 - stc->GetCount();

    stc->Reset();

    while (stc->Read(&Node1))
        printf("%s ", Node1->str);

    for (i = 0; i < n; i++)
        printf("  ");

    que->Reset();

    while (que->Read(&Node2))
        printf("%s ", Node2->str);

    printf("\n");
}

int precedence(char ch)
{
    int value;

    value = 0;

    switch (ch) {
    case '(':
    case ')': value = 99; break;
    case '^': value = 3; break;
    case '*':
    case '/': value = 2; break;
    case '+':
    case '-': value = 1; break;
    }

    return value;
}

int associativity(char ch)
{
    int value;

    value = 0;

    switch (ch) {
    case '(':
    case ')':; break;/**/
    case '^': value = RIGHT_TO_LEFT; break;/* R - L */
    case '*':
    case '/': value = LEFT_TO_RIGHT; break;/* L - R */
    case '+':
    case '-': value = LEFT_TO_RIGHT; break;/* L - R */
    }

    return value;
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

// kunin ang string str1 sa string str2 sa pagitan ng index i1 at i2
void GetString(char* str1, size_t n, char* str2, size_t i1, size_t i2)
{
    size_t i, k;

    k = 0;

    for (i = i1; i < i2; i++)
        str1[k++] = str2[i];

    str1[k] = '\0';
}

// ilagay sa que ang mga term ng operation str
//
// operation             ((10 + 20) - 30) * 40 / 50
//
//                        +---+    +---+    +----+    +---+    +----+    +---+    +---+    +----+    +---+    +---+    +----+    +---+    +----+
// que                    | ( |--->| ( |--->| 10 |--->| + |--->| 20 |--->| ) |--->| - |--->| 30 |--->| ) |--->| * |--->| 40 |--->| / |--->| 50 |
//                        +---+    +---+    +----+    +---+    +----+    +---+    +---+    +----+    +---+    +---+    +----+    +---+    +----+
//
void Parse(char* str, CQueue* que)
{
    size_t i, i1, i2, n;
    char number[12], operators[8], str1[100];

    strcpy_s(number, 12, "1234567890.");
    strcpy_s(operators, 8, "+-*/()^");

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
        else if (strchr(operators, str[i]) != NULL) {

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

// true - failed
// false - passed
bool CheckForArrangement(CQueue* que)
{
    QUEUE* Node;
    int count;
    char numbers[12];
    bool NotNumNext, NotPMNext, NotMDNext, NotOPNext, NotCPNext, NotEXNext;

    strcpy_s(numbers, 12, "1234567890.");

    NotNumNext = false;     // 5
    NotPMNext = false;      // +
    NotMDNext = true;       // *
    NotOPNext = false;      // (
    NotCPNext = true;       // )
    NotEXNext = true;       // ^

    count = 0;

    que->Reset();

    while (que->Read(&Node)) {

        if (strchr(numbers, Node->str[0]) != NULL) {

            //printf("%s\n", Node->str);

            if (NotNumNext) return true;

            NotNumNext = true;      // 5 5
            NotPMNext = false;      // 5 +
            NotMDNext = false;      // 5 *
            NotOPNext = true;       // 5 (
            NotCPNext = false;      // 5 )
            NotEXNext = false;      // 5 ^
        }
        else if (Node->str[0] == '+' || Node->str[0] == '-') {

            //printf("%c\n", Node->str[0]);

            if (NotPMNext) return true;

            NotNumNext = false;     // + 5
            NotPMNext = true;       // + +
            NotMDNext = true;       // + *
            NotOPNext = false;      // + (
            NotCPNext = true;       // + )
            NotEXNext = true;       // + ^

        }
        else if (Node->str[0] == '*' || Node->str[0] == '/' || Node->str[0] == '^') {

            //printf("%c\n", Node->str[0]);

            if (NotMDNext) return true;

            NotNumNext = false;     // * 5
            NotPMNext = true;       // * +
            NotMDNext = true;       // * *
            NotOPNext = false;      // * (
            NotCPNext = true;       // * )
            NotEXNext = true;       // * ^
        }
        else if (Node->str[0] == '(') {

            //printf("%c\n", Node->str[0]);

            if (NotOPNext) return true;

            NotNumNext = false;     // ( 5
            NotPMNext = false;      // ( +
            NotMDNext = true;       // ( *
            NotOPNext = false;      // ( (
            NotCPNext = true;       // ( )
            NotEXNext = true;       // ( ^

            ++count;

        }
        else if (Node->str[0] == ')') {

            //printf("%c\n", Node->str[0]);

            if (NotCPNext) return true;

            NotNumNext = true;      // ) 5
            NotPMNext = false;      // ) +
            NotMDNext = false;      // ) *
            NotOPNext = true;       // ) (
            NotCPNext = false;      // ) )
            NotEXNext = false;      // ) ^

            --count;
        }
        else {

            //printf("character error\n");

            return true;
        }
    }

    if (count != 0) return true;

    return false;
}

// 3.5 Infix to Postfix Conversion Rules using Stack | Data structures Tutorials
// https://www.youtube.com/watch?v=TB7qzDgX-pI
void InfixToPosfix(CQueue* que1, CQueue* que2)
{
    CStack stack;
    char numbers[12], operators[6];
    char* str1;
    char* str2;
    char* str3;
    char* str4;

    strcpy_s(numbers, 12, "1234567890.");
    strcpy_s(operators, 6, "+-*/^");

    while (!que1->IsEmpty()) {

        que1->Remove(&str1);

        //printf("%s\n", str1);

        if (strchr(numbers, str1[0]) != NULL) {

            // 1. Print operands as they arrive.

            que2->Add(str1);

        }
        else if (strchr(operators, str1[0]) != NULL) {

            if (stack.IsEmpty()) {

                // 2.1 If stack is empty,
                //     push the incomming operator onto the stack.

                stack.Push(str1);
            }
            else {

                stack.Peek(&str2);

                if (str2[0] == '(') {

                    // 2.2 If stack contains a left parethesis on top,
                    //     push the incomming operator onto the stack.

                    stack.Push(str1);
                }
                else if (precedence(str1[0]) > precedence(str2[0])) {

                    // 5. If incomming symbol has higher precedence than the top of the stack,
                    //    push it onto the stack.

                    stack.Push(str1);
                }
                else if (precedence(str1[0]) < precedence(str2[0])) {

                    // 6. If incomming symbol has lower precedence than the top of the stack,
                    //    pop and print the top. Then test the incomming operator against the new top of the stack. ????????????????????????????

                    stack.Pop(&str3);
                    que2->Add(str3);
                    delete[] str3;

                    while (!stack.IsEmpty()) {

                        stack.Peek(&str4);

                        if (precedence(str1[0]) > precedence(str4[0])) {
                            break;
                        }
                        else if (precedence(str1[0]) < precedence(str4[0])) {

                            stack.Pop(&str3);
                            que2->Add(str3);
                            delete[] str3;
                        }
                        else if (precedence(str1[0]) == precedence(str4[0])) {

                            if (associativity(str1[0]) == LEFT_TO_RIGHT) {

                                stack.Pop(&str3);
                                que2->Add(str3);
                                delete[] str3;

                            }
                            else if (associativity(str1[0]) == RIGHT_TO_LEFT) {
                                break;
                            }
                        }
                    }

                    stack.Push(str1);
                }
                else if (precedence(str1[0]) == precedence(str2[0])) {

                    // 7. If incomming symbol has equal precedence with the top of the stack,
                    //    use associativity rule

                    if (associativity(str1[0]) == LEFT_TO_RIGHT) {

                        // If associativity is Left to Right
                        // then pop and print the top of the stack and then push the incomming operator

                        stack.Pop(&str3);
                        que2->Add(str3);
                        delete[] str3;

                        stack.Push(str1);

                    }
                    else if (associativity(str1[0]) == RIGHT_TO_LEFT) {

                        // If associativity is Right to Left
                        // then push the incoming operator

                        stack.Push(str1);
                    }
                }

            }
        }
        else if (str1[0] == '(') {

            // 3. If incomming symbol is '(',
            //    push it onto stack.

            stack.Push(str1);
        }
        else if (str1[0] == ')') {

            // 4. If incomming symbol is ')',
            //    pop the stack and print the operator until left parenthesis is found.

            while (!stack.IsEmpty()) {

                stack.Pop(&str3);

                if (str3[0] == '(') {
                    delete[] str3;
                    break;
                }

                que2->Add(str3);

                delete[] str3;
            }
        }

        //PrintOutput(&stack, que2);

        delete[] str1;
    }

    // 8. At the end of the expression, pop and print all operators of stack

    while (!stack.IsEmpty()) {

        stack.Pop(&str3);
        que2->Add(str3);
        delete[] str3;
    }

    //PrintOutput(&stack, que2);
}

// Reverse Polish Notationand The Stack - Computerphile
// https://www.youtube.com/watch?v=7ha78yWRDlE
double DoTheMath(CQueue* que)
{
    CStack2 stack;
    double a, b, c;
    char numbers[12], operators[6];
    char* str;

    strcpy_s(numbers, 12, "1234567890.");
    strcpy_s(operators, 6, "+-*/^");

    while (!que->IsEmpty()) {

        que->Remove(&str);

        //printf("%s\n", str);

        if (strchr(numbers, str[0]) != NULL) {

            // kung operand iadd sa queue
            a = atof(str);
            stack.Push(a);
        }
        else if (strchr(operators, str[0]) != NULL) {

            // kung operator gawin ang mga ito:
            // kunin ang kailangang operand sa queue at gamitin ito sa operator

            switch (str[0]) {

            case '+':

                stack.Pop(&b);
                stack.Pop(&a);

                c = a + b;

                stack.Push(c);

                break;

            case '-':

                stack.Pop(&b);
                stack.Pop(&a);

                c = a - b;

                stack.Push(c);

                break;

            case '*':

                stack.Pop(&b);
                stack.Pop(&a);

                c = a * b;

                stack.Push(c);

                break;

            case '/':

                stack.Pop(&b);
                stack.Pop(&a);

                c = a / b;

                stack.Push(c);

                break;

            case '^':

                stack.Pop(&b);
                stack.Pop(&a);

                c = pow(a, b);

                stack.Push(c);

                break;

            }
        }

        delete[] str;
    }

    c = 0.0;

    if (stack.GetCount() > 1)
        printf("... error ...\n");
    else
        stack.Pop(&c);

    return c;
}
