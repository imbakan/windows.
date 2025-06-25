
#pragma once

typedef struct NODE_S NODE_S;

struct NODE_S
{
	wchar_t* str;
	NODE_S* Next;
};

class CQueue_s
{
private:
	NODE_S* First, * Last;
	int Count;

public:
	CQueue_s();
	~CQueue_s();

	int GetCount();
	bool IsEmpty();
	void Add(wchar_t* str);
	void Remove(wchar_t* str, size_t n);
};
