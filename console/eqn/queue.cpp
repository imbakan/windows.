
#include <ctype.h>
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

void CQueue::CapString(char* str)
{
	size_t i, n;

	n = strlen(str);

	for (i = 0; i < n; i++)
		str[i] = toupper(str[i]);
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
	NODE* Node;
	size_t n;

	Node = new NODE;

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
	NODE* Node;
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

bool CQueue::Read(NODE** Node)
{
	if (Pointer == NULL) return false;

	*Node = Pointer;

	Pointer = Pointer->Next;

	return true;
}

bool CQueue::Find(char* str)
{
	NODE* Node;
	char str1[100], str2[100];
	bool result;

	strcpy_s(str2, 100, str);
	CapString(str2);

	result = false;

	Reset();

	while (Read(&Node)) {

		strcpy_s(str1, 100, Node->str);
		CapString(str1);

		if (strcmp(str1, str2) == 0) {
			result = true;
			break;
		}
	}

	return result;
}

void CQueue::Clear()
{
	char* str;

	while (!IsEmpty()) {
		Remove(&str);
		delete[] str;
	}
}
