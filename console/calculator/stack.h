
#include "item.h"

#pragma once

typedef struct NODE_S NODE_S;

struct NODE_S
{
	CItem item;
	NODE_S* Next;
};

class CStack
{
private:
	NODE_S* First;
	int Count;

public:
	CStack();
	~CStack();

	int GetCount();
	bool IsEmpty();
	void Push(CItem item);
	void Pop(CItem* item);

	void Peek(CItem* item);
};

