
#pragma once

#include "resource.h"

class CConnectDialog
{
private:
	char ip[20];
	char* p;

	void OnInitDialog(HWND hWnd);
	void OnOk(HWND hWnd, WPARAM wParam);
	void OnCancel(HWND hWnd, WPARAM wParam);

public:
	CConnectDialog(const char* ip);
	~CConnectDialog();

	char* Show(HWND hWnd, HINSTANCE hInstance, DLGPROC pProc);
	INT_PTR Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};
