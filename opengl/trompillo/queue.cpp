
#include "framework.h"
#include "queue.h"

CQueue::CQueue()
{
	First = Last = NULL;
	Count = 0;
}

CQueue::~CQueue()
{
	Clear();
}

int CQueue::GetCount()
{
	return Count;
}

bool CQueue::IsEmpty()
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
void CQueue::Add(NODE* Node)
{
	QUEUE* Queue;

	Queue = new QUEUE;
	Queue->Node = Node;
	Queue->Next = NULL;

	if (IsEmpty()) {
		First = Last = Queue;
	}
	else {
		Last->Next = Queue;
		Last = Queue;
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
void CQueue::Remove(NODE** Node)
{
	QUEUE* Queue;

	Queue = First;
	*Node = Queue->Node;

	First = First->Next;
	Count--;

	if (IsEmpty())
		Last = First;

	delete Queue;
}

void CQueue::Clear()
{
	NODE* Node;

	while (!IsEmpty())
		Remove(&Node);
}
