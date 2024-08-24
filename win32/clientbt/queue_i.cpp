
#include "framework.h"
#include "queue_i.h"

//  constructor
CQueue_i::CQueue_i()
{
	First = Last = NULL;
	Count = 0;
}

// destructor
CQueue_i::~CQueue_i()
{
	RemoveAll();
}

//
int CQueue_i::GetCount()
{
	return Count;
}

//
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

//               return
//         ----------------->
//      Node                First                                            Last
//     +----+              +----+    +----+    +----+    +----+    +----+    +----+
//     |    |--->          |    |--->|    |--->|    |--->|    |--->|    |--->|    |---> NULL
//     +----+              +----+    +----+    +----+    +----+    +----+    +----+
//
void CQueue_i::Return(int value)
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

void CQueue_i::RemoveAll()
{
	int value;

	while (!IsEmpty())
		Remove(&value);
}
