## Description

This is an overview of possible ways to retrieve thumbnail for a Shell object.

### IShellItemImageFactory

[IShellItemImageFactory](https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-ishellitemimagefactory) is a modern way to retrieve a thumbnail/icon for a Shell object.

The interface can be obtained from [IShellItem](https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-ishellitem) object via [QueryInterface](https://learn.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-queryinterface(refiid_void)) method.

[IShellItemImageFactory::GetImage](https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-ishellitemimagefactory-getimage) method loads a thumbnail or an icon, if the thumbnail is not available.

The resulting bitmap is not premultiplied, so it should be [premultiplied](https://learn.microsoft.com/en-us/windows/apps/develop/win2d/premultiplied-alpha) if you want to use bitmap with [AlphaBlend](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-alphablend) function.

### IThumbnailProvider

[IThumbnailProvider](https://learn.microsoft.com/en-us/windows/win32/api/thumbcache/nn-thumbcache-ithumbnailprovider) is a replacement for [IExtractImage](https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-iextractimage) interface.

The interface can be obtained from [IShellFolder](https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-ishellfolder) object via [GetUIObjectOf](https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-ishellfolder-getuiobjectof) method.

Almost always, GetUIObjectOf returns E_NOINTERFACE or E_NOTIMPL. The only one successful case on Windows 7 is the camera RAW format when the Microsoft Camera Codec Pack is installed.

### IExtractImage

[IExtractImage](https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-iextractimage) is the old way to retrieve thumbnails.

The interface can be obtained from [IShellFolder](https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-ishellfolder) object via [GetUIObjectOf](https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-ishellfolder-getuiobjectof) method. It can return E_NOINTERFACE or E_NOTIMPL when the thumbnail is not available.

First call [IExtractImage::GetLocation](https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-iextractimage-getlocation) method.

Then call [IExtractImage::Extract](https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-iextractimage-extract) method. It can return E_FAIL when the thumbnail is not available, for example, when the Shell object is an empty directory.

The resulting bitmap is not premultiplied.

### Thumbnail not available

Case: IExtractImage or IThumbnailProvider is used, but the thumbnail is not available. In this case jumbo (256x256) icons can be used instead of thumbnails.

Jumbo icons can be retrieved from the system image list. Use [SHGetImageList](https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shgetimagelist) function to retrieve image list.

Image list can contain:

* real jumbo icon when the Shell item has jumbo icon
* smaller icon when the Shell item doesn't have jumbo icon

There is no way to tell if the image list icon is a real jumbo icon or not. [IImageList2::GetOriginalSize](https://learn.microsoft.com/en-us/windows/win32/api/commoncontrols/nf-commoncontrols-iimagelist2-getoriginalsize) method always returns E_NOTIMPL on Windows 7.

There are two ways to get an image: [IImageList::Draw](https://learn.microsoft.com/en-us/windows/win32/api/commoncontrols/nf-commoncontrols-iimagelist-draw) and [IImageList::GetIcon](https://learn.microsoft.com/en-us/windows/win32/api/commoncontrols/nf-commoncontrols-iimagelist-geticon).

#### IImageList::Draw

If the icon is a real jumbo icon, IImageList::Draw draws the icon as it is. If the thumbnail size is less than 256x256 and ILD_SCALE flag is used, IImageList::Draw downscales the icon, but scaling quality is poor on Windows 7 and lower. Discussed [here](https://microsoft.public.platformsdk.shell.narkive.com/jsbRBrkG/vista-quality-problem-when-drawing-scaled-down-icons) and [here](https://microsoft.public.platformsdk.shell.narkive.com/hg3wdINJ/down-scale-icons-from-shil-jumbo-image-list). The scaling quality has been improved in Windows 10 (maybe in Windows 8, I checked on Windows 10).

If the icon is not a real jumbo icon, IImageList::Draw draws a gray frame and a 48x48 (possibly stretched) icon in the center. Downscaling is not an issue in this case.

Theoretically, IImageList::Draw can be used without the ILD_SCALE flag for real jumbo icons and scaling can be performed, for example, via [IWICBitmapScaler](https://learn.microsoft.com/en-us/windows/win32/api/wincodec/nn-wincodec-iwicbitmapscaler) (solution described [here](https://microsoft.public.platformsdk.shell.narkive.com/hg3wdINJ/down-scale-icons-from-shil-jumbo-image-list#post7)), but there is no way to determine if the icon is a real jumbo icon or not. Applying downscaling to the image generated by IImageList::Draw for non-real jumbo icon makes the icon smaller i.e. the thumbnail doesn't look the same as in Explorer.

#### IImageList::GetIcon

If the icon is a real jumbo icon, IImageList::GetIcon returns the icon as it is, otherwise it returns small icon in the upper-left corner i.e. the thumbnail doesn't look the same as in Explorer.
The icon can be scaled using IWICBitmapScaler, but in the case of non-real jumbo icon, downscaling makes the upper-left corner icon even smaller.
