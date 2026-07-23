
#pragma once

typedef struct NODE NODE;

struct NODE
{
	char* str;
	NODE* Next;
};

class CQueue
{
private:
	NODE* First, * Last, * Pointer;
	int Count;

	void CapString(char* str);

public:
	CQueue();
	~CQueue();

	int GetCount();
	bool IsEmpty();
	void Add(char* str);
	void Remove(char** str);

	void Reset();
	bool Read(NODE** Node);

	bool Find(char* str);

	void Clear();
};

