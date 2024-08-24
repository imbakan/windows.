
#pragma once

typedef struct NODE_A NODE_A;

struct NODE_A
{
	wchar_t* str;
	long long value;
	NODE_A* Next;
};

class CQueue_a
{
private:
	NODE_A* First, * Last;
	int Count;

public:
	CQueue_a();
	~CQueue_a();

	int GetCount();
	bool IsEmpty();
	void Add(wchar_t* str, long long value);
	void Remove(wchar_t* str, size_t n, long long* value);
};
