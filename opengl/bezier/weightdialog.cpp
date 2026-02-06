
#include "framework.h"
#include "weightdialog.h"

CWeightDialog::CWeightDialog()
{
}

CWeightDialog::~CWeightDialog()
{
}

void CWeightDialog::OnInitDialog(HWND hWnd)
{
	// center dialog
	//
	//          +----------------------------+
	//          |                            |
	//          |           rect1            |
	//          |                            |
	//          |   +--------------------+   |
	//          |   |                    |   |
	//          |   |       rect2        |   |
	//          |   |                    |   |
	//          |   +--------------------+   |
	//          |                            |
	//          |                            |
	//          +----------------------------+

	HWND hwnd;
	RECT rect1, rect2;
	int x, y;

	hwnd = GetParent(hWnd);
	GetWindowRect(hwnd, &rect1);
	GetWindowRect(hWnd, &rect2);

	x = rect1.left + ((rect1.right - rect1.left) - (rect2.right - rect2.left)) / 2;
	y = rect1.top + ((rect1.bottom - rect1.top) - (rect2.bottom - rect2.top)) / 2;

	SetWindowPos(hWnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);

	// set scrollbar parameter
	SCROLLINFO si;
	HWND hScrollBar;
	int range;
	wchar_t str[100];

	min = MIN_VALUE;
	max = MAX_VALUE;
	page = PAGE;
	pos = (int)(FACTOR * value);

	range = max - min;

	// nMin = min
	// nPage = page
	// nMax =  min + range + nPage - 1
	//
	hScrollBar = GetDlgItem(hWnd, IDC_SCROLLBAR1);
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nMin = min;
	si.nMax = min + range + page - 1;
	si.nPage = page;
	si.nPos = pos;

	SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE);

	value = (double)pos / FACTOR;

	swprintf_s(str, 100, L"%0.2f", value);
	SetDlgItemText(hWnd, IDC_TEXT1, str);
}

void CWeightDialog::OnHScroll(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HWND hScrollBar = (HWND)lParam;
	SCROLLINFO si;
	wchar_t str[100];
	bool valid;

	// kunin ang scrollbar position
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(hScrollBar, SB_CTL, &si);

	valid = true;

	switch (LOWORD(wParam)) {
	case SB_LINELEFT:     si.nPos -= 1;            break;
	case SB_LINERIGHT:    si.nPos += 1;            break;
	case SB_PAGELEFT:     si.nPos -= si.nPage;     break;
	case SB_PAGERIGHT:    si.nPos += si.nPage;     break;
	case SB_THUMBTRACK:   si.nPos = si.nTrackPos;  break;
	default: valid = false;
	}

	// para maislide ang scrollbar
	// pag wala nito babalik sa dating position ang slider
	si.fMask = SIF_POS;
	SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE);

	// ang value ay para isang beses lang ito maexecute
	if (GetDlgCtrlID(hScrollBar) == IDC_SCROLLBAR1 && valid) {

		pos = si.nPos;
		if (pos < min) pos = min;
		if (pos > max) pos = max;

		value = (double)pos / FACTOR;

		swprintf_s(str, 100, L"%6.2f", value);
		SetDlgItemText(hWnd, IDC_TEXT1, str);

		HWND hwnd = GetParent(hWnd);
		SendMessage(hwnd, WM_WEIGHT, (WPARAM)0, (LPARAM)&value);
	}
}

void CWeightDialog::OnClose(HWND hWnd)
{
	DestroyWindow(hWnd);
	hDialog = NULL;
}

void CWeightDialog::Show(HWND hWnd, HINSTANCE hInstance, DLGPROC pProc, double value)
{
	if (IsWindow(hDialog)) return;

	this->value = value;

	hDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, pProc);
	ShowWindow(hDialog, SW_SHOW);
}

void CWeightDialog::Set(double value)
{
	if (!IsWindow(hDialog)) return;

	HWND hScrollBar;
	SCROLLINFO si;
	wchar_t str[100];

	pos = (int)(FACTOR * value);

	hScrollBar = GetDlgItem(hDialog, IDC_SCROLLBAR1);
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	si.nPos = pos;

	SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE);

	swprintf_s(str, 100, L"%0.2f", value);
	SetDlgItemText(hDialog, IDC_TEXT1, str);
}

void CWeightDialog::CloseWindow()
{
	if (!IsWindow(hDialog)) return;
	PostMessage(hDialog, WM_CLOSE, 0, 0);
}

INT_PTR CWeightDialog::Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_INITDIALOG: OnInitDialog(hWnd);					return (INT_PTR)TRUE;
	case WM_HSCROLL:	OnHScroll(hWnd, wParam, lParam);	return (INT_PTR)TRUE;
	case WM_CLOSE:		OnClose(hWnd);
	}

	return (INT_PTR)FALSE;
}
