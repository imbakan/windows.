
#include<stdlib.h>
#include <stdio.h>
#include "queue.h"

// constructor
CQueue::CQueue()
{
	First = Last = NULL;
	Count = 0;
}

// destructor
CQueue::~CQueue()
{
	CItem item;

	while (!IsEmpty())
		Remove(&item);
}

//
int CQueue::GetCount()
{
	return Count;
}

//
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
void CQueue::Add(CItem item)
{
	NODE_Q* Node;

	Node = new NODE_Q;
	Node->item = item;
	Node->Next = NULL;

	if (IsEmpty())
	{
		First = Last = Node;
	}
	else
	{
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
void CQueue::Remove(CItem* item)
{
	NODE_Q* Node;

	Node = First;
	*item = Node->item;

	First = First->Next;
	Count--;

	if (IsEmpty())
		Last = First;

	delete Node;
}
