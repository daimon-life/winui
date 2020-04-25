#include "StdAfx.h"
#include "DragDropTarget.h"

namespace DuiLib {

	IDragDropTarget::IDragDropTarget(HWND hParent)
		:m_hParent(hParent),cRefs(0),m_pDataObj(0)
	{
		::InitializeCriticalSection(&m_LockCS);
		m_vtUIs.clear();
	}
	IDragDropTarget::~IDragDropTarget()
	{
		m_hParent = 0;
		cRefs	  = 0;
		m_vtUIs.clear();
		::DeleteCriticalSection(&m_LockCS);
	}

	void IDragDropTarget::Lock()
	{

		::EnterCriticalSection(&m_LockCS);
	}
	void IDragDropTarget::UnLock()
	{
		::LeaveCriticalSection(&m_LockCS);
	}
	void IDragDropTarget::Register(CControlUI* pEvent)
	{
		Lock();
		VT_UI::iterator vtItem = std::find(m_vtUIs.begin(),m_vtUIs.end(),pEvent);
		if( vtItem != m_vtUIs.end() )
			*vtItem = pEvent;
		else
			m_vtUIs.push_back(pEvent);
		UnLock();
	}

	void IDragDropTarget::RemoveDropUI( CControlUI* pUI )
	{
		Lock();
		VT_UI::iterator vtItem = std::find(m_vtUIs.begin(),m_vtUIs.end(),pUI);
		if( vtItem != m_vtUIs.end() )
			m_vtUIs.erase(vtItem);
		UnLock();
	}
	CControlUI* IDragDropTarget::IsOverUI( POINTL* pt,POINT *pClient )
	{
		RECT	tgRc;
		int nCount = m_vtUIs.size();
		if( pt == 0 )	return 0;
		POINT tgPt = {pt->x,pt->y};
		::ScreenToClient(m_hParent,&tgPt);
		Lock();
		for(int n=0;n < nCount;n++)
		{
			if( m_vtUIs[n] )
			{
				tgRc = m_vtUIs[n]->GetPos();
				if( PtInRect(&tgRc,tgPt) )
				{
					if( pClient )
						memcpy(pClient,&tgPt,sizeof(POINT));
					UnLock();
					return m_vtUIs[n];
				}
			}
		}

		UnLock();
		return 0;
	}
	HRESULT STDMETHODCALLTYPE IDragDropTarget::DragEnter(IDataObject *pDataObj,
		DWORD grfKeyState,POINTL pt,
		DWORD *pdwEffect)
	{
		POINT ptClient;
		Lock();
		CControlUI* pUI = IsOverUI(&pt,&ptClient);
		m_pDataObj = pDataObj;
		memcpy(&m_pt,&pt,sizeof(POINTL));
		if( pUI )
		{
			IDragDropEvent *pEvent = pUI->getDragEvent();
			if( pEvent )
				return pEvent->DragEnter(pDataObj,grfKeyState,pt,ptClient,pdwEffect);
		}
		UnLock();
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE IDragDropTarget::DragOver(DWORD grfKeyState,POINTL pt,
		DWORD *pdwEffect)
	{
		POINT ptClient;
		Lock();
		CControlUI* pUI = IsOverUI(&pt,&ptClient);
		memcpy(&m_pt,&pt,sizeof(POINTL));
		if( pUI )
		{
			IDragDropEvent *pEvent = pUI->getDragEvent();
			if( pEvent )
				return pEvent->DragOver(m_pDataObj,grfKeyState,pt,ptClient,pdwEffect);
		}
		UnLock();
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE IDragDropTarget::DragLeave(void)
	{
		POINT ptClient;
		Lock();
		CControlUI* pUI = IsOverUI(&m_pt,&ptClient);
		if( pUI )
		{
			IDragDropEvent *pEvent = pUI->getDragEvent();
			if( pEvent )
				return pEvent->DragLeave(m_pDataObj);
		}
		UnLock();
		m_pDataObj = 0;
		memset(&m_pt,0,sizeof(POINTL));	
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE IDragDropTarget::Drop(IDataObject *pDataObj,
		DWORD grfKeyState,POINTL pt,
		DWORD *pdwEffect)
	{
		POINT ptClient;
		Lock();
		CControlUI* pUI = IsOverUI(&pt,&ptClient);
		memcpy(&m_pt,&pt,sizeof(POINTL));
		if( pUI )
		{
			IDragDropEvent *pEvent = pUI->getDragEvent();
			if( pEvent )
				return pEvent->Drop(pDataObj,grfKeyState,pt,ptClient,pdwEffect);
		}
		UnLock();
		return S_OK;
	}
	HRESULT  IDragDropTarget::QueryInterface(REFIID riid, void **ppvObject)
	{
		HRESULT hr = E_NOINTERFACE;
		*ppvObject = NULL;
		if(IsEqualIID(riid,IID_IUnknown)||IsEqualIID(riid,IID_IDropTarget)) 
		{
			AddRef();
			*ppvObject = (IDropTarget *) this;
			hr = S_OK;
		}
		return hr;
	}
	ULONG	 IDragDropTarget::AddRef(void)
	{
		return ++cRefs;
	}
	ULONG    IDragDropTarget::Release(void)
	{
		ULONG c_Refs = --cRefs;
		if (c_Refs == 0)
		{
			delete this;
		}
		return c_Refs;
	}

}