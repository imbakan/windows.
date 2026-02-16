
#include "framework.h"
#include "queue_2s.h"

CQueue_2s::CQueue_2s()
{
	First = Last = NULL;
	Count = 0;
}

CQueue_2s::~CQueue_2s()
{
	Clear();
}

int CQueue_2s::GetCount()
{
	return Count;
}

bool CQueue_2s::IsEmpty()
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
void CQueue_2s::Add(char* str1, char* str2)
{
	NODE_2S* Node;
	size_t n;

	Node = new NODE_2S;

	n = strlen(str1) + 1;
	Node->str1 = new char[n];
	strcpy_s(Node->str1, n, str1);

	n = strlen(str2) + 1;
	Node->str2 = new char[n];
	strcpy_s(Node->str2, n, str2);

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
void CQueue_2s::Remove(char* str1, char* str2, size_t n)
{
	NODE_2S* Node;

	Node = First;

	strcpy_s(str1, n, Node->str1);
	strcpy_s(str2, n, Node->str2);

	First = First->Next;
	Count--;

	if (IsEmpty())
		Last = First;

	delete[] Node->str1;
	delete[] Node->str2;
	delete Node;
}

void CQueue_2s::Clear()
{
	char str1[MAX_STRING], str2[MAX_STRING];

	while (!IsEmpty())
		Remove(str1, str2, MAX_STRING);
}

void CQueue_2s::Reset()
{
	Pointer = First;
}

bool CQueue_2s::Read(char** str1, char** str2)
{
	if (Pointer == NULL) return false;

	*str1 = Pointer->str1;
	*str2 = Pointer->str2;

	Pointer = Pointer->Next;

	return true;
}
