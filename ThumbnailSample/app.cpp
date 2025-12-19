#include "stdafx.h"
#include "utils.h"
#include "app.h"

CApp::CApp() :
CWnd(),
m_hInstance(NULL),
m_hFont(NULL)
{
}

int CApp::Main(HINSTANCE hInstance, int nCmdShow)
{
	m_hInstance = hInstance;

	INITCOMMONCONTROLSEX icex = {};
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	VERIFY(InitCommonControlsEx(&icex));

	VERIFY(SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)));

	if (!RegisterWndClass())
		return 0;

	if (!CreateMainWnd(nCmdShow))
		return 0;

	MSG msg = {};
	BOOL bResult = 0;
	while ((bResult = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bResult == -1)
			break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CoUninitialize();

	return static_cast<int>(msg.wParam);
}

ATOM CApp::RegisterWndClass()
{
	WNDCLASS wndClass = {};
	wndClass.style = CS_DBLCLKS;
	wndClass.lpfnWndProc = MainWndProc;
	wndClass.hInstance = m_hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
	wndClass.lpszClassName = L"MainClass";
	return RegisterClass(&wndClass);
}

bool CApp::CreateMainWnd(int nCmdShow)
{
	HWND hWnd = CreateWindow(L"MainClass", L"Thumbnail sample", WS_OVERLAPPEDWINDOW,
		GetSystemMetrics(SM_CXSCREEN) / 2 - 300, GetSystemMetrics(SM_CYSCREEN) / 2 - 250,
		600, 500, NULL, NULL, m_hInstance, this);
	if (!hWnd)
		return false;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return true;
}

BOOL CApp::OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	UNUSED(lpCreateStruct);

	LOGFONT lf = {};
	SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0);
	m_hFont = CreateFontIndirect(&lf);

	uint32_t nLeft = 10, nTop = 10;

	m_shell_item_image_factory_radio_button.Create(m_hInstance, hWnd, L"IShellItemImageFactory",
		IDC_SHELL_ITEM_IMAGE_FACTORY_RADIO_BUTTON, nLeft, nTop, 0, 0, WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON);
	m_shell_item_image_factory_radio_button.SetFont(m_hFont);
	m_shell_item_image_factory_radio_button.AutoSize();
	nTop += m_shell_item_image_factory_radio_button.GetHeight() + 5;
	m_shell_item_image_factory_radio_button.Check();

	m_thumbnail_provider_radio_button.Create(m_hInstance, hWnd, L"IThumbnailProvider",
		IDC_THUMBNAIL_PROVIDER_RADIO_BUTTON, nLeft, nTop, 0, 0, WS_VISIBLE | BS_AUTORADIOBUTTON);
	m_thumbnail_provider_radio_button.SetFont(m_hFont);
	m_thumbnail_provider_radio_button.AutoSize();
	nTop += m_thumbnail_provider_radio_button.GetHeight() + 5;

	m_extract_image_radio_button.Create(m_hInstance, hWnd, L"IExtractImage",
		IDC_EXTRACT_IMAGE_RADIO_BUTTON, nLeft, nTop, 0, 0, WS_VISIBLE | BS_AUTORADIOBUTTON);
	m_extract_image_radio_button.SetFont(m_hFont);
	m_extract_image_radio_button.AutoSize();

	nLeft += m_shell_item_image_factory_radio_button.GetWidth() + 10;
	nTop = 10;

	m_draw_scaled_radio_button.Create(m_hInstance, hWnd, L"Draw scaled", IDC_DRAW_SCALED_RADIO_BUTTON,
		nLeft, nTop, 0, 0, WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON);
	m_draw_scaled_radio_button.SetFont(m_hFont);
	m_draw_scaled_radio_button.AutoSize();
	nTop += m_draw_scaled_radio_button.GetHeight() + 5;
	m_draw_scaled_radio_button.Check();

	m_draw_wic_scale_radio_button.Create(m_hInstance, hWnd, L"Draw + WIC scale", IDC_DRAW_WIC_SCALE_RADIO_BUTTON,
		nLeft, nTop, 0, 0, WS_VISIBLE | BS_AUTORADIOBUTTON);
	m_draw_wic_scale_radio_button.SetFont(m_hFont);
	m_draw_wic_scale_radio_button.AutoSize();
	nTop += m_draw_wic_scale_radio_button.GetHeight() + 5;

	m_get_icon_wic_scale_radio_button.Create(m_hInstance, hWnd, L"GetIcon + WIC scale",
		IDC_GET_ICON_WIC_SCALE_RADIO_BUTTON, nLeft, nTop, 0, 0, WS_VISIBLE | BS_AUTORADIOBUTTON);
	m_get_icon_wic_scale_radio_button.SetFont(m_hFont);
	m_get_icon_wic_scale_radio_button.AutoSize();

	m_list_view.Create(m_hInstance, hWnd, IDC_LISTVIEW, 0, 0, 0, 0,
		WS_VISIBLE | LVS_ICON | LVS_AUTOARRANGE | LVS_OWNERDATA);
	m_list_view.SetExtendedStyle(LVS_EX_DOUBLEBUFFER); // Required for the Explorer theme to draw properly
	m_list_view.SetTheme(L"Explorer");
	m_list_view.SetLargeImageList(ImageList_Create(96, 96, ILC_COLOR32, 1, 1));

	ListDirectory(sBasePath);

	return TRUE;
}

void CApp::OnDestroy(HWND hWnd)
{
	UNUSED(hWnd);
	DeleteFont(m_hFont);
	PostQuitMessage(0);
}

void CApp::OnSize(HWND hWnd, UINT nState, int nWidth, int nHeight)
{
	RECT rect = m_get_icon_wic_scale_radio_button.GetRect(true);
	uint32_t nTop = rect.bottom + 10;
	m_list_view.Move(0, nTop, nWidth, nHeight - nTop);

	FORWARD_WM_SIZE(hWnd, nState, nWidth, nHeight, DefWindowProc);
}

void CApp::OnCommand(HWND hWnd, int nID, HWND hCtrl, UINT nNotify)
{
	switch (nID)
	{
	case IDC_SHELL_ITEM_IMAGE_FACTORY_RADIO_BUTTON:
	case IDC_THUMBNAIL_PROVIDER_RADIO_BUTTON:
	case IDC_EXTRACT_IMAGE_RADIO_BUTTON:
	case IDC_DRAW_SCALED_RADIO_BUTTON:
	case IDC_DRAW_WIC_SCALE_RADIO_BUTTON:
	case IDC_GET_ICON_WIC_SCALE_RADIO_BUTTON:
		if (Button_GetCheck(hCtrl) == BST_CHECKED)
			ListDirectory(m_sPath);
		break;
	}

	FORWARD_WM_COMMAND(hWnd, nID, hCtrl, nNotify, DefWindowProc);
}

INT_PTR CApp::OnNotify(HWND hWnd, int nID, NMHDR *pNMHDR)
{
	switch (pNMHDR -> code)
	{
	case LVN_GETDISPINFO:
		if (pNMHDR -> idFrom == IDC_LISTVIEW)
		{
			OnGetDispInfo(reinterpret_cast<NMLVDISPINFO*>(pNMHDR));
			return TRUE;
		}
		break;
	case LVN_ITEMACTIVATE:
		if (pNMHDR -> idFrom == IDC_LISTVIEW)
		{
			OnItemActivate(reinterpret_cast<NMITEMACTIVATE*>(pNMHDR));
			return TRUE;
		}
		break;
	case NM_CUSTOMDRAW:
		if (pNMHDR -> idFrom == IDC_LISTVIEW)
			return OnCustomDraw(reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR));
		break;
	}
	return FORWARD_WM_NOTIFY(hWnd, nID, pNMHDR, DefWindowProc);
}

void CApp::OnGetDispInfo(NMLVDISPINFO *pInfo)
{
	int index = pInfo -> item.iItem;
	if (index < 0 || static_cast<size_t>(index) >= m_items.size())
		return;
	if (pInfo -> item.mask & LVIF_TEXT)
		wcscpy_s(pInfo -> item.pszText, pInfo -> item.cchTextMax, m_items.at(index).cFileName);
}

void CApp::OnItemActivate(const NMITEMACTIVATE *pItem)
{
	int index = pItem -> iItem;
	if (index < 0 || static_cast<size_t>(index) >= m_items.size())
		return;
	if (!(m_items.at(index).dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		return;
	ListDirectory(PathAppend(m_sPath, m_items.at(index).cFileName));
}

INT_PTR CApp::OnCustomDraw(NMLVCUSTOMDRAW *pNMLVCD)
{
	switch (pNMLVCD -> nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		return CDRF_NOTIFYITEMDRAW;
	case CDDS_ITEMPREPAINT:
		DrawThumbnail(pNMLVCD);
		return CDRF_SKIPDEFAULT;
	}
	return CDRF_DODEFAULT;
}

void CApp::DrawThumbnail(NMLVCUSTOMDRAW *pNMLVCD)
{
	DWORD_PTR dwItem = pNMLVCD -> nmcd.dwItemSpec;
	if (dwItem >= m_items.size())
		return;

	HDC hDC = pNMLVCD -> nmcd.hdc;
	bool fFocused = m_list_view.IsFocused() && (pNMLVCD -> nmcd.uItemState & CDIS_FOCUS) == CDIS_FOCUS;
	bool fHot = (pNMLVCD -> nmcd.uItemState & CDIS_HOT) == CDIS_HOT;

	RECT rItemRect = m_list_view.GetItemRect(static_cast<int>(dwItem), LVIR_BOUNDS);
	RECT rIconRect = m_list_view.GetItemRect(static_cast<int>(dwItem), LVIR_ICON);
	RECT rTextRect = m_list_view.GetItemRect(static_cast<int>(dwItem), LVIR_LABEL);
	RECT rMemRect = CreateMemRect(rItemRect);
	RECT rMemIconRect = CreateMemRect(rItemRect, &rIconRect);
	RECT rMemTextRect = CreateMemRect(rItemRect, &rTextRect);

	CScopedNewDC mem_dc(CreateCompatibleDC(hDC));
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, rMemRect.right, rMemRect.bottom);
	HBITMAP hOldMemBitmap = SelectBitmap(mem_dc.Get(), hBitmap);

	HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(mem_dc.Get(), &rMemRect, hBrush);
	DeleteBrush(hBrush);
	if (fFocused || fHot)
	{
		HTHEME hTheme = OpenThemeData(m_list_view.GetHWND(), L"LISTVIEW");
		if (hTheme)
		{
			int nState = fHot?LISS_HOT:(fFocused?LISS_SELECTED:LISS_SELECTEDNOTFOCUS);
			DrawThemeBackground(hTheme, mem_dc.Get(), LVP_LISTITEM, nState, &rMemRect, NULL);
			CloseThemeData(hTheme);
		}
	}

	if (fFocused)
		DrawFocusRect(mem_dc.Get(), &rMemRect);

	HBITMAP hThumbnail = m_bitmaps.at(dwItem);
	if (hThumbnail)
	{
		BITMAP bitmap = {};
		GetObject(hThumbnail, sizeof(BITMAP), &bitmap);

		CScopedNewDC temp_dc(CreateCompatibleDC(hDC));
		HBITMAP hOldBitmap = SelectBitmap(temp_dc.Get(), hThumbnail);

		if (bitmap.bmBitsPixel == 32)
			AlphaBlend(mem_dc.Get(), rMemIconRect, temp_dc.Get(), bitmap.bmWidth, bitmap.bmHeight, true);
		else
			BitBlt(mem_dc.Get(), rMemIconRect, temp_dc.Get(), bitmap.bmWidth, bitmap.bmHeight, true);

		SelectBitmap(temp_dc.Get(), hOldBitmap);
	}

	SetTextColor(mem_dc.Get(), GetSysColor(COLOR_WINDOWTEXT));
	SetBkMode(mem_dc.Get(), TRANSPARENT);
	HFONT hOldFont = SelectFont(mem_dc.Get(), m_list_view.GetFont());
	const wchar_t *name = m_items.at(dwItem).cFileName;
	DrawText(mem_dc.Get(), name, static_cast<int>(wcslen(name)), &rMemTextRect,
		DT_WORDBREAK | DT_WORD_ELLIPSIS | DT_CENTER);
	SelectFont(mem_dc.Get(), hOldFont);

	BitBlt(hDC, rItemRect, mem_dc.Get());

	SelectBitmap(mem_dc.Get(), hOldMemBitmap);
}

void CApp::ListDirectory(const std::wstring & sPath)
{
	m_list_view.SetItemsCount(0);
	ClearItems();
	m_sPath = sPath;

	WIN32_FIND_DATA ffd = {};
	std::wstring sFilter = sPath + L"\\*";
	CScopedFindHandle handle(FindFirstFile(sFilter.c_str(), &ffd));
	if (handle.Get() != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (wcscmp(ffd.cFileName, L"."))
			{
				m_items.push_back(ffd);
				std::wstring sFullPath = PathAppend(sPath, ffd.cFileName);
				SHFILEINFO sfi = {};
				VERIFY(SHGetFileInfo(sFullPath.c_str(), ffd.dwFileAttributes, &sfi, sizeof(SHFILEINFO),
					SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES));
				m_bitmaps.push_back(GetThumbnail(sFullPath, sfi.iIcon, 96));
			}
		}
		while (FindNextFile(handle.Get(), &ffd));
	}

	m_list_view.SetItemsCount(static_cast<uint32_t>(m_items.size()));
}

HBITMAP CApp::GetThumbnail(const std::wstring & sPath, int nIconIndex, uint32_t nSize)
{
	CScopedInterface<IShellFolder> desktop_folder;
	if (FAILED(SHGetDesktopFolder(desktop_folder.GetPtr())))
		return NULL;

	CScopedPIDL<PIDLIST_ABSOLUTE> pidl;
	if (FAILED(desktop_folder -> ParseDisplayName(NULL, NULL, const_cast<wchar_t*>(sPath.c_str()),
		NULL, pidl.GetPtr(), NULL)))
		return NULL;

	CScopedInterface<IShellFolder> parent;
	PCUITEMID_CHILD child_pidl = NULL; // Should not be freed, because it's part of pidl
	if (FAILED(SHBindToParent(pidl.Get(), IID_IShellFolder, parent.GetVoidPtr(), &child_pidl)))
		return NULL;

	HBITMAP hThumbnail = NULL;
	if (m_shell_item_image_factory_radio_button.IsChecked())
		hThumbnail = IShellItemImageFactory_GetThumbnail(parent.Get(), child_pidl, nSize);
	else if (m_thumbnail_provider_radio_button.IsChecked())
		hThumbnail = IThumbnailProvider_GetThumbnail(parent.Get(), child_pidl, nSize);
	else if (m_extract_image_radio_button.IsChecked())
		hThumbnail = IExtractImage_GetThumbnail(parent.Get(), child_pidl, nSize);
	if (!hThumbnail)
		hThumbnail = IconToBitmap(nIconIndex, nSize);
	return hThumbnail;
}

HBITMAP CApp::IShellItemImageFactory_GetThumbnail(IShellFolder *pParent, PCUITEMID_CHILD child_pidl, uint32_t nSize)
{
	/* IShellItemImageFactory is a modern way to get thumbnail.
	IShellItemImageFactory::GetImage returns thumbnail or an icon, if thumbnail is not available. */
	CScopedInterface<IShellItem> shell_item;
	if (FAILED(SHCreateShellItem(NULL, pParent, child_pidl, shell_item.GetPtr())))
		return NULL;
	CScopedInterface<IShellItemImageFactory> image_factory;
	if (FAILED(shell_item -> QueryInterface(IID_IShellItemImageFactory, image_factory.GetVoidPtr())))
		return NULL;
	SIZE size = {nSize, nSize};
	HBITMAP hImage = NULL;
	if (FAILED(image_factory -> GetImage(size, SIIGBF_RESIZETOFIT, &hImage)))
		return NULL;
	MakePBGRA(hImage);
	return hImage;
}

HBITMAP CApp::IThumbnailProvider_GetThumbnail(IShellFolder *pParent, PCUITEMID_CHILD child_pidl, uint32_t nSize)
{
	/* IThumbnailProvider can return RAW previews, if Microsoft Camera Codec Pack is installed.
	IExtractImage can't return RAW previews. */
	CScopedInterface<IThumbnailProvider> thumbnail_provider;
	if (FAILED(pParent -> GetUIObjectOf(NULL, 1, &child_pidl, IID_IThumbnailProvider, NULL,
		thumbnail_provider.GetVoidPtr())))
		return NULL;
	HBITMAP hThumbnail = NULL;
	WTS_ALPHATYPE alpha = WTSAT_ARGB;
	if (FAILED(thumbnail_provider -> GetThumbnail(nSize, &hThumbnail, &alpha)))
		return NULL;
	return hThumbnail;
}

HBITMAP CApp::IExtractImage_GetThumbnail(IShellFolder *pParent, PCUITEMID_CHILD child_pidl, uint32_t nSize)
{
	// IExtractImage can return only thumbnail. If thumbnail is not available, IExtractImage::Extract fails.
	CScopedInterface<IExtractImage> extract_image;
	if (FAILED(pParent -> GetUIObjectOf(NULL, 1, &child_pidl, IID_IExtractImage, NULL, extract_image.GetVoidPtr())))
		return NULL;
	const DWORD dwBufferSize = MAX_PATH;
	wchar_t sBuffer[dwBufferSize];
	DWORD dwPriority = 0;
	SIZE size = {nSize, nSize};
	DWORD dwFlags = IEIFLAG_SCREEN;
	if (FAILED(extract_image -> GetLocation(sBuffer, dwBufferSize, &dwPriority, &size, 32, &dwFlags)))
		return NULL;
	HBITMAP hImage = NULL;
	if (FAILED(extract_image -> Extract(&hImage)))
		return NULL;
	MakePBGRA(hImage);
	return hImage;
}

HBITMAP CApp::IconToBitmap(int nIconIndex, uint32_t nSize)
{
	CScopedInterface<IImageList> image_list;
	VERIFY(SUCCEEDED(SHGetImageList(4, IID_IImageList, image_list.GetVoidPtr())));

	if (m_get_icon_wic_scale_radio_button.IsChecked())
		return WICIconToBitmap(image_list.Get(), nIconIndex, nSize, nSize);

	bool bDrawWICScale = m_draw_wic_scale_radio_button.IsChecked();

	uint32_t nBitmapSize = nSize;
	if (bDrawWICScale)
		nBitmapSize = 256;

	HBITMAP hBitmap = CreateBitmap32(nBitmapSize, nBitmapSize);

	CScopedNewDC mem_dc(CreateCompatibleDC(NULL));
	HBITMAP hOldBitmap = SelectBitmap(mem_dc.Get(), hBitmap);

	/* ILD_SCALE leads to poor quality scaling on Windows 7. Fixed in Windows 10.
	For example, there is jumbo (256x256) image list from SHGetImageList.
	Image list contains real jumbo icons and icons created from smaller icons, if file doesn't have jumbo icon.
	cx/cy is smaller than jumbo size.
	If the icon is real jumbo icon, then IImageList::Draw draws poor quality scaled icon, otherwise it draws gray
	frame and 48x48 (possibly stretched) icon in the center.
	IImageList::Draw (w/out ILD_SCALE) + WIC downscaling makes 48x48 icon smaller.
	IImageList::GetIcon returns small icon in the upper-left corner, if icon is not a jumbo icon,
	so IImageList::GetIcon + WIC downscaling makes icon even smaller.
	There is no way to tell if the icon is a real jumbo icon or not, so it's impossible to determine when scaling
	should be used with IImageList::Draw (w/out ILD_SCALE).
	https://microsoft.public.platformsdk.shell.narkive.com/hg3wdINJ/down-scale-icons-from-shil-jumbo-image-list */
	IMAGELISTDRAWPARAMS dp = {};
	dp.cbSize = sizeof(IMAGELISTDRAWPARAMS);
	dp.hdcDst = mem_dc.Get();
	dp.i = nIconIndex;
	dp.cx = nBitmapSize;
	dp.cy = nBitmapSize;
	dp.fStyle = ILD_TRANSPARENT;
	if (m_draw_scaled_radio_button.IsChecked())
		dp.fStyle |= ILD_SCALE;
	VERIFY(SUCCEEDED(image_list -> Draw(&dp)));

	SelectBitmap(mem_dc.Get(), hOldBitmap);

	if (bDrawWICScale)
	{
		HBITMAP hNew = WICScaleBitmap(hBitmap, nSize, nSize);
		DeleteBitmap(hBitmap);
		hBitmap = hNew;
	}
	return hBitmap;
}

void CApp::ClearItems()
{
	m_items.clear();
	for (BitmapVectorConstIt it = m_bitmaps.begin(); it != m_bitmaps.end(); ++it)
	{
		if (*it)
			DeleteBitmap(*it);
	}
	m_bitmaps.clear();
}

LRESULT CALLBACK CApp::MainWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (nMsg == WM_CREATE)
	{
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(
			reinterpret_cast<LPCREATESTRUCT>(lParam) -> lpCreateParams));
	}

	CApp *pApp = reinterpret_cast<CApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (pApp)
	{
		switch (nMsg)
		{
			HANDLE_MSG(hWnd, WM_CREATE, pApp -> OnCreate);
			HANDLE_MSG(hWnd, WM_DESTROY, pApp -> OnDestroy);
			HANDLE_MSG(hWnd, WM_SIZE, pApp -> OnSize);
			HANDLE_MSG(hWnd, WM_COMMAND, pApp -> OnCommand);
			HANDLE_MSG(hWnd, WM_NOTIFY, pApp -> OnNotify);
		}
	}

	return DefWindowProc(hWnd, nMsg, wParam, lParam);
}
