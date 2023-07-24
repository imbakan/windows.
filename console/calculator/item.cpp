
#include<stdio.h>
#include <math.h>
#include "item.h"

// constructor
CItem::CItem()
{
    type = 0;
    data = { 0 };
}

// constructor
CItem::CItem(double dbl)
{
    type = OPERAND;
    data.dbl = dbl;
}

// destructor
CItem::~CItem()
{

}

// kunin ang precedence ng operator
char CItem::GetPrecedence(char ch)
{
    char p = 0;

    switch (ch)
    {
    case '+':
    case '-': p = 1; break;
    case '*':
    case '/': p = 2; break;
    case '^': p = 3; break;
    case '(':
    case ')': p = 4; break;
    }

    return p;
}

// kunin ang associativity ng operator
char CItem::GetAssociativity(char ch)
{
    char a = 0;

    switch (ch)
    {
    case '+':
    case '-':
    case '*':
    case '/': a = LEFT_TO_RIGHT;  break;
    case '^': a = RIGHT_TO_LEFT; break;
    }

    return a;
}

// overload == operator
bool CItem::operator ==(char ch)
{
    return (data.ch[0] == ch);
}

// overload == operator
bool CItem::operator ==(short sh)
{
    return (type == (char)sh);
}

// overload == operator
bool CItem::operator ==(int i)
{
    return (data.ch[2] == (int)i);
}

// overload == operator
bool CItem::operator ==(CItem item)
{
    return (data.ch[1] == (char)item.data.ch[1]);
}

// overload < operator
bool CItem::operator <(CItem item)
{
    return (data.ch[1] < (char)item.data.ch[1]);
}

// overload > operator
bool CItem::operator >(CItem item)
{
    return (data.ch[1] > (char)item.data.ch[1]);
}

// overload = operator
void CItem::operator =(char ch)
{
    type = OPERATOR;
    data.ch[0] = ch;
    data.ch[1] = GetPrecedence(ch);
    data.ch[2] = GetAssociativity(ch);
}

// overload = operator
void CItem::operator =(double dbl)
{
    type = OPERAND;
    data.dbl = dbl;
}

// overload = operator
void CItem::operator =(CItem item)
{
    type = item.type;
    data.ch[0] = item.data.ch[0];
    data.ch[1] = item.data.ch[1];
    data.ch[2] = item.data.ch[2];
    data.dbl = item.data.dbl;
}

//  overload + operator
CItem CItem::operator +(CItem item)
{
    return CItem(data.dbl + item.data.dbl);
}

//  overload - operator
CItem CItem::operator -(CItem item)
{
    return CItem(data.dbl - item.data.dbl);
}

//  overload * operator
CItem CItem::operator *(CItem item)
{
    return CItem(data.dbl * item.data.dbl);
}

//  overload / operator
CItem CItem::operator /(CItem item)
{
    return CItem(data.dbl / item.data.dbl);
}

//  overload ^ operator
CItem CItem::operator ^(CItem item)
{
    return CItem(pow(data.dbl, item.data.dbl));
}

// cast to ch
CItem::operator char()
{
    return data.ch[0];
}

// cast to string
CItem::operator char* ()
{
    static char str[100];

    if (type == OPERATOR)
        sprintf_s(str, 100, "%c", data.ch[0]);
    else if (type == OPERAND)
        sprintf_s(str, 100, "%f", data.dbl);
    else
        sprintf_s(str, 100, "");

    return str;
}

// cast to double
CItem::operator double()
{
    return data.dbl;
}
