
#pragma once

typedef struct NODE1 NODE1;

struct NODE1
{
	char* str;
	NODE1* Next;
};

class CQueue1
{
private:
	NODE1* First, * Last, * Pointer;
	int Count;

	void CapString(char* str);

public:
	CQueue1();
	~CQueue1();

	int GetCount();
	bool IsEmpty();
	void Add(char* str);
	void Remove(char** str);

	void Reset();
	bool Read(NODE1** Node);

	bool Find(char* str);

	void Clear();

	void Peek(NODE1** Node);
};

