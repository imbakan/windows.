
#pragma once

enum Token { NUMBER, ALPHA, SIGN, EQUAL };

typedef struct NODE NODE;

struct NODE
{
	char* str;
	Token token;
	NODE* Next;
};

class CQueue
{
private:
	NODE* First, * Last, *Pointer;
	int Count;

	void CapString(char* str);

public:
	CQueue();
	~CQueue();

	int GetCount();
	bool IsEmpty();
	void Add(char* str, Token token);
	void Remove(char* str, int n);

	void Reset();
	bool Read(NODE** Node);

	bool Find(char* str);

	void Clear();
};

