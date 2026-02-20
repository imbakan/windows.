
#include "doublylinkedlist.h"

#pragma once

class CAnime
{
private:

	const double g = -9.8;
	const int N = 4;

	HWND hWnd;
	HANDLE thread, stopevent;
	DWORD id;

	double radius, spin_rate;
	double left, right, bottom, top;
	double* X, * Y, * Rx, * Ry;

	double* RotateZ(double a, double x, double y);

	static DWORD WINAPI Function(LPVOID lpParam);

public:

	CDoublyLinkedList list;

	CAnime();
	~CAnime();

	void Init(double radius);

	void Play(HWND hWnd);
	void Stop();

	void DoAnimation(float t);
	void SetBoundary(double left, double right, double bottom, double top);
	void SetSpinRate(double spin_rate);
	double GetSpinRate();
	void GenerateSprite();
	bool IsRunning();
};

