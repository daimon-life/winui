#include "stdafx.h"

#include <textserv.h>
#include "UIRichEdit.h"
#include "UIRichEditFun.h"
#include "OleHelper.h"

// These constants are for backward compatibility. They are the 
// sizes used for initialization and reset in RichEdit 1.0
#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

typedef std::map<char*,DuiLib::duistring*>	MAP_CONTENTS;

namespace DuiLib {

	const LONG cInitTextMax = (32 * 1024) - 1;

	const ATL::CString oleEnd = _T("<:end>");

	EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
		0x8d33f740,
		0xcf58,
		0x11ce,
		{0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
	};

	EXTERN_C const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
		0xc5bdd8d0,
		0xd26e,
		0x11ce,
		{0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
	};

	class CTxtWinHost : public ITextHost,public EnumOleObjectByRange
	{
	public:
		CTxtWinHost();
		virtual ~CTxtWinHost();	
		BOOL Init(CRichEditUI *re,const CREATESTRUCT *pcs);

		void setEventSink(RichEditEvent *pSink);
		RichEditEvent* getEventSink(void);

		ITextServices* GetTextServices(void) {return pserv; }
		void SetClientRect(RECT *prc);
		RECT* GetClientRect() { return &rcClient; }
		BOOL GetWordWrap(void) { return fWordWrap; }
		void SetWordWrap(BOOL fWordWrap);
		BOOL GetReadOnly();
		void SetReadOnly(BOOL fReadOnly);
		void SetFont(HFONT hFont);
		void SetColor(DWORD dwColor);
		SIZEL* GetExtent();
		void SetExtent(SIZEL *psizelExtent);
		void LimitText(LONG nChars);
		BOOL IsCaptured();

		BOOL GetAllowBeep();
		void SetAllowBeep(BOOL fAllowBeep);
		WORD GetDefaultAlign();
		void SetDefaultAlign(WORD wNewAlign);
		BOOL GetRichTextFlag();
		void SetRichTextFlag(BOOL fNew);
		LONG GetDefaultLeftIndent();
		void SetDefaultLeftIndent(LONG lNewIndent);
		BOOL SetSaveSelection(BOOL fSaveSelection);
		HRESULT OnTxInPlaceDeactivate();
		HRESULT OnTxInPlaceActivate(LPCRECT prcClient);
		BOOL GetActiveState(void) { return fInplaceActive; }
		BOOL DoSetCursor(RECT *prc, POINT *pt);
		void SetTransparent(BOOL fTransparent);
		void GetControlRect(LPRECT prc);
		LONG SetAccelPos(LONG laccelpos);
		WCHAR SetPasswordChar(WCHAR chPasswordChar);
		void SetDisabled(BOOL fOn);
		LONG SetSelBarWidth(LONG lSelBarWidth);
		BOOL GetTimerState();

		void SetCharFormat(CHARFORMAT2W &c);
		void SetParaFormat(PARAFORMAT2 &p);

		// -----------------------------
		//	IUnknown interface
		// -----------------------------
		virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
		virtual ULONG _stdcall AddRef(void);
		virtual ULONG _stdcall Release(void);

		// -----------------------------
		//	ITextHost interface
		// -----------------------------
		virtual HDC TxGetDC();
		virtual INT TxReleaseDC(HDC hdc);
		virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);
		virtual BOOL TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
		virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
		virtual BOOL TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
		virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
		virtual void TxViewChange(BOOL fUpdate);
		virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
		virtual BOOL TxShowCaret(BOOL fShow);
		virtual BOOL TxSetCaretPos(INT x, INT y);
		virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);
		virtual void TxKillTimer(UINT idTimer);
		virtual void TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
		virtual void TxSetCapture(BOOL fCapture);
		virtual void TxSetFocus();
		virtual void TxSetCursor(HCURSOR hcur, BOOL fText);
		virtual BOOL TxScreenToClient (LPPOINT lppt);
		virtual BOOL TxClientToScreen (LPPOINT lppt);
		virtual HRESULT TxActivate( LONG * plOldState );
		virtual HRESULT TxDeactivate( LONG lNewState );
		virtual HRESULT TxGetClientRect(LPRECT prc);
		virtual HRESULT TxGetViewInset(LPRECT prc);
		virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF );
		virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);
		virtual COLORREF TxGetSysColor(int nIndex);
		virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
		virtual HRESULT TxGetMaxLength(DWORD *plength);
		virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
		virtual HRESULT TxGetPasswordChar(TCHAR *pch);
		virtual HRESULT TxGetAcceleratorPos(LONG *pcp);
		virtual HRESULT TxGetExtent(LPSIZEL lpExtent);
		virtual HRESULT OnTxCharFormatChange (const CHARFORMATW * pcf);
		virtual HRESULT OnTxParaFormatChange (const PARAFORMAT * ppf);
		virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
		virtual HRESULT TxNotify(DWORD iNotify, void *pv);
		virtual HIMC TxImmGetContext(void);
		virtual void TxImmReleaseContext(HIMC himc);
		virtual HRESULT TxGetSelectionBarWidth (LONG *lSelBarWidth);

		bool OnOleObject(IOleObject* pObject,const CLSID& pIID,long lPos);

	private:
        HMODULE     m_hRichEdit20;      // RichEdit20 Dll Handle
		CRichEditUI *m_re;
		ULONG	cRefs;					// Reference Count
		ITextServices	*pserv;		    // pointer to Text Services object
		// Properties
		RichEditEvent	*m_pSink;

		DWORD		dwStyle;				// style bits

		unsigned	fEnableAutoWordSel	:1;	// enable Word style auto word selection?
		unsigned	fWordWrap			:1;	// Whether control should word wrap
		unsigned	fAllowBeep			:1;	// Whether beep is allowed
		unsigned	fRich				:1;	// Whether control is rich text
		unsigned	fSaveSelection		:1;	// Whether to save the selection when inactive
		unsigned	fInplaceActive		:1; // Whether control is inplace active
		unsigned	fTransparent		:1; // Whether control is transparent
		unsigned	fTimer				:1;	// A timer is set
		unsigned    fCaptured           :1;

		LONG		lSelBarWidth;			// Width of the selection bar
		LONG  		cchTextMost;			// maximum text size
		DWORD		dwEventMask;			// DoEvent mask to pass on to parent window
		LONG		icf;
		LONG		ipf;
		RECT		rcClient;				// Client Rect for this control
		SIZEL		sizelExtent;			// Extent array
		CHARFORMAT2W cf;					// Default character format
		PARAFORMAT2	pf;					    // Default paragraph format
		LONG		laccelpos;				// Accelerator position
		WCHAR		chPasswordChar;		    // Password character

        LONG                    m_lActivateState;
	};

	// Convert Pixels on the X axis to Himetric
	LONG DXtoHimetricX(LONG dx, LONG xPerInch)
	{
		return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
	}

	// Convert Pixels on the Y axis to Himetric
	LONG DYtoHimetricY(LONG dy, LONG yPerInch)
	{
		return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
	}

	HRESULT InitDefaultCharFormat(CRichEditUI* re, CHARFORMAT2W* pcf, HFONT hfont) 
	{
		memset(pcf, 0, sizeof(CHARFORMAT2W));
		LOGFONT lf;
		if( !hfont )
			hfont = re->GetManager()->GetFont(re->GetFont());
		::GetObject(hfont, sizeof(LOGFONT), &lf);

		DWORD dwColor = re->GetTextColor();
		pcf->cbSize = sizeof(CHARFORMAT2W);
		pcf->crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
		LONG yPixPerInch = GetDeviceCaps(re->GetManager()->GetPaintDC(), LOGPIXELSY);
		pcf->yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
		pcf->yOffset = 0;
		pcf->dwEffects = 0;
		pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
		if(lf.lfWeight >= FW_BOLD)
			pcf->dwEffects |= CFE_BOLD;
		if(lf.lfItalic)
			pcf->dwEffects |= CFE_ITALIC;
		if(lf.lfUnderline)
			pcf->dwEffects |= CFE_UNDERLINE;
		pcf->bCharSet = lf.lfCharSet;
		pcf->bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef _UNICODE
		_tcscpy_s(pcf->szFaceName, 32, lf.lfFaceName);
#else
		//need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
		MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, pcf->szFaceName, LF_FACESIZE) ;
#endif

		return S_OK;
	}

	HRESULT InitDefaultParaFormat(CRichEditUI* re, PARAFORMAT2* ppf) 
	{	
		memset(ppf, 0, sizeof(PARAFORMAT2));
		ppf->cbSize = sizeof(PARAFORMAT2);
		ppf->dwMask = PFM_ALL;
		ppf->wAlignment = PFA_LEFT;
		ppf->cTabCount = 1;
		ppf->rgxTabs[0] = lDefaultTab;

		return S_OK;
	}

	HRESULT CreateHost(CRichEditUI *re, const CREATESTRUCT *pcs, CTxtWinHost **pptec)
	{
		HRESULT hr = E_FAIL;
		//GdiSetBatchLimit(1);

		CTxtWinHost *phost = new CTxtWinHost();
		if(phost)
		{
			if (phost->Init(re, pcs))
			{
				*pptec = phost;
				hr = S_OK;
			}
		}

		if (FAILED(hr))
		{
			delete phost;
		}

		return TRUE;
	}


	void PreStreamOut(CRichEditUI *pRe,StreamOutResult* pResult,IRichEditOle* pOle)
	{
		long			lCount = 0;
		HRESULT			hr = 0;
		atlCString	szOutInfo;
		if( (pResult==0) || (pOle == 0) || (pRe == 0) )	return ;
		lCount =	pOle->GetObjectCount();
		for(int i = 0; i < lCount;i++)
		{	
			REOBJECT		reObj;
			atlCString		szPath;
			ZeroMemory(&reObj, sizeof(REOBJECT));
			reObj.cbStruct  = sizeof(REOBJECT);
			hr = pOle->GetObject(0,&reObj,REO_GETOBJ_ALL_INTERFACES);
			if( SUCCEEDED(hr) )
			{
				reObj.dwFlags = REO_BELOWBASELINE;
				if( IsPictureGuid((LPTSTR)reObj.dwUser) )
				{
					OleDataFormatPicEtc(pRe,szPath.GetString(),(LPTSTR)reObj.dwUser,
						reObj.cp,szOutInfo);
				}
				if( reObj.poleobj )
					reObj.poleobj->Release();
				if (reObj.polesite)
					reObj.polesite->Release();
				if (reObj.pstg)
					reObj.pstg->Release();
			}
		}
		szOutInfo += oleEnd;
		pResult->appendValue(szOutInfo.GetString(),szOutInfo.GetLength());
	}
	////////////////////// Create/Init/Destruct Commands ///////////////////////
	CTxtWinHost::CTxtWinHost() : m_re(NULL)
	{
		::ZeroMemory(&cRefs, sizeof(CTxtWinHost) - offsetof(CTxtWinHost, cRefs));
		cchTextMost = cInitTextMax;
		laccelpos = -1;
		m_pSink = 0;

        m_lActivateState = 0;
	}

	CTxtWinHost::~CTxtWinHost()
	{
		pserv->OnTxInPlaceDeactivate();
		pserv->Release();
	}
	void CTxtWinHost::setEventSink(RichEditEvent *pSink)
	{
		m_pSink = pSink;
	}
	RichEditEvent* CTxtWinHost::getEventSink(void)
	{
		return m_pSink;
	}

	BOOL CTxtWinHost::Init(CRichEditUI *re, const CREATESTRUCT *pcs)
	{
        typedef HRESULT(_stdcall *CTSFunc)(IUnknown *punkOuter, ITextHost *pITextHost, IUnknown **ppUnk);

		IUnknown *pUnk = NULL;
		HRESULT hr = S_OK;
        CTSFunc ctsFunc = NULL;

        if (m_hRichEdit20)
        {
            FreeLibrary(m_hRichEdit20);
            m_hRichEdit20 = NULL;
        }
        m_hRichEdit20 = LoadLibrary(_T("msftedit.dll"));
        if (m_hRichEdit20 == NULL)
        {
            return FALSE;
        }
        ctsFunc = (CTSFunc)GetProcAddress(m_hRichEdit20, "CreateTextServices");
        if (ctsFunc == NULL)
        {
            FreeLibrary(m_hRichEdit20);
            m_hRichEdit20 = NULL;
            return FALSE;
        }

		m_re = re;
		// Initialize Reference count
		cRefs = 1;

		// Create and cache CHARFORMAT for this control
		if(FAILED(InitDefaultCharFormat(re, &cf, NULL)))
			goto err;

		// Create and cache PARAFORMAT for this control
		if(FAILED(InitDefaultParaFormat(re, &pf)))
			goto err;

		// edit controls created without a window are multiline by default
		// so that paragraph formats can be
		dwStyle = ES_MULTILINE;

		// edit controls are rich by default
		fRich = re->IsRich();

		cchTextMost = re->GetLimitText();

		if (pcs )
		{
			dwStyle = pcs->style;

			if ( !(dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL)) )
			{
				fWordWrap = TRUE;
			}
		}

		if( !(dwStyle & ES_LEFT) )
		{
			if(dwStyle & ES_CENTER)
				pf.wAlignment = PFA_CENTER;
			else if(dwStyle & ES_RIGHT)
				pf.wAlignment = PFA_RIGHT;
		}

		fInplaceActive = TRUE;

		// Create Text Services component
        if (FAILED(ctsFunc(NULL, this, &pUnk)))
			goto err;

		hr = pUnk->QueryInterface(IID_ITextServices,(void **)&pserv);

		// Whether the previous call succeeded or failed we are done
		// with the private interface.
		pUnk->Release();

		if(FAILED(hr))
		{
			goto err;
		}

		// Set window text
		if(pcs && pcs->lpszName)
		{
#ifdef _UNICODE		
			if(FAILED(pserv->TxSetText((TCHAR *)pcs->lpszName)))
				goto err;
#else
			size_t iLen = _tcslen(pcs->lpszName);
			LPWSTR lpText = new WCHAR[iLen + 1];
			::ZeroMemory(lpText, (iLen + 1) * sizeof(WCHAR));
			::MultiByteToWideChar(CP_ACP, 0, pcs->lpszName, -1, (LPWSTR)lpText, iLen) ;
			if(FAILED(pserv->TxSetText((LPWSTR)lpText))) {
				delete[] lpText;
				goto err;
			}
			delete[] lpText;
#endif
		}
		IRichEditOle *pRichItem = NULL;
		pserv->TxSendMessage(EM_GETOLEINTERFACE,0,(LPARAM)((PVOID)&pRichItem),NULL);
		OleLib::IRichEditOleExCallback* pOleCall = new OleLib::IRichEditOleExCallback(pRichItem , m_re);
		if( pOleCall )
		{
			pserv->TxSendMessage(EM_SETOLECALLBACK,0,(LPARAM)pOleCall,0);
		}
		pRichItem->Release();
		return TRUE;
err:
		return FALSE;
	}

	/////////////////////////////////  IUnknown ////////////////////////////////


	HRESULT CTxtWinHost::QueryInterface(REFIID riid, void **ppvObject)
	{
		HRESULT hr = E_NOINTERFACE;
		*ppvObject = NULL;

		if (IsEqualIID(riid, IID_IUnknown) 
			|| IsEqualIID(riid, IID_ITextHost)) 
		{
			AddRef();
			*ppvObject = (ITextHost *) this;
			hr = S_OK;
		}

		return hr;
	}

	ULONG CTxtWinHost::AddRef(void)
	{
		return ++cRefs;
	}

	ULONG CTxtWinHost::Release(void)
	{
		ULONG c_Refs = --cRefs;

		if (c_Refs == 0)
		{
			delete this;
		}

		return c_Refs;
	}

	/////////////////////////////////  Far East Support  //////////////////////////////////////

	HIMC CTxtWinHost::TxImmGetContext(void)
	{
        return ImmGetContext(m_re->GetParentWindow());
	}

	void CTxtWinHost::TxImmReleaseContext(HIMC himc)
	{
        ImmReleaseContext(m_re->GetParentWindow(), himc);
	}

	//////////////////////////// ITextHost Interface  ////////////////////////////

	HDC CTxtWinHost::TxGetDC()
	{
		return m_re->GetManager()->GetPaintDC();
	}

	int CTxtWinHost::TxReleaseDC(HDC hdc)
	{
		return 1;
	}

	BOOL CTxtWinHost::TxShowScrollBar(INT fnBar, BOOL fShow)
	{
		CScrollBarUI* pVerticalScrollBar = m_re->GetVerticalScrollBar();
		CScrollBarUI* pHorizontalScrollBar = m_re->GetHorizontalScrollBar();
		if( fnBar == SB_VERT && pVerticalScrollBar ) {
			pVerticalScrollBar->SetVisible(fShow == TRUE);
		}
		else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
			pHorizontalScrollBar->SetVisible(fShow == TRUE);
		}
		else if( fnBar == SB_BOTH ) {
			if( pVerticalScrollBar ) pVerticalScrollBar->SetVisible(fShow == TRUE);
			if( pHorizontalScrollBar ) pHorizontalScrollBar->SetVisible(fShow == TRUE);
		}
		return TRUE;
	}

	BOOL CTxtWinHost::TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags)
	{
		if( fuSBFlags == SB_VERT ) {
			m_re->EnableScrollBar(true, m_re->GetHorizontalScrollBar() != NULL);
			m_re->GetVerticalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
		}
		else if( fuSBFlags == SB_HORZ ) {
			m_re->EnableScrollBar(m_re->GetVerticalScrollBar() != NULL, true);
			m_re->GetHorizontalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
		}
		else if( fuSBFlags == SB_BOTH ) {
			m_re->EnableScrollBar(true, true);
			m_re->GetVerticalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
			m_re->GetHorizontalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
		}
		return TRUE;
	}

	BOOL CTxtWinHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
	{
		CScrollBarUI* pVerticalScrollBar = m_re->GetVerticalScrollBar();
		CScrollBarUI* pHorizontalScrollBar = m_re->GetHorizontalScrollBar();
		if( fnBar == SB_VERT && pVerticalScrollBar ) {
			if( nMaxPos - nMinPos - rcClient.bottom + rcClient.top <= 0 ) {
				pVerticalScrollBar->SetVisible(false);
			}
			else {
				pVerticalScrollBar->SetVisible(true);
				pVerticalScrollBar->SetScrollRange(nMaxPos - nMinPos - rcClient.bottom + rcClient.top);
			}
		}
		else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
			if( nMaxPos - nMinPos - rcClient.right + rcClient.left <= 0 ) {
				pHorizontalScrollBar->SetVisible(false);
			}
			else {
				pHorizontalScrollBar->SetVisible(true);
				pHorizontalScrollBar->SetScrollRange(nMaxPos - nMinPos - rcClient.right + rcClient.left);
			}   
		}
		return TRUE;
	}

	BOOL CTxtWinHost::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw)
	{
		CScrollBarUI* pVerticalScrollBar = m_re->GetVerticalScrollBar();
		CScrollBarUI* pHorizontalScrollBar = m_re->GetHorizontalScrollBar();
		if( fnBar == SB_VERT && pVerticalScrollBar ) {
			pVerticalScrollBar->SetScrollPos(nPos);
		}
		else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
			pHorizontalScrollBar->SetScrollPos(nPos);
		}
		return TRUE;
	}

	void CTxtWinHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
	{
		if( prc == NULL ) {
			m_re->GetManager()->Invalidate(rcClient);
			return;
		}
		RECT rc = *prc;
		m_re->GetManager()->Invalidate(rc);
	}

	void CTxtWinHost::TxViewChange(BOOL fUpdate) 
	{
		if( m_re->OnTxViewChanged() ) m_re->Invalidate();
	}

	BOOL CTxtWinHost::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
	{
		return ::CreateCaret(m_re->GetManager()->GetPaintWindow(), hbmp, xWidth, yHeight);
	}

	BOOL CTxtWinHost::TxShowCaret(BOOL fShow)
	{
		if(fShow)
			return ::ShowCaret(m_re->GetManager()->GetPaintWindow());
		else
			return ::HideCaret(m_re->GetManager()->GetPaintWindow());
	}

	BOOL CTxtWinHost::TxSetCaretPos(INT x, INT y)
	{
		return ::SetCaretPos(x, y);
	}

	BOOL CTxtWinHost::TxSetTimer(UINT idTimer, UINT uTimeout)
	{
		fTimer = TRUE;
		return m_re->GetManager()->SetTimer(m_re, idTimer, uTimeout) == TRUE;
	}

	void CTxtWinHost::TxKillTimer(UINT idTimer)
	{
		m_re->GetManager()->KillTimer(m_re, idTimer);
		fTimer = FALSE;
	}

	void CTxtWinHost::TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll,	LPCRECT lprcClip,	HRGN hrgnUpdate, LPRECT lprcUpdate,	UINT fuScroll)	
	{
		return;
	}

	void CTxtWinHost::TxSetCapture(BOOL fCapture)
	{
		if (fCapture) m_re->GetManager()->SetCapture();
		else m_re->GetManager()->ReleaseCapture();
		fCaptured = fCapture;
	}

	void CTxtWinHost::TxSetFocus()
	{
		if(m_re)
			m_re->SetFocus();
	}

	void CTxtWinHost::TxSetCursor(HCURSOR hcur,	BOOL fText)
	{
		::SetCursor(hcur);
	}

	BOOL CTxtWinHost::TxScreenToClient(LPPOINT lppt)
	{
		return ::ScreenToClient(m_re->GetManager()->GetPaintWindow(), lppt);	
	}

	BOOL CTxtWinHost::TxClientToScreen(LPPOINT lppt)
	{
		return ::ClientToScreen(m_re->GetManager()->GetPaintWindow(), lppt);
	}

	HRESULT CTxtWinHost::TxActivate(LONG *plOldState)
	{
        (*plOldState) = m_lActivateState;
        m_lActivateState = TRUE;
		return S_OK;
	}

	HRESULT CTxtWinHost::TxDeactivate(LONG lNewState)
	{
        m_lActivateState = lNewState;
		return S_OK;
	}

	HRESULT CTxtWinHost::TxGetClientRect(LPRECT prc)
	{
		*prc = rcClient;
		GetControlRect(prc);
		return NOERROR;
	}

	HRESULT CTxtWinHost::TxGetViewInset(LPRECT prc) 
	{
		prc->left = prc->right = prc->top = prc->bottom = 0;
		return NOERROR;	
	}

	HRESULT CTxtWinHost::TxGetCharFormat(const CHARFORMATW **ppCF)
	{
		*ppCF = &cf;
		return NOERROR;
	}

	HRESULT CTxtWinHost::TxGetParaFormat(const PARAFORMAT **ppPF)
	{
		*ppPF = &pf;
		return NOERROR;
	}

	COLORREF CTxtWinHost::TxGetSysColor(int nIndex) 
	{
		return ::GetSysColor(nIndex);
	}

	HRESULT CTxtWinHost::TxGetBackStyle(TXTBACKSTYLE *pstyle)
	{
		*pstyle = !fTransparent ? TXTBACK_OPAQUE : TXTBACK_TRANSPARENT;
		return NOERROR;
	}

	HRESULT CTxtWinHost::TxGetMaxLength(DWORD *pLength)
	{
		*pLength = cchTextMost;
		return NOERROR;
	}

	HRESULT CTxtWinHost::TxGetScrollBars(DWORD *pdwScrollBar)
	{
		*pdwScrollBar =  dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | 
			ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

		return NOERROR;
	}

	HRESULT CTxtWinHost::TxGetPasswordChar(TCHAR *pch)
	{
#ifdef _UNICODE
		*pch = chPasswordChar;
#else
		::WideCharToMultiByte(CP_ACP, 0, &chPasswordChar, 1, pch, 1, NULL, NULL) ;
#endif
		return NOERROR;
	}

	HRESULT CTxtWinHost::TxGetAcceleratorPos(LONG *pcp)
	{
		*pcp = laccelpos;
		return S_OK;
	} 										   

	HRESULT CTxtWinHost::OnTxCharFormatChange(const CHARFORMATW *pcf)
	{
		return S_OK;
	}

	HRESULT CTxtWinHost::OnTxParaFormatChange(const PARAFORMAT *ppf)
	{
		return S_OK;
	}

	HRESULT CTxtWinHost::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) 
	{
		DWORD dwProperties = 0;

		if (fRich)
		{
			dwProperties = TXTBIT_RICHTEXT;
		}

		if (dwStyle & ES_MULTILINE)
		{
			dwProperties |= TXTBIT_MULTILINE;
		}

		if (dwStyle & ES_READONLY)
		{
			dwProperties |= TXTBIT_READONLY;
		}

		if (dwStyle & ES_PASSWORD)
		{
			dwProperties |= TXTBIT_USEPASSWORD;
		}

		if (!(dwStyle & ES_NOHIDESEL))
		{
			dwProperties |= TXTBIT_HIDESELECTION;
		}

		if (fEnableAutoWordSel)
		{
			dwProperties |= TXTBIT_AUTOWORDSEL;
		}

		if (fWordWrap)
		{
			dwProperties |= TXTBIT_WORDWRAP;
		}

		if (fAllowBeep)
		{
			dwProperties |= TXTBIT_ALLOWBEEP;
		}

		if (fSaveSelection)
		{
			dwProperties |= TXTBIT_SAVESELECTION;
		}

		*pdwBits = dwProperties & dwMask; 
		return NOERROR;
	}
	bool CTxtWinHost::OnOleObject(IOleObject* pObject,const CLSID& pIID,long lPos)
	{
		if( pIID == Guid_ImageObject )
		{
			OleLib::CImageOleObject* pImageOle = (OleLib::CImageOleObject*)pObject;
			if( pImageOle )
				pImageOle->DoUrl(lPos);
		}
		if( pIID == Guid_OleControlUi )
		{
			OleLib::COleControlUi* pControlOle = (OleLib::COleControlUi*)pObject;
			if( pControlOle )
				pControlOle->DoUrl(lPos);
		}
		return true;
	}
	HRESULT CTxtWinHost::TxNotify(DWORD iNotify, void *pv)
	{
		if( iNotify == EN_REQUESTRESIZE ) 
		{
			RECT rc;
			REQRESIZE *preqsz = (REQRESIZE *)pv;
			GetControlRect(&rc);
			rc.bottom = rc.top + preqsz->rc.bottom;
			rc.right  = rc.left + preqsz->rc.right;
			SetClientRect(&rc);
			return S_OK;
		}
		if( iNotify == EN_LINK )
		{
			ENLINK* pLinkInfo = (ENLINK*)pv;
			if( pLinkInfo == 0 )	return S_OK;
			if( pLinkInfo->msg == WM_LBUTTONDOWN )
			{
				IRichEditOle *pRichItem = NULL;
				pserv->TxSendMessage(EM_GETOLEINTERFACE,0,(LPARAM)((PVOID)&pRichItem),NULL);
				bool bReturn = EnumOleObjectByPos(pRichItem,pLinkInfo->chrg,this);
				pRichItem->Release();
				if (bReturn)
				{
					return S_OK;
				}
			}
			//return S_OK; // XNRicheditEx������Ҫ������������
		}
		m_re->OnTxNotify(iNotify, pv);
		return S_OK;
	}

	HRESULT CTxtWinHost::TxGetExtent(LPSIZEL lpExtent)
	{
		*lpExtent = sizelExtent;
		return S_OK;
	}

	HRESULT	CTxtWinHost::TxGetSelectionBarWidth (LONG *plSelBarWidth)
	{
		*plSelBarWidth = lSelBarWidth;
		return S_OK;
	}

	void CTxtWinHost::SetWordWrap(BOOL fWordWrap)
	{
		fWordWrap = fWordWrap;
		pserv->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, fWordWrap ? TXTBIT_WORDWRAP : 0);
	}

	BOOL CTxtWinHost::GetReadOnly()
	{
		return (dwStyle & ES_READONLY) != 0;
	}

	void CTxtWinHost::SetReadOnly(BOOL fReadOnly)
	{
		if (fReadOnly)
		{
			dwStyle |= ES_READONLY;
		}
		else
		{
			dwStyle &= ~ES_READONLY;
		}

		pserv->OnTxPropertyBitsChange(TXTBIT_READONLY, 
			fReadOnly ? TXTBIT_READONLY : 0);
	}

	void CTxtWinHost::SetFont(HFONT hFont) 
	{
		if( hFont == NULL ) return;
		LOGFONT lf;
		::GetObject(hFont, sizeof(LOGFONT), &lf);
		LONG yPixPerInch = ::GetDeviceCaps(m_re->GetManager()->GetPaintDC(), LOGPIXELSY);
		cf.yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
		if(lf.lfWeight >= FW_BOLD)
			cf.dwEffects |= CFE_BOLD;
		else
			cf.dwEffects &= ~CFE_BOLD;
		if(lf.lfItalic)
			cf.dwEffects |= CFE_ITALIC;
		else
			cf.dwEffects &= ~CFE_ITALIC;
		if(lf.lfUnderline)
			cf.dwEffects |= CFE_UNDERLINE;
		else
			cf.dwEffects &= ~CFE_UNDERLINE;
		cf.bCharSet = lf.lfCharSet;
		cf.bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef _UNICODE
		_tcscpy(cf.szFaceName, lf.lfFaceName);
#else
		//need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
		MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, cf.szFaceName, LF_FACESIZE) ;
#endif

		pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
			TXTBIT_CHARFORMATCHANGE);
	}

	void CTxtWinHost::SetColor(DWORD dwColor)
	{
		cf.crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
		pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
			TXTBIT_CHARFORMATCHANGE);
	}

	SIZEL* CTxtWinHost::GetExtent() 
	{
		return &sizelExtent;
	}

	void CTxtWinHost::SetExtent(SIZEL *psizelExtent) 
	{ 
		sizelExtent = *psizelExtent; 
		pserv->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);
	}

	void CTxtWinHost::LimitText(LONG nChars)
	{
		cchTextMost = nChars;
		if( cchTextMost <= 0 ) cchTextMost = cInitTextMax;
		pserv->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
	}

	BOOL CTxtWinHost::IsCaptured()
	{
		return fCaptured;
	}

	BOOL CTxtWinHost::GetAllowBeep()
	{
		return fAllowBeep;
	}

	void CTxtWinHost::SetAllowBeep(BOOL fAllowBeep)
	{
		fAllowBeep = fAllowBeep;

		pserv->OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, 
			fAllowBeep ? TXTBIT_ALLOWBEEP : 0);
	}

	WORD CTxtWinHost::GetDefaultAlign()
	{
		return pf.wAlignment;
	}

	void CTxtWinHost::SetDefaultAlign(WORD wNewAlign)
	{
		pf.wAlignment = wNewAlign;

		// Notify control of property change
		pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
	}

	BOOL CTxtWinHost::GetRichTextFlag()
	{
		return fRich;
	}

	void CTxtWinHost::SetRichTextFlag(BOOL fNew)
	{
		fRich = fNew;

		pserv->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, 
			fNew ? TXTBIT_RICHTEXT : 0);
	}

	LONG CTxtWinHost::GetDefaultLeftIndent()
	{
		return pf.dxOffset;
	}

	void CTxtWinHost::SetDefaultLeftIndent(LONG lNewIndent)
	{
		pf.dxOffset = lNewIndent;

		pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
	}

	void CTxtWinHost::SetClientRect(RECT *prc) 
	{
		rcClient = *prc;

		LONG xPerInch = ::GetDeviceCaps(m_re->GetManager()->GetPaintDC(), LOGPIXELSX); 
		LONG yPerInch =	::GetDeviceCaps(m_re->GetManager()->GetPaintDC(), LOGPIXELSY); 
		sizelExtent.cx = DXtoHimetricX(rcClient.right - rcClient.left, xPerInch);
		sizelExtent.cy = DYtoHimetricY(rcClient.bottom - rcClient.top, yPerInch);

		pserv->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
	}

	BOOL CTxtWinHost::SetSaveSelection(BOOL f_SaveSelection)
	{
		BOOL fResult = f_SaveSelection;

		fSaveSelection = f_SaveSelection;

		// notify text services of property change
		pserv->OnTxPropertyBitsChange(TXTBIT_SAVESELECTION, 
			fSaveSelection ? TXTBIT_SAVESELECTION : 0);

		return fResult;		
	}

	HRESULT	CTxtWinHost::OnTxInPlaceDeactivate()
	{
		HRESULT hr = pserv->OnTxInPlaceDeactivate();

		if (SUCCEEDED(hr))
		{
			fInplaceActive = FALSE;
		}

		return hr;
	}

	HRESULT	CTxtWinHost::OnTxInPlaceActivate(LPCRECT prcClient)
	{
		fInplaceActive = TRUE;

		HRESULT hr = pserv->OnTxInPlaceActivate(prcClient);

		if (FAILED(hr))
		{
			fInplaceActive = FALSE;
		}

		return hr;
	}

	BOOL CTxtWinHost::DoSetCursor(RECT *prc, POINT *pt)
	{
		RECT rc = prc ? *prc : rcClient;

		// Is this in our rectangle?
		if (PtInRect(&rc, *pt))
		{
			RECT *prcClient = (!fInplaceActive || prc) ? &rc : NULL;
			pserv->OnTxSetCursor(DVASPECT_CONTENT,	-1, NULL, NULL,  m_re->GetManager()->GetPaintDC(),
				NULL, prcClient, pt->x, pt->y);

			return TRUE;
		}

		return FALSE;
	}

	void CTxtWinHost::GetControlRect(LPRECT prc)
	{
		prc->top = rcClient.top;
		prc->bottom = rcClient.bottom;
		prc->left = rcClient.left;
		prc->right = rcClient.right;
	}

	void CTxtWinHost::SetTransparent(BOOL f_Transparent)
	{
		fTransparent = f_Transparent;

		// notify text services of property change
		pserv->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
	}

	LONG CTxtWinHost::SetAccelPos(LONG l_accelpos)
	{
		LONG laccelposOld = l_accelpos;

		laccelpos = l_accelpos;

		// notify text services of property change
		pserv->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);

		return laccelposOld;
	}

	WCHAR CTxtWinHost::SetPasswordChar(WCHAR ch_PasswordChar)
	{
		WCHAR chOldPasswordChar = chPasswordChar;

		chPasswordChar = ch_PasswordChar;

		// notify text services of property change
		pserv->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
			(chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);

		return chOldPasswordChar;
	}

	void CTxtWinHost::SetDisabled(BOOL fOn)
	{
		cf.dwMask	 |= CFM_COLOR | CFM_DISABLED;
		cf.dwEffects |= CFE_AUTOCOLOR | CFE_DISABLED;

		if( !fOn )
		{
			cf.dwEffects &= ~CFE_DISABLED;
		}

		pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
			TXTBIT_CHARFORMATCHANGE);
	}

	LONG CTxtWinHost::SetSelBarWidth(LONG l_SelBarWidth)
	{
		LONG lOldSelBarWidth = lSelBarWidth;

		lSelBarWidth = l_SelBarWidth;

		if (lSelBarWidth)
		{
			dwStyle |= ES_SELECTIONBAR;
		}
		else
		{
			dwStyle &= (~ES_SELECTIONBAR);
		}

		pserv->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);

		return lOldSelBarWidth;
	}

	BOOL CTxtWinHost::GetTimerState()
	{
		return fTimer;
	}

	void CTxtWinHost::SetCharFormat(CHARFORMAT2W &c)
	{
		cf = c;
	}

	void CTxtWinHost::SetParaFormat(PARAFORMAT2 &p)
	{
		pf = p;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	class ReleaseOleObject :public EnumOleObjectByRange
	{
	public:
		ReleaseOleObject(CRichEditUI *pUI,bool bSink)
		{
			m_pUi	= pUI;
			m_bSink = bSink;
		}
		~ReleaseOleObject()
		{
			m_pUi	= 0;
			m_bSink = 0;
		}
		bool OnOleObject(IOleObject* pObject,const CLSID& pIID,long lPos)
		{
			if( m_pUi == 0 )	return false;
			if( pIID == Guid_ImageObject )
			{
				OleLib::CImageOleObject* pImageOle = (OleLib::CImageOleObject*)pObject;
				char*	pPicGuid = 0;
				if( pImageOle )
				{
					if( m_bSink )
						pImageOle->RegisterUserEventSink(0);
					m_pUi->SetSel(lPos,lPos+1);			//OLE����ռһ���ַ�
					m_pUi->ReplaceSel(_T("   "),true);	//�˲����൱��ɾ��OLE����
				}
			}
			else if( pIID == Guid_OleControlUi )
			{
				OleLib::COleControlUi* pControlOle = (OleLib::COleControlUi*)pObject;
				if( pControlOle )
				{
					if( m_bSink )
						pControlOle->RegisterUserEventSink(0);
					m_pUi->SetSel(lPos,lPos+1);			//OLE����ռһ���ַ�
					m_pUi->ReplaceSel(_T("   "),true);	//�˲����൱��ɾ��OLE����
				}
			}
			return true;
		}
	private:
		CRichEditUI*	m_pUi;
		bool			m_bSink;
	};

	CRichEditUI::CRichEditUI() : m_pTwh(NULL), m_bVScrollBarFixing(false), m_bWantTab(true), m_bWantReturn(true), 
		m_bWantCtrlReturn(true), m_bRich(true), m_bReadOnly(false), m_bWordWrap(false), m_dwTextColor(0), m_iFont(-1), 
		m_iLimitText(cInitTextMax), m_lTwhStyle(ES_MULTILINE)
	{
	}

	RichEditEvent* CRichEditUI::GetEventSink(void)
	{
		if ( m_pTwh != NULL )
			return m_pTwh->getEventSink();
		return NULL;
	}
	void CRichEditUI::RegisterEventSink(RichEditEvent* pEvenSink)
	{
		if( m_pTwh )
			m_pTwh->setEventSink(pEvenSink);
	}
	CRichEditUI::~CRichEditUI()
	{
		if( m_pTwh )
		{
			ReleaseOleObject	cClearOle(this,true);
			IRichEditOle* pReOle = GetIRichEditOle();
			if( pReOle )
			{
				EnumAllOleObject(pReOle,&cClearOle);
				pReOle->Release();
			}
			m_pTwh->Release();
			m_pManager->RemoveMessageFilter(this);
		}
	}

	LPCTSTR CRichEditUI::GetClass() const
	{
		return _T("RichEditUI");
	}

	LPVOID CRichEditUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("RichEdit")) == 0 ) return static_cast<CRichEditUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	UINT CRichEditUI::GetControlFlags() const
	{
		if( !IsEnabled() ) return CControlUI::GetControlFlags();

		return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
	}

	bool CRichEditUI::IsWantTab()
	{
		return m_bWantTab;
	}

	void CRichEditUI::SetWantTab(bool bWantTab)
	{
		m_bWantTab = bWantTab;
	}

	bool CRichEditUI::IsWantReturn()
	{
		return m_bWantReturn;
	}

	void CRichEditUI::SetWantReturn(bool bWantReturn)
	{
		m_bWantReturn = bWantReturn;
	}

	bool CRichEditUI::IsWantCtrlReturn()
	{
		return m_bWantCtrlReturn;
	}

	void CRichEditUI::SetWantCtrlReturn(bool bWantCtrlReturn)
	{
		m_bWantCtrlReturn = bWantCtrlReturn;
	}

	bool CRichEditUI::IsRich()
	{
		return m_bRich;
	}

	void CRichEditUI::SetRich(bool bRich)
	{
		m_bRich = bRich;
		if( m_pTwh ) m_pTwh->SetRichTextFlag(bRich);
	}

	bool CRichEditUI::IsReadOnly()
	{
		return m_bReadOnly;
	}

	void CRichEditUI::SetReadOnly(bool bReadOnly)
	{
		m_bReadOnly = bReadOnly;
		if( m_pTwh ) m_pTwh->SetReadOnly(bReadOnly);
	}

	bool CRichEditUI::GetWordWrap()
	{
		return m_bWordWrap;
	}

	void CRichEditUI::SetWordWrap(bool bWordWrap)
	{
		m_bWordWrap = bWordWrap;
		if( m_pTwh ) m_pTwh->SetWordWrap(bWordWrap);
	}

	int CRichEditUI::GetFont()
	{
		return m_iFont;
	}

	void CRichEditUI::SetFont(int index)
	{
		m_iFont = index;
		if( m_pTwh ) {
			m_pTwh->SetFont(GetManager()->GetFont(m_iFont));
		}
	}

	void CRichEditUI::SetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
	{
		if( m_pTwh ) {
			LOGFONT lf = { 0 };
			::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
			_tcscpy(lf.lfFaceName, pStrFontName);
			lf.lfCharSet = DEFAULT_CHARSET;
			lf.lfHeight = -nSize;
			if( bBold ) lf.lfWeight += FW_BOLD;
			if( bUnderline ) lf.lfUnderline = TRUE;
			if( bItalic ) lf.lfItalic = TRUE;
			HFONT hFont = ::CreateFontIndirect(&lf);
			if( hFont == NULL ) return;
			m_pTwh->SetFont(hFont);
			::DeleteObject(hFont);
		}
	}

	LONG CRichEditUI::GetWinStyle()
	{
		return m_lTwhStyle;
	}

	void CRichEditUI::SetWinStyle(LONG lStyle)
	{
		m_lTwhStyle = lStyle;
	}

	DWORD CRichEditUI::GetTextColor()
	{
		return m_dwTextColor;
	}

	void CRichEditUI::SetTextColor(DWORD dwTextColor)
	{
		m_dwTextColor = dwTextColor;
		if( m_pTwh ) {
			m_pTwh->SetColor(dwTextColor);
		}
	}

	int CRichEditUI::GetLimitText()
	{
		return m_iLimitText;
	}

	void CRichEditUI::SetLimitText(int iChars)
	{
		m_iLimitText = iChars;
		if( m_pTwh ) {
			m_pTwh->LimitText(m_iLimitText);
		}
	}

	long CRichEditUI::GetTextLength(DWORD dwFlags) const
	{
		GETTEXTLENGTHEX textLenEx;
		textLenEx.flags = dwFlags;
#ifdef _UNICODE
		textLenEx.codepage = 1200;
#else
		textLenEx.codepage = CP_ACP;
#endif
		LRESULT lResult;
		TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0, &lResult);
		return (long)lResult;
	}

	CStdString CRichEditUI::GetText() const
	{
		long lLen = GetTextLength(GTL_DEFAULT);
		LPTSTR lpText = NULL;
		GETTEXTEX gt;
		gt.flags = GT_DEFAULT;
#ifdef _UNICODE
		gt.cb = sizeof(TCHAR) * (lLen + 1) ;
		gt.codepage = 1200;
		lpText = new TCHAR[lLen + 1];
		::ZeroMemory(lpText, (lLen + 1) * sizeof(TCHAR));
#else
		gt.cb = sizeof(TCHAR) * lLen * 2 + 1;
		gt.codepage = CP_ACP;
		lpText = new TCHAR[lLen * 2 + 1];
		::ZeroMemory(lpText, (lLen * 2 + 1) * sizeof(TCHAR));
#endif
		gt.lpDefaultChar = NULL;
		gt.lpUsedDefChar = NULL;
		TxSendMessage(EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)lpText, 0);
		CStdString sText(lpText);
		delete[] lpText;
		return sText;
	}

	void CRichEditUI::SetText(LPCTSTR pstrText)
	{
		m_sText = pstrText;
		if( !m_pTwh ) return;
		SetSel(0, -1);
		ReplaceSel(pstrText, FALSE);
	}

	bool CRichEditUI::GetModify() const
	{ 
		if( !m_pTwh ) return false;
		LRESULT lResult;
		TxSendMessage(EM_GETMODIFY, 0, 0, &lResult);
		return (BOOL)lResult == TRUE;
	}

	void CRichEditUI::SetModify(bool bModified) const
	{ 
		TxSendMessage(EM_SETMODIFY, bModified, 0, 0);
	}

	void CRichEditUI::GetSel(CHARRANGE &cr) const
	{ 
		TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0); 
	}

	void CRichEditUI::GetSel(long& nStartChar, long& nEndChar) const
	{
		CHARRANGE cr;
		TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0); 
		nStartChar = cr.cpMin;
		nEndChar = cr.cpMax;
	}

	int CRichEditUI::SetSel(CHARRANGE &cr)
	{ 
		LRESULT lResult;
		TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult); 
		return (int)lResult;
	}

	int CRichEditUI::SetSel(long nStartChar, long nEndChar)
	{
		CHARRANGE cr;
		cr.cpMin = nStartChar;
		cr.cpMax = nEndChar;
		LRESULT lResult;
		TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult); 
		return (int)lResult;
	}

	void CRichEditUI::ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo)
	{
#ifdef _UNICODE		
		TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText, 0); 
#else
		int iLen = _tcslen(lpszNewText);
		LPWSTR lpText = new WCHAR[iLen + 1];
		::ZeroMemory(lpText, (iLen + 1) * sizeof(WCHAR));
		::MultiByteToWideChar(CP_ACP, 0, lpszNewText, -1, (LPWSTR)lpText, iLen) ;
		TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpText, 0); 
		delete[] lpText;
#endif
	}

	void CRichEditUI::ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo)
	{
		TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText, 0); 
	}

	CStdString CRichEditUI::GetSelText() const
	{
		if( !m_pTwh ) return CStdString();
		CHARRANGE cr;
		cr.cpMin = cr.cpMax = 0;
		TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0);
		LPWSTR lpText = NULL;
		lpText = new WCHAR[cr.cpMax - cr.cpMin + 1];
		::ZeroMemory(lpText, (cr.cpMax - cr.cpMin + 1) * sizeof(WCHAR));
		TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpText, 0);
		CStdString sText;
		sText = (LPCWSTR)lpText;
		delete[] lpText;
		return sText;
	}

	int CRichEditUI::SetSelAll()
	{
		return SetSel(0, -1);
	}

	int CRichEditUI::SetSelNone()
	{
		return SetSel(-1, 0);
	}

	bool CRichEditUI::GetZoom(int& nNum, int& nDen) const
	{
		LRESULT lResult;
		TxSendMessage(EM_GETZOOM, (WPARAM)&nNum, (LPARAM)&nDen, &lResult);
		return (BOOL)lResult == TRUE;
	}

	bool CRichEditUI::SetZoom(int nNum, int nDen)
	{
		if (nNum < 0 || nNum > 64) return false;
		if (nDen < 0 || nDen > 64) return false;
		LRESULT lResult;
		TxSendMessage(EM_SETZOOM, nNum, nDen, &lResult);
		return (BOOL)lResult == TRUE;
	}

	bool CRichEditUI::SetZoomOff()
	{
		LRESULT lResult;
		TxSendMessage(EM_SETZOOM, 0, 0, &lResult);
		return (BOOL)lResult == TRUE;
	}

	WORD CRichEditUI::GetSelectionType() const
	{
		LRESULT lResult;
		TxSendMessage(EM_SELECTIONTYPE, 0, 0, &lResult);
		return (WORD)lResult;
	}

	bool CRichEditUI::GetAutoURLDetect() const
	{
		LRESULT lResult;
		TxSendMessage(EM_GETAUTOURLDETECT, 0, 0, &lResult);
		return (BOOL)lResult == TRUE;
	}

	bool CRichEditUI::SetAutoURLDetect(bool bAutoDetect)
	{
		LRESULT lResult;
		TxSendMessage(EM_AUTOURLDETECT, bAutoDetect, 0, &lResult);
		return (BOOL)lResult == FALSE;
	}

	DWORD CRichEditUI::GetEventMask() const
	{
		LRESULT lResult;
		TxSendMessage(EM_GETEVENTMASK, 0, 0, &lResult);
		return (DWORD)lResult;
	}

	DWORD CRichEditUI::SetEventMask(DWORD dwEventMask)
	{
		LRESULT lResult;
		TxSendMessage(EM_SETEVENTMASK, 0, dwEventMask, &lResult);
		return (DWORD)lResult;
	}

	CStdString CRichEditUI::GetTextRange(long nStartChar, long nEndChar) const
	{
		TEXTRANGEW tr = { 0 };
		tr.chrg.cpMin = nStartChar;
		tr.chrg.cpMax = nEndChar;
		LPWSTR lpText = NULL;
		lpText = new WCHAR[nEndChar - nStartChar + 1];
		::ZeroMemory(lpText, (nEndChar - nStartChar + 1) * sizeof(WCHAR));
		tr.lpstrText = lpText;
		TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr, 0);
		CStdString sText;
		sText = (LPCWSTR)lpText;
		delete[] lpText;
		return sText;
	}

	void CRichEditUI::HideSelection(bool bHide, bool bChangeStyle)
	{
		TxSendMessage(EM_HIDESELECTION, bHide, bChangeStyle, 0);
	}

	void CRichEditUI::ScrollCaret()
	{
		TxSendMessage(EM_SCROLLCARET, 0, 0, 0);
	}

	int CRichEditUI::InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo)
	{
		int nRet = SetSel(nInsertAfterChar, nInsertAfterChar);
		ReplaceSel(lpstrText, bCanUndo);
		return nRet;
	}

	int CRichEditUI::AppendText(LPCTSTR lpstrText, bool bCanUndo)
	{
		int nRet = SetSel(-1, -1);
		ReplaceSel(lpstrText, bCanUndo);
		return nRet;
	}

	DWORD CRichEditUI::GetDefaultCharFormat(CHARFORMAT2 &cf) const
	{
		cf.cbSize = sizeof(CHARFORMAT2);
		LRESULT lResult;
		TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
		return (DWORD)lResult;
	}

	bool CRichEditUI::SetDefaultCharFormat(CHARFORMAT2 &cf)
	{
		if( !m_pTwh ) return false;
		cf.cbSize = sizeof(CHARFORMAT2);
		LRESULT lResult;
		TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
		if( (BOOL)lResult == TRUE ) {
			CHARFORMAT2W cfw;
			cfw.cbSize = sizeof(CHARFORMAT2W);
			TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cfw, 0);
			m_pTwh->SetCharFormat(cfw);
			return true;
		}
		return false;
	}

	DWORD CRichEditUI::GetSelectionCharFormat(CHARFORMAT2 &cf) const
	{
		cf.cbSize = sizeof(CHARFORMAT2);
		LRESULT lResult;
		TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lResult);
		return (DWORD)lResult;
	}

	bool CRichEditUI::SetSelectionCharFormat(CHARFORMAT2 &cf)
	{
		if( !m_pTwh ) return false;
		cf.cbSize = sizeof(CHARFORMAT2);
		LRESULT lResult;
		TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lResult);
		return (BOOL)lResult == TRUE;
	}

	bool CRichEditUI::SetWordCharFormat(CHARFORMAT2 &cf)
	{
		if( !m_pTwh ) return false;
		cf.cbSize = sizeof(CHARFORMAT2);
		LRESULT lResult;
		TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION|SCF_WORD, (LPARAM)&cf, &lResult); 
		return (BOOL)lResult == TRUE;
	}

	DWORD CRichEditUI::GetParaFormat(PARAFORMAT2 &pf) const
	{
		pf.cbSize = sizeof(PARAFORMAT2);
		LRESULT lResult;
		TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
		return (DWORD)lResult;
	}

	bool CRichEditUI::SetParaFormat(PARAFORMAT2 &pf)
	{
		if( !m_pTwh ) return false;
		pf.cbSize = sizeof(PARAFORMAT2);
		LRESULT lResult;
		TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
		if( (BOOL)lResult == TRUE ) {
			m_pTwh->SetParaFormat(pf);
			return true;
		}
		return false;
	}

	bool CRichEditUI::Redo()
	{ 
		if( !m_pTwh ) return false;
		LRESULT lResult;
		TxSendMessage(EM_REDO, 0, 0, &lResult);
		return (BOOL)lResult == TRUE; 
	}

	bool CRichEditUI::Undo()
	{ 
		if( !m_pTwh ) return false;
		LRESULT lResult;
		TxSendMessage(EM_UNDO, 0, 0, &lResult);
		return (BOOL)lResult == TRUE; 
	}

	void CRichEditUI::Clear()
	{ 
		TxSendMessage(WM_CLEAR, 0, 0, 0); 
		RemoveAll();
	}

	void CRichEditUI::ClearAll()
	{
		SetSelAll();
		SetText(_T(""));
	}

	void CRichEditUI::Copy()
	{ 
		TxSendMessage(WM_COPY, 0, 0, 0); 
	}

	void CRichEditUI::Cut()
	{ 
		TxSendMessage(WM_CUT, 0, 0, 0); 
	}

	void CRichEditUI::Paste()
	{ 
		TxSendMessage(WM_PASTE, 0, 0, 0); 
	}

	BOOL CRichEditUI::OnPaste()
	{
		return TRUE;
	}

	int CRichEditUI::GetLineCount() const
	{ 
		if( !m_pTwh ) return 0;
		LRESULT lResult;
		TxSendMessage(EM_GETLINECOUNT, 0, 0, &lResult);
		return (int)lResult; 
	}

	CStdString CRichEditUI::GetLine(int nIndex, int nMaxLength) const
	{
		LPWSTR lpText = NULL;
		lpText = new WCHAR[nMaxLength + 1];
		::ZeroMemory(lpText, (nMaxLength + 1) * sizeof(WCHAR));
		*(LPWORD)lpText = (WORD)nMaxLength;
		TxSendMessage(EM_GETLINE, nIndex, (LPARAM)lpText, 0);
		CStdString sText;
		sText = (LPCWSTR)lpText;
		delete[] lpText;
		return sText;
	}

	int CRichEditUI::LineIndex(int nLine) const
	{
		LRESULT lResult;
		TxSendMessage(EM_LINEINDEX, nLine, 0, &lResult);
		return (int)lResult;
	}

	int CRichEditUI::LineLength(int nLine) const
	{
		LRESULT lResult;
		TxSendMessage(EM_LINELENGTH, nLine, 0, &lResult);
		return (int)lResult;
	}

	bool CRichEditUI::LineScroll(int nLines, int nChars)
	{
		LRESULT lResult;
		TxSendMessage(EM_LINESCROLL, nChars, nLines, &lResult);
		return (BOOL)lResult == TRUE;
	}

    bool CRichEditUI::ScrollEndLine()
    {
        LRESULT lResult;
        TxSendMessage(WM_VSCROLL, SB_BOTTOM, 0L, &lResult);
        return (BOOL)lResult == TRUE;
    }

	CDuiPoint CRichEditUI::GetCharPos(long lChar) const
	{ 
		CDuiPoint pt; 
		TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)lChar, 0); 
		return pt;
	}

	long CRichEditUI::LineFromChar(long nIndex) const
	{ 
		if( !m_pTwh ) return 0L;
		LRESULT lResult;
		TxSendMessage(EM_EXLINEFROMCHAR, 0, nIndex, &lResult);
		return (long)lResult;
	}

	CDuiPoint CRichEditUI::PosFromChar(UINT nChar) const
	{ 
		POINTL pt; 
		TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, nChar, 0); 
		return CDuiPoint(pt.x, pt.y); 
	}

	int CRichEditUI::CharFromPos(CDuiPoint pt) const
	{ 
		POINTL ptl = {pt.x, pt.y}; 
		if( !m_pTwh ) return 0;
		LRESULT lResult;
		TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl, &lResult);
		return (int)lResult; 
	}
	int	 CRichEditUI::getEndCharPos( void )
	{
		POINT pt = { 0,0 };
		EDIT_CHARPOS tgCharPos;
		if( FALSE == GetCaretPos(&pt) )
			return -1;
		tgCharPos.nVal = CharFromPos(pt);
		return tgCharPos.wIndex-1;
	}

	void CRichEditUI::EmptyUndoBuffer()
	{ 
		TxSendMessage(EM_EMPTYUNDOBUFFER, 0, 0, 0); 
	}

	UINT CRichEditUI::SetUndoLimit(UINT nLimit)
	{ 
		if( !m_pTwh ) return 0;
		LRESULT lResult;
		TxSendMessage(EM_SETUNDOLIMIT, (WPARAM) nLimit, 0, &lResult);
		return (UINT)lResult; 
	}

	long CRichEditUI::StreamIn(int nFormat, EDITSTREAM &es)
	{ 
		if( !m_pTwh ) return 0L;
		LRESULT lResult;
		TxSendMessage(EM_STREAMIN, nFormat, (LPARAM)&es, &lResult);
		return (long)lResult;
	}

	long CRichEditUI::StreamOut(int nFormat, EDITSTREAM &es)
	{ 
		if( !m_pTwh ) return 0L;
		LRESULT lResult;
		TxSendMessage(EM_STREAMOUT, nFormat, (LPARAM)&es, &lResult);
		return (long)lResult; 
	}

	void CRichEditUI::DoInit()
	{
		CREATESTRUCT cs;
		cs.style = m_lTwhStyle;
		cs.x = 0;
		cs.y = 0;
		cs.cy = 0;
		cs.cx = 0;
		cs.lpszName = m_sText.GetData();
		CreateHost(this, &cs, &m_pTwh);
		if( m_pTwh ) {
			m_pTwh->SetTransparent(TRUE);
			LRESULT lResult;
			m_pTwh->GetTextServices()->TxSendMessage(EM_SETLANGOPTIONS, 0, 0, &lResult);
			m_pTwh->OnTxInPlaceActivate(NULL);
			m_pManager->AddMessageFilter(this);
		}
        // �����о�
        PARAFORMAT2 pf2;
        memset(&pf2, 0, sizeof(pf2));
        pf2.cbSize = sizeof(pf2);
        pf2.dwMask = PFM_LINESPACING;
        pf2.dyLineSpacing = 3;
        SetParaFormat(pf2);
	}

	HRESULT	CRichEditUI::TxGetText(BSTR *pbstrText)
	{
		if( m_pTwh ) {
			return m_pTwh->GetTextServices()->TxGetText(pbstrText);
		}
		return S_FALSE;

	}
	HRESULT CRichEditUI::TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const
	{
		if( m_pTwh ) {
			if( msg == WM_KEYDOWN && TCHAR(wparam) == VK_RETURN ) {
				if( (!m_bWantReturn && ::GetKeyState(VK_CONTROL) >= 0) || (::GetKeyState(VK_CONTROL) < 0 && !m_bWantCtrlReturn) ) {
					if( m_pManager != NULL ) m_pManager->SendNotify((CControlUI*)this, _T("return"));
					return S_OK;
				}
			}
			return m_pTwh->GetTextServices()->TxSendMessage(msg, wparam, lparam, plresult);
		}
		return S_FALSE;
	}

	IDropTarget* CRichEditUI::GetTxDropTarget()
	{
		IDropTarget *pdt = NULL;
		if( m_pTwh->GetTextServices()->TxGetDropTarget(&pdt) == NOERROR ) return pdt;
		return NULL;
	}

	bool CRichEditUI::OnTxViewChanged()
	{
		return true;
	}

	void CRichEditUI::OnTxNotify(DWORD iNotify, void *pv)
	{
	}

	// ���з�rich��ʽ��richedit��һ��������bug�������һ���ǿ���ʱ��LineDown��SetScrollPos�޷����������
	// ����iPos����Ϊ���������bug
	void CRichEditUI::SetScrollPos(SIZE szPos)
	{
		int cx = 0;
		int cy = 0;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
			m_pVerticalScrollBar->SetScrollPos(szPos.cy);
			cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
		}
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
			m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
			cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
		}
		if( cy != 0 ) {
			int iPos = 0;
			if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
				iPos = m_pVerticalScrollBar->GetScrollPos();
			WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, m_pVerticalScrollBar->GetScrollPos());
			TxSendMessage(WM_VSCROLL, wParam, 0L, 0);
			if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
				if( cy > 0 && m_pVerticalScrollBar->GetScrollPos() <= iPos )
					m_pVerticalScrollBar->SetScrollPos(iPos);
			}
		}
		if( cx != 0 ) {
			WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, m_pHorizontalScrollBar->GetScrollPos());
			TxSendMessage(WM_HSCROLL, wParam, 0L, 0);
		}
	}

	void CRichEditUI::LineUp()
	{
		TxSendMessage(WM_VSCROLL, SB_LINEUP, 0L, 0);
	}

	void CRichEditUI::LineDown()
	{
		int iPos = 0;
		if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
			iPos = m_pVerticalScrollBar->GetScrollPos();
		TxSendMessage(WM_VSCROLL, SB_LINEDOWN, 0L, 0);
		if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			if( m_pVerticalScrollBar->GetScrollPos() <= iPos )
				m_pVerticalScrollBar->SetScrollPos(m_pVerticalScrollBar->GetScrollRange());
		}
	}

	void CRichEditUI::PageUp()
	{
		TxSendMessage(WM_VSCROLL, SB_PAGEUP, 0L, 0);
	}

	void CRichEditUI::PageDown()
	{
		TxSendMessage(WM_VSCROLL, SB_PAGEDOWN, 0L, 0);
	}

	void CRichEditUI::HomeUp()
	{
		TxSendMessage(WM_VSCROLL, SB_TOP, 0L, 0);
	}

	void CRichEditUI::EndDown()
	{
		TxSendMessage(WM_VSCROLL, SB_BOTTOM, 0L, 0);
	}

	void CRichEditUI::LineLeft()
	{
		TxSendMessage(WM_HSCROLL, SB_LINELEFT, 0L, 0);
	}

	void CRichEditUI::LineRight()
	{
		TxSendMessage(WM_HSCROLL, SB_LINERIGHT, 0L, 0);
	}

	void CRichEditUI::PageLeft()
	{
		TxSendMessage(WM_HSCROLL, SB_PAGELEFT, 0L, 0);
	}

	void CRichEditUI::PageRight()
	{
		TxSendMessage(WM_HSCROLL, SB_PAGERIGHT, 0L, 0);
	}

	void CRichEditUI::HomeLeft()
	{
		TxSendMessage(WM_HSCROLL, SB_LEFT, 0L, 0);
	}

	void CRichEditUI::EndRight()
	{
		TxSendMessage(WM_HSCROLL, SB_RIGHT, 0L, 0);
	}

	ITextServices* CRichEditUI::GetTextServices()
	{
		if(m_pTwh)
			return m_pTwh->GetTextServices();
        return NULL;
	}

	void CRichEditUI::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK ) 
		{
			int a = 3;
		}
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETCURSOR && IsEnabled() )
		{
			if( m_pTwh && m_pTwh->DoSetCursor(NULL, &event.ptMouse) ) {
				return;
			}
		}
		if( event.Type == UIEVENT_SETFOCUS ) {
			if( m_pTwh ) {
				m_pTwh->OnTxInPlaceActivate(NULL);
				m_pTwh->GetTextServices()->TxSendMessage(WM_SETFOCUS, 0, 0, 0);
			}
		}
		if( event.Type == UIEVENT_KILLFOCUS )  {
			if( m_pTwh ) {
				m_pTwh->OnTxInPlaceActivate(NULL);
				m_pTwh->GetTextServices()->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
			}
		}
		if( event.Type == UIEVENT_TIMER ) {
			if( m_pTwh ) {
				m_pTwh->GetTextServices()->TxSendMessage(WM_TIMER, event.wParam, event.lParam, 0);
			} 
		}
		if( event.Type == UIEVENT_SCROLLWHEEL ) {
			if( (event.wKeyState & MK_CONTROL) != 0  ) {
				return;
			}
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK ) 
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE ) 
		{
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP ) 
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			return;
		}
		if( event.Type > UIEVENT__KEYBEGIN && event.Type < UIEVENT__KEYEND )
		{
			return;
		}
		CContainerUI::DoEvent(event);
	}

	SIZE CRichEditUI::EstimateSize(SIZE szAvailable)
	{
		//return CDuiSize(m_rcItem); // ���ַ�ʽ�ڵ�һ�����ô�С֮��ʹ�С������
		return CContainerUI::EstimateSize(szAvailable);
	}

	void CRichEditUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		rc = m_rcItem;

		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;
		bool bVScrollBarVisiable = false;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			bVScrollBarVisiable = true;
			rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		}
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
		}

		if( m_pTwh ) {
			m_pTwh->SetClientRect(&rc);
			if( bVScrollBarVisiable && (!m_pVerticalScrollBar->IsVisible() || m_bVScrollBarFixing) ) {
				LONG lWidth = rc.right - rc.left + m_pVerticalScrollBar->GetFixedWidth();
				LONG lHeight = 0;
				SIZEL szExtent = { -1, -1 };
				m_pTwh->GetTextServices()->TxGetNaturalSize(
					DVASPECT_CONTENT, 
					GetManager()->GetPaintDC(), 
					NULL,
					NULL,
					TXTNS_FITTOCONTENT,
					&szExtent,
					&lWidth,
					&lHeight);
				if( lHeight > rc.bottom - rc.top ) {
					m_pVerticalScrollBar->SetVisible(true);
					m_pVerticalScrollBar->SetScrollPos(0);
					m_bVScrollBarFixing = true;
				}
				else {
					if( m_bVScrollBarFixing ) {
						m_pVerticalScrollBar->SetVisible(false);
						m_bVScrollBarFixing = false;
					}
				}
			}
		}

		if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() ) {
			RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom};
			m_pVerticalScrollBar->SetPos(rcScrollBarPos);
		}
		if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
			RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight()};
			m_pHorizontalScrollBar->SetPos(rcScrollBarPos);
		}

		for( int it = 0; it < m_items.GetSize(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it);
			}
			else {
				pControl->SetPos(rc); // ���з�float�ӿؼ��Ŵ������ͻ���
			}
		}
	}

	void CRichEditUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		AdjustSubCtrlUIPos();

		RECT rcTemp = { 0 };
		DWORD dwDcType = ::GetObjectType(hDC);
		if( (dwDcType != OBJ_DC) && (dwDcType != OBJ_MEMDC) )
		{
#if defined(_DEBUG)
			OutputDebugString(_T("\r\nsuport Device context and Memory DC\r\n"));
#endif
			return ;
		}
		if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return;

		CRenderClip clip;
		CRenderClip::GenerateClip(hDC, rcTemp, clip);
		CControlUI::DoPaint(hDC, rcPaint);

		if( m_pTwh ) {
			RECT rc;
			m_pTwh->GetControlRect(&rc);
			// Remember wparam is actually the hdc and lparam is the update
			// rect because this message has been preprocessed by the window.
			m_pTwh->GetTextServices()->TxDraw(
				DVASPECT_CONTENT,  		// Draw Aspect
				/*-1*/0,				// Lindex
				NULL,					// Info for drawing optimazation
				NULL,					// target device information
				hDC,			        // Draw device HDC
				NULL, 				   	// Target device HDC
				(RECTL*)&rc,			// Bounding client rectangle
				NULL, 		            // Clipping rectangle for metafiles
				(RECT*)&rcPaint,		// Update rectangle
				NULL, 	   				// Call back function
				NULL,					// Call back parameter
				0);				        // What view of the object
			if( m_bVScrollBarFixing ) {
				LONG lWidth = rc.right - rc.left + m_pVerticalScrollBar->GetFixedWidth();
				LONG lHeight = 0;
				SIZEL szExtent = { -1, -1 };
				m_pTwh->GetTextServices()->TxGetNaturalSize(
					DVASPECT_CONTENT, 
					GetManager()->GetPaintDC(), 
					NULL,
					NULL,
					TXTNS_FITTOCONTENT,
					&szExtent,
					&lWidth,
					&lHeight);
				if( lHeight <= rc.bottom - rc.top ) {
					NeedUpdate();
				}
			}
		}

		if( m_items.GetSize() > 0 ) {
			RECT rc = m_rcItem;
			rc.left += m_rcInset.left;
			rc.top += m_rcInset.top;
			rc.right -= m_rcInset.right;
			rc.bottom -= m_rcInset.bottom;
			if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
			if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

			if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
				for( int it = 0; it < m_items.GetSize(); it++ ) {
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if( !pControl->IsVisible() ) continue;
					if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
					if( pControl ->IsFloat() ) {
						if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
						pControl->DoPaint(hDC, rcPaint);
					}
				}
			}
			else {
				CRenderClip childClip;
				CRenderClip::GenerateClip(hDC, rcTemp, childClip);
				for( int it = 0; it < m_items.GetSize(); it++ ) {
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if( !pControl->IsVisible() ) continue;
					if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
					if( pControl ->IsFloat() ) {
						if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
						CRenderClip::UseOldClipBegin(hDC, childClip);
						pControl->DoPaint(hDC, rcPaint);
						CRenderClip::UseOldClipEnd(hDC, childClip);
					}
					else {
						if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
						pControl->DoPaint(hDC, rcPaint);
					}
				}
			}
		}

		if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() ) {
			if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()) ) {
				m_pVerticalScrollBar->DoPaint(hDC, rcPaint);
			}
		}

		if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
			if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()) ) {
				m_pHorizontalScrollBar->DoPaint(hDC, rcPaint);
			}
		}
	}

	void CRichEditUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("vscrollbar")) == 0 ) {
			if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_DISABLENOSCROLL | WS_VSCROLL;
		}
		if( _tcscmp(pstrName, _T("autovscroll")) == 0 ) {
			if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_AUTOVSCROLL;
		}
		else if( _tcscmp(pstrName, _T("hscrollbar")) == 0 ) {
			if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_DISABLENOSCROLL | WS_HSCROLL;
		}
		if( _tcscmp(pstrName, _T("autohscroll")) == 0 ) {
			if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_AUTOHSCROLL;
		}
		else if( _tcscmp(pstrName, _T("wanttab")) == 0 ) {
			SetWantTab(_tcscmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcscmp(pstrName, _T("wantreturn")) == 0 ) {
			SetWantReturn(_tcscmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcscmp(pstrName, _T("wantctrlreturn")) == 0 ) {
			SetWantCtrlReturn(_tcscmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcscmp(pstrName, _T("rich")) == 0 ) {
			SetRich(_tcscmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcscmp(pstrName, _T("multiline")) == 0 ) {
			if( _tcscmp(pstrValue, _T("false")) == 0 ) m_lTwhStyle &= ~ES_MULTILINE;
		}
		else if( _tcscmp(pstrName, _T("readonly")) == 0 ) {
			if( _tcscmp(pstrValue, _T("true")) == 0 ) { m_lTwhStyle |= ES_READONLY; m_bReadOnly = true; }
		}
		else if( _tcscmp(pstrName, _T("numberonly")) == 0 ) {
			if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_NUMBER;
		}
		else if( _tcscmp(pstrName, _T("password")) == 0 ) {
			if( _tcscmp(pstrValue, _T("true")) == 0 ) m_lTwhStyle |= ES_PASSWORD;
		}
		else if( _tcscmp(pstrName, _T("align")) == 0 ) {
			if( _tcsstr(pstrValue, _T("left")) != NULL ) {
				m_lTwhStyle &= ~(ES_CENTER | ES_RIGHT);
				m_lTwhStyle |= ES_LEFT;
			}
			if( _tcsstr(pstrValue, _T("center")) != NULL ) {
				m_lTwhStyle &= ~(ES_LEFT | ES_RIGHT);
				m_lTwhStyle |= ES_CENTER;
			}
			if( _tcsstr(pstrValue, _T("right")) != NULL ) {
				m_lTwhStyle &= ~(ES_LEFT | ES_CENTER);
				m_lTwhStyle |= ES_RIGHT;
			}
		}
		else if( _tcscmp(pstrName, _T("maxchar")) == 0 ) SetLimitText(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("font")) == 0 ) SetFont(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("textcolor")) == 0 ) {
			while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetTextColor(clrColor);
		}
		else CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	LRESULT CRichEditUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
	{
		if( !IsVisible() || !IsEnabled() ) return 0;
		if( !IsMouseEnabled() && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) return 0;
		if( uMsg == WM_MOUSEWHEEL && (LOWORD(wParam) & MK_CONTROL) == 0 ) return 0;

		bool bWasHandled = true;
		if( (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR )
		{
			if( !m_pTwh->IsCaptured() )
			{
				switch (uMsg)
				{
				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP:
				case WM_LBUTTONDBLCLK:
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
					{
						POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
						CControlUI* pHover = GetManager()->FindControl(pt);
						if(pHover != this)
						{
							bWasHandled = false;
							return 0;
						}
					}
					break;
				}
			}
			// Mouse message only go when captured or inside rect
			DWORD dwHitResult = m_pTwh->IsCaptured() ? HITRESULT_HIT : HITRESULT_OUTSIDE;
			if( dwHitResult == HITRESULT_OUTSIDE ) {
				RECT rc;
				m_pTwh->GetControlRect(&rc);
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				if( uMsg == WM_MOUSEWHEEL ) ::ScreenToClient(GetManager()->GetPaintWindow(), &pt);
				if( ::PtInRect(&rc, pt) && !GetManager()->IsCaptured() ) dwHitResult = HITRESULT_HIT;
			}
			if( dwHitResult != HITRESULT_HIT ) return 0;
			if( uMsg == WM_SETCURSOR ) bWasHandled = false;
			else if( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK || uMsg == WM_RBUTTONDOWN ) {
				SetFocus();
				if (uMsg == WM_LBUTTONDBLCLK)
				{
					POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
					OnDBClick(pt);
				}
			}
		}
#ifdef _UNICODE
		else if( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST ) {
#else
		else if( (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) || uMsg == WM_CHAR || uMsg == WM_IME_CHAR ) {
#endif
			if( !IsFocused() ) return 0;
		}
		else if( uMsg == WM_CONTEXTMENU ) {
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			::ScreenToClient(GetManager()->GetPaintWindow(), &pt);
			CControlUI* pHover = GetManager()->FindControl(pt);
			if(pHover != this) {
				bWasHandled = false;
				return 0;
			}
		}
		else
		{
			switch( uMsg ) {
	case WM_HELP:
		bWasHandled = false;
		break;
	default:
		return 0;
			}
		}
		LRESULT lResult = 0;
		HRESULT Hr = TxSendMessage(uMsg, wParam, lParam, &lResult);
		if( Hr == S_OK ) bHandled = bWasHandled;
		else if( (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) || uMsg == WM_CHAR || uMsg == WM_IME_CHAR )
			bHandled = bWasHandled;
		else if( uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) {
			if( m_pTwh->IsCaptured() ) bHandled = bWasHandled;
		}
		return lResult;
	}

	int CRichEditUI::InsertText(LPCTSTR text , COLORREF cr )
	{
		SetSel(-1, -1);
		CHARFORMAT2 cf ;
		PARAFORMAT2 pfNew;

		GetParaFormat(pfNew);
		pfNew.dwMask =pfNew.dwMask = PFM_LINESPACING|PFM_OFFSETINDENT|PFM_RIGHTINDENT|PFM_OFFSET | PFM_STARTINDENT|PFE_TABLE;// PFM_OFFSET | PFM_STARTINDENT;
		pfNew.dxStartIndent = 60;
		pfNew.dxOffset = 0;
		SetParaFormat(pfNew);

		memset( &cf , 0 , sizeof( cf ) ) ;
		cf.cbSize = sizeof( cf ) ;
		cf.dwMask = CFM_EFFECTS | CFM_ALL;
		cf.yHeight = 200;
		cf.yOffset = 10 ;
		_tcscpy_s( cf.szFaceName , _T("����") ) ;
		cf.crTextColor = cr ;

		//set selected char color .
		SetSelectionCharFormat( cf );
		// 	SetSel(-1, -1);
		// 	AppendText(text);
		// 	return;
		SETTEXTEX st;
		st.flags = ST_SELECTION;
		st.codepage = 1200; //CP_UNICODE (1200)
		LRESULT lResult = 0;
		TxSendMessage(EM_SETTEXTEX, (WPARAM)&st, (LPARAM)text,&lResult);

		return lResult;

	}

	int CRichEditUI::InsertText(LPCTSTR text , CHARFORMAT2 cf, BOOL bParse )
	{

		SetSel(-1, -1);
		CHARRANGE cr1;
		GetSel(cr1);
		//set selected char color .
		SetSelectionCharFormat( cf );

		PARAFORMAT2 pfDefault, pfNew;
		if(!bParse)
		{
			GetParaFormat(pfDefault);
			pfNew = pfDefault;
			//pfNew.bLineSpacingRule = 4;
			//pfNew.dyLineSpacing = 30 * (9 + 3);
			pfNew.dwMask = PFM_LINESPACING|PFM_OFFSETINDENT|PFM_RIGHTINDENT|PFM_OFFSET | PFM_STARTINDENT|PFE_TABLE;
			pfNew.dxStartIndent = 360;
			pfNew.dxOffset = 0;
			pfNew.dxRightIndent = 100;
			SetParaFormat(pfNew);
		}

		// 	SetSel(-1, -1);
		// 	AppendText(text);
		// 	return;

		//replace the select char .
		//modify by wanghaiyang. ReplaceSel can select the text mode either use rtf mode or not.
		//if you insert such as "{/rtf" text, it will display wrongly.
		SETTEXTEX st;
		st.flags = ST_SELECTION;
		st.codepage = 1200; //CP_UNICODE (1200)

		LRESULT lResult = 0;
		TxSendMessage(EM_SETTEXTEX, (WPARAM)&st, (LPARAM)text,&lResult);



		if(!bParse)
		{
			SetParaFormat(pfDefault);
			CHARRANGE cr;
			GetSel(cr);
			SetSel(cr1.cpMax, cr.cpMax);

			//�ж��Ƿ��г���
			for(int i = 0; i < 6; i ++)
			{

			}
			SetSel(-1, -1);
		}
		return lResult;
		//ReplaceSel( text ) ;
	}


	IRichEditOle* CRichEditUI::GetIRichEditOle()
	{
		IRichEditOle *pRichItem = NULL;
		TxSendMessage( EM_GETOLEINTERFACE, 0, (LPARAM)&pRichItem, 0);
		return pRichItem;
	}

	void CRichEditUI::clearAllOleObj( void )
	{
		ReleaseOleObject	cClearOle(this,false);
		IRichEditOle* pReOle = GetIRichEditOle();
		if( pReOle )
		{
			EnumAllOleObject(pReOle,&cClearOle);
			pReOle->Release();
		}
	}
	BOOL CRichEditUI::SetOLECallback(IRichEditOleCallback* pCallback)
	{
		TxSendMessage(EM_SETOLECALLBACK, 0, (LPARAM)pCallback, 0);
		return TRUE;
	}

	BOOL CRichEditUI::InsertOleImage(LPCTSTR szFilePath,DWORD dwUserData,long lPos, int faceid)
	{
		IRichEditOle *pRichEditOle = GetIRichEditOle();
		OleLib::CImageOleObject*	pOleObject = 0;
		if (pRichEditOle && szFilePath != NULL )
		{
			pOleObject = OleLib::CImageOleObject::InsertImage(this,GetManager()->GetPaintWindow(),
				pRichEditOle,szFilePath,dwUserData, faceid ,lPos);
			pRichEditOle->Release();
			pRichEditOle = NULL;
			if( pOleObject == 0)
				return FALSE;
			if( m_pTwh )
			{
				RichEditEvent* pEvenSink = 0;
				if( m_pTwh )
				{
					if( pEvenSink = m_pTwh->getEventSink() )
						pOleObject->RegisterUserEventSink(pEvenSink);
				}
			}
			return TRUE;
		}
		return FALSE;
	}
	BOOL CRichEditUI::InsertControlUI(const CControlUI *pUI,const SIZE& sz,long lPos,bool bClick)
	{
		IRichEditOle *pRichEditOle = GetIRichEditOle();
		char* pNewGuid = 0;
		CControlUI *pCastUI = const_cast<CControlUI*>(pUI);
		OleLib::COleControlUi* pControlUi = 0;
		if (pRichEditOle && pUI != NULL )
		{
			//pCastUI->SetManager(GetManager(),this);
			this->Add(pCastUI);
			pControlUi = OleLib::COleControlUi::InsertOleControlUi(this,GetManager()->GetPaintWindow(),
				pRichEditOle,pUI,sz,0,lPos);
			pRichEditOle->Release();
			pRichEditOle = NULL;
			if( pControlUi == 0 )
				return FALSE;
			if( m_pTwh )
			{
				RichEditEvent* pEvenSink = 0;
				if( m_pTwh )
				{
					if( pEvenSink = m_pTwh->getEventSink() )
						pControlUi->RegisterUserEventSink(pEvenSink);
				}
			}
			if( bClick )
			{
				CHARFORMAT2 cf2;
				DWORD dwMask = 0;
				ZeroMemory(&cf2, sizeof(CHARFORMAT2));//
				cf2.cbSize		=	sizeof(CHARFORMAT2);
				cf2.dwMask		=	CFM_LINK;
				cf2.dwEffects	|=	CFE_LINK;
				if( lPos == -1 )
					lPos = getEndCharPos();
				SetSel(lPos,lPos+1);
				SetSelectionCharFormat(cf2);
				SetSel(-1, -1);
				dwMask = GetEventMask();
				if( (dwMask & ENM_LINK) != ENM_LINK )
				{
					dwMask |= ENM_LINK;
					SetEventMask(dwMask);
				}
			}
			return TRUE;
		}
		return FALSE;

	}

	void CRichEditUI::AdjustSubCtrlUIPos()
	{
		CComPtr<IRichEditOle> role;
		role = GetIRichEditOle();
		int nObjs = role->GetObjectCount();

		std::vector<CControlUI*> toRemoveCtrlUI;
		int nItemNum = m_items.GetSize();
		toRemoveCtrlUI.reserve(nItemNum);
		for ( int n = 0; n < nItemNum; n++ )
		{
			toRemoveCtrlUI.push_back(static_cast<CControlUI*>(m_items.GetAt(n)));
		}

		for (int i = 0; i < nObjs; i++)
		{
			REOBJECT reob = {sizeof(reob)};
			role->GetObject(i, &reob ,REO_GETOBJ_POLEOBJ);
			if( reob.poleobj == NULL ) continue;

			CControlUI *pCtrlUI = GetInternalControlUI(reob.poleobj);
			if ( pCtrlUI == NULL ) continue;

			std::vector<CControlUI*>::iterator it = std::find(toRemoveCtrlUI.begin(), toRemoveCtrlUI.end(), pCtrlUI);
			if ( it != toRemoveCtrlUI.end() )
				toRemoveCtrlUI.erase(it);

			//  The unit of measure is 0.01 millimeters(HIMETRIC ).
			SIZEL szPixel;
			AtlHiMetricToPixel(&reob.sizel, &szPixel);

			CDuiPoint pt = GetCharPos(reob.cp); // EM_POSFROMCHAR
			RECT rc = {pt.x, pt.y, pt.x + szPixel.cx, pt.y + szPixel.cy};
			RECT rcOld = pCtrlUI->GetPos();
			if ( !EqualRect(&rc, &rcOld) )
				pCtrlUI->SetPos(rc);
			//CXNFileTransferUI *pFileTransUI = (CXNFileTransferUI*)pCtrlUI->GetInterface(filetrans::strFileTransInterfaceName);
		}

		std::vector<CControlUI*>::iterator it;
		std::vector<CControlUI*>::iterator end = toRemoveCtrlUI.end();
		for ( it = toRemoveCtrlUI.begin(); it != end; ++it )
		{
			Remove(*it);
		}
	}

	BOOL CRichEditUI::InsertThumbnail(LPCTSTR szFilePath,long lPos,SIZE& sz,bool bClick)
	{
		IRichEditOle *pRichEditOle = GetIRichEditOle();
		LPTSTR pNewGuid = 0;
		OleLib::CImageOleObject*	pOleObject = 0;
		if (pRichEditOle && szFilePath != NULL )
		{
			CreateNewGUID(&pNewGuid);
			pOleObject = OleLib::CImageOleObject::InsertThumbnail(this,GetManager()->GetPaintWindow(),
				pRichEditOle,szFilePath,(DWORD)pNewGuid,sz,lPos);
			pRichEditOle->Release();
			pRichEditOle = NULL;
			if( pOleObject == 0 )
				return FALSE;
			if( m_pTwh )
			{
				RichEditEvent* pEvenSink = 0;
				if( m_pTwh )
				{
					if( pEvenSink = m_pTwh->getEventSink() )
						pOleObject->RegisterUserEventSink(pEvenSink);
				}
			}
			if( bClick )
			{
				CHARFORMAT2 cf2;
				DWORD dwMask = 0;
				ZeroMemory(&cf2, sizeof(CHARFORMAT2));//
				cf2.cbSize		=	sizeof(CHARFORMAT2);
				cf2.dwMask		=	CFM_LINK;
				cf2.dwEffects	|=	CFE_LINK;
				if( lPos == -1 )
					lPos = getEndCharPos();		
				SetSel(lPos,lPos+1);
				SetSelectionCharFormat(cf2);
				SetSel(-1, -1);
				dwMask = GetEventMask();
				if( (dwMask & ENM_LINK) != ENM_LINK )
				{
					dwMask |= ENM_LINK;
					SetEventMask(dwMask);
				}

			}
			return TRUE;
		}
		return FALSE;
	}


	BOOL CRichEditUI::InsertGif(LPCTSTR szFilePath,long lPos,bool bClick)
	{
		return InsertImage(szFilePath , lPos , bClick);
	}

	BOOL CRichEditUI::InsertThumbnailImage(LPCTSTR szFilePath,long lPos,bool bClick)
	{
		SIZE size = {0};
		return InsertThumbnail(szFilePath , lPos ,size,bClick );
	}


	BOOL UILIB_API DeterminingImageFormat(LPCTSTR szFile)
	{
		CComBSTR	 wFilePath;
		wFilePath = PathFindExtension(szFile);
		MsGdiplus* pGdi = MsGdiplus::newInstance();
		if( pGdi == 0 )	return FALSE;
		return pGdi->DeterminingFormat(wFilePath,false);
	}

	BOOL CRichEditUI::InsertImage(LPCTSTR szFilePath,long lPos,bool bClick, int faceid)
	{
		//	SetSel(-1, -1);
		LPTSTR pNewGuid = 0;
		if( false == DeterminingImageFormat(szFilePath) )
			return FALSE;
		CreateNewGUID(&pNewGuid);
		// 	if( lPos == -1 )
		// 		lPos = getEndCharPos();//�������ô׼ȷ�����Ǳ�����
		if( false == InsertOleImage( szFilePath,(DWORD)pNewGuid, lPos , faceid) )
		{
			free(pNewGuid);
			pNewGuid = 0;
			return FALSE;
		}
		if( bClick )
		{
// 			CHARFORMAT2 cf2;
// 			DWORD dwMask = 0;
// 			ZeroMemory(&cf2, sizeof(CHARFORMAT2));//
// 			cf2.cbSize		=	sizeof(CHARFORMAT2);
// 			cf2.dwMask		=	CFM_LINK;
// 			cf2.dwEffects	|=	CFE_LINK;
// 			if( lPos == -1 )
// 				lPos = getEndCharPos();
// 			SetSel(lPos,lPos+1);
// 			SetSelectionCharFormat(cf2);
// 			SetSel(-1, -1);
// 			dwMask = GetEventMask();
// 			if( (dwMask & ENM_LINK) != ENM_LINK )
// 			{
// 				dwMask |= ENM_LINK;
// 				SetEventMask(dwMask);
// 			}
		}
		return TRUE;
	}

	duistring* CRichEditUI::getRtf(void)
	{
		IRichEditOle*		pOle = 0;
		duistring*		pRet = new duistring;
		StreamOutResult*	pResult = new StreamOutResult( pRet );
		EDITSTREAM	   es;
		es.dwCookie	   = (DWORD)(void*)(pResult);
		es.dwError	   = 0;
		es.pfnCallback = StreamOutRtf;
		TxSendMessage(EM_GETOLEINTERFACE,0,(LPARAM)((PVOID)&pOle),NULL);
		if( (pRet==0) || (pResult==0) || (pOle==0) )
		{
			if( pResult )
			{
				delete pResult;
				pResult = 0;
			}
			if( pRet )
			{
				delete pRet;
				pRet = 0;
			}
			return 0;
		}
		PreStreamOut(this,pResult,pOle);
		StreamOut(SF_RTF,es);
		if( pResult )
		{
			delete pResult;
			pResult = 0;
		}
		pOle->Release();
		return pRet;
	}
	void CRichEditUI::ReleaseRes(void* pRes)
	{
		GetManager()->GetPaintWindow();
		if( pRes )
		{
			delete pRes;
			pRes = 0;
		}
	}
	bool	CRichEditUI::EnableDropDrag( void )
	{
		if( GetManager()->EnableDropTarget() )
		{	
			GetManager()->RegisterUI(this);
			return true;
		}
		return false;
	}
	IDragDropEvent* CRichEditUI::getDragEvent()
	{
		return this;
	}
	HRESULT CRichEditUI::DragEnter(IDataObject *pDataObj,DWORD grfKeyState,
		POINTL pt,POINT ptClient,DWORD *pdwEffect)
	{
		DuiLib::duistring szFilePath;
		bool		 bAccept = false;
		if( false == getDropFilePath(pDataObj,szFilePath) )
		{
			*pdwEffect = DROPEFFECT_NONE;	//�ܾ����ܸ��ļ�	
			return 0;
		}
		RichEditEvent* pEvenSink = 0;
		if( m_pTwh )
		{
			if( pEvenSink = m_pTwh->getEventSink() )
			{
				bAccept = pEvenSink->OnDragDropOver(szFilePath);
				if( bAccept == false )
				{
					*pdwEffect = DROPEFFECT_NONE;	//�ܾ����ܸ��ļ�	
					return 0;
				}
			}
		}
		return 0;
	}
	HRESULT CRichEditUI::DragOver(IDataObject *pDataObj,DWORD grfKeyState,POINTL pt,
		POINT ptClient,DWORD *pdwEffect)
	{
		DuiLib::duistring szFilePath;
		POINT		ptChar;
		EDIT_CHARPOS	tgCharPos;
		bool		bAccept = false;
		if( false == getDropFilePath(pDataObj,szFilePath) )
		{
			*pdwEffect = DROPEFFECT_NONE;	//�ܾ����ܸ��ļ�	
			return 0;
		}
		tgCharPos.nVal = CharFromPos(ptClient);
		if( tgCharPos.nVal > 0 )
		{
			ptChar = PosFromChar(tgCharPos.wIndex);
			m_pTwh->TxShowCaret(false);
			m_pTwh->TxSetCaretPos(ptChar.x,ptChar.y);
			m_pTwh->TxShowCaret(true);
		}
		RichEditEvent* pEvenSink = 0;
		if( m_pTwh )
		{
			if( pEvenSink = m_pTwh->getEventSink() )
			{
				bAccept = pEvenSink->OnDragDropOver(szFilePath);
				if( bAccept == false )
				{
					*pdwEffect = DROPEFFECT_NONE;	//�ܾ����ܸ��ļ�	
					return 0;
				}
			}
		}
		return 0;
	}
	HRESULT CRichEditUI::DragLeave(IDataObject *pDataObj)
	{
		DuiLib::duistring szFilePath;
		bool		bAccept = false;
		if( false == getDropFilePath(pDataObj,szFilePath) )
		{
			return 0;
		}
		RichEditEvent* pEvenSink = 0;
		if( m_pTwh )
		{
			if( pEvenSink = m_pTwh->getEventSink() )
			{
				bAccept = pEvenSink->OnDropLeave(szFilePath);
				if( bAccept == false )
					return 0;
			}
		}
		return 0;
	}
	HRESULT CRichEditUI::Drop(IDataObject *pDataObj,DWORD grfKeyState,POINTL pt,
		POINT ptClient,DWORD *pdwEffect)
	{
		DuiLib::duistring szFilePath;
		bool		 bAccept = false;
		EDIT_CHARPOS	tgCharPos;
		if( false == getDropFilePath(pDataObj,szFilePath) )
		{
			*pdwEffect = DROPEFFECT_NONE;	//�ܾ����ܸ��ļ�	
			return 0;
		}
		tgCharPos.nVal = CharFromPos(ptClient);	
		RichEditEvent* pEvenSink = 0;
		if( m_pTwh )
		{
			if( pEvenSink = m_pTwh->getEventSink() )
			{
				bAccept = pEvenSink->OnDropDown(szFilePath,tgCharPos.nVal);
				if( bAccept == false )
				{
					*pdwEffect = DROPEFFECT_NONE;	//�ܾ����ܸ��ļ�	
					return 0;
				}
			}
		}
		return 0;
	}

	CControlUI* CRichEditUI::GetInternalControlUI(LPOLEOBJECT lpdata)
	{
		OleLib::COleControlUi* pCtrlUIObject = dynamic_cast<OleLib::COleControlUi*>(lpdata);
		if ( pCtrlUIObject != NULL )
		{
			CControlUI *pCtrlUI = pCtrlUIObject->getControlUI();
			return pCtrlUI;		
		}

		return NULL;
	}


	LPCTSTR CRichEditUI::GetFilePath(LPOLEOBJECT lpdata)
	{
		//OleLib::CImageOleObject* pimageobject = static_cast<OleLib::CImageOleObject*>(lpdata); // ���Ͳ���ȫ
		//return pimageobject->getFilePath();// ����û�м��

		OleLib::CImageOleObject* pimageobject = dynamic_cast<OleLib::CImageOleObject*>(lpdata);// ׼ȷ����Ӧ�������IMsgImage�ӿڣ�Ȼ����Query
		if ( pimageobject != NULL )
			return pimageobject->getFilePath();
		return L"";
	}

	int CRichEditUI::GetFaceId(LPOLEOBJECT lpdata)
	{
		//OleLib::CImageOleObject* pimageobject = static_cast<OleLib::CImageOleObject*>(lpdata); // ���Ͳ���ȫ
		//return pimageobject->GetFaceID(); // ����û�м��

		OleLib::CImageOleObject* pimageobject = dynamic_cast<OleLib::CImageOleObject*>(lpdata);// ׼ȷ����Ӧ�������IMsgSysFace�ӿڣ�Ȼ����Query
		if ( pimageobject != NULL )
			return pimageobject->GetFaceID();
		return -1;
	}

	BOOL CRichEditUI::OnDBClick(POINT pt)
	{
		LONG lType = GetSelectionType();
		if (lType == SEL_OBJECT)
		{
			//LRESULT lres = 0;
			CComPtr<IRichEditOle> pRichEditOle = GetIRichEditOle();
			if (!pRichEditOle)
			{
				return FALSE;
			}
			HRESULT hr;
			REOBJECT obj;
			obj.cbStruct = sizeof(obj);
			hr = pRichEditOle->GetObject(-1,&obj,REO_GETOBJ_ALL_INTERFACES);
			if (GetScode(hr) == S_OK)
			{
				CComPtr<IOleObject>pOleObj = obj.poleobj;
				if (pOleObj)
				{
					hr = pOleObj->DoVerb(OLEIVERB_OPEN,0,obj.polesite,-1,this->GetParentWindow(),0);
				}
				if (obj.poleobj)obj.poleobj->Release();
				if (obj.polesite)obj.polesite->Release();
				if (obj.pstg)obj.pstg->Release();
				//SetSel(-1,-1);
				if (SUCCEEDED(hr)) return TRUE;
			}
		}
        return FALSE;
	}
	BOOL CRichEditUI::CanPaste(UINT nFormat) const
	{	
		LRESULT lResult;
		TxSendMessage(EM_CANPASTE, nFormat, 0, &lResult);
		return (BOOL)lResult == TRUE; 
	}

	void CRichEditUI::PasteSpecial(UINT uClipFormat, DWORD dwAspect/* = 0*/, HMETAFILE hMF/* = 0*/)
	{
		REPASTESPECIAL reps = { dwAspect, (DWORD_PTR)hMF };
		/*	pserv->*/
		TxSendMessage(EM_PASTESPECIAL, uClipFormat, (LPARAM)&reps, NULL);
		//TxSendMessage(uClipFormat, dwAspect, hMF);
	}

} // namespace DuiLib
