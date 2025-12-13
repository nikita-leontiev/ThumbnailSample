class CNonCopyable
{
public:
	CNonCopyable() {}
private:
	CNonCopyable(const CNonCopyable &);
	CNonCopyable & operator=(const CNonCopyable &);
};

template<typename T>
class CScopedArray : public CNonCopyable
{
private:
	T *m_ptr;
public:
	explicit CScopedArray(size_t size)
	{
		m_ptr = new T[size];
		assert(m_ptr);
	}
	explicit CScopedArray(T *ptr)
	{
		assert(ptr);
		m_ptr = ptr;
	}
	~CScopedArray()
	{
		if (m_ptr)
			delete[] m_ptr;
	}

	void Realloc(size_t size)
	{
		delete[] m_ptr;
		m_ptr = new T[size];
	}
	T *Get() const
	{
		return m_ptr;
	}
	T *Release()
	{
		T *ptr = m_ptr;
		m_ptr = NULL;
		return ptr;
	}
};

class CScopedFindHandle : public CNonCopyable
{
private:
	HANDLE m_handle;
public:
	explicit CScopedFindHandle(HANDLE handle) : m_handle(handle) {}
	~CScopedFindHandle()
	{
		Close();
	}

	HANDLE Get() const
	{
		return m_handle;
	}

	void Close()
	{
		if (m_handle && m_handle != INVALID_HANDLE_VALUE)
		{
			VERIFY(FindClose(m_handle));
			m_handle = NULL;
		}
	}
};

class CScopedNewDC : public CNonCopyable
{
private:
	HDC m_hDC;
public:
	explicit CScopedNewDC(HDC hDC) : m_hDC(hDC) {}
	~CScopedNewDC()
	{
		if (m_hDC)
			VERIFY(DeleteDC(m_hDC));
	}

	HDC Get() const
	{
		return m_hDC;
	}
};

class CScopedBitmap : public CNonCopyable
{
private:
	HBITMAP m_hBitmap;
public:
	explicit CScopedBitmap(HBITMAP hBitmap) : m_hBitmap(hBitmap) {}
	CScopedBitmap() : m_hBitmap(NULL) {}
	~CScopedBitmap()
	{
		if (m_hBitmap)
			VERIFY(DeleteBitmap(m_hBitmap));
	}

	void Set(HBITMAP hBitmap)
	{
		m_hBitmap = hBitmap;
	}

	HBITMAP Get() const
	{
		return m_hBitmap;
	}

	HBITMAP Release()
	{
		HBITMAP hBitmap = m_hBitmap;
		m_hBitmap = NULL;
		return hBitmap;
	}
};

class CScopedIcon : public CNonCopyable
{
private:
	HICON m_hIcon;
public:
	explicit CScopedIcon(HICON hIcon) : m_hIcon(hIcon) {}
	CScopedIcon() : m_hIcon(NULL) {}
	~CScopedIcon()
	{
		if (m_hIcon)
			VERIFY(DestroyIcon(m_hIcon));
	}

	HICON Get() const
	{
		return m_hIcon;
	}

	HICON *GetPtr()
	{
		return &m_hIcon;
	}
};

template<typename T>
class CScopedInterface : public CNonCopyable
{
private:
	T *m_pInterface;
public:
	explicit CScopedInterface(T *pInterface) : m_pInterface(pInterface) {}
	CScopedInterface() : m_pInterface(NULL) {}
	~CScopedInterface()
	{
		if (m_pInterface)
			m_pInterface -> Release();
	}

	T *operator->() const
	{
		return m_pInterface;
	}

	void Set(T *pInterface)
	{
		m_pInterface = pInterface;
	}

	T *Get() const
	{
		return m_pInterface;
	}

	T **GetPtr()
	{
		return &m_pInterface;
	}

	void **GetVoidPtr()
	{
		return reinterpret_cast<void**>(&m_pInterface);
	}

	T *ReleaseOwnership()
	{
		T *pInterface = m_pInterface;
		m_pInterface = NULL;
		return pInterface;
	}
};

template<typename T>
class CScopedPIDL : public CNonCopyable
{
private:
	T m_pidl;
public:
	explicit CScopedPIDL(T pidl) : m_pidl(ILCombine(pidl, NULL)) {}
	CScopedPIDL() : m_pidl(NULL) {}
	~CScopedPIDL()
	{
		if (m_pidl)
			CoTaskMemFree(m_pidl);
	}

	T Get() const
	{
		return m_pidl;
	}

	T *GetPtr()
	{
		return &m_pidl;
	}
};

inline std::wstring PathAppend(const std::wstring & sPath, const std::wstring & sItem)
{
	const size_t buffer_size = MAX_PATH;
	wchar_t buffer[buffer_size];
	wcscpy_s(buffer, buffer_size, sPath.c_str());
	VERIFY(PathAppend(buffer, sItem.c_str()));
	return buffer;
}

inline BOOL BitBlt(HDC hDestDC, const RECT & dest_rect, HDC hSourceDC)
{
	return ::BitBlt(hDestDC, dest_rect.left, dest_rect.top, dest_rect.right - dest_rect.left,
		dest_rect.bottom - dest_rect.top, hSourceDC, 0, 0, SRCCOPY);
}

inline BOOL BitBlt(HDC hDestDC, const RECT & dest_rect, HDC hSourceDC, uint32_t nWidth,
				   uint32_t nHeight, bool bCenter)
{
	int nX = dest_rect.left;
	int nY = dest_rect.top;
	if (bCenter)
	{
		nX += (dest_rect.right - dest_rect.left) / 2 - nWidth / 2;
		nY += (dest_rect.bottom - dest_rect.top) / 2 - nHeight / 2;
	}

	return ::BitBlt(hDestDC, nX, nY, nWidth, nHeight, hSourceDC, 0, 0, SRCCOPY);
}

inline BOOL AlphaBlend(HDC hDestDC, const RECT & dest_rect, HDC hSourceDC, uint32_t nWidth,
					   uint32_t nHeight, bool bCenter)
{
	BLENDFUNCTION bfn = {};
	bfn.BlendOp = AC_SRC_OVER;
	bfn.SourceConstantAlpha = 255;
	bfn.AlphaFormat = AC_SRC_ALPHA;

	int nX = dest_rect.left;
	int nY = dest_rect.top;
	if (bCenter)
	{
		nX += (dest_rect.right - dest_rect.left) / 2 - nWidth / 2;
		nY += (dest_rect.bottom - dest_rect.top) / 2 - nHeight / 2;
	}

	return ::AlphaBlend(hDestDC, nX, nY, nWidth, nHeight, hSourceDC, 0, 0, nWidth, nHeight, bfn);
}

inline RECT CreateMemRect(const RECT & source, const RECT *area = NULL)
{
	RECT rect = {};
	SetRect(&rect, 0, 0, source.right - source.left, source.bottom - source.top);
	if (area)
	{
		int nXShift = area -> left - source.left;
		int nYShift = area -> top - source.top;
		SetRect(&rect, nXShift, nYShift, nXShift + area -> right - area -> left,
			nYShift + area -> bottom - area -> top);
	}
	return rect;
}

inline HBITMAP CreateBitmap32(uint32_t nWidth, uint32_t nHeight, void **pBits = NULL)
{
	BITMAPINFO bi = {};
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = nWidth;
	bi.bmiHeader.biHeight = -static_cast<long>(nHeight); // Top-down bitmap
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = nWidth * nHeight * sizeof(RGBQUAD);

	void *bits = NULL;
	HBITMAP hBitmap = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
	if (!hBitmap)
		return NULL;
	if (pBits)
		*pBits = bits;
	return hBitmap;
}

inline void MakePBGRA(HBITMAP hBitmap)
{
	BITMAP bitmap = {};
	if (GetObject(hBitmap, sizeof(BITMAP), &bitmap) && bitmap.bmBitsPixel == 32)
	{
		RGBQUAD *pPixels = reinterpret_cast<RGBQUAD*>(bitmap.bmBits);
		if (pPixels)
		{
			for (long i = 0; i < bitmap.bmWidth * bitmap.bmHeight; ++i)
			{
				pPixels[i].rgbBlue = (pPixels[i].rgbBlue * pPixels[i].rgbReserved) / 255;
				pPixels[i].rgbGreen = (pPixels[i].rgbGreen * pPixels[i].rgbReserved) / 255;
				pPixels[i].rgbRed = (pPixels[i].rgbRed * pPixels[i].rgbReserved) / 255;
			}
		}
	}
}

inline SIZE GetTextInPixels(HFONT hFont, const std::wstring & sText)
{
	SIZE size = {};

	CScopedNewDC dc(CreateDC(L"DISPLAY", NULL, NULL, NULL));

	HFONT hOldFont = SelectFont(dc.Get(), hFont);

	GetTextExtentPoint32(dc.Get(), sText.c_str(), static_cast<int>(sText.length()), &size);

	SelectFont(dc.Get(), hOldFont);

	size.cx++;

	return size;
}

inline HBITMAP WICScaleBitmap(IWICImagingFactory *pImagingFactory, IWICBitmap *pBitmap,
							  uint32_t nWidth, uint32_t nHeight)
{
	CScopedInterface<IWICFormatConverter> converter;
	if (FAILED(pImagingFactory -> CreateFormatConverter(converter.GetPtr())))
		return NULL;
	if (FAILED(converter -> Initialize(pBitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
		NULL, 0, WICBitmapPaletteTypeCustom)))
		return NULL;

	CScopedInterface<IWICBitmapScaler> scaler;
	if (FAILED(pImagingFactory -> CreateBitmapScaler(scaler.GetPtr())))
		return NULL;
	if (FAILED(scaler -> Initialize(converter.Get(), nWidth, nHeight, WICBitmapInterpolationModeFant)))
		return NULL;

	void *pBits = NULL;
	CScopedBitmap bitmap(CreateBitmap32(nWidth, nHeight, &pBits));
	if (!bitmap.Get())
		return NULL;

	const uint32_t nStride = nWidth * sizeof(RGBQUAD);
	if (FAILED(scaler -> CopyPixels(NULL, nStride, nStride * nHeight, static_cast<uint8_t*>(pBits))))
		return NULL;

	return bitmap.Release();
}

inline HBITMAP WICIconToBitmap(IImageList *pImageList, int nIndex, uint32_t nWidth, uint32_t nHeight)
{
	CScopedIcon icon;
	if (FAILED(pImageList -> GetIcon(nIndex, ILD_IMAGE, icon.GetPtr())))
		return NULL;

	CScopedInterface<IWICImagingFactory> imaging_factory;
	if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory, imaging_factory.GetVoidPtr())))
		return NULL;

	CScopedInterface<IWICBitmap> wic_bitmap;
	if (FAILED(imaging_factory -> CreateBitmapFromHICON(icon.Get(), wic_bitmap.GetPtr())))
		return NULL;

	return WICScaleBitmap(imaging_factory.Get(), wic_bitmap.Get(), nWidth, nHeight);
}

inline HBITMAP WICScaleBitmap(HBITMAP hBitmap, uint32_t nWidth, uint32_t nHeight)
{
	CScopedInterface<IWICImagingFactory> imaging_factory;
	if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory, imaging_factory.GetVoidPtr())))
		return NULL;

	CScopedInterface<IWICBitmap> wic_bitmap;
	if (FAILED(imaging_factory -> CreateBitmapFromHBITMAP(hBitmap, NULL, WICBitmapUseAlpha, wic_bitmap.GetPtr())))
		return NULL;

	return WICScaleBitmap(imaging_factory.Get(), wic_bitmap.Get(), nWidth, nHeight);
}
