
#include "framework.h"
#include "doublylinkedlist.h"

CDoublyLinkedList::CDoublyLinkedList()
{
	First = Last = Pointer = NULL;
	Count = 0;
}

CDoublyLinkedList::~CDoublyLinkedList()
{
	Clear();
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
//           |     |--->|    |--->        |    |--->        | ++ |--->|    |--->|    |--->
//  NULL <---|     |<---|    |        <---|    |        <---| ++ |<---|    |<---|    |
//           +-----+    +----+            +----+            +----+    +----+    +----+
//                       Node1                               Node2
//
void CDoublyLinkedList::Prefix(NODE* Node2, double x, double y, double vx, double vy)
{
	NODE* Node, * Node1;

	Node = new NODE;

	Node->x = x;
	Node->y = y;

	Node->vx = vx;
	Node->vy = vy;

	Node->Prev = Node->Next = NULL;

	if (IsEmpty()) {
		First = Last = Node;
		Count++;
		return;
	}

	Node1 = Node2->Prev;

	Node2->Prev = Node;
	Node->Next = Node2;

	if (Node1 != NULL) {
		Node1->Next = Node;
		Node->Prev = Node1;
	}

	if (Node->Next == NULL) Last = Node;
	if (Node->Prev == NULL) First = Node;

	Count++;
}

//            First                        Node                                  Last
//           +-----+    +----+            +----+            +----+    +----+    +----+
//           |     |--->| ++ |--->        |    |--->        |    |--->|    |--->|    |--->
//  NULL <---|     |<---| ++ |        <---|    |        <---|    |<---|    |<---|    |
//           +-----+    +----+            +----+            +----+    +----+    +----+
//                       Node1                               Node2
//
void CDoublyLinkedList::Suffix(NODE* Node1, double x, double y, double vx, double vy)
{
	NODE* Node, * Node2;

	Node = new NODE;

	Node->x = x;
	Node->y = y;

	Node->vx = vx;
	Node->vy = vy;

	Node->Prev = Node->Next = NULL;

	if (IsEmpty()) {
		First = Last = Node;
		Count++;
		return;
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
}

void CDoublyLinkedList::Add(double x, double y, double vx, double vy)
{
	Suffix(Last, x, y, vx, vy);
}

//            First                        Node                                  Last
//           +-----+    +----+            +----+            +----+    +----+    +----+
//           |     |--->|    |--->        | ++ |--->        |    |--->|    |--->|    |--->
//  NULL <---|     |<---|    |        <---| ++ |        <---|    |<---|    |<---|    |
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

void CDoublyLinkedList::Clear()
{
	while (!IsEmpty())
		Remove(First);
}

void CDoublyLinkedList::Reset()
{
	Pointer = First;
}

bool CDoublyLinkedList::Read(float* x, float* y)
{
	if (Pointer == NULL) return false;

	*x = (float)Pointer->x;
	*y = (float)Pointer->y;

	Pointer = Pointer->Next;

	return true;
}

bool CDoublyLinkedList::Read(NODE** Node)
{
	if (Pointer == NULL) return false;

	*Node = Pointer;

	Pointer = Pointer->Next;

	return true;
}
