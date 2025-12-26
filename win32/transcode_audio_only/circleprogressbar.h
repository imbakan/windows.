
#pragma once

class CCircleProgressBar
{
private:
	HWND hWnd;
	RECT rect;
	int percent;
	int left, top, right, bottom, x, y, xr1, yr1, xr2, yr2, a1;
	double radius;

public:
	CCircleProgressBar();
	~CCircleProgressBar();

	double Resize(int width, int height);
	void Draw(HDC hDC);

	void Reset(HWND hWnd);
	void Set(double factor);
};

