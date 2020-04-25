#include "stdafx.h"
#include "GeneralEventSink.h"
#include <Ole2.h>
#include <OCIdl.h>
#include <atlstr.h>

HRESULT AxAdviseAll(IUnknown * pUnk,CControlUI *pControlUI)
{
	HRESULT hr;
	IConnectionPointContainer * pContainer = NULL;
	IConnectionPoint * pConnectionPoint=NULL;
	IEnumConnectionPoints * pEnum = NULL;
	hr = pUnk->QueryInterface(IID_IConnectionPointContainer,(void**)&pContainer);
	if (FAILED(hr)) goto error1;
	hr = pContainer->EnumConnectionPoints(&pEnum);
	if (FAILED(hr)) goto error1;
	ULONG uFetched;
	while(S_OK == (pEnum->Next(1,&pConnectionPoint,&uFetched)) && uFetched>=1)
	{
		DWORD dwCookie;
		IID iid;
		hr = pConnectionPoint->GetConnectionInterface(&iid);
		if (FAILED(hr)) iid = IID_NULL;
		IUnknown * pSink = new CGeneralEventSink(iid,pUnk,pControlUI);
		hr = pConnectionPoint->Advise(pSink,&dwCookie);
		pSink->Release();
		pConnectionPoint->Release();
		pConnectionPoint = NULL;
		pSink = NULL;
	}
	hr = S_OK;
error1:
	if (pEnum)pEnum->Release();
	if (pContainer) pContainer->Release();
	if (pConnectionPoint) pConnectionPoint->Release();
	return hr;
}

void TraceW(const wchar_t * format,...)
{
	CAtlStringW s;
	va_list args;
	va_start(args, format);
	s.FormatV(format, args);
	va_end(args);
	OutputDebugStringW(s);
}
CGeneralEventSink::CGeneralEventSink(IID iid,IUnknown * pRelateObj,CControlUI *pControlUI)
{
	m_refCnt = 1;
	m_iid = iid;
	m_pControlUI = pControlUI;
	ITypeInfo * pInfo;
	bool isDispatch = false;
	HRESULT hr = GetIIDTypeInfo(iid,&pInfo,pRelateObj);
	if (SUCCEEDED(hr))
	{
		//是否Dispatch接口
		TYPEATTR *attr;
		if (SUCCEEDED(pInfo->GetTypeAttr(&attr)))
		{
			if (attr->typekind == TKIND_DISPATCH) isDispatch = true;
			pInfo->ReleaseTypeAttr(attr);
		}
		if (isDispatch)
		{
			unsigned int uIndex;
			for(uIndex = 0;;++uIndex)
			{
				FUNCDESC * fd;
				BSTR funcName;
				unsigned int cNames;
				if (FAILED(pInfo->GetFuncDesc(uIndex,&fd))) break;
				if (SUCCEEDED(pInfo->GetNames(fd->memid,&funcName,1,&cNames)) && cNames>=1)
				{
					m_MemberNameMap[fd->memid] = CAtlStringW(funcName);
					SysFreeString(funcName);
				}
			}
		}
		pInfo->Release();
	}
	
	if (!isDispatch)
		m_iid = IID_NULL;
}
HRESULT __stdcall CGeneralEventSink::Invoke( 
		  DISPID  dispIdMember,
		  REFIID  riid,              
		  LCID  lcid,                
		  WORD  wFlags,              
		  DISPPARAMS FAR*  pDispParams,  
		  VARIANT FAR*  pVarResult,  
		  EXCEPINFO FAR*  pExcepInfo,  
		  unsigned int FAR*  puArgErr  
		  )
{
	CAtlStringW name;
	CAtlStringW param_list;
	if (!m_MemberNameMap.Lookup(dispIdMember,name))
	{
		name.Format(L"<%d>",dispIdMember);
	}

	for(unsigned int i=0;i<pDispParams->cArgs;++i)
	{
		CAtlStringW pname;
		VARIANT Var = pDispParams->rgvarg[i];
		VariantToString(Var,pname);
		if (i==0) param_list = pname;
		else param_list.AppendFormat(L", %s",pname.GetString());
	}
	//TraceW(L"OnEvent:%s(%s)\n",name.GetString(),param_list.GetString());
	if (m_pControlUI != NULL)
	{
		m_pControlUI->Event_Invoke(m_iid,name,pDispParams);
	}
	return E_NOTIMPL;
}

HRESULT CGeneralEventSink::GetIIDTypeInfo(IID iid,ITypeInfo ** ppInfo,IUnknown * pRelateObj)
{
	ITypeInfo * pObjTypeInfo;
	HRESULT hr;
	//得到pRelateObj的ITypeObject
	hr = pRelateObj->QueryInterface(IID_ITypeInfo,(void**)&pObjTypeInfo);
	if (FAILED(hr))
	{
		IProvideClassInfo * pProvideInfo;
		hr = pRelateObj->QueryInterface(IID_IProvideClassInfo,(void**)&pProvideInfo);
		if (FAILED(hr)) return hr;
		hr = pProvideInfo->GetClassInfo(&pObjTypeInfo);
		pProvideInfo->Release();
		if (FAILED(hr)) return hr;
	}
	//通过ITypeObject的RefType得到最终的ITypeInfo
	unsigned int uIndex;
	for(uIndex=0;;++uIndex)
	{
		HREFTYPE refType;
		hr = pObjTypeInfo->GetRefTypeOfImplType(uIndex,&refType);
		if (FAILED(hr)) break;
		hr = pObjTypeInfo->GetRefTypeInfo(refType,ppInfo);
		if (SUCCEEDED(hr))
		{
			//is the iid
			TYPEATTR * attr;
			ITypeInfo * ti = *ppInfo;//temp use
			if ( SUCCEEDED(ti->GetTypeAttr(&attr)) && attr->guid == iid)
			{
				pObjTypeInfo->Release();
				ti->ReleaseTypeAttr(attr);
				return S_OK;
			}
			else
			{
				ti->ReleaseTypeAttr(attr);
				ti->Release();
				*ppInfo = 0;
			}
		}
	}
	hr = E_FAIL;
	//通过pRelateObj的TypeLib
	ITypeLib * pLib = NULL;
	hr = pObjTypeInfo->GetContainingTypeLib(&pLib,&uIndex);
	if(SUCCEEDED(hr))
	{
		hr = pLib->GetTypeInfoOfGuid(iid,ppInfo);
	}
	if (pLib) pLib->Release();
	pObjTypeInfo->Release();
	return hr;
}

HRESULT CGeneralEventSink::VariantToString(VARIANT var,CAtlStringW& out)
{

	if (var.vt == VT_BSTR)
	{
		out.Format(L"\"%s\"",var.bstrVal);
		return S_OK;
	}
	if (var.vt == VT_EMPTY)
	{
		out = L"<EMPTY>";
		return S_OK;
	}
	if (var.vt == VT_NULL)
	{
		out = L"<NULL>";
		return S_OK;
	}
	VARIANT outvar;
	VariantInit(&outvar);
	HRESULT hr = VariantChangeType(&outvar,&var,VARIANT_ALPHABOOL|VARIANT_NOUSEROVERRIDE,VT_BSTR);
	if (SUCCEEDED(hr))
	{
		out = outvar.bstrVal;
		::VariantClear(&outvar);
		return S_OK;
	}
	out.Format(L"<Unknown:%d>",var.vt);
	return S_OK;
}

HRESULT CGeneralEventSink::IIDToString(IID iid,CAtlStringW& out)
{
	LPOLESTR striid;
	StringFromIID(iid,&striid);
	CAtlStringW RegPath;
	RegPath.Format(L"Interface\\%s",striid);
	WCHAR RegValue[512];
	LONG ValueLen=512;
	if (ERROR_SUCCESS == RegQueryValueW(HKEY_CLASSES_ROOT,RegPath.GetString(),RegValue,&ValueLen))
	{
		out = RegValue;
	}
	else
	{
		out = striid;
	}
	CoTaskMemFree(striid);
	return S_OK;
}