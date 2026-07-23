
#include <stdlib.h>
#include <string.h>
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
void CQueue::Add(char* str)
{
	QUEUE* Node;
	size_t n;

	Node = new QUEUE;

	n = strlen(str) + 1;
	Node->str = new char[n];
	strcpy_s(Node->str, n, str);

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
void CQueue::Remove(char** str)
{
	QUEUE* Node;
	size_t n;

	Node = First;

	n = strlen(Node->str) + 1;
	*str = new char[n];
	strcpy_s(*str, n, Node->str);

	First = First->Next;
	Count--;

	if (IsEmpty())
		Last = First;

	delete[] Node->str;
	delete Node;
}

void CQueue::Reset()
{
	Pointer = First;
}

bool CQueue::Read(QUEUE** Node)
{
	if (Pointer == NULL) return false;

	*Node = Pointer;

	Pointer = Pointer->Next;

	return true;
}

void CQueue::Clear()
{
	char* str;

	while (!IsEmpty()) {
		Remove(&str);
		delete[] str;
	}
}
