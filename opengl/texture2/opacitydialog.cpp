#include "framework.h"
#include "opacitydialog.h"

COpacityDialog::COpacityDialog()
{
    hDialog = NULL;
    percent = 0;
    nMax = 100;
}

COpacityDialog::~COpacityDialog()
{

}

void COpacityDialog::OnInitDialog(HWND hWnd)
{
    HWND hwnd;
    RECT rect1, rect2;
    int x, y;

    // kunin ang sukat ng parent window at dialog window
    hwnd = GetParent(hWnd);
    GetWindowRect(hwnd, &rect1);
    GetWindowRect(hWnd, &rect2);

    // igitna ang dialog
    x = rect1.left + ((rect1.right - rect1.left) - (rect2.right - rect2.left)) / 2;
    y = rect1.top + ((rect1.bottom - rect1.top) - (rect2.bottom - rect2.top)) / 2;

    // ignores ang size arguments      0, 0,
    SetWindowPos(hWnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);

    // i-set ang scrollbar parameter
    SCROLLINFO si;
    HWND hScrollBar;
    wchar_t str[100];

    hScrollBar = GetDlgItem(hWnd, IDC_SCROLLBAR1);
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = nMax;
    si.nPage = 1;
    si.nPos = percent;

    SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE);

    swprintf_s(str, 100, L"OpenGL %2d%%     Vulkan %2d%%", 100 - percent, percent);
    SetDlgItemText(hWnd, IDC_TEXT1, str);

}

void COpacityDialog::OnHScroll(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HWND hScrollBar = (HWND)lParam;
    SCROLLINFO si;
    wchar_t str[100];

    // kunin ang scrollbar position
    si.cbSize = sizeof(si);
    si.fMask = SIF_ALL;
    GetScrollInfo(hScrollBar, SB_CTL, &si);

    switch (LOWORD(wParam)) {
    case SB_LINELEFT:     si.nPos -= 1;            break;
    case SB_LINERIGHT:    si.nPos += 1;            break;
    case SB_PAGELEFT:     si.nPos -= si.nPage;     break;
    case SB_PAGERIGHT:    si.nPos += si.nPage;     break;
    case SB_THUMBTRACK:   si.nPos = si.nTrackPos;  break;
    default:
        break;
    }

    // para maislide ang scrollbar
    // pag wala nito babalik sa dating position ang slider
    si.fMask = SIF_POS;
    SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE);

    // kunin ang value at idisplay 'to
    if (GetDlgCtrlID(hScrollBar) == IDC_SCROLLBAR1) {
        if (si.nPos < 0) si.nPos = 0;
        if (si.nPos > nMax) si.nPos = nMax;
        percent = si.nPos;
    }

    swprintf_s(str, 100, L"OpenGL %2d%%     Vulkan %2d%%", 100 - percent, percent);
    SetDlgItemText(hWnd, IDC_TEXT1, str);

    float opacity = (float)percent / 100.0f;

    HWND hwnd = GetParent(hWnd);
    SendMessage(hwnd, WM_OPACITY, (WPARAM)0, (LPARAM)&opacity);
}

void COpacityDialog::OnClose(HWND hWnd)
{
    DestroyWindow(hWnd);
    hDialog = NULL;
}

void COpacityDialog::Show(HWND hWnd, HINSTANCE hInstance, DLGPROC pProc)
{
    if (IsWindow(hDialog)) return;

    hDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, pProc);
    ShowWindow(hDialog, SW_SHOW);
}

INT_PTR COpacityDialog::Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    HWND hwnd = GetParent(hWnd);

    switch (message)
    {
    case WM_INITDIALOG: OnInitDialog(hWnd); return (INT_PTR)TRUE;
    case WM_HSCROLL: OnHScroll(hWnd, wParam, lParam); break;
    case WM_CLOSE: OnClose(hWnd);
    }

    return (INT_PTR)FALSE;
}

void COpacityDialog::Reset(HWND hWnd)
{
    float opacity = (float)percent / 100.0f;
    SendMessage(hWnd, WM_OPACITY, (WPARAM)0, (LPARAM)&opacity);
}
