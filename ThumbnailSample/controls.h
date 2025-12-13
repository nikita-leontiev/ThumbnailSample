class CWnd
{
protected:
	HWND m_hWnd;
	WNDPROC m_pOldProc;
public:
	CWnd();

	HWND GetHWND();
	void LoadFromDialog(HWND hDlg, int nID);
	void Destroy();
	bool IsWindow();
	DWORD GetStyle();
	void SetFont(HFONT hFont);
	HFONT GetFont();
	WNDPROC SetWindowProc(WNDPROC pProc);
	WNDPROC GetOldProc();
	void SetUserData(LONG_PTR lpData);
	void EnableSubclass(WNDPROC pProc);
	void DisableSubclass();
	void Show();
	void Hide();
	void Enable(bool bState);
	bool IsFocused() const;
	std::wstring GetText();
	void RegisterDragDrop(IDropTarget *pDropTarget);
	void Move(int nX, int nY, int nWidth, int nHeight);
	void Move(const RECT & rect);
	RECT GetRect(bool bInParent = false);
	RECT GetClient();
	int GetWidth();
	int GetHeight();
	void SetZOrder(HWND hInsertAfter);
	void SetTheme(const wchar_t *sAppName);
};

class CButton : public CWnd
{
public:
	void Create(HINSTANCE hInstance, HWND hParentWnd, const wchar_t *sText,
		int nID, int nX, int nY, int nWidth, int nHeight, DWORD dwStyle);
	bool IsRadioButton();
	void Check();
	bool IsChecked();
	void AutoSize();
	static SIZE GetCheckBoxMetrics();
};

class CListView : public CWnd
{
public:
	void Create(HINSTANCE hInstance, HWND hParentWnd, int nID, int nX, int nY, int nWidth, int nHeight,
		DWORD dwStyle = 0);
	void SetItemsCount(uint32_t nCount);
	void SetLargeImageList(HIMAGELIST hImageList);
	void SetExtendedStyle(DWORD dwStyle);
	RECT GetItemRect(int nIndex, int nType);
};
