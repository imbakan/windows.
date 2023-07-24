
#include<stdlib.h>
#include<stdio.h>
#include "stack.h"

// constructor
CStack::CStack()
{
	First = NULL;
	Count = 0;
}

// destructor
CStack::~CStack()
{
	CItem item;

	while (!IsEmpty())
		Pop(&item);
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

// magdagdag ng node sa ibabaw ng list
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
void CStack::Push(CItem item)
{
	NODE_S* Node;

	Node = new NODE_S;
	Node->item = item;
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

// magbawas ng node sa ibabaw ng list
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
void CStack::Pop(CItem* item)
{
	NODE_S* Node;

	Node = First;
	*item = Node->item;

	First = First->Next;
	Count--;

	delete Node;
}

// basahin ang nasa itaas ng stack ng walang releasing ng memory
void CStack::Peek(CItem* item)
{
	*item = First->item;
}
