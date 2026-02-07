
#include "line2.h"

#pragma once

typedef struct NODE NODE;

struct NODE
{
	double x[2], y[2], m;
	int count;
	CLine2 spline;
	NODE* Prev;
	NODE* Next;
};

class CDoublyLinkedList
{
private:
	const double LENGTH = 0.05;

	NODE *First, *Last, *Pointer;
	int Count;

public:
	CDoublyLinkedList();
	~CDoublyLinkedList();

	int GetCount();
	bool IsEmpty();

	NODE* Add(NODE* Node1, double x, double y, double m, int count);
	NODE* Add(double x, double y, double m, int count);

	void Remove(NODE* Node);

	void Set(NODE* Node, double m);

	void Clear();

	void Reset();
	bool Read(NODE** Node);

};
