
#pragma once

typedef struct NODE_I NODE_I;

struct NODE_I
{
	int value;
	NODE_I* Next;
};

class CQueue_i
{
private:
	NODE_I* First, * Last;
	int Count;

public:
	CQueue_i();
	~CQueue_i();

	int GetCount();
	bool IsEmpty();
	void Add(int value);
	void Remove(int* value);
	void Push(int value);
	void Clear();
};
