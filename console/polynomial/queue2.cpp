
#include <stdlib.h>
#include "queue2.h"

CQueue2::CQueue2()
{
	First = Last = NULL;
	Count = 0;
}

CQueue2::~CQueue2()
{
	Clear();
}

int CQueue2::GetCount()
{
	return Count;
}

bool CQueue2::IsEmpty()
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
void CQueue2::Add(int index, double value)
{
	NODE2* Node;

	Node = new NODE2;

	Node->index = index;
	Node->value = value;

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
void CQueue2::Remove(int* index, double* value)
{
	NODE2* Node;

	Node = First;

	*index = Node->index;
	*value = Node->value;

	First = First->Next;
	Count--;

	if (IsEmpty())
		Last = First;

	delete Node;
}

void CQueue2::Reset()
{
	Pointer = First;
}

bool CQueue2::Read(NODE2** Node)
{
	if (Pointer == NULL) return false;

	*Node = Pointer;

	Pointer = Pointer->Next;

	return true;
}

bool CQueue2::Find(int index)
{
	NODE2* Node;
	bool result;

	result = false;

	Node = First;

	while (Node != NULL) {

		if (Node->index == index) {

			result = true;
			break;
		}

		Node = Node->Next;
	}

	return result;
}

void CQueue2::Clear()
{
	int index;
	double value;

	while (!IsEmpty())
		Remove(&index, &value);
}

int CQueue2::GetMax()
{
	NODE2* Node;
	int max_index;

	max_index = 0;

	Node = First;

	while (Node != NULL) {

		if (Node->index > max_index) {
			max_index = Node->index;
		}

		Node = Node->Next;
	}

	return max_index;
}
