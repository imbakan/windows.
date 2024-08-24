
#include "framework.h"
#include "queue_s.h"

//  constructor
CQueue_s::CQueue_s()
{
	First = Last = NULL;
	Count = 0;
}

// destructor
CQueue_s::~CQueue_s()
{
	wchar_t str[MAX_PATH];

	while (!IsEmpty())
		Remove(str, MAX_PATH);
}

//
int CQueue_s::GetCount()
{
	return Count;
}

//
bool CQueue_s::IsEmpty()
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
void CQueue_s::Add(wchar_t* str)
{
	NODE_S* Node;
	size_t n;

	Node = new NODE_S;

	n = wcslen(str) + 1;
	Node->str = new wchar_t[n];
	wcscpy_s(Node->str, n, str);

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
void CQueue_s::Remove(wchar_t* str, size_t n)
{
	NODE_S* Node;

	Node = First;

	wcscpy_s(str, n, Node->str);

	First = First->Next;
	Count--;

	if (IsEmpty())
		Last = First;

	delete[] Node->str;
	delete Node;
}

void CQueue_s::GetFirstNode(wchar_t* str, size_t n)
{
	wcscpy_s(str, n, First->str);
}

void CQueue_s::SetFirstNode(wchar_t* str)
{
	NODE_S* Node;
	size_t n;

	Node = First;

	delete[] Node->str;

	n = wcslen(str) + 1;
	Node->str = new wchar_t[n];
	wcscpy_s(Node->str, n, str);
}
