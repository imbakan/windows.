
#include "framework.h"
#include "spinratedialog.h"

CSpinRateDialog::CSpinRateDialog()
{
    hDialog = NULL;
    nMin = 0;
    nMax = 80;
    nPage = 10;
    spin_rate = 0.0;
}

CSpinRateDialog::~CSpinRateDialog()
{

}

void CSpinRateDialog::OnInitDialog(HWND hWnd)
{
    HWND hwnd;
    RECT rect1, rect2;
    int x, y, nPos;

    nPos = (int)(spin_rate * 10.0);

    // center dialog window
    hwnd = GetParent(hWnd);
    GetWindowRect(hwnd, &rect1);
    GetWindowRect(hWnd, &rect2);

    x = rect1.left + ((rect1.right - rect1.left) - (rect2.right - rect2.left)) / 2;
    y = rect1.top + ((rect1.bottom - rect1.top) - (rect2.bottom - rect2.top)) / 2;

    SetWindowPos(hWnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);

    // set scrollbar parameter
    SCROLLINFO si;
    HWND hScrollBar;

    hScrollBar = GetDlgItem(hWnd, IDC_SCROLLBAR1);

    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = nMin + 1;
    si.nMax = nMax + nPage - 2;
    si.nPage = nPage;
    si.nPos = nPos;

    SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE);
}

void CSpinRateDialog::OnHScroll(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HWND hScrollBar = (HWND)lParam;
    SCROLLINFO si;

    // get scrollbar position
    si.cbSize = sizeof(si);
    si.fMask = SIF_ALL;
    GetScrollInfo(hScrollBar, SB_CTL, &si);

    switch (LOWORD(wParam)) {
    case SB_LINELEFT:     si.nPos -= 1;            break;
    case SB_LINERIGHT:    si.nPos += 1;            break;
    case SB_PAGELEFT:     si.nPos -= si.nPage;     break;
    case SB_PAGERIGHT:    si.nPos += si.nPage;     break;
    case SB_THUMBTRACK:   si.nPos = si.nTrackPos;  break;
    case SB_ENDSCROLL:    return;
    default:
        break;
    }

    si.fMask = SIF_POS;
    SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE);

    if (GetDlgCtrlID(hScrollBar) == IDC_SCROLLBAR1) {

        spin_rate = (double)si.nPos / 10.0;

        HWND hwnd = GetParent(hWnd);
        PostMessage(hwnd, WM_SPIN_RATE, (WPARAM)0, (LPARAM)&spin_rate);
    }
}

void CSpinRateDialog::OnClose(HWND hWnd)
{
    DestroyWindow(hWnd);
    hDialog = NULL;
}

void CSpinRateDialog::Show(HWND hWnd, HINSTANCE hInstance, DLGPROC pProc, double spin_rate)
{
    if (IsWindow(hDialog)) return;

    this->spin_rate = spin_rate;

    hDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, pProc);
    ShowWindow(hDialog, SW_SHOW);
}

INT_PTR CSpinRateDialog::Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG: OnInitDialog(hWnd); return (INT_PTR)TRUE;
    case WM_HSCROLL: OnHScroll(hWnd, wParam, lParam); return (INT_PTR)TRUE;
    case WM_CLOSE: OnClose(hWnd); return (INT_PTR)TRUE;
    }

    return (INT_PTR)FALSE;
}
