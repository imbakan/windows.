
#include "framework.h"
#include "connectdialog.h"

CConnectDialog::CConnectDialog(const char* ip)
{
	strcpy_s(this->ip, 20, ip);
	p = NULL;
}

CConnectDialog::~CConnectDialog()
{

}

void CConnectDialog::OnInitDialog(HWND hWnd)
{
	HWND hwnd;
	RECT rect1, rect2;
	int x, y;

	hwnd = GetParent(hWnd);
	GetWindowRect(hwnd, &rect1);
	GetWindowRect(hWnd, &rect2);

	x = rect1.left + ((rect1.right - rect1.left) - (rect2.right - rect2.left)) / 2;
	y = rect1.top + ((rect1.bottom - rect1.top) - (rect2.bottom - rect2.top)) / 2;

	SetWindowPos(hWnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);

	HWND hEdit = GetDlgItem(hWnd, IDC_EDIT4);
	SetWindowTextA(hEdit, ip);
}

void CConnectDialog::OnOk(HWND hWnd, WPARAM wParam)
{
	HWND hEdit = GetDlgItem(hWnd, IDC_EDIT4);
	GetWindowTextA(hEdit, ip, 20);

	p = ip;

	EndDialog(hWnd, LOWORD(wParam));
}

void CConnectDialog::OnCancel(HWND hWnd, WPARAM wParam)
{
	p = NULL;

	EndDialog(hWnd, LOWORD(wParam));
}

char* CConnectDialog::Show(HWND hWnd, HINSTANCE hInstance, DLGPROC pProc)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, pProc);

	return p;
}

INT_PTR CConnectDialog::Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (message) {
	case WM_INITDIALOG:
		OnInitDialog(hWnd);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			OnOk(hWnd, wParam);
			return (INT_PTR)TRUE;

		case IDCANCEL:
			OnCancel(hWnd, wParam);
			return (INT_PTR)TRUE;
		}
		break;
	}

	return (INT_PTR)FALSE;
}
