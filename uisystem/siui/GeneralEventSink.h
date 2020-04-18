#pragma once
#include <atlcoll.h>
#include <atlstr.h>
#include "UIControl.h"
using namespace DuiLib;
extern HRESULT AxAdviseAll(IUnknown * pUnk,CControlUI *pControlUI);

class CGeneralEventSink:public IDispatch
{
	ULONG m_refCnt;
	IID m_iid;
	CAtlMap<MEMBERID,CAtlStringW> m_MemberNameMap;
	CStringW m_InterfaceName;
	static HRESULT GetIIDTypeInfo(IID iid,ITypeInfo ** ppInfo,IUnknown * pRelateObj);
	static HRESULT VariantToString(VARIANT var,CAtlStringW& out);
	static HRESULT IIDToString(IID iid,CAtlStringW& out);
public:
	CGeneralEventSink(IID iid,IUnknown * pRelateObj,CControlUI *pControlUI=NULL/*UI*/);
	~CGeneralEventSink()
	{
	}
	//IUnknown
	STDMETHOD_(ULONG,AddRef())
	{
		return ++m_refCnt;
	}
	STDMETHOD_(ULONG,Release())
	{
		if (--m_refCnt == 0)
		{
			delete this;
			return 0;
		}
		return m_refCnt;
	}
	STDMETHOD(QueryInterface(REFIID riid,void **ppvObject))
	{
		*ppvObject = NULL;
		if ( IID_IUnknown == riid)
		{
			*ppvObject = (IUnknown*)this;
		}
		else if (IID_IDispatch == riid || m_iid == riid)
		{
			*ppvObject = (IDispatch*)this;
		}
		else
		{
			return E_NOINTERFACE;
		}
		AddRef();
		return S_OK;
	}
	//IDispatch
	//IDispatch
	STDMETHOD(GetTypeInfoCount(unsigned int FAR*  pctinfo))
	{
		return E_NOTIMPL;
	}
	STDMETHOD(GetTypeInfo(unsigned int  iTInfo,LCID  lcid,ITypeInfo FAR* FAR*  ppTInfo ))
	{
		return E_NOTIMPL;
	}
	STDMETHOD(GetIDsOfNames( 
		REFIID  riid,                  
		OLECHAR FAR* FAR*  rgszNames,  
		unsigned int  cNames,          
		LCID   lcid,                   
		DISPID FAR*  rgDispId          
		))
	{
		return E_NOTIMPL;
	}
	STDMETHOD(Invoke( 
		DISPID  dispIdMember,
		REFIID  riid,              
		LCID  lcid,                
		WORD  wFlags,              
		DISPPARAMS FAR*  pDispParams,  
		VARIANT FAR*  pVarResult,  
		EXCEPINFO FAR*  pExcepInfo,  
		unsigned int FAR*  puArgErr  
		));

	CControlUI *m_pControlUI;
};