
#pragma once

typedef struct QUEUE QUEUE;

struct QUEUE
{
	char* str;
	QUEUE* Next;
};

class CQueue
{
private:
	QUEUE* First, * Last, * Pointer;
	int Count;

public:
	CQueue();
	~CQueue();

	int GetCount();
	bool IsEmpty();
	void Add(char* str);
	void Remove(char** str);

	void Reset();
	bool Read(QUEUE** Node);

	void Clear();
};

