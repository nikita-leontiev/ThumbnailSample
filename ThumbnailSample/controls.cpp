#include "stdafx.h"
#include "utils.h"
#include "controls.h"

CWnd::CWnd() :
m_hWnd(NULL),
m_pOldProc(NULL)
{
}

HWND CWnd::GetHWND()
{
	return m_hWnd;
}

void CWnd::LoadFromDialog(HWND hDlg, int nID)
{
	m_hWnd = GetDlgItem(hDlg, nID);
	assert(m_hWnd);
}

void CWnd::Destroy()
{
	VERIFY(DestroyWindow(m_hWnd));
}

bool CWnd::IsWindow()
{
	return m_hWnd && ::IsWindow(m_hWnd);
}

DWORD CWnd::GetStyle()
{
	return static_cast<DWORD>(GetWindowLongPtr(m_hWnd, GWL_STYLE));
}

void CWnd::SetFont(HFONT hFont)
{
	SendMessage(m_hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
}

HFONT CWnd::GetFont()
{
	return reinterpret_cast<HFONT>(SendMessage(m_hWnd, WM_GETFONT, 0, 0));
}

WNDPROC CWnd::SetWindowProc(WNDPROC pProc)
{
	return reinterpret_cast<WNDPROC>(SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(pProc)));
}

WNDPROC CWnd::GetOldProc()
{
	return m_pOldProc;
}

void CWnd::SetUserData(LONG_PTR lpData)
{
	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, lpData);
}

void CWnd::EnableSubclass(WNDPROC pProc)
{
	assert(!m_pOldProc);
	if (!m_pOldProc)
	{
		SetUserData(reinterpret_cast<LONG_PTR>(this));
		m_pOldProc = SetWindowProc(pProc);
	}
}

void CWnd::DisableSubclass()
{
	if (m_pOldProc)
	{
		SetWindowProc(m_pOldProc);
		m_pOldProc = NULL;
		SetUserData(NULL);
	}
}

void CWnd::Show()
{
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
}

void CWnd::Hide()
{
	ShowWindow(m_hWnd, SW_HIDE);
}

void CWnd::Enable(bool bState)
{
	EnableWindow(m_hWnd, bState);
}

bool CWnd::IsFocused() const
{
	return GetFocus() == m_hWnd;
}

std::wstring CWnd::GetText()
{
	int nTextLength = GetWindowTextLength(m_hWnd);
	if (nTextLength <= 0)
		return L"";
	const int nBufferSize = nTextLength + 1;
	CScopedArray<wchar_t> buffer(nBufferSize);
	if (!GetWindowText(m_hWnd, buffer.Get(), nBufferSize))
		return L"";
	return buffer.Get();
}

void CWnd::RegisterDragDrop(IDropTarget *pDropTarget)
{
	VERIFY(SUCCEEDED(::RegisterDragDrop(m_hWnd, pDropTarget)));
}

void CWnd::Move(int nX, int nY, int nWidth, int nHeight)
{
	VERIFY(MoveWindow(m_hWnd, nX, nY, nWidth, nHeight, TRUE));
}

void CWnd::Move(const RECT & rect)
{
	Move(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

RECT CWnd::GetRect(bool bInParent)
{
	RECT rect = {};
	GetWindowRect(m_hWnd, &rect);
	if (bInParent)
		MapWindowRect(HWND_DESKTOP, GetParent(m_hWnd), &rect);
	return rect;
}

RECT CWnd::GetClient()
{
	RECT rect = {};
	GetClientRect(m_hWnd, &rect);
	return rect;
}

int CWnd::GetWidth()
{
	RECT rect = GetRect();
	return rect.right - rect.left;
}

int CWnd::GetHeight()
{
	RECT rect = GetRect();
	return rect.bottom - rect.top;
}

void CWnd::SetZOrder(HWND hInsertAfter)
{
	VERIFY(SetWindowPos(m_hWnd, hInsertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE));
}

void CWnd::SetTheme(const wchar_t *sAppName)
{
	VERIFY(SetWindowTheme(m_hWnd, sAppName, NULL) == S_OK);
}

void CButton::Create(HINSTANCE hInstance, HWND hParentWnd, const wchar_t *sText, int nID,
					 int nX, int nY, int nWidth, int nHeight, DWORD dwStyle)
{
	m_hWnd = CreateWindow(L"BUTTON", sText, WS_CHILD | dwStyle, nX, nY, nWidth, nHeight,
		hParentWnd, reinterpret_cast<HMENU>(nID), hInstance, NULL);
	assert(m_hWnd);
}

bool CButton::IsRadioButton()
{
	DWORD dwStyle = GetStyle();
	return (dwStyle & BS_RADIOBUTTON) == BS_RADIOBUTTON || (dwStyle & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON;
}

void CButton::Check()
{
	Button_SetCheck(m_hWnd, BST_CHECKED);
}

bool CButton::IsChecked()
{
	return Button_GetCheck(m_hWnd) == BST_CHECKED;
}

void CButton::AutoSize()
{
	if (IsRadioButton())
	{
		SIZE cb_size = GetCheckBoxMetrics();
		SIZE text_size = GetTextInPixels(GetFont(), GetText());
		RECT rect = GetRect(true);
		rect.right = rect.left + cb_size.cx + text_size.cx;
		rect.bottom = rect.top + max(cb_size.cy, text_size.cy);
		Move(rect);
	}
	else
		assert(false);
}

SIZE CButton::GetCheckBoxMetrics()
{
	static SIZE size = {GetSystemMetrics(SM_CXMENUCHECK) + 1, GetSystemMetrics(SM_CYMENUCHECK) - 2};
	return size;
}

void CListView::Create(HINSTANCE hInstance, HWND hParentWnd, int nID, int nX, int nY, int nWidth, int nHeight,
					   DWORD dwStyle)
{
	m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, WS_CHILD | dwStyle,
		nX, nY, nWidth, nHeight, hParentWnd, reinterpret_cast<HMENU>(nID), hInstance, NULL);
	assert(m_hWnd);
}

void CListView::SetItemsCount(uint32_t nCount)
{
	ListView_SetItemCountEx(m_hWnd, nCount, LVSICF_NOSCROLL);
}

void CListView::SetLargeImageList(HIMAGELIST hImageList)
{
	ListView_SetImageList(m_hWnd, hImageList, LVSIL_NORMAL);
}

void CListView::SetExtendedStyle(DWORD dwStyle)
{
	ListView_SetExtendedListViewStyleEx(m_hWnd, dwStyle, dwStyle);
}

RECT CListView::GetItemRect(int nIndex, int nType)
{
	RECT rect = {};
	VERIFY(ListView_GetItemRect(m_hWnd, nIndex, &rect, nType));
	return rect;
}
