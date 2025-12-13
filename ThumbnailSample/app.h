#include "controls.h"

#define IDC_SHELL_ITEM_IMAGE_FACTORY_RADIO_BUTTON 1000
#define IDC_THUMBNAIL_PROVIDER_RADIO_BUTTON 1001
#define IDC_EXTRACT_IMAGE_RADIO_BUTTON 1002
#define IDC_DRAW_SCALED_RADIO_BUTTON 1003
#define IDC_DRAW_WIC_SCALE_RADIO_BUTTON 1004
#define IDC_GET_ICON_WIC_SCALE_RADIO_BUTTON 1005
#define IDC_LISTVIEW 1006

static const wchar_t *sBasePath = L"D:\\TEST";

class CApp : public CWnd
{
private:
	HINSTANCE m_hInstance;
	HFONT m_hFont;
	CButton m_shell_item_image_factory_radio_button;
	CButton m_thumbnail_provider_radio_button;
	CButton m_extract_image_radio_button;
	CButton m_draw_scaled_radio_button;
	CButton m_draw_wic_scale_radio_button;
	CButton m_get_icon_wic_scale_radio_button;
	CListView m_list_view;
	std::wstring m_sPath;
	FFDVector m_items;
	BitmapVector m_bitmaps;
public:
	CApp();

	int Main(HINSTANCE hInstance, int nCmdShow);
	ATOM RegisterWndClass();
	bool CreateMainWnd(int nCmdShow);
	BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
	void OnDestroy(HWND hWnd);
	void OnSize(HWND hWnd, UINT nState, int nWidth, int nHeight);
	void OnCommand(HWND hWnd, int nID, HWND hCtrl, UINT nNotify);
	INT_PTR OnNotify(HWND hWnd, int nID, NMHDR *pNMHDR);
	void OnGetDispInfo(NMLVDISPINFO *pInfo);
	void OnItemActivate(const NMITEMACTIVATE *pItem);
	INT_PTR OnCustomDraw(NMLVCUSTOMDRAW *pNMLVCD);
	void DrawThumbnail(NMLVCUSTOMDRAW *pNMLVCD);
	void ListDirectory(const std::wstring & sPath);
	HBITMAP GetThumbnail(const std::wstring & sPath, int nIconIndex, uint32_t nSize);
	HBITMAP IShellItemImageFactory_GetThumbnail(IShellFolder *pParent, PCUITEMID_CHILD child_pidl, uint32_t nSize);
	HBITMAP IThumbnailProvider_GetThumbnail(IShellFolder *pParent, PCUITEMID_CHILD child_pidl, uint32_t nSize);
	HBITMAP IExtractImage_GetThumbnail(IShellFolder *pParent, PCUITEMID_CHILD child_pidl, uint32_t nSize);
	HBITMAP IconToBitmap(int nIconIndex, uint32_t nSize);
	void ClearItems();
	static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
};
