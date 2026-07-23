
#include <stdlib.h>
#include "stack2.h"

CStack2::CStack2()
{
	First = NULL;
	Count = 0;
}

CStack2::~CStack2()
{
	Clear();
}

//
int CStack2::GetCount()
{
	return Count;
}

//
bool CStack2::IsEmpty()
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
void CStack2::Push(double value)
{
	NODE* Node;

	Node = new NODE;
	Node->value = value;
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
void CStack2::Pop(double* value)
{
	NODE* Node;

	Node = First;
	*value = Node->value;

	First = First->Next;
	Count--;

	delete Node;
}

void CStack2::Clear()
{
	double value;

	while (!IsEmpty())
		Pop(&value);
}
