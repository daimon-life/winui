#if !defined(__DRAG_DROP_TARGET_H_)
#define __DRAG_DROP_TARGET_H_

namespace DuiLib
{
	class CControlUI;

	class IDragDropEvent
	{
	public:
		virtual HRESULT DragEnter(IDataObject *pDataObj,DWORD grfKeyState,
								 POINTL pt,POINT ptClient,DWORD *pdwEffect) = 0;
		
		virtual HRESULT DragOver(IDataObject *pDataObj,DWORD grfKeyState,
								POINTL pt,POINT ptClient,DWORD *pdwEffect) = 0;

		virtual HRESULT DragLeave(IDataObject *pDataObj) = 0;

		virtual HRESULT Drop(IDataObject *pDataObj,DWORD grfKeyState,POINTL pt,
							POINT ptClient,DWORD *pdwEffect) = 0;
	};	
	
	class IDragDropTarget : public IDropTarget
	{
	public:
		typedef std::vector<CControlUI*>	VT_UI;
		IDragDropTarget(HWND hParent);
		~IDragDropTarget();
		void Register(CControlUI* pEvent);
		void RemoveDropUI( CControlUI* pUI );
		HRESULT STDMETHODCALLTYPE DragEnter(IDataObject *pDataObj,DWORD grfKeyState,
										POINTL pt,DWORD *pdwEffect);
		HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState,POINTL pt,
										DWORD *pdwEffect);
		HRESULT STDMETHODCALLTYPE DragLeave(void);
		HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj,DWORD grfKeyState,
										POINTL pt,DWORD *pdwEffect);
		CControlUI*	IsOverUI( POINTL* pt,POINT *ptClient );
		virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
		virtual ULONG _stdcall AddRef(void);
		virtual ULONG _stdcall Release(void);

		void Lock();
		void UnLock();
	private:
		IDataObject*	m_pDataObj;
		HWND	m_hParent;
		VT_UI	m_vtUIs;
		ULONG	cRefs;
		POINTL	m_pt;
		CRITICAL_SECTION	m_LockCS;
	};
}

#endif /*__DRAG_DROP_TARGET_H_*/