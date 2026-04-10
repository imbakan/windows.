
#include <string.h>
#include "queue.h"

CQueue::CQueue()
{
	First = Last = NULL;
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

void CQueue::Add(char* str)
{
	NODE* Node;
	size_t n;

	Node = new NODE;

	n = strlen(str) + 1;
	Node->str = new char[n];
	strcpy_s(Node->str, n, str);

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

void CQueue::Remove(char* str, size_t n)
{
	NODE* Node;

	Node = First;

	strcpy_s(str, n, Node->str);

	First = First->Next;
	Count--;

	if (IsEmpty())
		Last = First;

	delete[] Node->str;
	delete Node;
}

void CQueue::Remove()
{
	NODE* Node;

	Node = First;
	First = First->Next;
	Count--;

	if (IsEmpty())
		Last = First;

	delete[] Node->str;
	delete Node;
}

void CQueue::Clear()
{
	while (!IsEmpty())
		Remove();
}
