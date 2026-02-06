
#pragma once

#include "resource.h"

class CWeightDialog
{
private:
	const int MIN_VALUE = -10000;
	const int MAX_VALUE = 10000;
	const int PAGE = 100;
	const double FACTOR = 100.0;

	HWND hDialog;
	int min, max, page, pos;
	double value;

	void OnInitDialog(HWND hWnd);
	void OnHScroll(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void OnClose(HWND hWnd);

public:
	CWeightDialog();
	~CWeightDialog();

	void Show(HWND hWnd, HINSTANCE hInstance, DLGPROC pProc, double value);
	INT_PTR Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Set(double value);
	void CloseWindow();
};

