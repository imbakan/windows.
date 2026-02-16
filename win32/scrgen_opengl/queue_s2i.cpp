
#include "framework.h"
#include "queue_s2i.h"

CQueue_s2i::CQueue_s2i()
{
	First = Last = NULL;
	Count = 0;
}

CQueue_s2i::~CQueue_s2i()
{
	Clear();
}

int CQueue_s2i::GetCount()
{
	return Count;
}

bool CQueue_s2i::IsEmpty()
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
void CQueue_s2i::Add(char* str, int i1, int i2)
{
	NODE_S2I* Node;
	size_t n;

	Node = new NODE_S2I;

	n = strlen(str) + 1;
	Node->str = new char[n];
	strcpy_s(Node->str, n, str);

	Node->i1 = i1;
	Node->i2 = i2;

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
void CQueue_s2i::Remove(char* str, size_t n, int* i1, int* i2)
{
	NODE_S2I* Node;

	Node = First;

	strcpy_s(str, n, Node->str);

	*i1 = Node->i1;
	*i2 = Node->i2;

	First = First->Next;
	Count--;

	if (IsEmpty())
		Last = First;

	delete[] Node->str;
	delete Node;
}

void CQueue_s2i::Clear()
{
	char str[MAX_STRING];
	int i1, i2;

	while (!IsEmpty())
		Remove(str, MAX_STRING, &i1, &i2);
}

void CQueue_s2i::Reset()
{
	Pointer = First;
}

bool CQueue_s2i::Read(char** str, int* i1, int* i2)
{
	if (Pointer == NULL) return false;

	*str = Pointer->str;
	*i1 = Pointer->i1;
	*i2 = Pointer->i2;

	Pointer = Pointer->Next;

	return true;
}
