
#include "doublylinkedlist.h"

#pragma once

typedef struct QUEUE QUEUE;

struct QUEUE
{
	NODE* Node;
	QUEUE* Next;
};

class CQueue
{
private:
	QUEUE* First, * Last;
	int Count;

public:
	CQueue();
	~CQueue();

	int GetCount();
	bool IsEmpty();
	void Add(NODE* Node);
	void Remove(NODE** Node);
	void Clear();
};
