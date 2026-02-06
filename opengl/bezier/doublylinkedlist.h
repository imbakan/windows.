
#include "line.h"

#pragma once

typedef struct NODE NODE;

struct NODE
{
	double x, y, w;
	NODE* Prev;
	NODE* Next;
};

class CDoublyLinkedList
{
private:
	NODE *First, *Last, *Pointer;
	int Count;

public:
	CDoublyLinkedList();
	~CDoublyLinkedList();

	int GetCount();
	bool IsEmpty();

	NODE* Add(NODE* Node1, double x, double y, double w);
	NODE* Add(double x, double y, double w);

	void Remove(NODE* Node);

	void Clear();

	void Reset();
	bool Read(NODE** Node);

};
