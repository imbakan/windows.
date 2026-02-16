
#pragma once

typedef struct NODE_2S NODE_2S;

struct NODE_2S
{
	char* str1;
	char* str2;
	NODE_2S* Next;
};

class CQueue_2s
{
private:
	NODE_2S* First, * Last, * Pointer;
	int Count;

public:
	CQueue_2s();
	~CQueue_2s();

	int GetCount();
	bool IsEmpty();
	void Add(char* str1, char* str2);
	void Remove(char* str1, char* str2, size_t n);

	void Clear();

	void Reset();
	bool Read(char** str1, char** str2);
};
