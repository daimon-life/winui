//-------------------------------------------------------------------------
//  GifRichEdit - RichEdit Plus Control
//--------------------------------------------------------------------------
#pragma once

#include <richole.h>
#include <GdiPlus.h>
#include <atlstr.h>
#include <atlctl.h>
#include <Tom.h>

#pragma comment( lib, "GdiPlus.lib" )
using namespace Gdiplus;
// {6CD7AE52-B413-4eff-A70B-A39CAD063A6B}
static const GUID Guid_ImageObject = { 0x6cd7ae52,0xb413,0x4eff,{ 0xa7,0xb,0xa3,0x9c,0xad,0x6,0x3a,0x6b } };
// {574B00FE-66DC-458d-99C7-3622EAD256A1}
static const GUID Guid_OleControlUi = { 0x574b00fe,0x66dc,0x458d,{ 0x99,0xc7,0x36,0x22,0xea,0xd2,0x56, 0xa1 } };

namespace DuiLib{ namespace OleLib{
/*--------------------------IRichEditOleExCallback-------------------------------------------*/
	class IRichEditOleExCallback : public IRichEditOleCallback
	{
	public :
		IRichEditOleExCallback(IRichEditOle* pOle,CRichEditUI* pui);
		virtual ~ IRichEditOleExCallback();
		int m_iNumStorages;
		IStorage * pStorage;
		DWORD m_dwRef;
		IRichEditOle*	m_pOle;
		CRichEditUI* m_pOwner;
		virtual HRESULT STDMETHODCALLTYPE GetNewStorage(LPSTORAGE * lplpstg);
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
		virtual ULONG STDMETHODCALLTYPE AddRef();
		virtual ULONG STDMETHODCALLTYPE Release();
		virtual HRESULT STDMETHODCALLTYPE GetInPlaceContext(LPOLEINPLACEFRAME FAR * lplpFrame,
			LPOLEINPLACEUIWINDOW FAR * lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo);
		virtual HRESULT STDMETHODCALLTYPE ShowContainerUI(BOOL fShow);
		virtual HRESULT STDMETHODCALLTYPE QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp);
		virtual HRESULT STDMETHODCALLTYPE DeleteObject(LPOLEOBJECT lpoleobj);
		virtual HRESULT STDMETHODCALLTYPE QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR * lpcfFormat,
			DWORD reco, BOOL fReally, HGLOBAL hMetaPict);
		virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
		virtual HRESULT STDMETHODCALLTYPE GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj);
		virtual HRESULT STDMETHODCALLTYPE GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect);
		virtual HRESULT STDMETHODCALLTYPE GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg, HMENU FAR * lphmenu);
	};
/*--------------------------CImageOleObject-------------------------------------------*/
	class CImageOleObject : public IOleObject, public IViewObject2,public IDataObject
	{
	public:
		CImageOleObject();
		~CImageOleObject();
		// IUnknown接口
		virtual HRESULT WINAPI QueryInterface(REFIID iid, void ** ppvObject);
		virtual ULONG   WINAPI AddRef(void);
		virtual ULONG   WINAPI Release(void);
		// IOleObject接口
		virtual HRESULT WINAPI SetClientSite(IOleClientSite *pClientSite);
		virtual HRESULT WINAPI GetClientSite(IOleClientSite **ppClientSite);
		virtual HRESULT WINAPI SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj);
		virtual HRESULT WINAPI Close(DWORD dwSaveOption);
		virtual HRESULT WINAPI SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk);
		virtual HRESULT WINAPI GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk);
		virtual HRESULT WINAPI InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved);
		virtual HRESULT WINAPI GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject);
		virtual HRESULT WINAPI DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect);
		virtual HRESULT WINAPI EnumVerbs(IEnumOLEVERB **ppEnumOleVerb);
		virtual HRESULT WINAPI Update(void);
		virtual HRESULT WINAPI IsUpToDate(void);
		virtual HRESULT WINAPI GetUserClassID(CLSID *pClsid);
		virtual HRESULT WINAPI GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType);
		virtual HRESULT WINAPI SetExtent(DWORD dwDrawAspect, SIZEL *psizel);
		virtual HRESULT WINAPI GetExtent(DWORD dwDrawAspect, SIZEL *psizel);
		virtual HRESULT WINAPI Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection);
		virtual HRESULT WINAPI Unadvise(DWORD dwConnection);
		virtual HRESULT WINAPI EnumAdvise(IEnumSTATDATA **ppenumAdvise);
		virtual HRESULT WINAPI GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus);
		virtual HRESULT WINAPI SetColorScheme(LOGPALETTE *pLogpal);

		// IViewObject接口
		virtual HRESULT WINAPI Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd,
			HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
			BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), ULONG_PTR dwContinue);
		virtual HRESULT WINAPI GetColorSet(DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
			DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet);
		virtual HRESULT WINAPI Freeze(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze);
		virtual HRESULT WINAPI Unfreeze(DWORD dwFreeze);
		virtual HRESULT WINAPI SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink);
		virtual HRESULT WINAPI GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink);

		// IViewObject2接口
		virtual HRESULT WINAPI GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel);




		// Methods of the IDataObject Interface
		//
		STDMETHOD(GetData)(FORMATETC *pformatetcIn, STGMEDIUM *pmedium) {
// 			HANDLE hDst; 
// 			hDst = ::OleDuplicateData(m_stgmed.hBitmap, CF_BITMAP, NULL); 
// 			if (hDst == NULL) 
// 			{ 
// 				return E_HANDLE; 
// 			} 
// 
// 			pmedium->tymed = TYMED_GDI; 
// 			pmedium->hBitmap = (HBITMAP)hDst; 
// 			pmedium->pUnkForRelease = NULL; 

			return S_OK;
		}
		STDMETHOD(GetDataHere)(FORMATETC* pformatetc, STGMEDIUM*  pmedium ) {
			return E_NOTIMPL;
		}
		STDMETHOD(QueryGetData)(FORMATETC*  pformatetc ) {
			return E_NOTIMPL;
		}
		STDMETHOD(GetCanonicalFormatEtc)(FORMATETC*  pformatectIn ,FORMATETC* pformatetcOut ) 	{
			return E_NOTIMPL;
		}
		STDMETHOD(SetData)(FORMATETC* pformatetc , STGMEDIUM*  pmedium , BOOL  fRelease ) {

			return S_OK;
		}
		STDMETHOD(EnumFormatEtc)(DWORD  dwDirection , IEnumFORMATETC**  ppenumFormatEtc ) {
			return E_NOTIMPL;
		}
		STDMETHOD(DAdvise)(FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink,
			DWORD *pdwConnection) {
				return E_NOTIMPL;
		}
		STDMETHOD(DUnadvise)(DWORD dwConnection) {
			return E_NOTIMPL;
		}
		STDMETHOD(EnumDAdvise)(IEnumSTATDATA **ppenumAdvise) {
			return E_NOTIMPL;
		}


		void SetRichEditHwnd(HWND hWnd);
		HWND GetRichEditHwnd();

		BOOL LoadFromFile(const duistring& lpszGifFile);
		
		BOOL ShowThumbnailFromFile(const duistring& lpszGifFile,SIZE& sz);

		LPCTSTR getFilePath(void);

		int  GetFaceID();
		
		void GetImageSize(LPSIZE lpSize);
		
		UINT GetFrameCount();
		
		void OnTimer(UINT_PTR idEvent);
		
		void GetOleObjectRect(LPRECT lpRect);
		
		void DrawImage(HDC hDC, LPRECT lpRect);
		
		void resizeThumbnail(const SIZE& sz);

		void	setUser(DWORD dwUser);
		DWORD	getUser(void);

		static CImageOleObject* InsertImage(CControlUI* owner, HWND hWnd, IRichEditOle* pRichEditOle, 
			                          const duistring& lpGifPath, DWORD dwUserData, int id = -1,long nPos = -1);

		static CImageOleObject* InsertThumbnail(CControlUI* owner, HWND hWnd, IRichEditOle* pRichEditOle, 
									   const duistring& lpGifPath, DWORD dwUserData,SIZE& sz,long nPos = -1);

		void RegisterUserEventSink(RichEditEvent *pEvent);

		void DoUrl(long pos);

		HRESULT OnDoVerb(LONG iVerb);
	protected:
		void DeleteImage();
		BOOL IsAnimatedGIF();
		void DoContextMenu(long pt);
		void DoRelease(void);
	private:
		IOleClientSite *m_pOleClientSite;
		CControlUI* m_pOwner;
		RichEditEvent*	m_pEvenSink; 
		ULONG m_ulRef;
		DuiLib::duistring m_szGifFile;
		MsImage*	m_pImage;
		UINT*	m_puFrameElapse;
		UINT	m_nFrameCount;
		UINT	m_nFramePosition;
		HWND	m_hWndRichEdit;
		RECT	m_DrawRc;

		int		m_ifaceid;
		BOOL	m_bThumbnail;	//启动缩略图模式
		SIZE	m_szThumbnail;

		DWORD	m_dwUser;
		friend IRichEditOleExCallback;
	};
/*--------------------------COleControlUi-------------------------------------------*/
	class COleControlUi : public IOleObject, public IViewObject2
	{
	public:
		COleControlUi();
		~COleControlUi();
		// IUnknown接口
		virtual HRESULT WINAPI QueryInterface(REFIID iid, void ** ppvObject);
		virtual ULONG   WINAPI AddRef(void);
		virtual ULONG   WINAPI Release(void);
		// IOleObject接口
		virtual HRESULT WINAPI SetClientSite(IOleClientSite *pClientSite);
		virtual HRESULT WINAPI GetClientSite(IOleClientSite **ppClientSite);
		virtual HRESULT WINAPI SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj);
		virtual HRESULT WINAPI Close(DWORD dwSaveOption);
		virtual HRESULT WINAPI SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk);
		virtual HRESULT WINAPI GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk);
		virtual HRESULT WINAPI InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved);
		virtual HRESULT WINAPI GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject);
		virtual HRESULT WINAPI DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect);
		virtual HRESULT WINAPI EnumVerbs(IEnumOLEVERB **ppEnumOleVerb);
		virtual HRESULT WINAPI Update(void);
		virtual HRESULT WINAPI IsUpToDate(void);
		virtual HRESULT WINAPI GetUserClassID(CLSID *pClsid);
		virtual HRESULT WINAPI GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType);
		virtual HRESULT WINAPI SetExtent(DWORD dwDrawAspect, SIZEL *psizel);
		virtual HRESULT WINAPI GetExtent(DWORD dwDrawAspect, SIZEL *psizel);
		virtual HRESULT WINAPI Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection);
		virtual HRESULT WINAPI Unadvise(DWORD dwConnection);
		virtual HRESULT WINAPI EnumAdvise(IEnumSTATDATA **ppenumAdvise);
		virtual HRESULT WINAPI GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus);
		virtual HRESULT WINAPI SetColorScheme(LOGPALETTE *pLogpal);

		// IViewObject接口
		virtual HRESULT WINAPI Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd,
			HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
			BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), ULONG_PTR dwContinue);
		virtual HRESULT WINAPI GetColorSet(DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
			DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet);
		virtual HRESULT WINAPI Freeze(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze);
		virtual HRESULT WINAPI Unfreeze(DWORD dwFreeze);
		virtual HRESULT WINAPI SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink);
		virtual HRESULT WINAPI GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink);

		// IViewObject2接口
		virtual HRESULT WINAPI GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel);
		//Inside interface
		void RegisterUserEventSink(RichEditEvent *pEvent);

		void SetRichEditHwnd(HWND hWnd);
		HWND GetRichEditHwnd();

		void	setUser(DWORD dwUser);
		DWORD	getUser(void);
		
		void GetSize(LPSIZE lpSize);

		bool InitParams(const CControlUI *lpShowCtrl,const SIZE& sz);

		CControlUI*	getControlUI(void);
		
		void resize(const SIZE& sz );
		//插入OleControlUi
		static COleControlUi* InsertOleControlUi(CControlUI* owner,HWND hWnd,IRichEditOle* pRichEditOle,
									  const CControlUI *lpShowCtrl,const SIZE& sz,DWORD dwUserData,long nPos = -1);

		void DoUrl(long pos);
		HRESULT OnDoVerb(LONG iVerb)
		{
			return S_OK;
		}
	protected:
		void DoContextMenu(long pt);
		void DoRelease(void);
	private:
		IOleClientSite *m_pOleClientSite;
		RichEditEvent*	m_pEvenSink; 
		CControlUI*		m_pOwner;
		CControlUI*		m_pShowCtrl;
		SIZE	m_sz;
		ULONG	m_ulRef;
		HWND	m_hWndRichEdit;
		DWORD	m_dwUserPtr;
		friend IRichEditOleExCallback;
	};
}/*OleLib*/ }/*DuiLib*/