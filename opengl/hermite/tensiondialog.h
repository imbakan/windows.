
#pragma once

#include "resource.h"

class CTensionDialog
{
private:
	const int MIN_VALUE = 0;
	const int MAX_VALUE = 1000;
	const int PAGE = 100;
	const double FACTOR = 1000.0;

	HWND hDialog;
	int min, max, page, pos;
	double value;

	void OnInitDialog(HWND hWnd);
	void OnHScroll(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void OnClose(HWND hWnd);

public:
	CTensionDialog();
	~CTensionDialog();

	void Show(HWND hWnd, HINSTANCE hInstance, DLGPROC pProc, double value);
	INT_PTR Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Set(double value);
	void CloseWindow();
};

