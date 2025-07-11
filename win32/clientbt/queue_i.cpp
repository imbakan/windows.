
#include "framework.h"
#include "queue_i.h"

CQueue_i::CQueue_i()
{
	First = Last = NULL;
	Count = 0;
}

CQueue_i::~CQueue_i()
{
	Clear();
}

int CQueue_i::GetCount()
{
	return Count;
}

bool CQueue_i::IsEmpty()
{
	return (Count == 0);
}

//                                                                       add
//                                                               <---------------
//      First                                             Last                     Node
//     +----+    +----+    +----+    +----+    +----+    +----+                   +----+
//     |    |--->|    |--->|    |--->|    |--->|    |--->|    |---> NULL          |    |---> NULL
//     +----+    +----+    +----+    +----+    +----+    +----+                   +----+
//
void CQueue_i::Add(int value)
{
	NODE_I* Node;

	Node = new NODE_I;
	Node->value = value;
	Node->Next = NULL;

	if (IsEmpty()) {
		First = Last = Node;
	}
	else {
		Last->Next = Node;
		Last = Node;
	}

	Count++;
}

//               remove
//         <-----------------
//      First                                                                 Last
//     +----+              +----+    +----+    +----+    +----+    +----+    +----+
//     |    |--->          |    |--->|    |--->|    |--->|    |--->|    |--->|    |---> NULL
//     +----+              +----+    +----+    +----+    +----+    +----+    +----+
//
void CQueue_i::Remove(int* value)
{
	NODE_I* Node;

	Node = First;
	*value = Node->value;

	First = First->Next;
	Count--;

	if (IsEmpty())
		Last = First;

	delete Node;
}

//               push
//         ----------------->
//      Node                First                                            Last
//     +----+              +----+    +----+    +----+    +----+    +----+    +----+
//     |    |--->          |    |--->|    |--->|    |--->|    |--->|    |--->|    |---> NULL
//     +----+              +----+    +----+    +----+    +----+    +----+    +----+
//
void CQueue_i::Push(int value)
{
	NODE_I* Node;

	Node = new NODE_I;
	Node->value = value;
	Node->Next = NULL;

	if (IsEmpty()) {
		First = Last = Node;
	}
	else {
		Node->Next = First;
		First = Node;
	}

	Count++;
}

void CQueue_i::Clear()
{
	int value;

	while (!IsEmpty())
		Remove(&value);
}
