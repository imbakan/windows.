
#pragma once

typedef struct STACK STACK;

struct STACK
{
	char* str;
	STACK* Next;
};

class CStack
{
private:
	STACK* First, *Pointer;
	int Count;

public:
	CStack();
	~CStack();

	int GetCount();
	bool IsEmpty();
	void Push(char* str);
	void Pop(char** str);

	void Peek(char** str);

	void Reset();
	bool Read(STACK** Node);

	void Clear();
};

