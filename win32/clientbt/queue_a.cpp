
#include "framework.h"
#include "queue_a.h"

// constructor
CQueue_a::CQueue_a()
{
	First = Last = NULL;
	Count = 0;
}

// destructor
CQueue_a::~CQueue_a()
{
	wchar_t str[100];
	long long value;

	while (!IsEmpty())
		Remove(str, 100, &value);
}

//
int CQueue_a::GetCount()
{
	return Count;
}

//
bool CQueue_a::IsEmpty()
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
void CQueue_a::Add(wchar_t* str, long long value)
{
	NODE_A* Node;
	size_t n;

	Node = new NODE_A;

	n = wcslen(str) + 1;
	Node->str = new wchar_t[n];
	wcscpy_s(Node->str, n, str);
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

//               remove
//         <-----------------
//      First                                                                 Last
//     +----+              +----+    +----+    +----+    +----+    +----+    +----+
//     |    |--->          |    |--->|    |--->|    |--->|    |--->|    |--->|    |---> NULL
//     +----+              +----+    +----+    +----+    +----+    +----+    +----+
//
void CQueue_a::Remove(wchar_t* str, size_t n, long long* value)
{
	NODE_A* Node;

	Node = First;

	wcscpy_s(str, n, Node->str);
	*value = Node->value;

	First = First->Next;
	Count--;

	if (IsEmpty())
		Last = First;

	delete[] Node->str;
	delete Node;
}
