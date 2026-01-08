
#include "framework.h"
#include "circleprogressbar.h"

// constructor
CCircleProgressBar::CCircleProgressBar()
{
	percent = 0;
}

// destructor
CCircleProgressBar::~CCircleProgressBar()
{

}

//
double CCircleProgressBar::Resize(int width, int height)
{
	int r;

	r = min(width, height) / 4;
	x = width / 2;
	y = height / 2;

	left = x - r;
	right = x + r;
	top = y - r;
	bottom = y + r;

	// The x - coordinate and y - coordinate, in logical units,
	// of the endpoint of the radial defining the starting point of the arc.
	xr1 = x + r;
	yr1 = y;

	// The x-coordinate and y - coordinate, in logical units,
	// of the endpoint of the radial defining the ending point of the arc.
	xr2 = x + r;
	yr2 = y;

	rect.left = left - 5;
	rect.right = right + 5;
	rect.top = top - 5;
	rect.bottom = bottom + 5;

	radius = (double)r;

	return radius;
}

//
void CCircleProgressBar::Draw(HDC hDC)
{
	HPEN hPen;
	HFONT hFont;
	wchar_t str[100];

	hFont = CreateFont(-32, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, L"Courier New");
	hPen = CreatePen(PS_SOLID, 6, RGB(0, 0, 255));

	//Rectangle(hDC, left, top, right, bottom);

	SelectObject(hDC, hPen);
	SelectObject(hDC, hFont);

	MoveToEx(hDC, xr1, yr1, NULL);
	ArcTo(hDC, left, top, right, bottom, xr1, yr1, xr2, yr2);

	swprintf_s(str, 100, L"%3d%%", percent);

	SetBkMode(hDC, TRANSPARENT);
	SetTextAlign(hDC, TA_BASELINE | TA_CENTER);
	SetTextColor(hDC, RGB(0, 0, 255));
	TextOut(hDC, x, y, str, (int)wcslen(str));

	DeleteObject(hFont);
	DeleteObject(hPen);
}

void CCircleProgressBar::Reset(HWND hWnd)
{
	this->hWnd = hWnd;
	a1 = 0;
}

//                         (xr2, yr2)
//              +----------o--
//              |\
//              |a\
//              |  \
//              |   \
//              |    o (xr1, yr1)
//              |     \

void CCircleProgressBar::Set(double factor)
{
	double a, angle;
	int a2;

	angle = 360.0 * factor;

	a2 = (int)angle;

	if (a2 > a1) {

		percent = (int)(100.0 * factor);

		a = M_PI * ((double)angle / 180.0);

		xr1 = x + (int)(radius * cos(a));
		yr1 = y + (int)(radius * sin(a));

		InvalidateRect(hWnd, &rect, true);

		a1 = a2;
	}

}
