
#include <stdlib.h>
#include <string.h>
#include "stack.h"

CStack::CStack()
{
	First = NULL;
	Count = 0;
}

CStack::~CStack()
{
	Clear();
}

//
int CStack::GetCount()
{
	return Count;
}

//
bool CStack::IsEmpty()
{
	return (Count == 0);
}

// 
//     +-----+
//     |     |  Node     |
//     +-----+           |
//        |              |
//        V              | push
//                       |
//                       |
//     +-----+           |
//     |     | First     V
//     +-----+
//        |
//        V
//     +-----+
//     |     |
//     +-----+
//        |
//        V
//     +-----+
//     |     |
//     +-----+
//        |
//        V
//       NULL
//
void CStack::Push(char* str)
{
	STACK* Node;
	size_t n;

	Node = new STACK;

	n = strlen(str) + 1;
	Node->str = new char[n];
	strcpy_s(Node->str, n, str);

	Node->Next = NULL;

	if (IsEmpty()) {
		First = Node;
	}
	else {
		Node->Next = First;
		First = Node;
	}

	Count++;
}

// 
//     +-----+
//     |     |  First     ^
//     +-----+            |
//        |               |
//        V               |
//                        | pop
//                        |
//     +-----+            |
//     |     |            |
//     +-----+
//        |
//        V
//     +-----+
//     |     |
//     +-----+
//        |
//        V
//     +-----+
//     |     |
//     +-----+
//        |
//        V
//       NULL
//
void CStack::Pop(char** str)
{
	STACK* Node;
	size_t n;

	Node = First;

	n = strlen(Node->str) + 1;
	*str = new char[n];
	strcpy_s(*str, n, Node->str);

	First = First->Next;
	Count--;

	delete[] Node->str;
	delete Node;
}

void CStack::Peek(char** str)
{
	*str = First->str;
}

void CStack::Reset()
{
	Pointer = First;
}

bool CStack::Read(STACK** Node)
{
	if (Pointer == NULL) return false;

	*Node = Pointer;

	Pointer = Pointer->Next;

	return true;
}

void CStack::Clear()
{
	char* str;

	while (!IsEmpty()) {
		Pop(&str);
		delete[] str;
	}
}
