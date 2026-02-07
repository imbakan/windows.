
#include "framework.h"
#include "doublylinkedlist.h"

CDoublyLinkedList::CDoublyLinkedList()
{
	First = Last = Pointer = NULL;
	Count = 0;
}

CDoublyLinkedList::~CDoublyLinkedList()
{

}

int CDoublyLinkedList::GetCount()
{
	return Count;
}

bool CDoublyLinkedList::IsEmpty()
{
	return (Count == 0);
}

//            First                        Node                                  Last
//           +-----+    +----+            +----+            +----+    +----+    +----+
//           |     |--->|    |--->        |    |--->        |    |--->|    |--->|    |--->
//  NULL <---|     |<---|    |        <---|    |        <---|    |<---|    |<---|    |
//           +-----+    +----+            +----+            +----+    +----+    +----+
//                       Node1                               Node2
//
NODE* CDoublyLinkedList::Add(NODE* Node1, double x, double y, double m, int count)
{
	NODE* Node, * Node2;

	Node = new NODE;

	Node->m = m;

	Node->x[0] = x;
	Node->y[0] = y;

	Node->x[1] = Node->x[0] + LENGTH;
	Node->y[1] = Node->y[0] + LENGTH * Node->m;

	Node->count = count;

	Node->Prev = Node->Next = NULL;

	if (IsEmpty()) {
		First = Last = Node;
		Count++;
		return Node;
	}

	Node2 = Node1->Next;

	Node1->Next = Node;
	Node->Prev = Node1;

	if (Node2 != NULL) {
		Node2->Prev = Node;
		Node->Next = Node2;
	}

	if (Node->Next == NULL) Last = Node;
	if (Node->Prev == NULL) First = Node;

	Count++;

	return Node;
}

NODE* CDoublyLinkedList::Add(double x, double y, double m, int count)
{
	return Add(Last, x, y, m, count);
}

//            First                        Node                                  Last
//           +-----+    +----+            +----+            +----+    +----+    +----+
//           |     |--->|    |--->        |    |--->        |    |--->|    |--->|    |--->
//  NULL <---|     |<---|    |        <---|    |        <---|    |<---|    |<---|    |
//           +-----+    +----+            +----+            +----+    +----+    +----+
//                       Node1                               Node2
//
void CDoublyLinkedList::Remove(NODE* Node)
{
	NODE* Node1, * Node2;

	Node1 = Node->Prev;
	Node2 = Node->Next;

	if (Node1 != NULL) Node1->Next = Node2;
	if (Node2 != NULL) Node2->Prev = Node1;

	if (Node == First) First = First->Next;
	if (Node == Last) Last = Last->Prev;

	Count--;

	delete Node;
}

void CDoublyLinkedList::Set(NODE* Node, double m)
{
	Node->m = m;

	Node->x[1] = Node->x[0] + LENGTH;
	Node->y[1] = Node->y[0] + LENGTH * Node->m;
}

void CDoublyLinkedList::Clear()
{
	while (!IsEmpty())
		Remove(First);
}

void CDoublyLinkedList::Reset()
{
	Pointer = First;
}

bool CDoublyLinkedList::Read(NODE** Node)
{
	if (Pointer == NULL) return false;

	*Node = Pointer;

	Pointer = Pointer->Next;

	return true;
}
