#include "stdafx.h"
#include "win_impl_base.h"


namespace DuiLib {
#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#ifdef USE_ZIP_SKIN
static const TCHAR* const kResourceSkinZipFileName = _T("XunNiu.zip");
#endif



	LPBYTE WindowImplBase::resource_zip_buffer_ = NULL;

	WindowImplBase::WindowImplBase()
	{}

	WindowImplBase::~WindowImplBase()
	{}

	UINT WindowImplBase::GetClassStyle() const
	{
		return CS_DBLCLKS;
	}

	CControlUI* WindowImplBase::CreateControl(LPCTSTR pstrClass)
	{
		return NULL;
	}

	LPCTSTR WindowImplBase::GetWindowClassName() const
	{
		return _T("xDuiFoundation");
	}


	void WindowImplBase::OnFinalMessage(HWND hWnd)
	{
		paint_manager_.RemovePreMessageFilter(this);
		paint_manager_.RemoveNotifier(this);
		paint_manager_.ReapObjects(paint_manager_.GetRoot());
		__super::OnFinalMessage(hWnd);
	}

	void WindowImplBase::Init()
	{}

	LRESULT WindowImplBase::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

#if defined(WIN32) && !defined(UNDER_CE)
	LRESULT WindowImplBase::OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(::IsIconic(m_hWnd)) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT WindowImplBase::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
        LONG xFrame = 0; // 左右边框大小
        LONG yFrame = 0; // 下边边框大小
        LONG lTileHeight = 0; // 标题栏高度
        LPNCCALCSIZE_PARAMS lpNcCalcSizeParam = NULL;
        LPRECT lpRect = NULL;
        RECT aRect = { 0 };
        RECT bRect = { 0 };
        RECT bcRect = { 0 };

        bHandled = TRUE;
        if (wParam == TRUE)
        {
            lpNcCalcSizeParam = (LPNCCALCSIZE_PARAMS)lParam; // 矩形是B A AC，目标是改成BC B A
            CopyRect(&aRect, &lpNcCalcSizeParam->rgrc[1]);
            CopyRect(&bRect, &lpNcCalcSizeParam->rgrc[0]);
            /*指定BC的矩形的位置*/
            bcRect.left = bRect.left + xFrame;
            bcRect.top = bRect.top + lTileHeight;
            bcRect.right = bRect.right - xFrame;
            bcRect.bottom = bRect.bottom - yFrame;
            /*各个矩形归位*/
            CopyRect(&lpNcCalcSizeParam->rgrc[0], &bcRect);
            CopyRect(&lpNcCalcSizeParam->rgrc[1], &bRect);
            CopyRect(&lpNcCalcSizeParam->rgrc[2], &aRect);
        }
        else
        {
            lpRect = (LPRECT)lParam;
            lpRect->left = lpRect->left + xFrame;
            lpRect->top = lpRect->top + lTileHeight;
            lpRect->right = lpRect->right - xFrame;
            lpRect->bottom = lpRect->bottom - yFrame;
        }
		return 0;
	}

	LRESULT WindowImplBase::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
        bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		if( !::IsZoomed(*this) ) 
		{
			RECT rcSizeBox = paint_manager_.GetSizeBox();
			if( pt.y < rcClient.top + rcSizeBox.top ) 
			{
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
				return HTTOP;
			}
			else if( pt.y > rcClient.bottom - rcSizeBox.bottom )
			{
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
				return HTBOTTOM;
			}

			if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
		}

		RECT rcCaption = paint_manager_.GetCaptionRect();
        if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(pt));
			if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
				_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 && _tcsicmp(pControl->GetClass(), _T("RichEditUI")) != 0 )
				return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT WindowImplBase::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		rcWork.Offset(-rcWork.left, -rcWork.top);

		LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
		lpMMI->ptMaxPosition.x	= rcWork.left;
		lpMMI->ptMaxPosition.y	= rcWork.top;
		lpMMI->ptMaxSize.x		= rcWork.right;
		lpMMI->ptMaxSize.y		= rcWork.bottom;

		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}
#endif


	LRESULT WindowImplBase::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = paint_manager_.GetRoundCorner();
#if defined(WIN32) && !defined(UNDER_CE)
		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
			CDuiRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}
#endif
		bHandled = FALSE;
		return 0;
	}


	LRESULT WindowImplBase::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{

		if(GetWindowStyle(m_hWnd) & WS_CHILD)//不希望是子窗口的时候还能被放大。放小的
			return 0;
		if (wParam == SC_CLOSE){
			bHandled = TRUE;
			SendMessage(WM_CLOSE);
			return 0;
		}		

		BOOL bZoomed = ::IsZoomed(*this);

		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		if( ::IsZoomed(*this) != bZoomed ) {
			if( !bZoomed ) {
				if( m_hWnd == NULL || !::IsWindow(m_hWnd) || !::IsWindowEnabled(m_hWnd) )
					return S_OK;
				CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(_T("Sys_MaxBtn")));
				if( pControl ) pControl->SetVisible(false);
				pControl = static_cast<CControlUI*>(paint_manager_.FindControl(_T("Sys_RestoreBtn")));
				if( pControl ) pControl->SetVisible(true);
			}
			else {
				if( m_hWnd == NULL || !::IsWindow(m_hWnd) || !::IsWindowEnabled(m_hWnd) )
					return S_OK;
				CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(_T("Sys_MaxBtn")));
				if( pControl ) pControl->SetVisible(true);
				pControl = static_cast<CControlUI*>(paint_manager_.FindControl(_T("Sys_RestoreBtn")));
				if( pControl ) pControl->SetVisible(false);
			}
		}
		return lRes;
	}


	CStdString WindowImplBase::GetSkinFolder()
	{
		return CPaintManagerUI::GetInstancePath() + _T("..\\skin\\");
	}

    CStdString WindowImplBase::GetSkinFile()
    {
        // 外文件模式, 此函数子类一定要实现
        ASSERT(false);
        return _T("");
    }

    UINT WindowImplBase::GetSkinResourceID()
    {
        return 0;
    }

    CStdString WindowImplBase::GetSkinResourceType()
    {
        // 资源模式, 此函数子类一定要实现
        ASSERT(false);
        return _T("");
    }

	LRESULT WindowImplBase::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
        RECT stClientRect = { 0 };
        CDialogBuilder builder;
        CControlUI* pRootUI = NULL;
        UINT uResourceID = 0;

		::ReleaseCapture();
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= (WS_OVERLAPPEDWINDOW);
        styleValue &= ~WS_CAPTION;
        ::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

        // ::SetClassLong(m_hWnd, GCL_STYLE, GetClassLong(m_hWnd, GCL_STYLE) | CS_DROPSHADOW);

		::GetClientRect(m_hWnd, &stClientRect);
		::SetWindowPos(m_hWnd, NULL, stClientRect.left, stClientRect.top, stClientRect.right - stClientRect.left, \
                       stClientRect.bottom - stClientRect.top, SWP_FRAMECHANGED);

		paint_manager_.Init(m_hWnd);
		paint_manager_.AddPreMessageFilter(this);
		paint_manager_.SetClassName(_T("XDUIFoundation"));
		
		// paint_manager_.SetResourcePath(GetSkinFolder().GetData());

#ifdef USE_ZIP_SKIN
		tString tstrSkin = GetSkinFile();
		if (paint_manager_.GetResourceZip().IsEmpty())
		{
#if USE(EMBEDED_RESOURCE)
			HRSRC hResource = ::FindResource(paint_manager_.GetResourceDll(), MAKEINTRESOURCE(NULL), _T("ZIPRES"));
			if( hResource == NULL )
				return 0L;
			DWORD dwSize = 0;
			HGLOBAL hGlobal = ::LoadResource(paint_manager_.GetResourceDll(), hResource);
			if( hGlobal == NULL ) {
#if defined(WIN32) && !defined(UNDER_CE)
				FreeResource(hResource);
#endif
				return 0L;
			}
			dwSize = ::SizeofResource(paint_manager_.GetResourceDll(), hResource);
			if( dwSize == 0 )
				return 0L;
			resource_zip_buffer_ = new BYTE[ dwSize ];
			if (resource_zip_buffer_ != NULL)
			{
				::CopyMemory(resource_zip_buffer_, (LPBYTE)::LockResource(hGlobal), dwSize);
			}
#if defined(WIN32) && !defined(UNDER_CE)
			::FreeResource(hResource);
#endif
			paint_manager_.SetResourceZip(resource_zip_buffer_, dwSize);
#else
			paint_manager_.SetResourceZip(kResourceSkinZipFileName, true);
#endif
		}

#else
        uResourceID = GetSkinResourceID();
        if (uResourceID == 0)
        {
            // 使用皮肤文件
            pRootUI = builder.Create(GetSkinFile().GetData(), (UINT)0, this, &paint_manager_);
        }
        else
        {
            pRootUI = builder.Create(uResourceID, GetSkinResourceType(), this, &paint_manager_);
        }
#endif
		ASSERT(pRootUI != NULL);
		paint_manager_.AttachDialog(pRootUI);
		paint_manager_.AddNotifier(this);
        // paint_manager_.SetBackgroundTransparent(true);

		Init();
		return 0;
	}

	LRESULT WindowImplBase::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch (uMsg)
		{
		case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:			lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
#if defined(WIN32) && !defined(UNDER_CE)
		case WM_NCACTIVATE:		lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:		lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:		lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:		lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO:	lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEWHEEL:		lRes = OnMouseWheel(uMsg, wParam, lParam, bHandled); break;
#endif
		case WM_SIZE:			lRes = OnSize(uMsg, wParam, lParam, bHandled); break;	
		case WM_SYSCOMMAND:		lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		case WM_KEYDOWN:		lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
		case WM_KILLFOCUS:		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
		case WM_SETFOCUS:		lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
		case WM_LBUTTONUP:		lRes = OnLButtonUp(uMsg, wParam, lParam, bHandled); break;
		case WM_LBUTTONDOWN:	lRes = OnLButtonDown(uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEMOVE:		lRes = OnMouseMove(uMsg, wParam, lParam, bHandled); break;
		case WM_PAINT:			lRes = OnPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_TIMER:			lRes = OnTimer(uMsg, wParam, lParam, bHandled); break;
		default:				bHandled = FALSE; break;
		}
		if (bHandled) return lRes;

		lRes = HandleCustomMessage(uMsg, wParam, lParam, bHandled);
		if (bHandled) return lRes;

		if (paint_manager_.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	LRESULT WindowImplBase::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/)
	{
		if (uMsg == WM_KEYDOWN)
		{
			switch (wParam)
			{
			case VK_RETURN:
			case VK_ESCAPE:
				return ResponseDefaultKeyEvent(wParam);
			default:
				break;
			}
		}
		return FALSE;
	}

	LRESULT WindowImplBase::ResponseDefaultKeyEvent(WPARAM wParam)
	{
		if (wParam == VK_RETURN)
		{
			return FALSE;
		}
		else if (wParam == VK_ESCAPE)
		{
			//Close();
			return TRUE;
		}

		return FALSE;
	}

	void WindowImplBase::Cleanup()
	{
		if (resource_zip_buffer_ != NULL)
		{
			delete[] resource_zip_buffer_;
			resource_zip_buffer_ = NULL;
		}
	}

	LRESULT WindowImplBase::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{

		bHandled=FALSE;
		return FALSE;
	}
	LRESULT WindowImplBase::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{

		bHandled=FALSE;
		return FALSE;
	}
}