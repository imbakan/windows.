
#include "queue_2s.h"

#pragma once

typedef struct NODE NODE;

struct NODE
{
	char* str;
	CQueue_2s* que;
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
	CQueue_2s* Add(char* str);
	void Remove(char* str, size_t n);

	void Clear();

	void Reset();
	bool Read(char** str, CQueue_2s** que);
};
