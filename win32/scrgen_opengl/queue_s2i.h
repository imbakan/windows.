
#pragma once

typedef struct NODE_S2I NODE_S2I;

struct NODE_S2I
{
	char* str;
	int i1, i2;
	NODE_S2I* Next;
};

class CQueue_s2i
{
private:
	NODE_S2I* First, * Last, * Pointer;
	int Count;

public:
	CQueue_s2i();
	~CQueue_s2i();

	int GetCount();
	bool IsEmpty();
	void Add(char* str, int i1, int i2);
	void Remove(char* str, size_t n, int* i1, int* i2);

	void Clear();

	void Reset();
	bool Read(char** str, int* i1, int* i2);
};
