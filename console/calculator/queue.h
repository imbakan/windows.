
#include "item.h"

#pragma once

typedef struct NODE_Q NODE_Q;

struct NODE_Q
{
	CItem item;
	NODE_Q* Next;
};

class CQueue
{
private:
	NODE_Q* First, * Last;
	int Count;

public:
	CQueue();
	~CQueue();

	int GetCount();
	bool IsEmpty();
	void Add(CItem item);
	void Remove(CItem* item);
};

