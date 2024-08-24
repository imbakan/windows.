
#include "queue_s.h"

#pragma once

#include "resource.h"

class CConnectDialog
{
private:
	wchar_t str[BTH_MAX_NAME_SIZE];
	wchar_t* p;

	void GetBluetoothDevices(CQueue_s* que);

	void OnInitDialog(HWND hWnd);
	void OnOk(HWND hWnd, WPARAM wParam);
	void OnCancel(HWND hWnd, WPARAM wParam);
	void OnItemChanged(HWND hWnd, LPARAM lParam);
	void OnSize(HWND hWnd, int width, int height);

public:
	CConnectDialog();
	~CConnectDialog();

	wchar_t* Show(HWND hWnd, HINSTANCE hInstance, DLGPROC pProc);
	INT_PTR Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};
