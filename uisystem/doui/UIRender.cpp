#include "StdAfx.h"

///////////////////////////////////////////////////////////////////////////////////////
DECLARE_HANDLE(HZIP);	// An HZIP identifies a zip file that has been opened
typedef DWORD ZRESULT;
typedef struct
{ 
    int index;                 // index of this file within the zip
    char name[MAX_PATH];       // filename within the zip
    DWORD attr;                // attributes, as in GetFileAttributes.
    FILETIME atime,ctime,mtime;// access, create, modify filetimes
    long comp_size;            // sizes of item, compressed and uncompressed. These
    long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRY;
typedef struct
{ 
    int index;                 // index of this file within the zip
    TCHAR name[MAX_PATH];      // filename within the zip
    DWORD attr;                // attributes, as in GetFileAttributes.
    FILETIME atime,ctime,mtime;// access, create, modify filetimes
    long comp_size;            // sizes of item, compressed and uncompressed. These
    long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRYW;
#define OpenZip OpenZipU
#define CloseZip(hz) CloseZipU(hz)
extern HZIP OpenZipU(void *z,unsigned int len,DWORD flags);
extern ZRESULT CloseZipU(HZIP hz);
#ifdef _UNICODE
#define ZIPENTRY ZIPENTRYW
#define GetZipItem GetZipItemW
#define FindZipItem FindZipItemW
#else
#define GetZipItem GetZipItemA
#define FindZipItem FindZipItemA
#endif
extern ZRESULT GetZipItemA(HZIP hz, int index, ZIPENTRY *ze);
extern ZRESULT GetZipItemW(HZIP hz, int index, ZIPENTRYW *ze);
extern ZRESULT FindZipItemA(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze);
extern ZRESULT FindZipItemW(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *ze);
extern ZRESULT UnzipItem(HZIP hz, int index, void *dst, unsigned int len, DWORD flags);
///////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
    extern unsigned char *stbi_load_from_memory(unsigned char const *buffer, int len, int *x, int *y, \
        int *comp, int req_comp);
	extern void     stbi_image_free(void *retval_from_stbi_load);

};

namespace DuiLib {

//图形对齐列表
	enum ALIGNTYPE{
		CENTER = 1,
		CENTERHEIGHT, //保持高度居中。拉伸宽度
		CENTERWIDTH,
		LEFT,
		LEFT_TOP,
		LEFT_BOTTOM,
		RIGHT,
		RIGHT_TOP,
		RIGHT_BOTTOM,
		TOP,
		BOTTOM,
		TILELAST, //拉伸最后一像素
		XAUTOGRADUALTILE,
		YAUTOGRADUALTILE
	};

/////////////////////////////////////////////////////////////////////////////////////
//
//



CRenderClip::~CRenderClip()
{
	if (hRgn == NULL)
	{
		return;
	}
    ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
    ASSERT(::GetObjectType(hRgn)==OBJ_REGION);
    ASSERT(::GetObjectType(hOldRgn)==OBJ_REGION);
    ::SelectClipRgn(hDC, hOldRgn);
    ::DeleteObject(hOldRgn);
    ::DeleteObject(hRgn);
}

void CRenderClip::GenerateClip(HDC hDC, RECT rc, CRenderClip& clip)
{
    RECT rcClip = { 0 };
    ::GetClipBox(hDC, &rcClip);
    clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
    clip.hRgn = ::CreateRectRgnIndirect(&rc);
    ::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
    clip.hDC = hDC;
    clip.rcItem = rc;
}

void CRenderClip::GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CRenderClip& clip)
{
    RECT rcClip = { 0 };
    ::GetClipBox(hDC, &rcClip);
    clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
    clip.hRgn = ::CreateRectRgnIndirect(&rc);
    HRGN hRgnItem = ::CreateRoundRectRgn(rcItem.left, rcItem.top, rcItem.right + 1, rcItem.bottom + 1, width, height);
    ::CombineRgn(clip.hRgn, clip.hRgn, hRgnItem, RGN_AND);
    ::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
    clip.hDC = hDC;
    clip.rcItem = rc;
    ::DeleteObject(hRgnItem);
}

void CRenderClip::UseOldClipBegin(HDC hDC, CRenderClip& clip)
{
    ::SelectClipRgn(hDC, clip.hOldRgn);
}

void CRenderClip::UseOldClipEnd(HDC hDC, CRenderClip& clip)
{
    ::SelectClipRgn(hDC, clip.hRgn);
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

static const float OneThird = 1.0f / 3;

static void RGBtoHSL(DWORD ARGB, float* H, float* S, float* L) {
    const float
        R = (float)GetRValue(ARGB),
        G = (float)GetGValue(ARGB),
        B = (float)GetBValue(ARGB),
        nR = (R<0?0:(R>255?255:R))/255,
        nG = (G<0?0:(G>255?255:G))/255,
        nB = (B<0?0:(B>255?255:B))/255,
        m = min(min(nR,nG),nB),
        M = max(max(nR,nG),nB);
    *L = (m + M)/2;
    if (M==m) *H = *S = 0;
    else {
        const float
            f = (nR==m)?(nG-nB):((nG==m)?(nB-nR):(nR-nG)),
            i = (nR==m)?3.0f:((nG==m)?5.0f:1.0f);
        *H = (i-f/(M-m));
        if (*H>=6) *H-=6;
        *H*=60;
        *S = (2*(*L)<=1)?((M-m)/(M+m)):((M-m)/(2-M-m));
    }
}

static void HSLtoRGB(DWORD* ARGB, float H, float S, float L) {
    const float
        q = 2*L<1?L*(1+S):(L+S-L*S),
        p = 2*L-q,
        h = H/360,
        tr = h + OneThird,
        tg = h,
        tb = h - OneThird,
        ntr = tr<0?tr+1:(tr>1?tr-1:tr),
        ntg = tg<0?tg+1:(tg>1?tg-1:tg),
        ntb = tb<0?tb+1:(tb>1?tb-1:tb),
        R = 255*(6*ntr<1?p+(q-p)*6*ntr:(2*ntr<1?q:(3*ntr<2?p+(q-p)*6*(2.0f*OneThird-ntr):p))),
        G = 255*(6*ntg<1?p+(q-p)*6*ntg:(2*ntg<1?q:(3*ntg<2?p+(q-p)*6*(2.0f*OneThird-ntg):p))),
        B = 255*(6*ntb<1?p+(q-p)*6*ntb:(2*ntb<1?q:(3*ntb<2?p+(q-p)*6*(2.0f*OneThird-ntb):p)));
    *ARGB &= 0xFF000000;
    *ARGB |= RGB( (BYTE)(R<0?0:(R>255?255:R)), (BYTE)(G<0?0:(G>255?255:G)), (BYTE)(B<0?0:(B>255?255:B)) );
}

static COLORREF PixelAlpha(COLORREF clrSrc, double src_darken, COLORREF clrDest, double dest_darken)
{
    return RGB (GetRValue (clrSrc) * src_darken + GetRValue (clrDest) * dest_darken, 
        GetGValue (clrSrc) * src_darken + GetGValue (clrDest) * dest_darken, 
        GetBValue (clrSrc) * src_darken + GetBValue (clrDest) * dest_darken);

}

static BOOL WINAPI AlphaBitBlt(HDC hDC, int nDestX, int nDestY, int dwWidth, int dwHeight, HDC hSrcDC, \
                        int nSrcX, int nSrcY, int wSrc, int hSrc, BLENDFUNCTION ftn)
{
    HDC hTempDC = ::CreateCompatibleDC(hDC);
    if (NULL == hTempDC)
        return FALSE;

    //Creates Source DIB
    LPBITMAPINFO lpbiSrc = NULL;
    // Fill in the BITMAPINFOHEADER
    lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	if (lpbiSrc == NULL)
	{
		::DeleteDC(hTempDC);
		return FALSE;
	}
    lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lpbiSrc->bmiHeader.biWidth = dwWidth;
    lpbiSrc->bmiHeader.biHeight = dwHeight;
    lpbiSrc->bmiHeader.biPlanes = 1;
    lpbiSrc->bmiHeader.biBitCount = 32;
    lpbiSrc->bmiHeader.biCompression = BI_RGB;
    lpbiSrc->bmiHeader.biSizeImage = dwWidth * dwHeight;
    lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
    lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
    lpbiSrc->bmiHeader.biClrUsed = 0;
    lpbiSrc->bmiHeader.biClrImportant = 0;

    COLORREF* pSrcBits = NULL;
    HBITMAP hSrcDib = CreateDIBSection (
        hSrcDC, lpbiSrc, DIB_RGB_COLORS, (void **)&pSrcBits,
        NULL, NULL);

    if ((NULL == hSrcDib) || (NULL == pSrcBits)) 
    {
		delete [] lpbiSrc;
        ::DeleteDC(hTempDC);
        return FALSE;
    }

    HBITMAP hOldTempBmp = (HBITMAP)::SelectObject (hTempDC, hSrcDib);
    ::StretchBlt(hTempDC, 0, 0, dwWidth, dwHeight, hSrcDC, nSrcX, nSrcY, wSrc, hSrc, SRCCOPY);
    ::SelectObject (hTempDC, hOldTempBmp);

    //Creates Destination DIB
    LPBITMAPINFO lpbiDest = NULL;
    // Fill in the BITMAPINFOHEADER
    lpbiDest = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	if (lpbiDest == NULL)
	{
        delete [] lpbiSrc;
        ::DeleteObject(hSrcDib);
        ::DeleteDC(hTempDC);
        return FALSE;
	}

    lpbiDest->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lpbiDest->bmiHeader.biWidth = dwWidth;
    lpbiDest->bmiHeader.biHeight = dwHeight;
    lpbiDest->bmiHeader.biPlanes = 1;
    lpbiDest->bmiHeader.biBitCount = 32;
    lpbiDest->bmiHeader.biCompression = BI_RGB;
    lpbiDest->bmiHeader.biSizeImage = dwWidth * dwHeight;
    lpbiDest->bmiHeader.biXPelsPerMeter = 0;
    lpbiDest->bmiHeader.biYPelsPerMeter = 0;
    lpbiDest->bmiHeader.biClrUsed = 0;
    lpbiDest->bmiHeader.biClrImportant = 0;

    COLORREF* pDestBits = NULL;
    HBITMAP hDestDib = CreateDIBSection (
        hDC, lpbiDest, DIB_RGB_COLORS, (void **)&pDestBits,
        NULL, NULL);

    if ((NULL == hDestDib) || (NULL == pDestBits))
    {
        delete [] lpbiSrc;
        ::DeleteObject(hSrcDib);
        ::DeleteDC(hTempDC);
        return FALSE;
    }

    ::SelectObject (hTempDC, hDestDib);
    ::BitBlt (hTempDC, 0, 0, dwWidth, dwHeight, hDC, nDestX, nDestY, SRCCOPY);
    ::SelectObject (hTempDC, hOldTempBmp);

    double src_darken;
    BYTE nAlpha;

    for (int pixel = 0; pixel < dwWidth * dwHeight; pixel++, pSrcBits++, pDestBits++)
    {
        nAlpha = LOBYTE(*pSrcBits >> 24);
        src_darken = (double) (nAlpha * ftn.SourceConstantAlpha) / 255.0 / 255.0;
        if( src_darken < 0.0 ) src_darken = 0.0;
        *pDestBits = PixelAlpha(*pSrcBits, src_darken, *pDestBits, 1.0 - src_darken);
    } //for

    ::SelectObject (hTempDC, hDestDib);
    ::BitBlt (hDC, nDestX, nDestY, dwWidth, dwHeight, hTempDC, 0, 0, SRCCOPY);
    ::SelectObject (hTempDC, hOldTempBmp);

    delete [] lpbiDest;
    ::DeleteObject(hDestDib);

    delete [] lpbiSrc;
    ::DeleteObject(hSrcDib);

    ::DeleteDC(hTempDC);
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

DWORD CRenderEngine::AdjustColor(DWORD dwColor, short H, short S, short L)
{
    if( H == 180 && S == 100 && L == 100 ) return dwColor;
    float fH, fS, fL;
    float S1 = S / 100.0f;
    float L1 = L / 100.0f;
    RGBtoHSL(dwColor, &fH, &fS, &fL);
    fH += (H - 180);
    fH = fH > 0 ? fH : fH + 360; 
    fS *= S1;
    fL *= L1;
    HSLtoRGB(&dwColor, fH, fS, fL);
    return dwColor;
}

TImageInfo* CRenderEngine::LoadImage(STRINGorID bitmap, LPCTSTR type, DWORD mask)
{
    LPBYTE pData = NULL;
    DWORD dwSize = 0;

	do 
	{
		if( type == NULL ) {
			CStdString sFile = CPaintManagerUI::GetResourcePath();
			if( CPaintManagerUI::GetResourceZip().IsEmpty() ) {
				sFile += bitmap.m_lpstr;
				HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
					FILE_ATTRIBUTE_NORMAL, NULL);
				if( hFile == INVALID_HANDLE_VALUE ) break;
				dwSize = ::GetFileSize(hFile, NULL);
				if( dwSize == 0 ) break;

				DWORD dwRead = 0;
				pData = new BYTE[ dwSize ];
				::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
				::CloseHandle( hFile );

				if( dwRead != dwSize ) {
					delete[] pData;
					break;
				}
			}
			else {
				sFile += CPaintManagerUI::GetResourceZip();
				HZIP hz = NULL;
				if( CPaintManagerUI::IsCachedResourceZip() ) hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
				else hz = OpenZip((void*)sFile.GetData(), 0, 2);
				if( hz == NULL ) break;
				ZIPENTRY ze; 
				int i; 
				if( FindZipItem(hz, bitmap.m_lpstr, true, &i, &ze) != 0 ) break;
				dwSize = ze.unc_size;
				if( dwSize == 0 ) break;
				pData = new BYTE[ dwSize ];
				int res = UnzipItem(hz, i, pData, dwSize, 3);
				if( res != 0x00000000 && res != 0x00000600) {
					delete[] pData;
					if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
					break;
				}
				if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
			}
		}
		else {
			HRSRC hResource = ::FindResource(CPaintManagerUI::GetGlobalResourceDll(), bitmap.m_lpstr, type);
			if( hResource == NULL ) break;
			HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetGlobalResourceDll(), hResource);
			if( hGlobal == NULL ) {
				FreeResource(hResource);
				break;
			}

			dwSize = ::SizeofResource(CPaintManagerUI::GetGlobalResourceDll(), hResource);
			if( dwSize == 0 ) break;
			pData = new BYTE[ dwSize ];
			::CopyMemory(pData, (LPBYTE)::LockResource(hGlobal), dwSize);
			::FreeResource(hResource);
		}
	} while (0);

	if (!pData)
	{
		//读不到图片, 则直接去读取bitmap.m_lpstr执向的路径
		HANDLE hFile = ::CreateFile(bitmap.m_lpstr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
			FILE_ATTRIBUTE_NORMAL, NULL);
		if( hFile == INVALID_HANDLE_VALUE ) return NULL;
		dwSize = ::GetFileSize(hFile, NULL);
		if( dwSize == 0 ) return NULL;

		DWORD dwRead = 0;
		pData = new BYTE[ dwSize ];
		::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
		::CloseHandle( hFile );

		if( dwRead != dwSize ) {
			delete[] pData;
			return NULL;
		}
	}

    LPBYTE pImage = NULL;
    int x,y,n;
    pImage = stbi_load_from_memory(pData, dwSize, &x, &y, &n, 4);
    delete[] pData;
    if( !pImage ) return NULL;

    BITMAPINFO bmi;
    ::ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = x;
    bmi.bmiHeader.biHeight = -y;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = x * y * 4;

    bool bAlphaChannel = false;
    LPBYTE pDest = NULL;
    HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
    if( !hBitmap ) return NULL;

    for( int i = 0; i < x * y; i++ ) 
    {
        pDest[i*4 + 3] = pImage[i*4 + 3];
        if( pDest[i*4 + 3] < 255 )
        {
            pDest[i*4] = (BYTE)(DWORD(pImage[i*4 + 2])*pImage[i*4 + 3]/255);
            pDest[i*4 + 1] = (BYTE)(DWORD(pImage[i*4 + 1])*pImage[i*4 + 3]/255);
            pDest[i*4 + 2] = (BYTE)(DWORD(pImage[i*4])*pImage[i*4 + 3]/255); 
            bAlphaChannel = true;
        }
        else
        {
            pDest[i*4] = pImage[i*4 + 2];
            pDest[i*4 + 1] = pImage[i*4 + 1];
            pDest[i*4 + 2] = pImage[i*4]; 
        }

        if( *(DWORD*)(&pDest[i*4]) == mask ) {
            pDest[i*4] = (BYTE)0;
            pDest[i*4 + 1] = (BYTE)0;
            pDest[i*4 + 2] = (BYTE)0; 
            pDest[i*4 + 3] = (BYTE)0;
            bAlphaChannel = true;
        }
    }

    stbi_image_free(pImage);

    TImageInfo* data = new TImageInfo;
    data->hBitmap = hBitmap;
    data->nX = x;
    data->nY = y;
    data->alphaChannel = bAlphaChannel;
	data->hGrayBitmap = NULL;
    return data;
}

void CRenderEngine::FreeImage(const TImageInfo* bitmap)
{
	if (bitmap->hBitmap) {
		::DeleteObject(bitmap->hBitmap) ; 
	}
	delete bitmap ;
}

Gdiplus::Bitmap* CreateBitmapFromHBITMAP(HBITMAP hBitmap)
{
    BITMAP bmp = { 0 };
    if (0 == GetObject(hBitmap, sizeof(BITMAP), (LPVOID)&bmp))
    {
        return NULL;
    }
    // Although we can get bitmap data address by bmp.bmBits member of BITMAP   
    // which is got by GetObject function sometime,  
    // we can determine the bitmap data in the HBITMAP is arranged bottom-up   
    // or top-down, so we should always use GetDIBits to get bitmap data.  
    BYTE *piexlsSrc = NULL;
    BITMAPINFO bmpInfo = { 0 };
    LONG cbSize = bmp.bmWidthBytes * bmp.bmHeight;
    piexlsSrc = new BYTE[cbSize];

    // We should initialize the first six members of BITMAPINFOHEADER structure.  
    // A bottom-up DIB is specified by setting the height to a positive number,   
    // while a top-down DIB is specified by setting the height to a negative number.  
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = bmp.bmWidth;
    bmpInfo.bmiHeader.biHeight = bmp.bmHeight; // 正数，说明数据从下到上，如未负数，则从上到下  
    bmpInfo.bmiHeader.biPlanes = bmp.bmPlanes;
    bmpInfo.bmiHeader.biBitCount = bmp.bmBitsPixel;
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    HDC hdcScreen = CreateDC(L"DISPLAY", NULL, NULL, NULL);
    LONG cbCopied = GetDIBits(hdcScreen, hBitmap, 0, bmp.bmHeight, piexlsSrc, &bmpInfo, DIB_RGB_COLORS);
    DeleteDC(hdcScreen);
    if (0 == cbCopied)
    {
        delete[] piexlsSrc;
        return NULL;
    }
    // Create an GDI+ Bitmap has the same dimensions with hbitmap  
    Gdiplus::Bitmap *pBitmap = new Gdiplus::Bitmap(bmp.bmWidth, bmp.bmHeight, PixelFormat32bppPARGB);
    // Access to the Gdiplus::Bitmap's pixel data  
    BitmapData bitmapData;
    Rect rect(0, 0, bmp.bmWidth, bmp.bmHeight);
    if (Ok != pBitmap->LockBits(&rect, ImageLockModeRead, PixelFormat32bppPARGB, &bitmapData))
    {
        delete pBitmap;
        return NULL;
    }
    BYTE *pixelsDest = (BYTE*)bitmapData.Scan0;
    int nLinesize = bmp.bmWidth * sizeof(UINT);
    int nHeight = bmp.bmHeight;
    // Copy pixel data from HBITMAP by bottom-up.  
    for (int y = 0; y < nHeight; y++)
    {
        // 从下到上复制数据，因为前面设置高度时是正数。  
        memcpy_s((pixelsDest + y * nLinesize), nLinesize, (piexlsSrc + (nHeight - y - 1) * nLinesize), nLinesize);
    }
    // Copy the data in temporary buffer to pBitmap  
    if (Ok != pBitmap->UnlockBits(&bitmapData))
    {
        delete pBitmap;
    }
    delete[] piexlsSrc;
    return pBitmap;
}

void CRenderEngine::DrawImage(HDC hDC, HBITMAP hBitmap, const RECT& rc, const RECT& rcPaint,
                                    const RECT& rcBmpPart, const RECT& rcCorners, bool alphaChannel, 
                                    BYTE uFade, bool hole, bool xtiled, bool ytiled)
{
    ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

    // 使用GDI+ 绘制图片
#ifdef USE_GDIPlUS_DRAW
    Gdiplus::Graphics ghDCDraw(hDC);
    Gdiplus::Rect ghDestRect;
    RECT rcTemp = { 0 };
    RECT rcDest = { 0 };
    Gdiplus::Bitmap *ghSrcBmp = CreateBitmapFromHBITMAP(hBitmap);

    if (ghSrcBmp == NULL)
        return;

    ghDCDraw.SetSmoothingMode(SmoothingModeAntiAlias);
    ghDCDraw.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    if (!hole) 
    {
        rcDest.left = rc.left + rcCorners.left;
        rcDest.top = rc.top + rcCorners.top;
        rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
        rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
        rcDest.right += rcDest.left;
        rcDest.bottom += rcDest.top;
        if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
        {
            if (!xtiled && !ytiled)
            {
                ghDestRect.X = rcDest.left;
                ghDestRect.Y = rcDest.top;
                ghDestRect.Width = rcDest.right -= rcDest.left;
                ghDestRect.Height = rcDest.bottom -= rcDest.top;
                ghDCDraw.DrawImage(ghSrcBmp, ghDestRect,
                    rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top,
                    rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right,
                    rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, UnitPixel);
            }
            else if (xtiled && ytiled) {
                LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                for (int j = 0; j < iTimesY; ++j) {
                    LONG lDestTop = rcDest.top + lHeight * j;
                    LONG lDestBottom = rcDest.top + lHeight * (j + 1);
                    LONG lDrawHeight = lHeight;
                    if (lDestBottom > rcDest.bottom) {
                        lDrawHeight -= lDestBottom - rcDest.bottom;
                        lDestBottom = rcDest.bottom;
                    }
                    for (int i = 0; i < iTimesX; ++i) 
                    {
                        LONG lDestLeft = rcDest.left + lWidth * i;
                        LONG lDestRight = rcDest.left + lWidth * (i + 1);
                        LONG lDrawWidth = lWidth;
                        if (lDestRight > rcDest.right)
                        {
                            lDrawWidth -= lDestRight - rcDest.right;
                            lDestRight = rcDest.right;
                        }
                        ghDestRect.X = rcDest.left + lWidth * i;
                        ghDestRect.Y = rcDest.top + lHeight * j;
                        ghDestRect.Width = lDestRight - lDestLeft;
                        ghDestRect.Height = lDestBottom - lDestTop;
                        ghDCDraw.DrawImage(ghSrcBmp, ghDestRect,
                            rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, lDrawWidth, lDrawHeight, UnitPixel);
                    }
                }
            }
            else if (xtiled) 
            {
                LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                for (int i = 0; i < iTimes; ++i)
                {
                    LONG lDestLeft = rcDest.left + lWidth * i;
                    LONG lDestRight = rcDest.left + lWidth * (i + 1);
                    LONG lDrawWidth = lWidth;
                    if (lDestRight > rcDest.right)
                    {
                        lDrawWidth -= lDestRight - rcDest.right;
                        lDestRight = rcDest.right;
                    }
                    ghDestRect.X = lDestLeft;
                    ghDestRect.Y = rcDest.top;
                    ghDestRect.Width = lDestRight - lDestLeft;
                    ghDestRect.Height = rcDest.bottom;
                    ghDCDraw.DrawImage(ghSrcBmp, ghDestRect,
                        rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, 
                        lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, UnitPixel);
                }
            }
            else { // ytiled
                LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                for (int i = 0; i < iTimes; ++i)
                {
                    LONG lDestTop = rcDest.top + lHeight * i;
                    LONG lDestBottom = rcDest.top + lHeight * (i + 1);
                    LONG lDrawHeight = lHeight;
                    if (lDestBottom > rcDest.bottom)
                    {
                        lDrawHeight -= lDestBottom - rcDest.bottom;
                        lDestBottom = rcDest.bottom;
                    }
                    ghDestRect.X = rcDest.left;
                    ghDestRect.Y = rcDest.top + lHeight * i;
                    ghDestRect.Width = rcDest.right;
                    ghDestRect.Height = lDestBottom - lDestTop;
                    ghDCDraw.DrawImage(ghSrcBmp, ghDestRect,
                        rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
                        rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, UnitPixel);
                }
            }
        }
    }

    // left-top
    if (rcCorners.left > 0 && rcCorners.top > 0) {
        rcDest.left = rc.left;
        rcDest.top = rc.top;
        rcDest.right = rcCorners.left;
        rcDest.bottom = rcCorners.top;
        rcDest.right += rcDest.left;
        rcDest.bottom += rcDest.top;
        if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;
            ghDestRect.X = rcDest.left;
            ghDestRect.Y = rcDest.top;
            ghDestRect.Width = rcDest.right;
            ghDestRect.Height = rcDest.bottom;
            ghDCDraw.DrawImage(ghSrcBmp, ghDestRect,
                rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, UnitPixel);
        }
    }
    // top
    if (rcCorners.top > 0) {
        rcDest.left = rc.left + rcCorners.left;
        rcDest.top = rc.top;
        rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
        rcDest.bottom = rcCorners.top;
        rcDest.right += rcDest.left;
        rcDest.bottom += rcDest.top;
        if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
        {
            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;
            ghDestRect.X = rcDest.left;
            ghDestRect.Y = rcDest.top;
            ghDestRect.Width = rcDest.right;
            ghDestRect.Height = rcDest.bottom;
            ghDCDraw.DrawImage(ghSrcBmp, ghDestRect,
                rcBmpPart.left + rcCorners.left, rcBmpPart.top, 
                rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.top, UnitPixel);
        }
    }
    // right-top
    if (rcCorners.right > 0 && rcCorners.top > 0) {
        rcDest.left = rc.right - rcCorners.right;
        rcDest.top = rc.top;
        rcDest.right = rcCorners.right;
        rcDest.bottom = rcCorners.top;
        rcDest.right += rcDest.left;
        rcDest.bottom += rcDest.top;
        if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
        {
            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;
            ghDestRect.X = rcDest.left;
            ghDestRect.Y = rcDest.top;
            ghDestRect.Width = rcDest.right;
            ghDestRect.Height = rcDest.bottom;
            ghDCDraw.DrawImage(ghSrcBmp, ghDestRect,
                rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, UnitPixel);
        }
    }
    // left
    if (rcCorners.left > 0)
    {
        rcDest.left = rc.left;
        rcDest.top = rc.top + rcCorners.top;
        rcDest.right = rcCorners.left;
        rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
        rcDest.right += rcDest.left;
        rcDest.bottom += rcDest.top;
        if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
        {
            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;
            ghDestRect.X = rcDest.left;
            ghDestRect.Y = rcDest.top;
            ghDestRect.Width = rcDest.right;
            ghDestRect.Height = rcDest.bottom;
            ghDCDraw.DrawImage(ghSrcBmp, ghDestRect,
                rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, 
                rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, UnitPixel);
        }
    }
    // right
    if (rcCorners.right > 0) {
        rcDest.left = rc.right - rcCorners.right;
        rcDest.top = rc.top + rcCorners.top;
        rcDest.right = rcCorners.right;
        rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
        rcDest.right += rcDest.left;
        rcDest.bottom += rcDest.top;
        if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
        {
            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;
            ghDestRect.X = rcDest.left;
            ghDestRect.Y = rcDest.top;
            ghDestRect.Width = rcDest.right;
            ghDestRect.Height = rcDest.bottom;
            ghDCDraw.DrawImage(ghSrcBmp, ghDestRect,
                rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, 
                rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, UnitPixel);
        }
    }
    // left-bottom
    if (rcCorners.left > 0 && rcCorners.bottom > 0) {
        rcDest.left = rc.left;
        rcDest.top = rc.bottom - rcCorners.bottom;
        rcDest.right = rcCorners.left;
        rcDest.bottom = rcCorners.bottom;
        rcDest.right += rcDest.left;
        rcDest.bottom += rcDest.top;
        if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
        {
            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;
            ghDestRect.X = rcDest.left;
            ghDestRect.Y = rcDest.top;
            ghDestRect.Width = rcDest.right;
            ghDestRect.Height = rcDest.bottom;
            ghDCDraw.DrawImage(ghSrcBmp, ghDestRect,
                rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, UnitPixel);
        }
    }
    // bottom
    if (rcCorners.bottom > 0) {
        rcDest.left = rc.left + rcCorners.left;
        rcDest.top = rc.bottom - rcCorners.bottom;
        rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
        rcDest.bottom = rcCorners.bottom;
        rcDest.right += rcDest.left;
        rcDest.bottom += rcDest.top;
        if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
        {
            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;
            ghDestRect.X = rcDest.left;
            ghDestRect.Y = rcDest.top;
            ghDestRect.Width = rcDest.right;
            ghDestRect.Height = rcDest.bottom;
            ghDCDraw.DrawImage(ghSrcBmp, ghDestRect,
                rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
                rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, UnitPixel);
        }
    }
    // right-bottom
    if (rcCorners.right > 0 && rcCorners.bottom > 0) {
        rcDest.left = rc.right - rcCorners.right;
        rcDest.top = rc.bottom - rcCorners.bottom;
        rcDest.right = rcCorners.right;
        rcDest.bottom = rcCorners.bottom;
        rcDest.right += rcDest.left;
        rcDest.bottom += rcDest.top;
        if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
            rcDest.right -= rcDest.left;
            rcDest.bottom -= rcDest.top;
            ghDestRect.X = rcDest.left;
            ghDestRect.Y = rcDest.top;
            ghDestRect.Width = rcDest.right;
            ghDestRect.Height = rcDest.bottom;
            ghDCDraw.DrawImage(ghSrcBmp, ghDestRect,
                rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, 
                rcCorners.right, rcCorners.bottom, UnitPixel);
        }
    }
    ghDCDraw.ReleaseHDC(hDC);
    delete ghSrcBmp;
    // Gdiplus::CachedBitmap ghCachedBmp(&ghBKBmp, &ghDCDraw);
    // ghDCDraw.DrawCachedBitmap(&ghCachedBmp, 0, 0);

#else
    typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
    static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");

    if( lpAlphaBlend == NULL ) lpAlphaBlend = AlphaBitBlt;
    if( hBitmap == NULL ) return;

    HDC hCloneDC = ::CreateCompatibleDC(hDC);
    HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
    ::SetStretchBltMode(hDC, COLORONCOLOR);


    RECT rcTemp = {0};
    RECT rcDest = {0};
    if( lpAlphaBlend && (alphaChannel || uFade < 255) ) {
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, uFade, AC_SRC_ALPHA };
        // middle
        if( !hole ) {
            rcDest.left = rc.left + rcCorners.left;
            rcDest.top = rc.top + rcCorners.top;
            rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
            rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                if( !xtiled && !ytiled ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
                        rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
                        rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
                }
                else if( xtiled && ytiled ) {
                    LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                    LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                    int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                    int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                    for( int j = 0; j < iTimesY; ++j ) {
                        LONG lDestTop = rcDest.top + lHeight * j;
                        LONG lDestBottom = rcDest.top + lHeight * (j + 1);
                        LONG lDrawHeight = lHeight;
                        if( lDestBottom > rcDest.bottom ) {
                            lDrawHeight -= lDestBottom - rcDest.bottom;
                            lDestBottom = rcDest.bottom;
                        }
                        for( int i = 0; i < iTimesX; ++i ) {
                            LONG lDestLeft = rcDest.left + lWidth * i;
                            LONG lDestRight = rcDest.left + lWidth * (i + 1);
                            LONG lDrawWidth = lWidth;
                            if( lDestRight > rcDest.right ) {
                                lDrawWidth -= lDestRight - rcDest.right;
                                lDestRight = rcDest.right;
                            }
                            lpAlphaBlend(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, 
                                lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, 
                                rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, lDrawWidth, lDrawHeight, bf);
                        }
                    }
                }
                else if( xtiled ) {
                    LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                    int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                    for( int i = 0; i < iTimes; ++i ) {
                        LONG lDestLeft = rcDest.left + lWidth * i;
                        LONG lDestRight = rcDest.left + lWidth * (i + 1);
                        LONG lDrawWidth = lWidth;
                        if( lDestRight > rcDest.right ) {
                            lDrawWidth -= lDestRight - rcDest.right;
                            lDestRight = rcDest.right;
                        }
                        lpAlphaBlend(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
                            hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
                            lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
                    }
                }
                else { // ytiled
                    LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                    int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                    for( int i = 0; i < iTimes; ++i ) {
                        LONG lDestTop = rcDest.top + lHeight * i;
                        LONG lDestBottom = rcDest.top + lHeight * (i + 1);
                        LONG lDrawHeight = lHeight;
                        if( lDestBottom > rcDest.bottom ) {
                            lDrawHeight -= lDestBottom - rcDest.bottom;
                            lDestBottom = rcDest.bottom;
                        }
                        lpAlphaBlend(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
                            hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
                            rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, bf);                    
                    }
                }
            }
        }

        // left-top
        if( rcCorners.left > 0 && rcCorners.top > 0 ) {
            rcDest.left = rc.left;
            rcDest.top = rc.top;
            rcDest.right = rcCorners.left;
            rcDest.bottom = rcCorners.top;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                    rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, bf);
            }
        }
        // top
        if( rcCorners.top > 0 ) {
            rcDest.left = rc.left + rcCorners.left;
            rcDest.top = rc.top;
            rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
            rcDest.bottom = rcCorners.top;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                    rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
                    rcCorners.left - rcCorners.right, rcCorners.top, bf);
            }
        }
        // right-top
        if( rcCorners.right > 0 && rcCorners.top > 0 ) {
            rcDest.left = rc.right - rcCorners.right;
            rcDest.top = rc.top;
            rcDest.right = rcCorners.right;
            rcDest.bottom = rcCorners.top;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                    rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, bf);
            }
        }
        // left
        if( rcCorners.left > 0 ) {
            rcDest.left = rc.left;
            rcDest.top = rc.top + rcCorners.top;
            rcDest.right = rcCorners.left;
            rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                    rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
                    rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
            }
        }
        // right
        if( rcCorners.right > 0 ) {
            rcDest.left = rc.right - rcCorners.right;
            rcDest.top = rc.top + rcCorners.top;
            rcDest.right = rcCorners.right;
            rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                    rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
                    rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
            }
        }
        // left-bottom
        if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
            rcDest.left = rc.left;
            rcDest.top = rc.bottom - rcCorners.bottom;
            rcDest.right = rcCorners.left;
            rcDest.bottom = rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                    rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, bf);
            }
        }
        // bottom
        if( rcCorners.bottom > 0 ) {
            rcDest.left = rc.left + rcCorners.left;
            rcDest.top = rc.bottom - rcCorners.bottom;
            rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
            rcDest.bottom = rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                    rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
                    rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, bf);
            }
        }
        // right-bottom
        if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
            rcDest.left = rc.right - rcCorners.right;
            rcDest.top = rc.bottom - rcCorners.bottom;
            rcDest.right = rcCorners.right;
            rcDest.bottom = rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                    rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
                    rcCorners.bottom, bf);
            }
        }
    }
    else 
    {
        if (rc.right - rc.left == rcBmpPart.right - rcBmpPart.left \
            && rc.bottom - rc.top == rcBmpPart.bottom - rcBmpPart.top \
            && rcCorners.left == 0 && rcCorners.right == 0 && rcCorners.top == 0 && rcCorners.bottom == 0)
        {
            if( ::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
                ::BitBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top, \
                    hCloneDC, rcBmpPart.left + rcTemp.left - rc.left, rcBmpPart.top + rcTemp.top - rc.top, SRCCOPY);
            }
        }
        else
        {
            // middle
            if( !hole ) {
                rcDest.left = rc.left + rcCorners.left;
                rcDest.top = rc.top + rcCorners.top;
                rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
                rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    if( !xtiled && !ytiled ) {
                        rcDest.right -= rcDest.left;
                        rcDest.bottom -= rcDest.top;
                        ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                            rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
                            rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
                            rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
                    }
                    else if( xtiled && ytiled ) {
                        LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                        LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                        int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                        int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                        for( int j = 0; j < iTimesY; ++j ) {
                            LONG lDestTop = rcDest.top + lHeight * j;
                            LONG lDestBottom = rcDest.top + lHeight * (j + 1);
                            LONG lDrawHeight = lHeight;
                            if( lDestBottom > rcDest.bottom ) {
                                lDrawHeight -= lDestBottom - rcDest.bottom;
                                lDestBottom = rcDest.bottom;
                            }
                            for( int i = 0; i < iTimesX; ++i ) {
                                LONG lDestLeft = rcDest.left + lWidth * i;
                                LONG lDestRight = rcDest.left + lWidth * (i + 1);
                                LONG lDrawWidth = lWidth;
                                if( lDestRight > rcDest.right ) {
                                    lDrawWidth -= lDestRight - rcDest.right;
                                    lDestRight = rcDest.right;
                                }
                                ::BitBlt(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, \
                                    lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, \
                                    rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, SRCCOPY);
                            }
                        }
                    }
                    else if( xtiled ) {
                        LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                        int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                        for( int i = 0; i < iTimes; ++i ) {
                            LONG lDestLeft = rcDest.left + lWidth * i;
                            LONG lDestRight = rcDest.left + lWidth * (i + 1);
                            LONG lDrawWidth = lWidth;
                            if( lDestRight > rcDest.right ) {
                                lDrawWidth -= lDestRight - rcDest.right;
                                lDestRight = rcDest.right;
                            }
                            ::StretchBlt(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
                                hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
                                lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
                        }
                    }
                    else { // ytiled
                        LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                        int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                        for( int i = 0; i < iTimes; ++i ) {
                            LONG lDestTop = rcDest.top + lHeight * i;
                            LONG lDestBottom = rcDest.top + lHeight * (i + 1);
                            LONG lDrawHeight = lHeight;
                            if( lDestBottom > rcDest.bottom ) {
                                lDrawHeight -= lDestBottom - rcDest.bottom;
                                lDestBottom = rcDest.bottom;
                            }
                            ::StretchBlt(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
                                hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
                                rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, SRCCOPY);                    
                        }
                    }
                }
            }
            
            // left-top
            if( rcCorners.left > 0 && rcCorners.top > 0 ) {
                rcDest.left = rc.left;
                rcDest.top = rc.top;
                rcDest.right = rcCorners.left;
                rcDest.bottom = rcCorners.top;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, SRCCOPY);
                }
            }
            // top
            if( rcCorners.top > 0 ) {
                rcDest.left = rc.left + rcCorners.left;
                rcDest.top = rc.top;
                rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
                rcDest.bottom = rcCorners.top;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
                        rcCorners.left - rcCorners.right, rcCorners.top, SRCCOPY);
                }
            }
            // right-top
            if( rcCorners.right > 0 && rcCorners.top > 0 ) {
                rcDest.left = rc.right - rcCorners.right;
                rcDest.top = rc.top;
                rcDest.right = rcCorners.right;
                rcDest.bottom = rcCorners.top;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, SRCCOPY);
                }
            }
            // left
            if( rcCorners.left > 0 ) {
                rcDest.left = rc.left;
                rcDest.top = rc.top + rcCorners.top;
                rcDest.right = rcCorners.left;
                rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
                        rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
                }
            }
            // right
            if( rcCorners.right > 0 ) {
                rcDest.left = rc.right - rcCorners.right;
                rcDest.top = rc.top + rcCorners.top;
                rcDest.right = rcCorners.right;
                rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
                        rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
                }
            }
            // left-bottom
            if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
                rcDest.left = rc.left;
                rcDest.top = rc.bottom - rcCorners.bottom;
                rcDest.right = rcCorners.left;
                rcDest.bottom = rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, SRCCOPY);
                }
            }
            // bottom
            if( rcCorners.bottom > 0 ) {
                rcDest.left = rc.left + rcCorners.left;
                rcDest.top = rc.bottom - rcCorners.bottom;
                rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
                rcDest.bottom = rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
                        rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, SRCCOPY);
                }
            }
            // right-bottom
            if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
                rcDest.left = rc.right - rcCorners.right;
                rcDest.top = rc.bottom - rcCorners.bottom;
                rcDest.right = rcCorners.right;
                rcDest.bottom = rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
                        rcCorners.bottom, SRCCOPY);
                }
            }
        }
    }

    ::SelectObject(hCloneDC, hOldBitmap);
    ::DeleteDC(hCloneDC);
#endif 
}


bool DrawImage(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, const CStdString& sImageName, \
		const CStdString& sImageResType, RECT rcItem, RECT rcBmpPart, RECT rcCorner, DWORD dwMask, BYTE bFade, \
		bool bHole, bool bTiledX, bool bTiledY, bool grayimage ,int aligncenter)
{
	const TImageInfo* data = NULL;
	if (grayimage)
	{
		if( sImageResType.IsEmpty() ) {
			data = pManager->GetGrayImageEx((LPCTSTR)sImageName, NULL, dwMask);
		}
		else {
			data = pManager->GetGrayImageEx((LPCTSTR)sImageName, (LPCTSTR)sImageResType, dwMask);
		}
	}
	else
	{
		if( sImageResType.IsEmpty() ) {
			data = pManager->GetImageEx((LPCTSTR)sImageName, NULL, dwMask);
		}
		else {
			data = pManager->GetImageEx((LPCTSTR)sImageName, (LPCTSTR)sImageResType, dwMask);
		}
	}

	if (!data)
	{
		return false;
	}
	if( rcBmpPart.left == 0 && rcBmpPart.right == 0 && rcBmpPart.top == 0 && rcBmpPart.bottom == 0 ) {
		rcBmpPart.right = data->nX;
		rcBmpPart.bottom = data->nY;
	}
	if (rcBmpPart.right > data->nX) rcBmpPart.right = data->nX;
	if (rcBmpPart.bottom > data->nY) rcBmpPart.bottom = data->nY;

	if (aligncenter > 0)//用来图形显示到中间
	{
		switch(aligncenter)
		{
		case CENTER:
			{
				int offsetwidth = (rc.right - rc.left - data->nX)/2;
				rcItem.left = offsetwidth + rc.left;
				rcItem.right = rcItem.left + data->nX;
				int offsetheight = (rc.bottom - rc.top - data->nY)/2;
				rcItem.top = offsetheight + rc.top;
				rcItem.bottom = rcItem.top + data->nY;
			}break;	

		case CENTERHEIGHT:
			{
				int offsetheight = (rc.bottom - rc.top - data->nY)/2;
				rcItem.top = offsetheight + rc.top;
				rcItem.bottom = rcItem.top + data->nY;
			}break;	
		case RIGHT_BOTTOM:
			{
				rcItem.left = rcItem.right - data->nX;
				rcItem.top = rcItem.bottom - data->nY;
			}break;	
		case TILELAST:
			{//拉伸右下角处。
				int length = 5;//5像素。2太少了
				if (data->nX > length && data->nY > length)
				{
					rcCorner.left = data->nX - length;
					rcCorner.top = data->nY - length;
					rcCorner.right = 1;
					rcCorner.bottom = 1;
					bTiledX = false;
				}
			}break;	
		}
			
	}
	RECT rcTemp;
	if( !::IntersectRect(&rcTemp, &rcItem, &rc) ) return true;
	if( !::IntersectRect(&rcTemp, &rcItem, &rcPaint) ) return true;

	if (grayimage)
		CRenderEngine::DrawImage(hDC, data->hGrayBitmap, rcItem, rcPaint, rcBmpPart, rcCorner, pManager->GetBackgroundTransparent()? true : data->alphaChannel, bFade, bHole, bTiledX, bTiledY);
	else
		CRenderEngine::DrawImage(hDC, data->hBitmap, rcItem, rcPaint, rcBmpPart, rcCorner, pManager->GetBackgroundTransparent()? true : data->alphaChannel, bFade, bHole, bTiledX, bTiledY);

	return true;
}

bool CRenderEngine::DrawImageString(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, 
                                          LPCTSTR pStrImage, LPCTSTR pStrModify,bool grayimage)
{
	if ((pManager == NULL) || (hDC == NULL)) return false;

    // 1、aaa.jpg
    // 2、file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' 
    // mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false'

    CStdString sImageName = pStrImage;
    CStdString sImageResType;
    RECT rcItem = rc;
    RECT rcBmpPart = {0};
    RECT rcCorner = {0};
    DWORD dwMask = 0;
    BYTE bFade = 0xFF;
    bool bHole = false;
    bool bTiledX = false;
    bool bTiledY = false;
	int aligncenter = 0;

	int image_count = 0;

    CStdString sItem;
    CStdString sValue;
    LPTSTR pstr = NULL;

    for( int i = 0; i < 2; ++i,image_count = 0 ) {
        if( i == 1)
            pStrImage = pStrModify;

        if( !pStrImage ) continue;

        while( *pStrImage != _T('\0') ) {
            sItem.Empty();
            sValue.Empty();
            while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
            while( *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ') ) {
                LPTSTR pstrTemp = ::CharNext(pStrImage);
                while( pStrImage < pstrTemp) {
                    sItem += *pStrImage++;
                }
            }
            while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
            if( *pStrImage++ != _T('=') ) break;
            while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
            if( *pStrImage++ != _T('\'') ) break;
            while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) {
                LPTSTR pstrTemp = ::CharNext(pStrImage);
                while( pStrImage < pstrTemp) {
                    sValue += *pStrImage++;
                }
            }
            if( *pStrImage++ != _T('\'') ) break;
            if( !sValue.IsEmpty() ) {
                if( sItem == _T("file") || sItem == _T("res") ) {
					if( image_count > 0 )
						DuiLib::DrawImage(hDC, pManager, rc, rcPaint, sImageName, sImageResType,
							rcItem, rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY,grayimage,aligncenter);

                    sImageName = sValue;
					if( sItem == _T("file") )
						++image_count;
                }
                else if( sItem == _T("restype") ) {
					if( image_count > 0 )
						DuiLib::DrawImage(hDC, pManager, rc, rcPaint, sImageName, sImageResType,
							rcItem, rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY,grayimage,aligncenter);

                    sImageResType = sValue;
					++image_count;
                }
                else if( sItem == _T("dest") ) {
                    rcItem.left = rc.left + _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);    
                    rcItem.top = rc.top + _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
                    rcItem.right = rc.left + _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					if (rcItem.right > rc.right) rcItem.right = rc.right;
                    rcItem.bottom = rc.top + _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
					if (rcItem.bottom > rc.bottom) rcItem.bottom = rc.bottom;
                }
                else if( sItem == _T("source") ) {
                    rcBmpPart.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);    
                    rcBmpPart.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
                    rcBmpPart.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
                    rcBmpPart.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);  
                }
                else if( sItem == _T("corner") ) {
                    rcCorner.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);    
                    rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
                    rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
                    rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
                }
                else if( sItem == _T("mask") ) {
                    if( sValue[0] == _T('#')) dwMask = _tcstoul(sValue.GetData() + 1, &pstr, 16);
                    else dwMask = _tcstoul(sValue.GetData(), &pstr, 16);
                }
                else if( sItem == _T("fade") ) {
                    bFade = (BYTE)_tcstoul(sValue.GetData(), &pstr, 10);
                }
                else if( sItem == _T("hole") ) {
                    bHole = (_tcscmp(sValue.GetData(), _T("true")) == 0);
                }
                else if( sItem == _T("xtiled") ) {
                    bTiledX = (_tcscmp(sValue.GetData(), _T("true")) == 0);
                }
                else if( sItem == _T("ytiled") ) {
                    bTiledY = (_tcscmp(sValue.GetData(), _T("true")) == 0);
                }
				else if( sItem == _T("align") ) {
					if(_tcscmp(sValue.GetData(), _T("center")) == 0)
						aligncenter = CENTER;
					else if(_tcscmp(sValue.GetData(), _T("true")) == 0)//覆盖以前的数据
						aligncenter = CENTER;
					else if(_tcscmp(sValue.GetData(), _T("rightbottom")) == 0)
						aligncenter = RIGHT_BOTTOM;
					else if(_tcscmp(sValue.GetData(), _T("tile")) == 0)
						aligncenter = TILELAST;
					else if(_tcscmp(sValue.GetData(), _T("cheight")) == 0)
						aligncenter = CENTERHEIGHT;
					else if(_tcscmp(sValue.GetData(), _T("cwidth")) == 0)
						aligncenter = CENTERWIDTH;
					else if(_tcscmp(sValue.GetData(), _T("left")) == 0)
						aligncenter = LEFT;
					else if(_tcscmp(sValue.GetData(), _T("lefttop")) == 0)
						aligncenter = LEFT_TOP;
					else if(_tcscmp(sValue.GetData(), _T("leftbottom")) == 0)
						aligncenter = LEFT_BOTTOM;
					else if(_tcscmp(sValue.GetData(), _T("right")) == 0)
						aligncenter = RIGHT;
					else if(_tcscmp(sValue.GetData(), _T("righttop")) == 0)
						aligncenter = RIGHT_TOP;
					else if(_tcscmp(sValue.GetData(), _T("top")) == 0)
						aligncenter = TOP;
					else if(_tcscmp(sValue.GetData(), _T("bottom")) == 0)
						aligncenter = BOTTOM;
					else if(_tcscmp(sValue.GetData(), _T("xagradtile")) == 0) //X方向自动右边渐变平铺
						aligncenter = XAUTOGRADUALTILE;
					else if(_tcscmp(sValue.GetData(), _T("yagradtile")) == 0) //X方向自动右边渐变平铺
						aligncenter = YAUTOGRADUALTILE;

				}
			}
            if( *pStrImage++ != _T(' ') ) break;
        }
    }

	DuiLib::DrawImage(hDC, pManager, rc, rcPaint, sImageName, sImageResType,
		rcItem, rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY,grayimage,aligncenter);

    return true;
}

void CRenderEngine::DrawColor(HDC hDC, const RECT& rc, DWORD color)
{
    if( color <= 0x00FFFFFF ) return;
#ifdef USE_GDIPlUS_DRAW
    Gdiplus::Graphics ghDCDraw(hDC);
    Gdiplus::SolidBrush ghBGBrush(Gdiplus::Color(HIBYTE((color) >> 16), GetRValue(color), GetGValue(color), GetBValue(color)));
    Gdiplus::Rect ghDestRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
    
    ghDCDraw.FillRectangle(&ghBGBrush, ghDestRect);
    ghDCDraw.ReleaseHDC(hDC);
#else
    if( color >= 0xFF000000 )
    {
        ::SetBkColor(hDC, RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
        ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
    }
    else
    {
        // Create a new 32bpp bitmap with room for an alpha channel
        BITMAPINFO bmi = { 0 };
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = 1;
        bmi.bmiHeader.biHeight = 1;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = 1 * 1 * sizeof(DWORD);
        LPDWORD pDest = NULL;
        HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (LPVOID*) &pDest, NULL, 0);
        if( !hBitmap ) return;

        *pDest = color;

        RECT rcBmpPart = {0, 0, 1, 1};
        RECT rcCorners = {0};
        DrawImage(hDC, hBitmap, rc, rc, rcBmpPart, rcCorners, true, 255);
        ::DeleteObject(hBitmap);
    }
#endif //
}

void CRenderEngine::DrawGradient(HDC hDC, const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps)
{
    typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
    static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");
    if( lpAlphaBlend == NULL ) lpAlphaBlend = AlphaBitBlt;
    typedef BOOL (WINAPI *PGradientFill)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
    static PGradientFill lpGradientFill = (PGradientFill) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "GradientFill");

    BYTE bAlpha = (BYTE)(((dwFirst >> 24) + (dwSecond >> 24)) >> 1);
    if( bAlpha == 0 ) return;
    int cx = rc.right - rc.left;
    int cy = rc.bottom - rc.top;
    RECT rcPaint = rc;
    HDC hPaintDC = hDC;
    HBITMAP hPaintBitmap = NULL;
    HBITMAP hOldPaintBitmap = NULL;
    if( bAlpha < 255 ) {
        rcPaint.left = rcPaint.top = 0;
        rcPaint.right = cx;
        rcPaint.bottom = cy;
        hPaintDC = ::CreateCompatibleDC(hDC);
        hPaintBitmap = ::CreateCompatibleBitmap(hDC, cx, cy);
        ASSERT(hPaintDC);
        ASSERT(hPaintBitmap);
        hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
    }
    if( lpGradientFill != NULL ) 
    {
        TRIVERTEX triv[2] = 
        {
            { rcPaint.left, rcPaint.top, GetBValue(dwFirst) << 8, GetGValue(dwFirst) << 8, GetRValue(dwFirst) << 8, 0xFF00 },
            { rcPaint.right, rcPaint.bottom, GetBValue(dwSecond) << 8, GetGValue(dwSecond) << 8, GetRValue(dwSecond) << 8, 0xFF00 }
        };
        GRADIENT_RECT grc = { 0, 1 };
        lpGradientFill(hPaintDC, triv, 2, &grc, 1, bVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
    }
    else 
    {
        // Determine how many shades
        int nShift = 1;
        if( nSteps >= 64 ) nShift = 6;
        else if( nSteps >= 32 ) nShift = 5;
        else if( nSteps >= 16 ) nShift = 4;
        else if( nSteps >= 8 ) nShift = 3;
        else if( nSteps >= 4 ) nShift = 2;
        int nLines = 1 << nShift;
        for( int i = 0; i < nLines; i++ ) {
            // Do a little alpha blending
            BYTE bR = (BYTE) ((GetBValue(dwSecond) * (nLines - i) + GetBValue(dwFirst) * i) >> nShift);
            BYTE bG = (BYTE) ((GetGValue(dwSecond) * (nLines - i) + GetGValue(dwFirst) * i) >> nShift);
            BYTE bB = (BYTE) ((GetRValue(dwSecond) * (nLines - i) + GetRValue(dwFirst) * i) >> nShift);
            // ... then paint with the resulting color
            HBRUSH hBrush = ::CreateSolidBrush(RGB(bR,bG,bB));
            RECT r2 = rcPaint;
            if( bVertical ) {
                r2.bottom = rc.bottom - ((i * (rc.bottom - rc.top)) >> nShift);
                r2.top = rc.bottom - (((i + 1) * (rc.bottom - rc.top)) >> nShift);
                if( (r2.bottom - r2.top) > 0 ) ::FillRect(hDC, &r2, hBrush);
            }
            else {
                r2.left = rc.right - (((i + 1) * (rc.right - rc.left)) >> nShift);
                r2.right = rc.right - ((i * (rc.right - rc.left)) >> nShift);
                if( (r2.right - r2.left) > 0 ) ::FillRect(hPaintDC, &r2, hBrush);
            }
            ::DeleteObject(hBrush);
        }
    }
    if( bAlpha < 255 ) {
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, bAlpha, AC_SRC_ALPHA };
        lpAlphaBlend(hDC, rc.left, rc.top, cx, cy, hPaintDC, 0, 0, cx, cy, bf);
        ::SelectObject(hPaintDC, hOldPaintBitmap);
        ::DeleteObject(hPaintBitmap);
        ::DeleteDC(hPaintDC);
    }
}

void CRenderEngine::DrawLine(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor)
{
    ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
#ifdef USE_GDIPlUS_DRAW
    Gdiplus::Graphics ghDCDraw(hDC);
    Gdiplus::Pen ghPen(Gdiplus::Color((dwPenColor) >> 24, GetRValue(dwPenColor), GetGValue(dwPenColor), GetBValue(dwPenColor)), (REAL)nSize);

    ghDCDraw.DrawLine(&ghPen, rc.left, rc.top, rc.right, rc.bottom);
    ghDCDraw.ReleaseHDC(hDC);
#else
    POINT ptTemp = { 0 };
    HPEN hPen = ::CreatePen(PS_SOLID, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
    HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
    ::MoveToEx(hDC, rc.left, rc.top, &ptTemp);
    ::LineTo(hDC, rc.right, rc.bottom);
    ::SelectObject(hDC, hOldPen);
    ::DeleteObject(hPen);
#endif
}

void CRenderEngine::DrawRect(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor)
{
    ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
#ifdef USE_GDIPlUS_DRAW
    Gdiplus::Graphics ghDCDraw(hDC);
    Gdiplus::Pen ghPen(Gdiplus::Color(dwPenColor >> 24, GetRValue(dwPenColor), GetGValue(dwPenColor), GetBValue(dwPenColor)), (REAL)nSize);
    Gdiplus::Rect ghDestRect(rc.left, rc.top, rc.right - rc.left - 1, rc.bottom - rc.top - 1);

    ghDCDraw.DrawRectangle(&ghPen, ghDestRect);
    ghDCDraw.ReleaseHDC(hDC);
#else
    HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
    HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
    ::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
    ::Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
    ::SelectObject(hDC, hOldPen);
    ::DeleteObject(hPen);
#endif
}

#ifdef USE_GDIPlUS_DRAW
class CGraphicsRoundRectPath : public Gdiplus::GraphicsPath
{
public:
    CGraphicsRoundRectPath() : Gdiplus::GraphicsPath()
    {
    }
    CGraphicsRoundRectPath(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY) : Gdiplus::GraphicsPath()
    {
        AddRoundRect(x, y, width, height, cornerX, cornerY);
    }

public:
    void AddRoundRect(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY)
    {
        INT elWid = 2 * cornerX;
        INT elHei = 2 * cornerY;
        
        AddArc(x, y, elWid, elHei, 180, 90); // 左上角圆弧
        AddLine(x + cornerX, y, x + width - cornerX, y); // 上边
        AddArc(x + width - elWid, y, elWid, elHei, 270, 90); // 右上角圆弧
        AddLine(x + width, y + cornerY, x + width, y + height - cornerY);// 右边
        AddArc(x + width - elWid, y + height - elHei, elWid, elHei, 0, 90); // 右下角圆弧
        AddLine(x + width - cornerX, y + height, x + cornerX, y + height); // 下边
        AddArc(x, y + height - elHei, elWid, elHei, 90, 90);
        AddLine(x, y + cornerY, x, y + height - cornerY);
    }
};
#endif

void CRenderEngine::DrawRoundRect(HDC hDC, const RECT& rc, int nSize, int width, int height, DWORD dwPenColor)
{
    ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
#ifdef USE_GDIPlUS_DRAW
    Gdiplus::Graphics ghDCDraw(hDC);
    Gdiplus::Pen ghPen(Gdiplus::Color(dwPenColor >> 24, GetRValue(dwPenColor), GetGValue(dwPenColor), GetBValue(dwPenColor)), (REAL)nSize);
    CGraphicsRoundRectPath ghRoundRectPath(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, width, height);

    ghDCDraw.DrawPath(&ghPen, &ghRoundRectPath);
    ghDCDraw.ReleaseHDC(hDC);
#else
    HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
    HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
    ::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
    ::RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, width, height);
    ::SelectObject(hDC, hOldPen);
    ::DeleteObject(hPen);
#endif
}

void CRenderEngine::DrawText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, int iFont, UINT uStyle, UINT uAngle)
{
	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	if( pstrText == NULL || pManager == NULL ) return;
    ::SetBkMode(hDC, TRANSPARENT);

    HFONT hTextFont = pManager->GetFont(iFont);

#ifdef USE_GDIPlUS_DRAW
    Gdiplus::Graphics ghDCDraw(hDC);
    Gdiplus::Font ghFont(hDC, hTextFont);
    Gdiplus::RectF ghDestRect((REAL)rc.left, (REAL)rc.top, (REAL)(rc.right - rc.left), (REAL)(rc.bottom - rc.top));
    Gdiplus::StringFormat ghStringFmt;
    //这里经过测试，获取到的R和B颠倒了，这里可能是GetBValue、GetRValue的提取方式与dwTextColor写入方式不同，  
    //即如果用RGB写入，用COLOR的顺序方式提取是会出现颠倒现象的
    Gdiplus::SolidBrush ghTextBrush(Color((dwTextColor) >> 24, GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));

    ghDCDraw.SetTextRenderingHint(TextRenderingHintSystemDefault);
    ghStringFmt.SetAlignment(StringAlignmentNear);
    ghStringFmt.SetLineAlignment(StringAlignmentNear);
    ghStringFmt.SetFormatFlags(StringFormatFlags::StringFormatFlagsDirectionVertical);
    if (uStyle & DT_CENTER)
        ghStringFmt.SetAlignment(StringAlignmentCenter);
    if (uStyle & DT_RIGHT)
        ghStringFmt.SetAlignment(StringAlignmentFar);
    if (uStyle & DT_VCENTER)
        ghStringFmt.SetLineAlignment(StringAlignmentCenter);
    if (uStyle & DT_BOTTOM)
        ghStringFmt.SetLineAlignment(StringAlignmentFar);
    if (uStyle & DT_END_ELLIPSIS)
        ghStringFmt.SetTrimming(StringTrimmingEllipsisCharacter);
    if (uStyle & DT_SINGLELINE)
        ghStringFmt.SetFormatFlags(StringFormatFlagsNoWrap);

    ghDCDraw.DrawString(pstrText, -1, &ghFont, ghDestRect, &ghStringFmt, &ghTextBrush);
    ghDCDraw.ReleaseHDC(hDC);
#else
    if (pManager->GetBackgroundTransparent())
    {

        BYTE alphaValue = (dwTextColor) >> 24;

        Gdiplus::Graphics graphics(hDC);
        Gdiplus::Font myFont(hDC, hTextFont);
        Gdiplus::RectF layoutRect(
            static_cast<REAL>(rc.left),
            static_cast<REAL>(rc.top),
            static_cast<REAL>(rc.right - rc.left),
            static_cast<REAL>(rc.bottom - rc.top));
        StringFormat format;

        int i = uStyle % 16;

        format.SetAlignment(StringAlignmentNear);
        format.SetLineAlignment(StringAlignmentNear);
        format.SetFormatFlags(StringFormatFlags::StringFormatFlagsDirectionVertical);

        if (i & DT_CENTER) format.SetAlignment(StringAlignmentCenter);
        if (i & DT_RIGHT) format.SetAlignment(StringAlignmentFar);
        if (i & DT_VCENTER) format.SetLineAlignment(StringAlignmentCenter);
        if (i & DT_BOTTOM) format.SetLineAlignment(StringAlignmentFar);

        if (uStyle & DT_END_ELLIPSIS)format.SetTrimming(StringTrimmingEllipsisCharacter);
        if (uStyle & DT_SINGLELINE) format.SetFormatFlags(StringFormatFlagsNoWrap);

        Color textcolor(dwTextColor);

        //这里经过测试，获取到的R和B颠倒了，这里可能是GetBValue、GetRValue的提取方式与dwTextColor写入方式不同，  
        //即如果用RGB写入，用COLOR的顺序方式提取是会出现颠倒现象的  
        BYTE R = GetBValue(dwTextColor);
        BYTE B = GetRValue(dwTextColor);
        BYTE G = GetGValue(dwTextColor);
        SolidBrush textBrush(Color(254, R, G, B));


#ifdef UNICODE
        graphics.DrawString(pstrText,
            -1,
            &myFont,
            layoutRect,
            &format,
            &textBrush);
#else
        int    textlen;
        wchar_t * result;
        textlen = MultiByteToWideChar(CP_ACP, 0, pstrText, -1, NULL, 0);
        result = (wchar_t *)malloc((textlen + 1)*sizeof(wchar_t));
        memset(result, 0, (textlen + 1)*sizeof(wchar_t));
        MultiByteToWideChar(CP_ACP, 0, pstrText, -1, (LPWSTR)result, textlen);


        graphics.DrawString(result,
            -1,
            &myFont,
            layoutRect,
            &format,
            &textBrush);
        free(result);
#endif // DEBUG
    }
    else
    {

        ::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
        HFONT hOldFont = (HFONT)::SelectObject(hDC, hTextFont);
        if (uAngle == 270)
        {
            // 纵向文本输出
            RECT stVOutRect = { 0 };
            SIZE stTextSize = { 0 };

            GetTextExtentPoint(hDC, _T("文"), 1, &stTextSize);
            uStyle &= ~DT_SINGLELINE;
            stVOutRect.left = rc.left;
            stVOutRect.top = rc.top;
            stVOutRect.right = stVOutRect.left + stTextSize.cx;
            stTextSize.cy *= _tcslen(pstrText);
            stVOutRect.bottom = stVOutRect.top + stTextSize.cy;
            if (uStyle & DT_RIGHT)
            {
                // 右对齐
                uStyle &= ~DT_RIGHT;
                stVOutRect.left = rc.right - stTextSize.cx;
                stVOutRect.right = stVOutRect.left + stTextSize.cx;
            }
            if (uStyle & DT_CENTER)
            {
                // 水平居中对齐
                uStyle &= ~DT_CENTER;
                stVOutRect.left = rc.left + ((rc.right - rc.left) / 2) - (stTextSize.cx / 2);
                stVOutRect.right = stVOutRect.left + stTextSize.cx;
            }
            if (uStyle & DT_VCENTER)
            {
                // 纵向居中对齐
                uStyle &= ~DT_VCENTER;
                stVOutRect.top = rc.top + ((rc.bottom - rc.top) / 2) - (stTextSize.cy / 2);
                stVOutRect.bottom = stVOutRect.top + stTextSize.cy;
            }
            if (uStyle & DT_BOTTOM)
            {
                // 底部对齐
                uStyle &= ~DT_BOTTOM;
                stVOutRect.top = rc.bottom - stTextSize.cy;
                stVOutRect.bottom = stVOutRect.top + stTextSize.cy;
            }
            if (stVOutRect.left < rc.left)
                stVOutRect.left = rc.left;
            if (stVOutRect.top < rc.top)
                stVOutRect.top = rc.top;
            if (stVOutRect.right > rc.right)
                stVOutRect.right = rc.right;
            if (stVOutRect.bottom > rc.bottom)
                stVOutRect.bottom = rc.bottom;
            ::DrawText(hDC, pstrText, -1, &stVOutRect, uStyle | DT_NOPREFIX | DT_WORDBREAK | DT_NOCLIP);
        }
        else
        {
            ::DrawText(hDC, pstrText, -1, &rc, uStyle | DT_NOPREFIX);
        }
        ::SelectObject(hDC, hOldFont);
    }
#endif
}


void CRenderEngine::DrawHtmlText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, RECT* prcLinks, CStdString* sLinks, int& nLinkRects, UINT uStyle)
{
    // 考虑到在xml编辑器中使用<>符号不方便，可以使用{}符号代替
    // 支持标签嵌套（如<l><b>text</b></l>），但是交叉嵌套是应该避免的（如<l><b>text</l></b>）
    // The string formatter supports a kind of "mini-html" that consists of various short tags:
    //
    //   Bold:             <b>text</b>
    //   Color:            <c #xxxxxx>text</c>  where x = RGB in hex
    //   Font:             <f x>text</f>        where x = font id
    //   Italic:           <i>text</i>
    //   Image:            <i x y z>            where x = image name and y = imagelist num and z(optional) = imagelist id
    //   Link:             <a x>text</a>        where x(optional) = link content, normal like app:notepad or http:www.xxx.com
    //   NewLine           <n>                  
    //   Paragraph:        <p x>text</p>        where x = extra pixels indent in p
    //   Raw Text:         <r>text</r>
    //   Selected:         <s>text</s>
    //   Underline:        <u>text</u>
    //   X Indent:         <x i>                where i = hor indent in pixels
    //   Y Indent:         <y i>                where i = ver indent in pixels 

    ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
    if( pstrText == NULL || pManager == NULL ) return;
    if( ::IsRectEmpty(&rc) ) return;

    bool bDraw = (uStyle & DT_CALCRECT) == 0;

    CStdPtrArray aFontArray(10);
    CStdPtrArray aColorArray(10);
    CStdPtrArray aPIndentArray(10);

    RECT rcClip = { 0 };
    ::GetClipBox(hDC, &rcClip);
    HRGN hOldRgn = ::CreateRectRgnIndirect(&rcClip);
    HRGN hRgn = ::CreateRectRgnIndirect(&rc);
    if( bDraw ) ::ExtSelectClipRgn(hDC, hRgn, RGN_AND);

    TEXTMETRIC* pTm = &pManager->GetDefaultFontInfo()->tm;
    HFONT hOldFont = (HFONT) ::SelectObject(hDC, pManager->GetDefaultFontInfo()->hFont);
    ::SetBkMode(hDC, TRANSPARENT);
    ::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
    DWORD dwBkColor = pManager->GetDefaultSelectedBkColor();
    ::SetBkColor(hDC, RGB(GetBValue(dwBkColor), GetGValue(dwBkColor), GetRValue(dwBkColor)));

    // If the drawstyle include a alignment, we'll need to first determine the text-size so
    // we can draw it at the correct position...
    if( ( (uStyle & DT_BOTTOM ) != 0 || (uStyle &  DT_VCENTER) != 0 ) && (uStyle & DT_CALCRECT) == 0 ) {
        RECT rcText = { 0, 0, 9999, 100 };
        int nLinks = 0;
        DrawHtmlText(hDC, pManager, rcText, pstrText, dwTextColor, NULL, NULL, nLinks, uStyle | DT_CALCRECT);
		if( (uStyle & DT_BOTTOM) != 0 ) {
			rc.top = rc.bottom - (rcText.bottom - rcText.top);
		}
		if( (uStyle & DT_VCENTER) != 0 ) {
			rc.top = rc.top + ((rc.bottom - rc.top) / 2) - ((rcText.bottom - rcText.top) / 2);
			rc.bottom = rc.top + (rcText.bottom - rcText.top);
		}
    }

    bool bHoverLink = false;
    CStdString sHoverLink;
    POINT ptMouse = pManager->GetMousePos();
    for( int i = 0; !bHoverLink && i < nLinkRects; i++ ) {
        if( ::PtInRect(prcLinks + i, ptMouse) ) {
            sHoverLink = *(CStdString*)(sLinks + i);
            bHoverLink = true;
        }
    }

    POINT pt = { rc.left, rc.top };
    int iLinkIndex = 0;
    int cyLine = pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1);
    int cyMinHeight = 0;
    int cxMaxWidth = 0;
    POINT ptLinkStart = { 0 };
    bool bLineEnd = false;
    bool bInRaw = false;
    bool bInLink = false;
    bool bInSelected = false;
    int iLineLinkIndex = 0;

    // 排版习惯是图文底部对齐，所以每行绘制都要分两步，先计算高度，再绘制
    CStdPtrArray aLineFontArray;
    CStdPtrArray aLineColorArray;
    CStdPtrArray aLinePIndentArray;
    LPCTSTR pstrLineBegin = pstrText;
    bool bLineInRaw = false;
    bool bLineInLink = false;
    bool bLineInSelected = false;
    int cyLineHeight = 0;
    bool bLineDraw = false; // 行的第二阶段：绘制
    while( *pstrText != _T('\0') ) {
        if( pt.x >= rc.right || *pstrText == _T('\n') || bLineEnd ) {
            if( *pstrText == _T('\n') ) pstrText++;
            if( bLineEnd ) bLineEnd = false;
            if( !bLineDraw ) {
                if( bInLink && iLinkIndex < nLinkRects ) {
                    ::SetRect(&prcLinks[iLinkIndex++], ptLinkStart.x, ptLinkStart.y, MIN(pt.x, rc.right), pt.y + cyLine);
                    CStdString *pStr1 = (CStdString*)(sLinks + iLinkIndex - 1);
                    CStdString *pStr2 = (CStdString*)(sLinks + iLinkIndex);
                    *pStr2 = *pStr1;
                }
                for( int i = iLineLinkIndex; i < iLinkIndex; i++ ) {
                    prcLinks[i].bottom = pt.y + cyLine;
                }
                if( bDraw ) {
                    bInLink = bLineInLink;
                    iLinkIndex = iLineLinkIndex;
                }
            }
            else {
                if( bInLink && iLinkIndex < nLinkRects ) iLinkIndex++;
                bLineInLink = bInLink;
                iLineLinkIndex = iLinkIndex;
            }
            if( (uStyle & DT_SINGLELINE) != 0 && (!bDraw || bLineDraw) ) break;
            if( bDraw ) bLineDraw = !bLineDraw; // !
            pt.x = rc.left;
            if( !bLineDraw ) pt.y += cyLine;
            if( pt.y > rc.bottom && bDraw ) break;
            ptLinkStart = pt;
            cyLine = pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1);
            if( pt.x >= rc.right ) break;
        }
        else if( !bInRaw && ( *pstrText == _T('<') || *pstrText == _T('{') )
            && ( pstrText[1] >= _T('a') && pstrText[1] <= _T('z') )
            && ( pstrText[2] == _T(' ') || pstrText[2] == _T('>') || pstrText[2] == _T('}') ) ) {
                pstrText++;
                LPCTSTR pstrNextStart = NULL;
                switch( *pstrText ) {
            case _T('a'):  // Link
                {
                    pstrText++;
                    while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                    if( iLinkIndex < nLinkRects && !bLineDraw ) {
                        CStdString *pStr = (CStdString*)(sLinks + iLinkIndex);
                        pStr->Empty();
                        while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) {
                            LPCTSTR pstrTemp = ::CharNext(pstrText);
                            while( pstrText < pstrTemp) {
                                *pStr += *pstrText++;
                            }
                        }
                    }

                    DWORD clrColor = pManager->GetDefaultLinkFontColor();
                    if( bHoverLink && iLinkIndex < nLinkRects ) {
                        CStdString *pStr = (CStdString*)(sLinks + iLinkIndex);
                        if( sHoverLink == *pStr ) clrColor = pManager->GetDefaultLinkHoverFontColor();
                    }
                    //else if( prcLinks == NULL ) {
                    //    if( ::PtInRect(&rc, ptMouse) )
                    //        clrColor = pManager->GetDefaultLinkHoverFontColor();
                    //}
                    aColorArray.Add((LPVOID)clrColor);
                    ::SetTextColor(hDC,  RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                    TFontInfo* pFontInfo = pManager->GetDefaultFontInfo();
                    if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
                    if( pFontInfo->bUnderline == false ) {
                        HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
                        if( hFont == NULL ) hFont = pManager->AddFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
                        pFontInfo = pManager->GetFontInfo(hFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        ::SelectObject(hDC, pFontInfo->hFont);
                        cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                    }
                    ptLinkStart = pt;
                    bInLink = true;
                }
                break;
            case _T('b'):  // Bold
                {
                    pstrText++;
                    TFontInfo* pFontInfo = pManager->GetDefaultFontInfo();
                    if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
                    if( pFontInfo->bBold == false ) {
                        HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, true, pFontInfo->bUnderline, pFontInfo->bItalic);
                        if( hFont == NULL ) hFont = pManager->AddFont(pFontInfo->sFontName, pFontInfo->iSize, true, pFontInfo->bUnderline, pFontInfo->bItalic);
                        pFontInfo = pManager->GetFontInfo(hFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        ::SelectObject(hDC, pFontInfo->hFont);
                        cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                    }
                }
                break;
            case _T('c'):  // Color
                {
                    pstrText++;
                    while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                    if( *pstrText == _T('#')) pstrText++;
                    DWORD clrColor = _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 16);
                    aColorArray.Add((LPVOID)clrColor);
                    ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                }
                break;
            case _T('f'):  // Font
                {
                    pstrText++;
                    while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                    LPCTSTR pstrTemp = pstrText;
                    int iFont = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
                    //if( isdigit(*pstrText) ) { // debug版本会引起异常
                    if( pstrTemp != pstrText ) {
                        TFontInfo* pFontInfo = pManager->GetFontInfo(iFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        ::SelectObject(hDC, pFontInfo->hFont);
                    }
                    else {
                        CStdString sFontName;
                        int iFontSize = 10;
                        CStdString sFontAttr;
                        bool bBold = false;
                        bool bUnderline = false;
                        bool bItalic = false;
                        while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') && *pstrText != _T(' ') ) {
                            pstrTemp = ::CharNext(pstrText);
                            while( pstrText < pstrTemp) {
                                sFontName += *pstrText++;
                            }
                        }
                        while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                        if( isdigit(*pstrText) ) {
                            iFontSize = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
                        }
                        while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                        while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) {
                            pstrTemp = ::CharNext(pstrText);
                            while( pstrText < pstrTemp) {
                                sFontAttr += *pstrText++;
                            }
                        }
                        sFontAttr.MakeLower();
                        if( sFontAttr.Find(_T("bold")) >= 0 ) bBold = true;
                        if( sFontAttr.Find(_T("underline")) >= 0 ) bUnderline = true;
                        if( sFontAttr.Find(_T("italic")) >= 0 ) bItalic = true;
                        HFONT hFont = pManager->GetFont(sFontName, iFontSize, bBold, bUnderline, bItalic);
                        if( hFont == NULL ) hFont = pManager->AddFont(sFontName, iFontSize, bBold, bUnderline, bItalic);
                        TFontInfo* pFontInfo = pManager->GetFontInfo(hFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        ::SelectObject(hDC, pFontInfo->hFont);
                    }
                    cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                }
                break;
            case _T('i'):  // Italic or Image
                {    
                    pstrNextStart = pstrText - 1;
                    pstrText++;
					CStdString sImageString = pstrText;
                    int iWidth = 0;
                    int iHeight = 0;
                    while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                    const TImageInfo* pImageInfo = NULL;
                    CStdString sName;
                    while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') && *pstrText != _T(' ') ) {
                        LPCTSTR pstrTemp = ::CharNext(pstrText);
                        while( pstrText < pstrTemp) {
                            sName += *pstrText++;
                        }
                    }
                    if( sName.IsEmpty() ) { // Italic
                        pstrNextStart = NULL;
                        TFontInfo* pFontInfo = pManager->GetDefaultFontInfo();
                        if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
                        if( pFontInfo->bItalic == false ) {
                            HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, pFontInfo->bUnderline, true);
                            if( hFont == NULL ) hFont = pManager->AddFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, pFontInfo->bUnderline, true);
                            pFontInfo = pManager->GetFontInfo(hFont);
                            aFontArray.Add(pFontInfo);
                            pTm = &pFontInfo->tm;
                            ::SelectObject(hDC, pFontInfo->hFont);
                            cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                        }
                    }
                    else {
                        while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                        int iImageListNum = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
						if( iImageListNum <= 0 ) iImageListNum = 1;
						while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
						int iImageListIndex = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
						if( iImageListIndex < 0 || iImageListIndex >= iImageListNum ) iImageListIndex = 0;

						if( _tcsstr(sImageString.GetData(), _T("file=\'")) != NULL || _tcsstr(sImageString.GetData(), _T("res=\'")) != NULL ) {
							CStdString sImageResType;
							CStdString sImageName;
							LPCTSTR pStrImage = sImageString.GetData();
							CStdString sItem;
							CStdString sValue;
							while( *pStrImage != _T('\0') ) {
								sItem.Empty();
								sValue.Empty();
								while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
								while( *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ') ) {
									LPTSTR pstrTemp = ::CharNext(pStrImage);
									while( pStrImage < pstrTemp) {
										sItem += *pStrImage++;
									}
								}
								while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
								if( *pStrImage++ != _T('=') ) break;
								while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
								if( *pStrImage++ != _T('\'') ) break;
								while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) {
									LPTSTR pstrTemp = ::CharNext(pStrImage);
									while( pStrImage < pstrTemp) {
										sValue += *pStrImage++;
									}
								}
								if( *pStrImage++ != _T('\'') ) break;
								if( !sValue.IsEmpty() ) {
									if( sItem == _T("file") || sItem == _T("res") ) {
										sImageName = sValue;
									}
									else if( sItem == _T("restype") ) {
										sImageResType = sValue;
									}
								}
								if( *pStrImage++ != _T(' ') ) break;
							}

							pImageInfo = pManager->GetImageEx((LPCTSTR)sImageName, sImageResType);
						}
						else
							pImageInfo = pManager->GetImageEx((LPCTSTR)sName);

						if( pImageInfo ) {
							iWidth = pImageInfo->nX;
							iHeight = pImageInfo->nY;
							if( iImageListNum > 1 ) iWidth /= iImageListNum;

                            if( pt.x + iWidth > rc.right && pt.x > rc.left && (uStyle & DT_SINGLELINE) == 0 ) {
                                bLineEnd = true;
                            }
                            else {
                                pstrNextStart = NULL;
                                if( bDraw && bLineDraw ) {
                                    CDuiRect rcImage(pt.x, pt.y + cyLineHeight - iHeight, pt.x + iWidth, pt.y + cyLineHeight);
                                    if( iHeight < cyLineHeight ) { 
                                        rcImage.bottom -= (cyLineHeight - iHeight) / 2;
                                        rcImage.top = rcImage.bottom -  iHeight;
                                    }
                                    CDuiRect rcBmpPart(0, 0, iWidth, iHeight);
                                    rcBmpPart.left = iWidth * iImageListIndex;
                                    rcBmpPart.right = iWidth * (iImageListIndex + 1);
                                    CDuiRect rcCorner(0, 0, 0, 0);
                                    DrawImage(hDC, pImageInfo->hBitmap, rcImage, rcImage, rcBmpPart, rcCorner, \
                                        pImageInfo->alphaChannel, 255);
                                }

                                cyLine = MAX(iHeight, cyLine);
                                pt.x += iWidth;
                                cyMinHeight = pt.y + iHeight;
                                cxMaxWidth = MAX(cxMaxWidth, pt.x);
                            }
                        }
                        else pstrNextStart = NULL;
                    }
                }
                break;
            case _T('n'):  // Newline
                {
                    pstrText++;
                    if( (uStyle & DT_SINGLELINE) != 0 ) break;
                    bLineEnd = true;
                }
                break;
            case _T('p'):  // Paragraph
                {
                    pstrText++;
                    if( pt.x > rc.left ) bLineEnd = true;
                    while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                    int cyLineExtra = (int)_tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
                    aPIndentArray.Add((LPVOID)cyLineExtra);
                    cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + cyLineExtra);
                }
                break;
            case _T('r'):  // Raw Text
                {
                    pstrText++;
                    bInRaw = true;
                }
                break;
            case _T('s'):  // Selected text background color
                {
                    pstrText++;
                    bInSelected = !bInSelected;
                    if( bDraw && bLineDraw ) {
                        if( bInSelected ) ::SetBkMode(hDC, OPAQUE);
                        else ::SetBkMode(hDC, TRANSPARENT);
                    }
                }
                break;
            case _T('u'):  // Underline text
                {
                    pstrText++;
                    TFontInfo* pFontInfo = pManager->GetDefaultFontInfo();
                    if( aFontArray.GetSize() > 0 ) pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
                    if( pFontInfo->bUnderline == false ) {
                        HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
                        if( hFont == NULL ) hFont = pManager->AddFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true, pFontInfo->bItalic);
                        pFontInfo = pManager->GetFontInfo(hFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        ::SelectObject(hDC, pFontInfo->hFont);
                        cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                    }
                }
                break;
            case _T('x'):  // X Indent
                {
                    pstrText++;
                    while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                    int iWidth = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
                    pt.x += iWidth;
                    cxMaxWidth = MAX(cxMaxWidth, pt.x);
                }
                break;
            case _T('y'):  // Y Indent
                {
                    pstrText++;
                    while( *pstrText > _T('\0') && *pstrText <= _T(' ') ) pstrText = ::CharNext(pstrText);
                    cyLine = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
                }
                break;
                }
                if( pstrNextStart != NULL ) pstrText = pstrNextStart;
                else {
                    while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) pstrText = ::CharNext(pstrText);
                    pstrText = ::CharNext(pstrText);
                }
        }
        else if( !bInRaw && ( *pstrText == _T('<') || *pstrText == _T('{') ) && pstrText[1] == _T('/') )
        {
            pstrText++;
            pstrText++;
            switch( *pstrText )
            {
            case _T('c'):
                {
                    pstrText++;
                    aColorArray.Remove(aColorArray.GetSize() - 1);
                    DWORD clrColor = dwTextColor;
                    if( aColorArray.GetSize() > 0 ) clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
                    ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                }
                break;
            case _T('p'):
                pstrText++;
                if( pt.x > rc.left ) bLineEnd = true;
                aPIndentArray.Remove(aPIndentArray.GetSize() - 1);
                cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                break;
            case _T('s'):
                {
                    pstrText++;
                    bInSelected = !bInSelected;
                    if( bDraw && bLineDraw ) {
                        if( bInSelected ) ::SetBkMode(hDC, OPAQUE);
                        else ::SetBkMode(hDC, TRANSPARENT);
                    }
                }
                break;
            case _T('a'):
                {
                    if( iLinkIndex < nLinkRects ) {
                        if( !bLineDraw ) ::SetRect(&prcLinks[iLinkIndex], ptLinkStart.x, ptLinkStart.y, MIN(pt.x, rc.right), pt.y + pTm->tmHeight + pTm->tmExternalLeading);
                        iLinkIndex++;
                    }
                    aColorArray.Remove(aColorArray.GetSize() - 1);
                    DWORD clrColor = dwTextColor;
                    if( aColorArray.GetSize() > 0 ) clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
                    ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                    bInLink = false;
                }
            case _T('b'):
            case _T('f'):
            case _T('i'):
            case _T('u'):
                {
                    pstrText++;
                    aFontArray.Remove(aFontArray.GetSize() - 1);
                    TFontInfo* pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
                    if( pFontInfo == NULL ) pFontInfo = pManager->GetDefaultFontInfo();
                    if( pTm->tmItalic && pFontInfo->bItalic == false ) {
                        ABC abc;
                        ::GetCharABCWidths(hDC, _T(' '), _T(' '), &abc);
                        pt.x += abc.abcC / 2; // 简单修正一下斜体混排的问题, 正确做法应该是http://support.microsoft.com/kb/244798/en-us
                    }
                    pTm = &pFontInfo->tm;
                    ::SelectObject(hDC, pFontInfo->hFont);
                    cyLine = MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                }
                break;
            }
            while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') ) pstrText = ::CharNext(pstrText);
            pstrText = ::CharNext(pstrText);
        }
        else if( !bInRaw &&  *pstrText == _T('<') && pstrText[2] == _T('>') && (pstrText[1] == _T('{')  || pstrText[1] == _T('}')) )
        {
            SIZE szSpace = { 0 };
            ::GetTextExtentPoint32(hDC, &pstrText[1], 1, &szSpace);
            if( bDraw && bLineDraw ) ::TextOut(hDC, pt.x, pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, &pstrText[1], 1);
            pt.x += szSpace.cx;
            cxMaxWidth = MAX(cxMaxWidth, pt.x);
            pstrText++;pstrText++;pstrText++;
        }
        else if( !bInRaw &&  *pstrText == _T('{') && pstrText[2] == _T('}') && (pstrText[1] == _T('<')  || pstrText[1] == _T('>')) )
        {
            SIZE szSpace = { 0 };
            ::GetTextExtentPoint32(hDC, &pstrText[1], 1, &szSpace);
            if( bDraw && bLineDraw ) ::TextOut(hDC, pt.x,  pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, &pstrText[1], 1);
            pt.x += szSpace.cx;
            cxMaxWidth = MAX(cxMaxWidth, pt.x);
            pstrText++;pstrText++;pstrText++;
        }
        else if( !bInRaw &&  *pstrText == _T(' ') )
        {
            SIZE szSpace = { 0 };
            ::GetTextExtentPoint32(hDC, _T(" "), 1, &szSpace);
            // Still need to paint the space because the font might have
            // underline formatting.
            if( bDraw && bLineDraw ) ::TextOut(hDC, pt.x,  pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, _T(" "), 1);
            pt.x += szSpace.cx;
            cxMaxWidth = MAX(cxMaxWidth, pt.x);
            pstrText++;
        }
        else
        {
            POINT ptPos = pt;
            int cchChars = 0;
            int cchSize = 0;
            int cchLastGoodWord = 0;
            int cchLastGoodSize = 0;
            LPCTSTR p = pstrText;
            LPCTSTR pstrNext;
            SIZE szText = { 0 };
            if( !bInRaw && *p == _T('<') || *p == _T('{') ) p++, cchChars++, cchSize++;
            while( *p != _T('\0') && *p != _T('\n') ) {
                // This part makes sure that we're word-wrapping if needed or providing support
                // for DT_END_ELLIPSIS. Unfortunately the GetTextExtentPoint32() call is pretty
                // slow when repeated so often.
                // TODO: Rewrite and use GetTextExtentExPoint() instead!
                if( bInRaw ) {
                    if( ( *p == _T('<') || *p == _T('{') ) && p[1] == _T('/') 
                        && p[2] == _T('r') && ( p[3] == _T('>') || p[3] == _T('}') ) ) {
                            p += 4;
                            bInRaw = false;
                            break;
                    }
                }
                else {
                    if( *p == _T('<') || *p == _T('{') ) break;
                }
                pstrNext = ::CharNext(p);
                cchChars++;
                cchSize += (int)(pstrNext - p);
                szText.cx = cchChars * pTm->tmMaxCharWidth;
                if( pt.x + szText.cx >= rc.right ) {
                    ::GetTextExtentPoint32(hDC, pstrText, cchSize, &szText);
                }
                if( pt.x + szText.cx > rc.right ) {
                    if( pt.x + szText.cx > rc.right && pt.x != rc.left) {
                        cchChars--;
                        cchSize -= (int)(pstrNext - p);
                    }
                    if( (uStyle & DT_WORDBREAK) != 0 && cchLastGoodWord > 0 ) {
                        cchChars = cchLastGoodWord;
                        cchSize = cchLastGoodSize;                 
                    }
                    if( (uStyle & DT_END_ELLIPSIS) != 0 && cchChars > 0 ) {
                        cchChars -= 1;
                        LPCTSTR pstrPrev = ::CharPrev(pstrText, p);
                        if( cchChars > 0 ) {
                            cchChars -= 1;
                            pstrPrev = ::CharPrev(pstrText, pstrPrev);
                            cchSize -= (int)(p - pstrPrev);
                        }
                        else 
                            cchSize -= (int)(p - pstrPrev);
                        pt.x = rc.right;
                    }
                    bLineEnd = true;
                    cxMaxWidth = MAX(cxMaxWidth, pt.x);
                    break;
                }
                if (!( ( p[0] >= _T('a') && p[0] <= _T('z') ) || ( p[0] >= _T('A') && p[0] <= _T('Z') ) )) {
                    cchLastGoodWord = cchChars;
                    cchLastGoodSize = cchSize;
                }
                if( *p == _T(' ') ) {
                    cchLastGoodWord = cchChars;
                    cchLastGoodSize = cchSize;
                }
                p = ::CharNext(p);
            }
            
            ::GetTextExtentPoint32(hDC, pstrText, cchSize, &szText);
            if( bDraw && bLineDraw ) {
				if( (uStyle & DT_CENTER) != 0 ) {
					ptPos.x += (rc.right - rc.left - szText.cx)/2;
				}
				else if( (uStyle & DT_RIGHT) != 0) {
					ptPos.x += (rc.right - rc.left - szText.cx);
				}
				::TextOut(hDC, ptPos.x, ptPos.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, pstrText, cchSize);
				if( pt.x >= rc.right && (uStyle & DT_END_ELLIPSIS) != 0 ) 
                    ::TextOut(hDC, ptPos.x + szText.cx, ptPos.y, _T("..."), 3);
            }
            pt.x += szText.cx;
            cxMaxWidth = MAX(cxMaxWidth, pt.x);
            pstrText += cchSize;
        }

        if( pt.x >= rc.right || *pstrText == _T('\n') || *pstrText == _T('\0') ) bLineEnd = true;
        if( bDraw && bLineEnd ) {
            if( !bLineDraw ) {
                aFontArray.Resize(aLineFontArray.GetSize());
                ::CopyMemory(aFontArray.GetData(), aLineFontArray.GetData(), aLineFontArray.GetSize() * sizeof(LPVOID));
                aColorArray.Resize(aLineColorArray.GetSize());
                ::CopyMemory(aColorArray.GetData(), aLineColorArray.GetData(), aLineColorArray.GetSize() * sizeof(LPVOID));
                aPIndentArray.Resize(aLinePIndentArray.GetSize());
                ::CopyMemory(aPIndentArray.GetData(), aLinePIndentArray.GetData(), aLinePIndentArray.GetSize() * sizeof(LPVOID));

                cyLineHeight = cyLine;
                pstrText = pstrLineBegin;
                bInRaw = bLineInRaw;
                bInSelected = bLineInSelected;

                DWORD clrColor = dwTextColor;
                if( aColorArray.GetSize() > 0 ) clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
                ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                TFontInfo* pFontInfo = (TFontInfo*)aFontArray.GetAt(aFontArray.GetSize() - 1);
                if( pFontInfo == NULL ) pFontInfo = pManager->GetDefaultFontInfo();
                pTm = &pFontInfo->tm;
                ::SelectObject(hDC, pFontInfo->hFont);
                if( bInSelected ) ::SetBkMode(hDC, OPAQUE);
            }
            else {
                aLineFontArray.Resize(aFontArray.GetSize());
                ::CopyMemory(aLineFontArray.GetData(), aFontArray.GetData(), aFontArray.GetSize() * sizeof(LPVOID));
                aLineColorArray.Resize(aColorArray.GetSize());
                ::CopyMemory(aLineColorArray.GetData(), aColorArray.GetData(), aColorArray.GetSize() * sizeof(LPVOID));
                aLinePIndentArray.Resize(aPIndentArray.GetSize());
                ::CopyMemory(aLinePIndentArray.GetData(), aPIndentArray.GetData(), aPIndentArray.GetSize() * sizeof(LPVOID));
                pstrLineBegin = pstrText;
                bLineInSelected = bInSelected;
                bLineInRaw = bInRaw;
            }
        }

        ASSERT(iLinkIndex<=nLinkRects);
    }

    nLinkRects = iLinkIndex;

    // Return size of text when requested
    if( (uStyle & DT_CALCRECT) != 0 ) {
        rc.bottom = MAX(cyMinHeight, pt.y + cyLine);
        rc.right = MIN(rc.right, cxMaxWidth);
    }

    if( bDraw ) ::SelectClipRgn(hDC, hOldRgn);
    ::DeleteObject(hOldRgn);
    ::DeleteObject(hRgn);

    ::SelectObject(hDC, hOldFont);
}

HBITMAP CRenderEngine::GenerateBitmap(CPaintManagerUI* pManager, CControlUI* pControl, RECT rc)
{
    int cx = rc.right - rc.left;
    int cy = rc.bottom - rc.top;

    HDC hPaintDC = ::CreateCompatibleDC(pManager->GetPaintDC());
    HBITMAP hPaintBitmap = ::CreateCompatibleBitmap(pManager->GetPaintDC(), rc.right, rc.bottom);
    ASSERT(hPaintDC);
    ASSERT(hPaintBitmap);
    HBITMAP hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
    pControl->DoPaint(hPaintDC, rc);

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cx;
    bmi.bmiHeader.biHeight = cy;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = cx * cy * sizeof(DWORD);
    LPDWORD pDest = NULL;
    HDC hCloneDC = ::CreateCompatibleDC(pManager->GetPaintDC());
    HBITMAP hBitmap = ::CreateDIBSection(pManager->GetPaintDC(), &bmi, DIB_RGB_COLORS, (LPVOID*) &pDest, NULL, 0);
    ASSERT(hCloneDC);
    ASSERT(hBitmap);
    if( hBitmap != NULL )
    {
        HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
        ::BitBlt(hCloneDC, 0, 0, cx, cy, hPaintDC, rc.left, rc.top, SRCCOPY);
        ::SelectObject(hCloneDC, hOldBitmap);
        ::DeleteDC(hCloneDC);  
        ::GdiFlush();
    }

    // Cleanup
    ::SelectObject(hPaintDC, hOldPaintBitmap);
    ::DeleteObject(hPaintBitmap);
    ::DeleteDC(hPaintDC);

    return hBitmap;
}

/************************************************************************/
/* 将24位位图转化为灰度图只是将对应的点化为RGB相同                                       */
/************************************************************************/
HBITMAP CRenderEngine::ConverColToGray(HBITMAP srcbitmap)
{
	DIBSECTION dibSrc; 
	::ZeroMemory (&dibSrc, sizeof (DIBSECTION));
	if (::GetObject (srcbitmap, sizeof (DIBSECTION), &dibSrc) != sizeof (DIBSECTION)
		|| dibSrc.dsBm.bmBits == NULL) 
		return NULL;  
	int x=dibSrc.dsBm.bmWidth;
	int y=dibSrc.dsBm.bmHeight;
	BITMAPINFO bmi;
	::ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = x;
	bmi.bmiHeader.biHeight = -y;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = dibSrc.dsBm.bmWidth * dibSrc.dsBm.bmHeight * 4;
	LPBYTE bitdib = NULL;
	HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&bitdib, NULL, 0);
	if( !hBitmap ) return NULL;
	//BYTE* matrix = (BYTE*)GlobalAlloc(GPTR, bitmapSize);
	BYTE* lpSrcDib = (LPBYTE)dibSrc.dsBm.bmBits; 
	if (lpSrcDib==NULL)
	{
		return NULL;
	}
	//每个像素点转化为灰度值，用的公式是：Gray = R*0.299 + G*0.587 + B*0.114
	for( int i = 0; i < x * y; i++ ) 
	{
		bitdib[i*4 + 3] = lpSrcDib[i*4 + 3];
        bitdib[i * 4] = (BYTE)((lpSrcDib[i * 4])*0.299 + (lpSrcDib[i * 4 + 1])*0.587 + (lpSrcDib[i * 4 + 2])*0.114); 
        bitdib[i * 4 + 1] = bitdib[i * 4];
        bitdib[i * 4 + 2] = bitdib[i * 4];
	}
	return hBitmap;
}

} // namespace DuiLib
