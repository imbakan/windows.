
#include "framework.h"
#include "clock.h"

// constructor
CClock::CClock()
{
	thread = NULL;
	stopevent = NULL;
	id = 0;

	 // gawing unique ang pangalan ng event
	stopevent = CreateEvent(NULL, TRUE, FALSE, L"ClockStopEvent");
	if (stopevent == NULL) {
		OutputDebugString(L"Create event failed.\n");
	}
}

// destructor
CClock::~CClock()
{
	if (thread != NULL)    CloseHandle(thread);
	if (stopevent != NULL) CloseHandle(stopevent);
}

// multiplication ng complex number
// ito ay gagamitin ko para sa rotation
// A = a1 + b1j
// B = a2 + b2j
// R = A * B
//   = (a1 + b1j) * (a2 + b2j)
//   = a1a2 + a2b1j + b2a1j + b1b2j^2
//   = (a1a2 - b1b2) + (a2b1 + a1b2)j
//
void CClock::Rotate(double* x, double* y, double a2, double b2)
{
	double s, a1, b1, a, b, r;

	a1 = *x;
	b1 = *y;

	s = sqrt(a1 * a1 + b1 * b1);
	a1 /= s;
	b1 /= s;

	a = a1 * a2 - b1 * b2;
	b = a2 * b1 + a1 * b2;

	r = sqrt(a * a + b * b);
	a /= r;
	b /= r;

	*x = s * a;
	*y = s * b;
}

//
void CClock::Create(int x, int y, int r)
{
	int i, k;
	double rad, rx, ry, c, w, h, x1, y1, x2, y2;
	POINT_DOUBLE p[4];

	this->x = (double)x;
	this->y = (double)y;

	left1 = x - 20;    right1  = x + 20;
	top1  = y - 20;    bottom1 = y + 20;

	left2 = x - r;    right2  = x + r;
	top2  = y - r;    bottom2 = y + r;
	
	rect.left   = left2;
	rect.right  = right2;
	rect.top    = top2;
	rect.bottom = bottom2;

	// HOUR UNIT
	//                                        r -------
	//                                             ^
	//                                             |
	//                                             7
	//                                             |
	//                                             v
	//         y2 +-----+                   r-7 +-----+
	//            |     |                       |     |
	//            h     |                       h     |
	//            |     |                       |     |
	//         y1 +--w--+                 r-7-h +--w--+
	//            x1    x2                    -w/2   w/2

	// hour unit
	c = 2.0 * M_PI * (double)r;
	w = c / 150.0;
	h = (double)r / 10.0;

	// 360 / 12 = 30
	rad = (30.0 / 180.0) * M_PI;
	rx = cos(rad);
	ry = sin(rad);

	x1 = -w / 2.0;
	y1 = r - 7.0 - h;

	x2 = w / 2.0;
	y2 = r - 7.0;

	p[0].x = x1;     p[0].y = y2;
	p[1].x = x2;     p[1].y = y2;
	p[2].x = x2;     p[2].y = y1;
	p[3].x = x1;     p[3].y = y1;

	for (i = 0; i < 12; i++) {

		Rotate(&p[0].x, &p[0].y, rx, ry);
		Rotate(&p[1].x, &p[1].y, rx, ry);
		Rotate(&p[2].x, &p[2].y, rx, ry);
		Rotate(&p[3].x, &p[3].y, rx, ry);

		hour_unit[i][0].x = (LONG)(x + p[0].x);     hour_unit[i][0].y = (LONG)(y + p[0].y);
		hour_unit[i][1].x = (LONG)(x + p[1].x);     hour_unit[i][1].y = (LONG)(y + p[1].y);
		hour_unit[i][2].x = (LONG)(x + p[2].x);     hour_unit[i][2].y = (LONG)(y + p[2].y);
		hour_unit[i][3].x = (LONG)(x + p[3].x);     hour_unit[i][3].y = (LONG)(y + p[3].y);
	}

	// MINUTE UNIT
	//                                        r -------
	//                                             ^
	//                                             |
	//                                             8
	//                                             |
	//                                             v
	//         y2 +-----+                   r-8 +-----+
	//            |     |                       |     |
	//            h     |                       h     |
	//            |     |                       |     |
	//         y1 +--w--+                 r-8-h +--w--+
	//            x1    x2                    -w/2   w/2

	// minute unit
	w = c / 600.0;
	h = (double)r / 25.0;

	// 360 / 60 = 6
	rad = (6.0 / 180.0) * M_PI;
	rx = cos(rad);
	ry = sin(rad);

	x1 = -w / 2.0;
	y1 = r - 8.0 - h;

	x2 = w / 2.0;
	y2 = r - 8.0;

	p[0].x = x1;     p[0].y = y2;
	p[1].x = x2;     p[1].y = y2;
	p[2].x = x2;     p[2].y = y1;
	p[3].x = x1;     p[3].y = y1;

	k = 0;

	for (i = 0; i < 60; i++) {

		Rotate(&p[0].x, &p[0].y, rx, ry);
		Rotate(&p[1].x, &p[1].y, rx, ry);
		Rotate(&p[2].x, &p[2].y, rx, ry);
		Rotate(&p[3].x, &p[3].y, rx, ry);

		if (((i + 1) % 5) == 0) continue;

		min_unit[k][0].x = (LONG)(x + p[0].x);     min_unit[k][0].y = (LONG)(y + p[0].y);
		min_unit[k][1].x = (LONG)(x + p[1].x);     min_unit[k][1].y = (LONG)(y + p[1].y);
		min_unit[k][2].x = (LONG)(x + p[2].x);     min_unit[k][2].y = (LONG)(y + p[2].y);
		min_unit[k][3].x = (LONG)(x + p[3].x);     min_unit[k][3].y = (LONG)(y + p[3].y);

		++k;
	}

	// SECOND HAND
	// 
	//         y2 +-----+
	//            |     |
	//            r     |
	//            |     |
	//            .  +  .
	//            |     |
	//          0.3r    |
	//            |     |
	//         y1 +-2.0-+
	//            x1    x2

	x1 = -1.0;
	y1 = 0.3 * r;

	x2 = 1.0;
	y2 = -r;

	p1[0].x = x1;     p1[0].y = y2;
	p1[1].x = x2;     p1[1].y = y2;
	p1[2].x = x2;     p1[2].y = y1;
	p1[3].x = x1;     p1[3].y = y1;

	sec_hand[0].x = (LONG)(x + p1[0].x);     sec_hand[0].y = (LONG)(y + p1[0].y);
	sec_hand[1].x = (LONG)(x + p1[1].x);     sec_hand[1].y = (LONG)(y + p1[1].y);
	sec_hand[2].x = (LONG)(x + p1[2].x);     sec_hand[2].y = (LONG)(y + p1[2].y);
	sec_hand[3].x = (LONG)(x + p1[3].x);     sec_hand[3].y = (LONG)(y + p1[3].y);

	// MINUTE HAND
	// 
	//         y2 +-----+
	//            |     |
	//          0.97r   |
	//            |     |
	//            .  +  .
	//            |     |
	//          0.2r    |
	//            |     |
	//         y1 +--w--+
	//            x1    x2

	w = c / 160.0;

	x1 = -w / 2.0;;
	y1 = 0.2 * r;

	x2 = w / 2.0;
	y2 = -0.97 * r;

	p2[0].x = x1+1.5;     p2[0].y = y2;
	p2[1].x = x2-1.5;     p2[1].y = y2;
	p2[2].x = x2;         p2[2].y = y1;
	p2[3].x = x1;         p2[3].y = y1;

	min_hand[0].x = (LONG)(x + p2[0].x);     min_hand[0].y = (LONG)(y + p2[0].y);
	min_hand[1].x = (LONG)(x + p2[1].x);     min_hand[1].y = (LONG)(y + p2[1].y);
	min_hand[2].x = (LONG)(x + p2[2].x);     min_hand[2].y = (LONG)(y + p2[2].y);
	min_hand[3].x = (LONG)(x + p2[3].x);     min_hand[3].y = (LONG)(y + p2[3].y);

	// HOUR HAND
	// 
	//         y2 +-----+
	//            |     |
	//          0.65r   |
	//            |     |
	//            .  +  .
	//            |     |
	//          0.2r    |
	//            |     |
	//         y1 +--w--+
	//            x1    x2

	w = c / 100.0;

	x1 = -w / 2.0;;
	y1 = 0.2 * r;

	x2 = w / 2.0;
	y2 = -0.65 * r;

	p3[0].x = x1+3.0;     p3[0].y = y2;
	p3[1].x = x2-3.0;     p3[1].y = y2;
	p3[2].x = x2;         p3[2].y = y1;
	p3[3].x = x1;         p3[3].y = y1;

	hour_hand[0].x = (LONG)(x + p3[0].x);     hour_hand[0].y = (LONG)(y + p3[0].y);
	hour_hand[1].x = (LONG)(x + p3[1].x);     hour_hand[1].y = (LONG)(y + p3[1].y);
	hour_hand[2].x = (LONG)(x + p3[2].x);     hour_hand[2].y = (LONG)(y + p3[2].y);
	hour_hand[3].x = (LONG)(x + p3[3].x);     hour_hand[3].y = (LONG)(y + p3[3].y);

}

//
void CClock::Draw(HDC hDC)
{
	HBRUSH hBrush1, hBrush2, hBrush3, hBrush4;
	HPEN hPen1, hPen2, hPen3;
	int i;

	// gumawa ng mga pen at mga brush
	hBrush1 = CreateSolidBrush(RGB(0, 0, 0));
	hBrush2 = CreateSolidBrush(RGB(0x60, 0x60, 0x60));
	hBrush3 = CreateSolidBrush(RGB(0x80, 0x80, 0x80));
	hBrush4 = CreateSolidBrush(RGB(0xff, 0xff, 0xff));
	
	hPen1 = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	hPen2 = CreatePen(PS_SOLID, 1, RGB(0x40, 0x40, 0x40));
	hPen3 = CreatePen(PS_SOLID, 4, RGB(0x60, 0x60, 0x60));

	// outer circle
	SelectObject(hDC, hPen3);
	SelectObject(hDC, hBrush4);

	Ellipse(hDC, left2, top2, right2, bottom2);

	SelectObject(hDC, hPen2);
	SelectObject(hDC, hBrush3);

	// hour unit
	for (i = 0; i < 12; i++)
		Polygon(hDC, &hour_unit[i][0], 4);

	// minute unit
	for (i = 0; i < 48; i++)
		Polygon(hDC, &min_unit[i][0], 4);

	SelectObject(hDC, hPen1);
	SelectObject(hDC, hBrush2);

	// hour hand
	Polygon(hDC, hour_hand, 4);

	// minute hand
	Polygon(hDC, min_hand, 4);

	// second hand
	Polygon(hDC, sec_hand, 4);

	// inner circle
	SelectObject(hDC, hPen1);
	SelectObject(hDC, hBrush3);

	Ellipse(hDC, left1, top1, right1, bottom1);

	// irelease ang lahat na resources
	DeleteObject(hPen1);
	DeleteObject(hPen2);
	DeleteObject(hPen3);
	DeleteObject(hBrush1);
	DeleteObject(hBrush2);
	DeleteObject(hBrush3);
	DeleteObject(hBrush4);
}

//
void CClock::Start(HWND hWnd)
{
	this->hWnd = hWnd;

	ResetEvent(stopevent);

	if (thread != NULL) CloseHandle(thread);

	thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Function, this, 0, &id);
}

//
void CClock::Stop()
{
	SetEvent(stopevent);
}

//
DWORD WINAPI CClock::Function(LPVOID lpParam)
{
	CClock* p = (CClock*)lpParam;
	HMENU hMenu;
	DWORD delay, result;
	double a, rad, r1x, r1y, r2x, r2y, r3x, r3y;

	delay = 1;

	// second hand
	// 1 complete rotation per 360 unit
	a = 360.0 / 360.0;
	rad = (a / 180.0) * M_PI;
	r1x = cos(rad);
	r1y = sin(rad);

	// minute hand
	// 1 complete rotation per (360 * 60) 21600 unit time
	a = 360.0 / 21600.0;
	rad = (a / 180.0) * M_PI;
	r2x = cos(rad);
	r2y = sin(rad);

	// hour hand
	// 1 complete rotation per (360 * 60 * 12) 259200 unit time
	a = 360.0 / 259200.0;
	rad = (a / 180.0) * M_PI;
	r3x = cos(rad);
	r3y = sin(rad);

	hMenu = GetMenu(p->hWnd);
	EnableMenuItem(hMenu, IDM_START, MF_DISABLED);
	EnableMenuItem(hMenu, IDM_STOP, MF_ENABLED);

	while (true) {

		result = WaitForSingleObject(p->stopevent, delay);
		if (result == WAIT_OBJECT_0) break;

		// second hand
		p->Rotate(&p->p1[0].x, &p->p1[0].y, r1x, r1y);
		p->Rotate(&p->p1[1].x, &p->p1[1].y, r1x, r1y);
		p->Rotate(&p->p1[2].x, &p->p1[2].y, r1x, r1y);
		p->Rotate(&p->p1[3].x, &p->p1[3].y, r1x, r1y);

		p->sec_hand[0].x = (LONG)(p->x + p->p1[0].x);     p->sec_hand[0].y = (LONG)(p->y + p->p1[0].y);
		p->sec_hand[1].x = (LONG)(p->x + p->p1[1].x);     p->sec_hand[1].y = (LONG)(p->y + p->p1[1].y);
		p->sec_hand[2].x = (LONG)(p->x + p->p1[2].x);     p->sec_hand[2].y = (LONG)(p->y + p->p1[2].y);
		p->sec_hand[3].x = (LONG)(p->x + p->p1[3].x);     p->sec_hand[3].y = (LONG)(p->y + p->p1[3].y);

		// minute hand
		p->Rotate(&p->p2[0].x, &p->p2[0].y, r2x, r2y);
		p->Rotate(&p->p2[1].x, &p->p2[1].y, r2x, r2y);
		p->Rotate(&p->p2[2].x, &p->p2[2].y, r2x, r2y);
		p->Rotate(&p->p2[3].x, &p->p2[3].y, r2x, r2y);

		p->min_hand[0].x = (LONG)(p->x + p->p2[0].x);     p->min_hand[0].y = (LONG)(p->y + p->p2[0].y);
		p->min_hand[1].x = (LONG)(p->x + p->p2[1].x);     p->min_hand[1].y = (LONG)(p->y + p->p2[1].y);
		p->min_hand[2].x = (LONG)(p->x + p->p2[2].x);     p->min_hand[2].y = (LONG)(p->y + p->p2[2].y);
		p->min_hand[3].x = (LONG)(p->x + p->p2[3].x);     p->min_hand[3].y = (LONG)(p->y + p->p2[3].y);

		// hour hand
		p->Rotate(&p->p3[0].x, &p->p3[0].y, r3x, r3y);
		p->Rotate(&p->p3[1].x, &p->p3[1].y, r3x, r3y);
		p->Rotate(&p->p3[2].x, &p->p3[2].y, r3x, r3y);
		p->Rotate(&p->p3[3].x, &p->p3[3].y, r3x, r3y);

		p->hour_hand[0].x = (LONG)(p->x + p->p3[0].x);     p->hour_hand[0].y = (LONG)(p->y + p->p3[0].y);
		p->hour_hand[1].x = (LONG)(p->x + p->p3[1].x);     p->hour_hand[1].y = (LONG)(p->y + p->p3[1].y);
		p->hour_hand[2].x = (LONG)(p->x + p->p3[2].x);     p->hour_hand[2].y = (LONG)(p->y + p->p3[2].y);
		p->hour_hand[3].x = (LONG)(p->x + p->p3[3].x);     p->hour_hand[3].y = (LONG)(p->y + p->p3[3].y);

		InvalidateRect(p->hWnd, &p->rect, FALSE);

	}

	EnableMenuItem(hMenu, IDM_START, MF_ENABLED);
	EnableMenuItem(hMenu, IDM_STOP, MF_DISABLED);

	return 0;
}
