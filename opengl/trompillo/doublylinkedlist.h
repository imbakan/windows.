
#pragma once

typedef struct NODE NODE;

struct NODE
{
	double x, y, vx, vy;
	NODE* Prev;
	NODE* Next;
};

class CDoublyLinkedList
{
private:
	NODE* First, * Last, * Pointer;
	int Count;

public:
	CDoublyLinkedList();
	~CDoublyLinkedList();

	int GetCount();
	bool IsEmpty();
	void Prefix(NODE* Node2, double x, double y, double vx, double vy);
	void Suffix(NODE* Node1, double x, double y, double vx, double vy);
	void Add(double x, double y, double vx, double vy);
	void Remove(NODE* Node);
	void Clear();

	void Reset();
	bool Read(float* x, float* y);
	bool Read(NODE** Node);
};
