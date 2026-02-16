
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
CQueue_2s* CQueue::Add(char* str)
{
	NODE* Node;
	size_t n;

	Node = new NODE;

	n = strlen(str) + 1;
	Node->str = new char[n];
	strcpy_s(Node->str, n, str);

	Node->que = new CQueue_2s();

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

	return Node->que;
}

//               remove
//         <-----------------
//      First                                                                 Last
//     +----+              +----+    +----+    +----+    +----+    +----+    +----+
//     |    |--->          |    |--->|    |--->|    |--->|    |--->|    |--->|    |---> NULL
//     +----+              +----+    +----+    +----+    +----+    +----+    +----+
//
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
	delete Node->que;
	delete Node;
}

void CQueue::Clear()
{
	char str[MAX_STRING];

	while (!IsEmpty())
		Remove(str, MAX_STRING);
}

void CQueue::Reset()
{
	Pointer = First;
}

bool CQueue::Read(char** str, CQueue_2s** que)
{
	if (Pointer == NULL) return false;

	*str = Pointer->str;
	*que = Pointer->que;

	Pointer = Pointer->Next;

	return true;
}
