#ifndef __UIACTIVEX_H__
#define __UIACTIVEX_H__

#pragma once
#include "GeneralEventSink.h"

struct IOleObject;

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class IActiveXEvent
{
public:
	IActiveXEvent(){}
	~IActiveXEvent(){}
	virtual HRESULT	DoActiveXEvent(IID iid, CAtlStringW name, DISPPARAMS *  pDispParams)=0;
};

// #include <vector>
// using namespace std;
// class EventItem
// {
// public:
// 	EventItem(){}
// 	~EventItem(){}
// public:
//  	IID m_EventIID;
//  	CAtlStringW m_FunName;
//  	IActiveXEvent *m_pIActiveXEvent;
//  };
// typedef vector<EventItem> EventList;

//////////////////////////////////////////////////////////////////////////
class CActiveXCtrl;

template< class T >
class CSafeRelease
{
public:
    CSafeRelease(T* p) : m_p(p) { };
    ~CSafeRelease() { if( m_p != NULL ) m_p->Release(); };
    T* Detach() { T* t = m_p; m_p = NULL; return t; };
    T* m_p;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CActiveXUI : public CControlUI, public IMessageFilterUI
{
    friend class CActiveXCtrl;
public:
    CActiveXUI();
    virtual ~CActiveXUI();

    LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

    HWND GetHostWindow() const;

	bool IsDelayCreate() const;
	void SetDelayCreate(bool bDelayCreate = true);

	bool CreateControl(const CLSID clsid);
	bool CreateControl(LPCTSTR pstrCLSID);
	HRESULT GetControl(const IID iid, LPVOID* ppRet);
	CLSID GetClisd() const;
	CStdString GetModuleName() const;
	void SetModuleName(LPCTSTR pstrText);

    void SetVisible(bool bVisible = true);
    void SetInternVisible(bool bVisible = true);
    void SetPos(RECT rc);
    void DoPaint(HDC hDC, const RECT& rcPaint);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

public:
	void InsertEvent(IActiveXEvent *ptr);
	virtual HRESULT Event_Invoke(IID iid, CAtlStringW name, DISPPARAMS *  pDispParams);
protected:
    virtual void ReleaseControl();
    virtual bool DoCreateControl();

protected:
    CLSID m_clsid;
    CStdString m_sModuleName;
    bool m_bCreated;
    bool m_bDelayCreate;
    IOleObject* m_pUnk;
    CActiveXCtrl* m_pControl;
    HWND m_hwndHost;
	
	IActiveXEvent *m_pIActiveXEvent;
};

} // namespace DuiLib

#endif // __UIACTIVEX_H__
