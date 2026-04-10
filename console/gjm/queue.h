
#pragma once

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

public:
	CQueue();
	~CQueue();

	int GetCount();
	bool IsEmpty();
	void Add(char* str);
	void Remove(char* str, size_t n);
	void Remove();
	void Clear();

};

