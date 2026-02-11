#pragma once

#include "resource.h"

class COpacityDialog
{
private:
	HWND hDialog;
	int percent, nMax;

	void OnInitDialog(HWND hWnd);
	void OnHScroll(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void OnClose(HWND hWnd);

public:
	COpacityDialog();
	~COpacityDialog();

	void Show(HWND hWnd, HINSTANCE hInstance, DLGPROC pProc);
	INT_PTR Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Reset(HWND hWnd);

};

