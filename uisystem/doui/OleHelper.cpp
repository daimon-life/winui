//-------------------------------------------------------------------------
//  GifRichEdit - RichEdit Plus Control
//--------------------------------------------------------------------------
#include "StdAfx.h"
#include "OleHelper.h"
#include "UIRichEditFun.h"
#include <TextServ.h>
//////////////////////////////////////////////////////////////////////////
#define DEFINE_GUIDXXX(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	EXTERN_C const GUID CDECL name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
DEFINE_GUIDXXX(IID_ITextDocument,0x8CC497C0,0xA1DF,0x11CE,0x80,0x98,
			   0x00,0xAA,0x00,0x47,0xBE,0x5D);
//////////////////////////////////////////////////////////////////////////
namespace DuiLib{
	namespace OleLib{
		//////////////////////////////////////////////////////////////////////////
		//
		/*--------------------------TimerObject-------------------------------------------*/
		class TimerObject
		{
		public:
			TimerObject();	
			~TimerObject();
		public:
			void setTimer(HWND hWnd,CImageOleObject* pObj,UINT uElapse);
			void killTimer(CImageOleObject* pObj);
			void Invoke(UINT_PTR idEvent);
		private:
			std::vector<CImageOleObject*>	m_Object;
			HWND							m_hWnd;
		};

		TimerObject	gTimerObject;

		VOID CALLBACK GlobalTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
		{
			TimerObject *pT = (TimerObject *)idEvent;
			if( pT )
				pT->Invoke(idEvent);
		}


		TimerObject::TimerObject()
		{
			m_hWnd = 0;
		}
		TimerObject::~TimerObject()
		{
			if( m_hWnd )
				::KillTimer(m_hWnd,(UINT_PTR)&gTimerObject);
		}
		void TimerObject::setTimer(HWND hWnd,CImageOleObject* pObj,UINT uElapse)
		{
			if( m_Object.size()&& (m_hWnd == hWnd)  )
				m_Object.push_back(pObj);
			else
			{
				m_hWnd = hWnd;
				::SetTimer(m_hWnd,(UINT_PTR)&gTimerObject,uElapse,GlobalTimerProc);
				m_Object.push_back(pObj);
			}
		}
		void TimerObject::killTimer(CImageOleObject* pObj)
		{		
			std::vector<CImageOleObject*>::iterator itPos;
			itPos = std::find(m_Object.begin(),m_Object.end(),pObj);
			if( itPos != m_Object.end() )
				m_Object.erase(itPos);
			if( m_Object.size() == 0 )
				::KillTimer(m_hWnd,(UINT_PTR)&gTimerObject);
		}
		void TimerObject::Invoke(UINT_PTR idEvent)
		{
            std::vector<CImageOleObject*>::iterator vtIt;
            for (vtIt = m_Object.begin(); vtIt != m_Object.end(); vtIt++)
            {
                if ((*vtIt))
                {
                    (*vtIt)->OnTimer((UINT_PTR)(*vtIt));
                }
            }
            /*for(int n=0;n<m_Object.size();n++)
            {
            if( m_Object[n] )
            m_Object[n]->OnTimer( (UINT_PTR)(m_Object[n]) );
            }*/
		}
		///////////////////////////////////////////////////////////////////////////////
		//
		/*--------------------------CImageOleObject-------------------------------------------*/
		CImageOleObject::CImageOleObject()
		{
			m_bThumbnail	= false;
			m_ulRef			= 0;
			m_pImage		= NULL;
			m_puFrameElapse = NULL;
			m_nFrameCount = 0;
			m_nFramePosition = 0;
			m_pOleClientSite = NULL;
			m_hWndRichEdit	= NULL;
			m_pOwner		= NULL;
			m_pEvenSink		= 0;
			m_dwUser		= 0;
			m_ifaceid		= -1;
			memset(&m_szThumbnail,0,sizeof(SIZE));
			SetRectEmpty(&m_DrawRc);
		}
		void	CImageOleObject::setUser(DWORD dwUser)
		{
			m_dwUser = dwUser;
		}
		DWORD	CImageOleObject::getUser(void)
		{
			return m_dwUser;
		}
		CImageOleObject::~CImageOleObject()
		{
			DeleteImage();
		}
		void CImageOleObject::DoUrl(long pos)
		{
			if( m_pEvenSink )
				m_pEvenSink->OnUrl((void*)m_szGifFile.c_str(),1,pos);
		}

		HRESULT CImageOleObject::OnDoVerb(LONG iVerb)
		{
			if (iVerb == OLEIVERB_OPEN)
			{
				if( m_pEvenSink )
					m_pEvenSink->OnUrl((void*)m_szGifFile.c_str(),1,0);
				return S_OK;
			}

			return S_OK;
		}
		void CImageOleObject::DoRelease(void)
		{
			if( m_pEvenSink )
				m_pEvenSink->OnDeleteOleObject((void*)m_szGifFile.c_str(),1);
		}
		void CImageOleObject::DoContextMenu(long pt)
		{
			if( m_pEvenSink )
				m_pEvenSink->OnClickOleObject((void*)m_szGifFile.c_str(),1,pt,true);
		}

		void CImageOleObject::RegisterUserEventSink(RichEditEvent *pEvent)
		{
			m_pEvenSink = pEvent;
		}

		HRESULT WINAPI CImageOleObject::QueryInterface(REFIID iid, void ** ppvObject)
		{
			if (iid == IID_IUnknown || iid == IID_IOleObject)
			{
				*ppvObject = (IOleObject *)this;
				((IOleObject *)(*ppvObject))->AddRef();
				return S_OK;
			}
			else if (iid == IID_IViewObject || iid == IID_IViewObject2)
			{
				*ppvObject = (IViewObject2 *)this;
				((IViewObject2 *)(*ppvObject))->AddRef();
				return S_OK;
			}
			else if (iid == Guid_ImageObject || iid == Guid_ImageObject)
			{
				*ppvObject = (CImageOleObject *)this;
				((CImageOleObject *)(*ppvObject))->AddRef();
				return S_OK;
			}
			else if (iid == IID_IDataObject)
			{
				*ppvObject = (IDataObject *)this;
				((IDataObject *)(*ppvObject))->AddRef();
				return S_OK;
			}
			return E_NOINTERFACE;
		}

		ULONG WINAPI CImageOleObject::AddRef(void)
		{
			m_ulRef++;
			return m_ulRef;
		}

		ULONG WINAPI CImageOleObject::Release(void)
		{
			if (--m_ulRef == 0)
			{
				delete this;
				return 0;
			}

			return m_ulRef;
		}

		HRESULT WINAPI CImageOleObject::SetClientSite(IOleClientSite *pClientSite)
		{
			m_pOleClientSite = pClientSite;
			return S_OK;
		}

		HRESULT WINAPI CImageOleObject::GetClientSite(IOleClientSite **ppClientSite)
		{
			*ppClientSite = m_pOleClientSite;
			return S_OK;
		}

		HRESULT WINAPI CImageOleObject::SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj)
		{
			return S_OK;
		}

		HRESULT WINAPI CImageOleObject::SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect)
		{
			return OnDoVerb(iVerb);
			return S_OK;
		}

		HRESULT WINAPI CImageOleObject::EnumVerbs(IEnumOLEVERB **ppEnumOleVerb)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::Update(void)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::IsUpToDate(void)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::GetUserClassID(CLSID *pClsid)
		{
			*pClsid = Guid_ImageObject;
			return S_OK;
		}

		HRESULT WINAPI CImageOleObject::GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::SetExtent(DWORD dwDrawAspect, SIZEL *psizel)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::GetExtent(DWORD dwDrawAspect, SIZEL *psizel)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::Unadvise(DWORD dwConnection)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::EnumAdvise(IEnumSTATDATA **ppenumAdvise)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::SetColorScheme(LOGPALETTE *pLogpal)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd,
			HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
			BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), ULONG_PTR dwContinue)
		{
			if (lindex != -1)
				return S_FALSE;
			m_DrawRc.left   = lprcBounds->left;
			m_DrawRc.top    = lprcBounds->top;
			m_DrawRc.right  = lprcBounds->right;
			m_DrawRc.bottom = lprcBounds->bottom;
			DrawImage(hdcDraw, &m_DrawRc);
			return S_OK;
		}

		HRESULT WINAPI CImageOleObject::GetColorSet(DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
			DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::Freeze(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::Unfreeze(DWORD dwFreeze)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink)
		{
			if( m_puFrameElapse && m_pImage->getRealDelay() )
			{
				long lElapse = m_puFrameElapse[m_nFramePosition];
				gTimerObject.setTimer(m_hWndRichEdit,this,lElapse);
			}
			return E_NOTIMPL;
		}
		HRESULT WINAPI CImageOleObject::Close(DWORD dwSaveOption)
		{
			gTimerObject.killTimer(this);
			DeleteImage();
			return S_OK;
		}

		HRESULT WINAPI CImageOleObject::GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI CImageOleObject::GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel)
		{
			SIZE size;
			HDC hDC = ::GetDC(NULL);
			if( m_bThumbnail == false )
			{
				GetImageSize(&size);
				lpsizel->cx = ::MulDiv(size.cx + 2, 2540, GetDeviceCaps(hDC, LOGPIXELSX));
				lpsizel->cy = ::MulDiv(size.cy + 2, 2540, GetDeviceCaps(hDC, LOGPIXELSY));
			}
			else
			{
				lpsizel->cx = ::MulDiv(m_szThumbnail.cx + 2, 2540, GetDeviceCaps(hDC, LOGPIXELSX));
				lpsizel->cy = ::MulDiv(m_szThumbnail.cy + 2, 2540, GetDeviceCaps(hDC, LOGPIXELSY));
			}
			::ReleaseDC(NULL, hDC);
			return S_OK;
		}

		void CImageOleObject::SetRichEditHwnd(HWND hWnd)
		{
			m_hWndRichEdit = hWnd;
		}

		HWND CImageOleObject::GetRichEditHwnd()
		{
			return m_hWndRichEdit;
		}
		LPCTSTR CImageOleObject::getFilePath(void)
		{
			return m_szGifFile.c_str();
		}

		int CImageOleObject::GetFaceID(void)
		{
			return m_ifaceid;
		}

		void CImageOleObject::resizeThumbnail(const SIZE& sz)
		{
			memcpy(&m_szThumbnail,&sz,sizeof(SIZE));
			if( m_pOwner)
				m_pOwner->Invalidate();
		}

		BOOL CImageOleObject::ShowThumbnailFromFile(const duistring& lpszGifFile,SIZE& sz)
		{
			DeleteImage();
			CAtlStringW strFilePath( lpszGifFile.c_str() );
			m_pImage = new MsImage( MsGdiplus::newInstance() );
			if (m_pImage == NULL)	return FALSE;
			if( false == m_pImage->Initialize(strFilePath,&m_nFrameCount) )
			{
				delete m_pImage;
				m_pImage = 0;
				return FALSE;
			}
			m_szGifFile = lpszGifFile;
			if( m_nFrameCount )
			{
				m_puFrameElapse = new UINT[m_nFrameCount];
				unsigned int i = 0;
				for (i = 0;i < m_nFrameCount; i++)
					m_puFrameElapse[i] = m_pImage->getRealDelay();
			}
			m_bThumbnail = true;
			memcpy(&m_szThumbnail,&sz,sizeof(SIZE));
			return TRUE;
		}
		BOOL CImageOleObject::LoadFromFile(const duistring& lpszGifFile)
		{
			DeleteImage();
			CAtlStringW strFilePath( lpszGifFile.c_str() );
			m_pImage = new MsImage( MsGdiplus::newInstance() );
			if (m_pImage == NULL)	return FALSE;
			if( false == m_pImage->Initialize(strFilePath,&m_nFrameCount) )
			{
				delete m_pImage;
				m_pImage = 0;
				return FALSE;
			}
			m_szGifFile = lpszGifFile;
			if( m_nFrameCount )
			{
				m_puFrameElapse = new UINT[m_nFrameCount];
				unsigned int i = 0;
				for (i = 0;i < m_nFrameCount; i++)
					m_puFrameElapse[i] = m_pImage->getRealDelay();
			}
			return TRUE;
		}

		void CImageOleObject::GetImageSize(LPSIZE lpSize)
		{
			if (lpSize == NULL || m_pImage == NULL)
				return;
			lpSize->cx = m_pImage->GetWidth();
			lpSize->cy = m_pImage->GetHeight();
		}

		UINT CImageOleObject::GetFrameCount()
		{
			return m_nFrameCount;
		}

		void CImageOleObject::DeleteImage()
		{
			if (m_pImage)
			{
				delete m_pImage;
				m_pImage = NULL;
			}
			m_szGifFile.clear();
			if (m_puFrameElapse)
			{
				delete m_puFrameElapse;
				m_puFrameElapse = NULL;
			}
			::KillTimer(m_hWndRichEdit, (UINT_PTR)this);
			m_nFrameCount = 0;
			m_nFramePosition = 0;
		}
		BOOL CImageOleObject::IsAnimatedGIF()
		{
			return m_nFrameCount > 1;
		}

		void CImageOleObject::OnTimer(UINT_PTR idEvent)
		{
			if (idEvent != (UINT_PTR)this)	return;
			::KillTimer(m_hWndRichEdit, (UINT_PTR)this);

			RECT rcOleObject;
			::SetRectEmpty(&rcOleObject);
			GetOleObjectRect(&rcOleObject);
			m_nFramePosition++;
			if (m_nFramePosition == m_nFrameCount)
				m_nFramePosition = 0;
			if (NULL != m_pOwner)
			{
				m_pOwner->Invalidate();
			}
		}

		void CImageOleObject::GetOleObjectRect(LPRECT lpRect)
		{
			if (lpRect == NULL || m_hWndRichEdit == NULL)
				return;

			IRichEditOle *pRichEditOle = NULL;
			::SendMessage(m_hWndRichEdit, EM_GETOLEINTERFACE, 0, (LPARAM)&pRichEditOle);

			if (pRichEditOle == NULL)
				return;

			int nObjCount = pRichEditOle->GetObjectCount();
			int i = 0;
			for (i = 0;i < nObjCount;i++)
			{	
				REOBJECT reo;
				ZeroMemory(&reo, sizeof(REOBJECT));
				reo.cbStruct = sizeof(REOBJECT);

				HRESULT hr = pRichEditOle->GetObject(i, &reo, REO_GETOBJ_ALL_INTERFACES);
				if (hr != S_OK)
					continue;

				if (reo.poleobj)
					reo.poleobj->Release();

				if (reo.polesite)
					reo.polesite->Release();

				if (reo.pstg)
					reo.pstg->Release();

				if (reo.poleobj == (IOleObject *)this)
				{
					SIZE size;
					ITextDocument *pTextDocument = NULL;
					ITextRange *pTextRange = NULL;

					pRichEditOle->QueryInterface(IID_ITextDocument, (void **)&pTextDocument);
					if (!pTextDocument)
						return;

					long nLeft = 0;
					long nBottom = 0;
					pTextDocument->Range(reo.cp, reo.cp, &pTextRange);
					if (reo.dwFlags & REO_BELOWBASELINE)
						hr = pTextRange->GetPoint(TA_BOTTOM|TA_LEFT, &nLeft, &nBottom);
					else
						hr = pTextRange->GetPoint(TA_BASELINE|TA_LEFT, &nLeft, &nBottom);

					pTextDocument->Release();
					pTextRange->Release();

					RECT rectWindow;
					::GetWindowRect(m_hWndRichEdit, &rectWindow);
					if( m_bThumbnail )
					{
						lpRect->left   = nLeft - rectWindow.left;
						lpRect->bottom = nBottom - rectWindow.top;
						lpRect->right  = lpRect->left + m_szThumbnail.cx + 2;
						lpRect->top    = lpRect->bottom - m_szThumbnail.cy - 2;
					}
					else
					{
						GetImageSize(&size);
						lpRect->left   = nLeft - rectWindow.left;
						lpRect->bottom = nBottom - rectWindow.top;
						lpRect->right  = lpRect->left + size.cx + 2;
						lpRect->top    = lpRect->bottom - size.cy - 2;
					}
					break;
				}
			}

			pRichEditOle->Release();
		}

		void CImageOleObject::DrawImage(HDC hDC, LPRECT lpRect)
		{
			GUID pageGuid = Gdiplus::FrameDimensionTime;

			if (!m_pImage)
			{
				return;
			}
			long hmWidth = m_pImage->GetWidth();
			long hmHeight = m_pImage->GetHeight();
			if( m_bThumbnail == false )
			{
				m_pImage->AutoRender(hDC,lpRect->left+1, lpRect->top+1, lpRect->right-lpRect->left-2, 
					lpRect->bottom-lpRect->top-2);
			}
			else
			{
				Image* pI = m_pImage->GetThumbnail(m_szThumbnail);
				if( pI )
				{
					Graphics    graphics(hDC);
					if( pI )
					{
						graphics.DrawImage(pI,lpRect->left+1, lpRect->top+1,m_szThumbnail.cx-2, 
							m_szThumbnail.cy-2);
					}
				}
			}
		}

		/* InsertImage

		Routine Description:

		在RICHEDIT编辑框控件中插入GIF图片

		Arguments:

		hWnd           - RICHEDIT控件句柄
		pRichEditOle   - RICHEDIT OLE对象指针
		lpGifPath      - GIF图片文件位置

		Return Value:

		如果成功,返回真(TRUE).如果失败,返回假(FALSE).
		*/
		CImageOleObject* CImageOleObject::InsertImage(CControlUI* owner, HWND hWnd, IRichEditOle* pRichEditOle, 
			const duistring& lpGifPath, DWORD dwUserData, int id, long nPos)
		{
			SCODE sc;

			IOleClientSite *pOleClientSite = NULL;
			pRichEditOle->GetClientSite(&pOleClientSite);

			IStorage *pStorage = NULL;

			LPLOCKBYTES lpLockBytes = NULL;
			sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
			if (sc != S_OK)
				return FALSE;

			sc = ::StgCreateDocfileOnILockBytes(lpLockBytes,
				STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &pStorage);
			if (sc != S_OK)
			{
				lpLockBytes->Release();
				lpLockBytes = NULL;
				return FALSE;
			}

			CImageOleObject *pGifOleObject = new CImageOleObject;
			pGifOleObject->SetRichEditHwnd(hWnd);
			IDataObject *pDataObject = NULL;
			pGifOleObject->QueryInterface(IID_IDataObject, (void **)&pDataObject);
			if( FALSE == pGifOleObject->LoadFromFile(lpGifPath) )
			{
				delete pGifOleObject;
				pGifOleObject = 0;
				return 0;
			}

			
			//pGifOleObject->SetBitmap();

			pGifOleObject->setUser(dwUserData);
			pGifOleObject->m_pOwner = owner;
			pGifOleObject->m_ifaceid = id;

			IOleObject *pOleObject = NULL;
			pGifOleObject->QueryInterface(IID_IOleObject, (void **)&pOleObject);
			pOleObject->SetClientSite(pOleClientSite);

			HRESULT hr = ::OleSetContainedObject(pOleObject, TRUE);
			if( hr )
			{
				delete pGifOleObject;
				pGifOleObject = 0;
				if( pStorage )
					pStorage->Release();
				return 0;
			}
			REOBJECT reobject;
			ZeroMemory(&reobject, sizeof(REOBJECT));
			reobject.cbStruct = sizeof(REOBJECT);

			reobject.clsid    = Guid_ImageObject;
			reobject.cp       = REO_CP_SELECTION;
			reobject.dvaspect = DVASPECT_CONTENT;
			reobject.poleobj  = pOleObject;
			reobject.polesite = pOleClientSite;
			reobject.pstg     = pStorage;
			reobject.dwUser   = dwUserData;

			sc = pRichEditOle->InsertObject(&reobject);
			pOleObject->Release();
			pOleClientSite->Release();
			pStorage->Release();
			if( sc )
			{
				delete pGifOleObject;
				pGifOleObject = 0;
				return 0;
			}
			return pGifOleObject;
		}

		CImageOleObject* CImageOleObject::InsertThumbnail(CControlUI* owner, HWND hWnd, IRichEditOle* pRichEditOle, 
			const duistring& lpGifPath, DWORD dwUserData,SIZE& sz,long nPos)
		{
			SCODE sc;

			IOleClientSite *pOleClientSite = NULL;
			pRichEditOle->GetClientSite(&pOleClientSite);

			IStorage *pStorage = NULL;

			LPLOCKBYTES lpLockBytes = NULL;
			sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
			if (sc != S_OK)
				return FALSE;

			sc = ::StgCreateDocfileOnILockBytes(lpLockBytes,
				STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &pStorage);
			if (sc != S_OK)
			{
				lpLockBytes->Release();
				lpLockBytes = NULL;
				return FALSE;
			}

			CImageOleObject *pGifOleObject = new CImageOleObject;
			pGifOleObject->SetRichEditHwnd(hWnd);
			pGifOleObject->ShowThumbnailFromFile(lpGifPath,sz);
			pGifOleObject->setUser(dwUserData);
			pGifOleObject->m_pOwner = owner;

			IOleObject *pOleObject = NULL;
			pGifOleObject->QueryInterface(IID_IOleObject, (void **)&pOleObject);
			pOleObject->SetClientSite(pOleClientSite);

			HRESULT hr = ::OleSetContainedObject(pOleObject, TRUE);
			if( hr )
			{
				delete pGifOleObject;
				pGifOleObject = 0;
				if( pStorage )
					pStorage->Release();
				return 0;
			}
			REOBJECT reobject;
			ZeroMemory(&reobject, sizeof(REOBJECT));
			reobject.cbStruct = sizeof(REOBJECT);

			reobject.clsid    = Guid_ImageObject;
			reobject.cp       = nPos;
			reobject.dvaspect = DVASPECT_CONTENT;
			reobject.poleobj  = pOleObject;
			reobject.polesite = pOleClientSite;
			reobject.pstg     = pStorage;
			reobject.dwUser   = dwUserData;

			sc = pRichEditOle->InsertObject(&reobject);

			pOleObject->Release();
			pOleClientSite->Release();
			pStorage->Release();
			if( sc )
			{
				delete pGifOleObject;
				pGifOleObject = 0;
				return 0;
			}
			return pGifOleObject;
		}

		///////////////////////////////////////////////////////////////////////////////
		//
		/*--------------------------CProgressBar-------------------------------------------*/
		COleControlUi::COleControlUi()
		{
			m_ulRef		= 0;
			m_pOleClientSite = NULL;
			m_hWndRichEdit	= NULL;
			m_pOwner	= NULL;
			m_pEvenSink = 0;
			m_dwUserPtr = 0;
			memset(&m_sz,0,sizeof(SIZE));
		}

		COleControlUi::~COleControlUi()
		{

		}
		void	COleControlUi::setUser(DWORD dwUser)
		{
			m_dwUserPtr = dwUser;
		}
		DWORD	COleControlUi::getUser(void)
		{
			return m_dwUserPtr;
		}
		void	COleControlUi::DoRelease(void)
		{
			if( m_pEvenSink )
				m_pEvenSink->OnDeleteOleObject((void*)m_pShowCtrl,2);
		}
		void	COleControlUi::DoUrl(long pos)
		{
			if( m_pEvenSink )
				m_pEvenSink->OnUrl((void*)m_pShowCtrl,2,pos);
		}
		void COleControlUi::DoContextMenu(long pt)
		{
			if( m_pEvenSink )
				m_pEvenSink->OnClickOleObject((void*)m_pShowCtrl,2,pt,true);
		}
		void COleControlUi::RegisterUserEventSink(RichEditEvent *pEvent)
		{
			m_pEvenSink = pEvent;
		}
		HRESULT WINAPI COleControlUi::QueryInterface(REFIID iid, void ** ppvObject)
		{
			if (iid == IID_IUnknown || iid == IID_IOleObject)
			{
				*ppvObject = (IOleObject *)this;
				((IOleObject *)(*ppvObject))->AddRef();
				return S_OK;
			}
			else if (iid == IID_IViewObject || iid == IID_IViewObject2)
			{
				*ppvObject = (IViewObject2 *)this;
				((IViewObject2 *)(*ppvObject))->AddRef();
				return S_OK;
			}
			else if (iid == Guid_OleControlUi || iid == Guid_OleControlUi)
			{
				*ppvObject = (COleControlUi *)this;
				((COleControlUi *)(*ppvObject))->AddRef();
				return S_OK;
			}
			return E_NOINTERFACE;
		}

		ULONG WINAPI COleControlUi::AddRef(void)
		{
			m_ulRef++;
			return m_ulRef;
		}

		ULONG WINAPI COleControlUi::Release(void)
		{
			if (--m_ulRef == 0)
			{
				delete this;
				return 0;
			}

			return m_ulRef;
		}

		HRESULT WINAPI COleControlUi::SetClientSite(IOleClientSite *pClientSite)
		{
			m_pOleClientSite = pClientSite;
			return S_OK;
		}

		HRESULT WINAPI COleControlUi::GetClientSite(IOleClientSite **ppClientSite)
		{
			*ppClientSite = m_pOleClientSite;
			return S_OK;
		}

		HRESULT WINAPI COleControlUi::SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj)
		{
			return S_OK;
		}

		HRESULT WINAPI COleControlUi::SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect)
		{
			return S_OK;
		}

		HRESULT WINAPI COleControlUi::EnumVerbs(IEnumOLEVERB **ppEnumOleVerb)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::Update(void)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::IsUpToDate(void)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::GetUserClassID(CLSID *pClsid)
		{
			*pClsid = Guid_OleControlUi;
			return S_OK;
		}

		HRESULT WINAPI COleControlUi::GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::SetExtent(DWORD dwDrawAspect, SIZEL *psizel)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::GetExtent(DWORD dwDrawAspect, SIZEL *psizel)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::Unadvise(DWORD dwConnection)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::EnumAdvise(IEnumSTATDATA **ppenumAdvise)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::SetColorScheme(LOGPALETTE *pLogpal)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd,
			HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
			BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), ULONG_PTR dwContinue)
		{
			RECT	tgDrawRc = { 0 };
			RECT	tgRc = {lprcBounds->left,lprcBounds->top,lprcBounds->right,lprcBounds->bottom};
			if (lindex != -1)	return S_FALSE;
			tgDrawRc = m_pShowCtrl->GetPos();
			if( FALSE == EqualRect(&tgRc,&tgDrawRc) ) 
				m_pShowCtrl->SetPos(tgRc);	
			m_pShowCtrl->DoPaint(hdcDraw,tgRc);
			return S_OK;
		}
		HRESULT WINAPI COleControlUi::GetColorSet(DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
			DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::Freeze(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::Unfreeze(DWORD dwFreeze)
		{
			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink)
		{

			return E_NOTIMPL;
		}
		HRESULT WINAPI COleControlUi::Close(DWORD dwSaveOption)
		{
			return S_OK;
		}

		HRESULT WINAPI COleControlUi::GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink)
		{

			return E_NOTIMPL;
		}

		HRESULT WINAPI COleControlUi::GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel)
		{
			if( lindex != -1 )	return S_FALSE;
			HDC hDC = ::GetDC(NULL);
			lpsizel->cx = ::MulDiv(m_sz.cx + 2, 2540, GetDeviceCaps(hDC, LOGPIXELSX));
			lpsizel->cy = ::MulDiv(m_sz.cy + 2, 2540, GetDeviceCaps(hDC, LOGPIXELSY));
			::ReleaseDC(NULL, hDC);
			return S_OK;
		}

		void COleControlUi::SetRichEditHwnd(HWND hWnd)
		{
			m_hWndRichEdit = hWnd;
		}

		HWND COleControlUi::GetRichEditHwnd()
		{
			return m_hWndRichEdit;
		}

		void COleControlUi::GetSize(LPSIZE lpSize)
		{
			if( lpSize )
				memcpy(lpSize,&m_sz,sizeof(SIZE));
		}
		void COleControlUi::resize(const SIZE& sz)
		{
			memcpy(&m_sz,&sz,sizeof(SIZE));
			if( m_pOwner)
				m_pOwner->Invalidate();
		}
		bool COleControlUi::InitParams(const CControlUI *lpShowCtrl,const SIZE& sz)
		{
			m_pShowCtrl = const_cast<CControlUI*>(lpShowCtrl);
			memcpy(&m_sz,&sz,sizeof(SIZE));
			return true;
		}
		CControlUI*	COleControlUi::getControlUI(void)
		{
			return m_pShowCtrl;
		}
		COleControlUi* COleControlUi::InsertOleControlUi(CControlUI* owner,HWND hWnd,IRichEditOle* pRichEditOle,
			const CControlUI *lpShowCtrl,const SIZE& sz,DWORD dwUserData,long nPos)
		{
			SCODE sc;

			IOleClientSite *pOleClientSite = NULL;
			pRichEditOle->GetClientSite(&pOleClientSite);

			IStorage *pStorage = NULL;

			LPLOCKBYTES lpLockBytes = NULL;
			sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
			if (sc != S_OK)
				return FALSE;

			sc = ::StgCreateDocfileOnILockBytes(lpLockBytes,
				STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &pStorage);
			if (sc != S_OK)
			{
				lpLockBytes->Release();
				lpLockBytes = NULL;
				return FALSE;
			}

			COleControlUi *pProgressOle = new COleControlUi;
			pProgressOle->SetRichEditHwnd(hWnd);
			pProgressOle->InitParams(lpShowCtrl,sz);
			pProgressOle->setUser(dwUserData);
			pProgressOle->m_pOwner = owner;

			IOleObject *pOleObject = NULL;
			pProgressOle->QueryInterface(IID_IOleObject, (void **)&pOleObject);
			pOleObject->SetClientSite(pOleClientSite);

			HRESULT hr = ::OleSetContainedObject(pOleObject, TRUE);
			if( hr )
			{
				delete pProgressOle;
				pProgressOle = 0;
				if( pStorage )
					pStorage->Release();
				return 0;
			}
			REOBJECT reobject;
			ZeroMemory(&reobject, sizeof(REOBJECT));
			reobject.cbStruct = sizeof(REOBJECT);

			reobject.clsid    = Guid_OleControlUi;
			reobject.cp       = nPos;
			reobject.dvaspect = DVASPECT_CONTENT;
			reobject.poleobj  = pOleObject;
			reobject.polesite = pOleClientSite;
			reobject.pstg     = pStorage;
			reobject.dwUser   = dwUserData;

			sc = pRichEditOle->InsertObject(&reobject);

			pOleObject->Release();
			pOleClientSite->Release();
			pStorage->Release();
			if( sc )
			{
				pProgressOle->Release();
				return 0;
			}
			return pProgressOle;
		}
		/*--------------------------IRichEditOleExCallback-------------------------------------------*/
		IRichEditOleExCallback::IRichEditOleExCallback(IRichEditOle* pOle,CRichEditUI* pui)
		{
			pStorage = NULL;
			m_iNumStorages = 0 ;
			m_dwRef = 0 ;
			m_pOle = pOle;
			m_pOwner = pui;
			// set up OLE storage 
			HRESULT hResult = ::StgCreateDocfile( NULL,
				STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE| STGM_CREATE ,
				0 , & pStorage );
			ASSERT( pStorage != NULL && hResult == S_OK && "Failed to setup OLE storage" );
		}

		IRichEditOleExCallback::~ IRichEditOleExCallback()
		{
		}

		HRESULT STDMETHODCALLTYPE IRichEditOleExCallback::GetNewStorage(LPSTORAGE * lplpstg)
		{
			m_iNumStorages ++ ;
			CAtlStringW storageName;
			storageName.Format(L" REOLEStorage%d ", m_iNumStorages);
			HRESULT hResult = pStorage -> CreateStorage(  storageName.GetString(), 
				STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE ,
				0 , 0 , lplpstg );
			ASSERT( hResult == S_OK && "Failed to create storage" );

			return hResult;
		}

		EXTERN_C const IID IID_IRichEditOleExCallback = { 
			0x00020D03,
			0,
			0,
			{0xC0,0,0,0,0,0,0,0x46}
		};

		HRESULT STDMETHODCALLTYPE IRichEditOleExCallback::QueryInterface(REFIID iid, void ** ppvObject)
		{
			HRESULT hr = S_OK;
			* ppvObject = NULL;

			if ( iid == IID_IUnknown || 
				iid == IID_IRichEditOleExCallback )
			{
				* ppvObject = this ;
				AddRef();
				hr = NOERROR;
			}
			else 
			{
				hr = E_NOINTERFACE;
			}

			return hr;
		}

		ULONG STDMETHODCALLTYPE IRichEditOleExCallback::AddRef()
		{
			return ++ m_dwRef;
		}

		ULONG STDMETHODCALLTYPE IRichEditOleExCallback::Release()
		{
			if ( -- m_dwRef == 0 )
			{
				delete this ;
				return 0 ;
			}
			return m_dwRef;
		}

		HRESULT STDMETHODCALLTYPE IRichEditOleExCallback::GetInPlaceContext(LPOLEINPLACEFRAME FAR * lplpFrame,
			LPOLEINPLACEUIWINDOW FAR * lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE IRichEditOleExCallback::ShowContainerUI(BOOL fShow)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE IRichEditOleExCallback::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE IRichEditOleExCallback::DeleteObject(LPOLEOBJECT lpoleobj)
		{
			long			lCount = 0;
			HRESULT			hr = 0;
			CImageOleObject* pImageOle = 0;
			COleControlUi*	 pOleUi    = 0;
			hr = lpoleobj->QueryInterface(Guid_ImageObject,(void**)&pImageOle);
			if( hr == 0)
			{
				LPTSTR pPicInfo = (LPTSTR)pImageOle->getUser();
				pImageOle->DoRelease();
				if( pPicInfo && IsPictureGuid(pPicInfo) )
				{
					free(pPicInfo);
					pPicInfo = 0;
					pImageOle->setUser(0);
				}
				pImageOle->Release();
				return S_OK;
			}
			hr = lpoleobj->QueryInterface(Guid_OleControlUi,(void**)&pOleUi);
			if( hr == 0 )
			{
				pOleUi->DoRelease();
				pOleUi->Release();
				return S_OK;
			}
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE IRichEditOleExCallback::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR * lpcfFormat,
			DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
		{
			
			if(!fReally) return S_OK;
			if (m_pOwner)
			{
				m_pOwner->OnPaste();
			}
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE IRichEditOleExCallback::ContextSensitiveHelp(BOOL fEnterMode)
		{
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE IRichEditOleExCallback::GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj)
		{

			if(RECO_DRAG == reco || RECO_COPY == reco || RECO_CUT == reco)
			{
				m_pOwner->OnCopy(lpchrg,reco,lplpdataobj);
// 				if(RECO_DRAG == reco)
// 				{
// 					return S_FALSE;
// 				}
				return S_OK;
			}
			return E_NOTIMPL;
		}

		HRESULT STDMETHODCALLTYPE IRichEditOleExCallback::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE IRichEditOleExCallback::GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg,
			HMENU FAR * lphmenu)
		{

			if(seltyp & GCM_RIGHTMOUSEDROP || m_pOwner == NULL) return S_OK;

			if (m_pOwner)
			{
				m_pOwner->OnContextMenu(seltyp,lpoleobj,lpchrg,lphmenu);
			}

			long			lCount = 0;
			HRESULT			hr = 0;
			CHARRANGE		tgRange = { 0 };
			CImageOleObject* pImageOle = 0;
			COleControlUi*	 pOleUi    = 0;
			LPOLEOBJECT		pTmp = lpoleobj;
			if( lpchrg )
			{
				tgRange.cpMax = lpchrg->cpMax;
				tgRange.cpMin = lpchrg->cpMin;
			}
			if( lpoleobj == 0)	return S_OK;

			if( (seltyp & SEL_MULTIOBJECT) == SEL_MULTIOBJECT )
			{
				lCount = m_pOle->GetObjectCount();
				for(int n=0;n < lCount;n++)
				{
					REOBJECT tgObj;
					tgObj.cbStruct = sizeof(REOBJECT);
					hr = m_pOle->GetObject(n,&tgObj,REO_GETOBJ_ALL_INTERFACES);
					if( hr != S_OK )	continue;
					if( (tgRange.cpMin <= tgObj.cp) && (tgRange.cpMax >= tgObj.cp) )
					{				
						if( tgObj.clsid == Guid_ImageObject )
						{
							pImageOle = (CImageOleObject*)tgObj.poleobj;
							pImageOle->DoContextMenu(tgObj.cp);
							pImageOle->Release();
							if( tgObj.polesite )
								tgObj.polesite->Release();
							if( tgObj.pstg )
								tgObj.pstg->Release();
							continue;
						}
						if( tgObj.clsid == Guid_OleControlUi )
						{
							pOleUi = (COleControlUi*)tgObj.poleobj;
							pOleUi->DoContextMenu(tgObj.cp);
							pOleUi->Release();
							if( tgObj.polesite )
								tgObj.polesite->Release();
							if( tgObj.pstg )
								tgObj.pstg->Release();
							continue;
						}
						if( tgObj.poleobj )
							tgObj.poleobj->Release();
						if( tgObj.polesite )
							tgObj.polesite->Release();
						if( tgObj.pstg )
							tgObj.pstg->Release();
					}
				}
			}
			if( (seltyp & SEL_OBJECT) == SEL_OBJECT )
			{
				hr = pTmp->QueryInterface(Guid_ImageObject,(void**)&pImageOle);
				if( hr == 0)
				{
					pImageOle->DoContextMenu(lpchrg->cpMax);
					pImageOle->Release();
					return S_OK;
				}
				hr = pTmp->QueryInterface(Guid_OleControlUi,(void**)&pOleUi);
				if( hr == 0 )
				{
					pOleUi->DoContextMenu(lpchrg->cpMax);
					pOleUi->Release();
					return S_OK;
				}
			}
			return S_OK;
		}

	}/*namespace OleLib*/
}/*namespace DuiLib*/