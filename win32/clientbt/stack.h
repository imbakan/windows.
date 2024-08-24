
#pragma once

typedef struct NODE_K NODE_K;

struct NODE_K
{
	wchar_t* str;
	NODE_K* Next;
};

class CStack
{
private:
	NODE_K* First;
	int Count;

public:
	CStack();
	~CStack();

	int GetCount();
	bool IsEmpty();
	void Push(wchar_t* str);
	void Pop(wchar_t* str, size_t n);

};

