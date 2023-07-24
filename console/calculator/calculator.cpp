//
// calculator
//

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <math.h>
#include "queue.h"
#include "stack.h"

bool IsOperator(char ch);
bool IsOperand(char ch);

void GetOperator(char* str, size_t* i, char* operetor);
void GetOperand(char* str, size_t* i, double* operand);

void PrecedenceLT(CItem item1, CQueue* queue, CStack* stack);
void PrecedenceGT(CItem item1, CQueue* queue, CStack* stack);
void PrecedenceEQ(CItem item1, CQueue* queue, CStack* stack);

void Parse(char* str, CQueue* queue);
void InfixToPosfix(CQueue* queue1, CQueue* queue2);
double DoTheMath(CQueue* queue);

int main()
{
    char ch, str[300];
    CQueue infix, posfix;
    double result;

    // iset ang title ng console
    SetConsoleTitle(L"Calculator");

    printf("\n");

    ch = 'Y';

    while (ch == 'Y' || ch == 'y') {

        printf("Enter mathematical expression: ");
        gets_s(str, 300);

        Parse(str, &infix);
        InfixToPosfix(&infix, &posfix);
        result = DoTheMath(&posfix);

        printf("%f\n", result);

        // ulitin
        printf("\nContinue (y/n)?");

        do {
            ch = _getch();
        } while (!(ch == 'Y' || ch == 'y' || ch == 'N' || ch == 'n'));

        printf((ch == 'Y' || ch == 'y') ? "y\n\n" : "n\n");
    }
}

// alamin kung ang character ch ay operator
bool IsOperator(char ch)
{
    char str[10];
    char* p;

    strcpy_s(str, 10, "/*-+^()");
    p = strchr(str, ch);

    return (p != NULL);
}

// alamin kung ang character ch ay operand
bool IsOperand(char ch)
{
    char str[12];
    char* p;

    strcpy_s(str, 12, "0123456789.");
    p = strchr(str, ch);

    return (p != NULL);
}

// kunin ang operator sa string str na nagsisimula sa index i
// ilagay ang nakuha sa variable operetor
void GetOperator(char* str, size_t* i, char* operetor)
{
    *operetor = str[*i];
    ++(*i);
}

// kunin ang operand sa string str na nagsisimula sa index i
// ilagay ang nakuha sa variable operand
void GetOperand(char* str, size_t* i, double* operand)
{
    char str1[12], str2[100];
    size_t j, k;

    strcpy_s(str1, 12, "0123456789.");

    k = *i;
    j = 0;

    while (strchr(str1, str[k]) != NULL) {

        str2[j] = str[k];

        ++j;
        ++k;
    }

    str2[j] = '\0';

    *i = k;

    *operand = atof(str2);
}

// function kung ang precedence ng operator ay mas mababa kay sa nasa stack
// ipop ang operator na nasa stack, tapos iadd sa queue
// itest ang precedence ng susunod na item na nasa stack
// pagkatapos matest ang lahat na item na nasa stack, ipush ang operator sa stack
void PrecedenceLT(CItem item1, CQueue* queue, CStack* stack)
{
    CItem item;

    while (!stack->IsEmpty()) {

        stack->Peek(&item);

        if (item1 < item) {
            
            stack->Pop(&item);
            queue->Add(item);
        }
        else if (item1 > item) {          
            PrecedenceGT(item1, queue, stack);
        }
        else {           
            PrecedenceEQ(item1, queue, stack);
        }
    }
}

// function kung ang precedence ng operator ay mas mataas kay sa nasa stack
// ipush ang operator sa stack
void PrecedenceGT(CItem item1, CQueue* queue, CStack* stack)
{
}

// function kung ang precedence ng operator ay kapareho ng nasa stack
// kung left to right, ipop ang operator na nasa stack, tapos iadd sa queue
// kung right to left, ipush ang operator sa stack
void PrecedenceEQ(CItem item1, CQueue* queue, CStack* stack)
{
    CItem item;

    if (item1 == LEFT_TO_RIGHT) {

        stack->Pop(&item);
        queue->Add(item);

    }
    else if (item1 == RIGHT_TO_LEFT) {
    }
}

// kunin ang  operand at operator ng string str
// ilagay ang mga nakuha sa variable queue
void Parse(char* str, CQueue* queue)
{
    size_t i, n;
    char operetor;
    double operand;
    CItem item;

    i = 0;
    n = strlen(str);

    while (i < n) {

        if (IsOperator(str[i])) {
            GetOperator(str, &i, &operetor);
            item = operetor;
            queue->Add(item);
        }
        else if (IsOperand(str[i])) {
            GetOperand(str, &i, &operand);
            item = operand;
            queue->Add(item);
        }
        else {
            ++i;
        }
    }
}

// gawing posfix ang infix
//
// infix          posfix
// A + B          A B +
// queue1         queue2
//
void InfixToPosfix(CQueue* queue1, CQueue* queue2)
{
    CStack stack;
    CItem item, item1, item2;

    // isa-isahin ang bawat item na nasa infix
    while (!queue1->IsEmpty()) {

        // kunin ang item 1 na nasa infix
        queue1->Remove(&item1);

       //printf("[%s]\n", (char*)item1);

        if (item1 == OPERAND) {

            // kung ang item 1 ay operand iadd 'to sa queue
            queue2->Add(item1);
        }
        else if (item1 == OPERATOR) {

            // kung ang item 1 ay operator gawin ang mga ito:

            if (item1 == '(') {
                
                // kung open parenthesis, gawin uli ang function na 'to
                InfixToPosfix(queue1, queue2);
            }
            else if (item1 == ')') {
                
                // kung close parenthesis, mag exit sa function na 'to
                break;
            }
            else if (stack.IsEmpty()) {

                // kung walang laman ang stack, ipush ang operator sa stack
                stack.Push(item1);
            }
            else {

                // kung may laman ang stack, itest ang precedence nito
                stack.Peek(&item2);

                if (item1 < item2) {

                    // kung ang precedence ng operator ay mas mababa kay sa nasa stack
                    PrecedenceLT(item1, queue2, &stack);
                }
                else if (item1 > item2) {

                    // kung ang precedence ng operator ay mas mataas kay sa nasa stack
                    PrecedenceGT(item1, queue2, &stack);
                }
                else {

                    // kung ang precedence ng operator ay kapareho ng nasa stack
                    PrecedenceEQ(item1, queue2, &stack);
                }

                // ito ay common sa PrecedenceLT, PrecedenceGT at PrecedenceEQ function
                // kaya inilagay ko dito
                stack.Push(item1);
            }
        }
    }

    // iadd ang natitirang laman ng stack sa queue 2
    while (!stack.IsEmpty()) {

        stack.Pop(&item);
        queue2->Add(item);
    }
}

// reverse polish notation
double DoTheMath(CQueue* queue)
{
    CItem item, item1, item2, item3;
    CStack stack;
    double result = 0.0;

    // isa-isahin ang bawat item na nasa posfix
    while (!queue->IsEmpty()) {

        // kunin ang item na nasa posfix
        queue->Remove(&item);

        if (item == OPERAND) {

            // kung ang item ay operand, ipush 'to sa stack
            stack.Push(item);
        }
        else if (item == OPERATOR) {

            // kung ang item ay operator
            // ipop ang operand na nasa stack
            // gamitin ang operand at operator sa computation
            // ipush ang resulta sa stack
            stack.Pop(&item2);
            stack.Pop(&item1);

            switch (item) {
            case '+': item3 = item1 + item2;    break;
            case '-': item3 = item1 - item2;    break;
            case '*': item3 = item1 * item2;    break;
            case '/': item3 = item1 / item2;    break;
            case '^': item3 = item1 ^ item2;    break;
            }

            stack.Push(item3);
        }
    }

    // ipop ang natitirang isang laman ng stack, ito ang resulta ng computation
    if (stack.GetCount() > 1) {
        printf("... error ...\n");
    }
    else {
        stack.Pop(&item);
        result = (double)item;
    }

    return result;
}
