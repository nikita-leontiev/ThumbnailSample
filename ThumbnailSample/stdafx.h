#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "UxTheme.lib")
#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Windowscodecs.lib")

#include "targetver.h"

#include <inttypes.h>
#include <string>
#include <vector>
#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <CommonControls.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <thumbcache.h>
#include <wincodec.h>

#define UNUSED(param) (void)(param);

#define VERIFY(expression) \
	if (!(expression)) { assert(false && #expression); }

typedef std::vector<WIN32_FIND_DATA> FFDVector;

typedef std::vector<HBITMAP> BitmapVector;
typedef BitmapVector::const_iterator BitmapVectorConstIt;
