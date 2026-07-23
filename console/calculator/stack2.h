
#pragma once

typedef struct NODE NODE;

struct NODE
{
	double value;
	NODE* Next;
};

class CStack2
{
private:
	NODE* First;
	int Count;

public:
	CStack2();
	~CStack2();

	int GetCount();
	bool IsEmpty();
	void Push(double value);
	void Pop(double* value);

	void Clear();
};

