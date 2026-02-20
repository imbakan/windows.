
#pragma once

#include "resource.h"

class CSpinRateDialog
{
private:
	HWND hDialog;
	int nMin, nMax, nPage;
	double spin_rate;

	void OnInitDialog(HWND hWnd);
	void OnHScroll(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void OnClose(HWND hWnd);

public:
	CSpinRateDialog();
	~CSpinRateDialog();

	void Show(HWND hWnd, HINSTANCE hInstance, DLGPROC pProc, double spin_rate);
	INT_PTR Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

