
#pragma once

#include "resource.h"

typedef struct {
	double x, y;
}POINT_DOUBLE;

class CClock
{
private:
	HWND hWnd;
	RECT rect;
	HANDLE thread, stopevent;
	DWORD id;
	int left1, top1, right1, bottom1;
	int left2, top2, right2, bottom2;
	double x, y;
	POINT hour_unit[12][4], min_unit[48][4], sec_hand[4], min_hand[4], hour_hand[4];
	POINT_DOUBLE p1[4], p2[4], p3[4];

	
	void Rotate(double* x, double* y, double a2, double b2);

	static DWORD WINAPI Function(LPVOID lpParam);

public:
	CClock();
	~CClock();

	void Create(int x, int y, int r);
	void Draw(HDC hDC);

	void Start(HWND hWnd);
	void Stop();
};

