
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "queue1.h"

CQueue1::CQueue1()
{
	First = Last = NULL;
	Count = 0;
}

CQueue1::~CQueue1()
{
	Clear();
}

void CQueue1::CapString(char* str)
{
	size_t i, n;

	n = strlen(str);

	for (i = 0; i < n; i++)
		str[i] = toupper(str[i]);
}

int CQueue1::GetCount()
{
	return Count;
}

bool CQueue1::IsEmpty()
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
void CQueue1::Add(char* str)
{
	NODE1* Node;
	size_t n;

	Node = new NODE1;

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
void CQueue1::Remove(char** str)
{
	NODE1* Node;
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

void CQueue1::Reset()
{
	Pointer = First;
}

bool CQueue1::Read(NODE1** Node)
{
	if (Pointer == NULL) return false;

	*Node = Pointer;

	Pointer = Pointer->Next;

	return true;
}

bool CQueue1::Find(char* str)
{
	NODE1* Node;
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

void CQueue1::Clear()
{
	char* str;

	while (!IsEmpty()) {
		Remove(&str);
		delete[] str;
	}
}

void CQueue1::Peek(NODE1** Node)
{
	*Node = First;
}
