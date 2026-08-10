
#pragma once

typedef struct NODE2 NODE2;

struct NODE2
{
	int index;
	double value;
	NODE2* Next;
};

class CQueue2
{
private:
	NODE2* First, * Last, * Pointer;
	int Count;

public:
	CQueue2();
	~CQueue2();

	int GetCount();
	bool IsEmpty();
	void Add(int index, double value);
	void Remove(int* index, double* value);

	void Reset();
	bool Read(NODE2** Node);

	bool Find(int index);

	void Clear();

	int GetMax();
};

