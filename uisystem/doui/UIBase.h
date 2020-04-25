#ifndef __UIBASE_H__
#define __UIBASE_H__


#pragma once

#define WF_TOOLTIPS         0x0001  // window is enabled for tooltips
#define WF_TEMPHIDE         0x0002  // window is temporarily hidden
#define WF_STAYDISABLED     0x0004  // window should stay disabled
#define WF_MODALLOOP        0x0008  // currently in modal loop
#define WF_CONTINUEMODAL    0x0010  // modal loop should continue running
#define WF_OLECTLCONTAINER  0x0100  // some descendant is an OLE control
#define WF_TRACKINGTOOLTIPS 0x0400  // window is enabled for tracking tooltips

//MSGLOOP
#define MLF_NOIDLEMSG       0x0001  // don't send WM_ENTERIDLE messages
#define MLF_NOKICKIDLE      0x0002  // don't send WM_KICKIDLE messages
#define MLF_SHOWONIDLE      0x0004  // show window if not visible at idle time

#define WM_KICKIDLE    0x036A

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//
#define _WTL_NO_CSTRING
#include <atlstr.h>

#if defined(_UNICODE)
	typedef std::basic_string<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t> > duistring;
#define STRVS(s)	( L#s )
#define enumStr2Num(v,x,d)  ((v == L#x)?x:d);
	typedef ATL::CAtlStringW	atlCString;
#define snprintf	_snwprintf
#define strtok		wcstok	
#else
	typedef std::basic_string<char,std::char_traits<char>,std::allocator<char> >		  duistring;
#define STRVS(s)	( s )
#define enumStr2Num(v,x,d)  ((v == x)?x:d);
	typedef ATL::CStringA	atlCString;
#define snprintf	_snprintf	
#define strtok		strtok
#endif

#define  ATLTYPES_INLINE inline


#define UI_WNDSTYLE_CONTAINER  (0)
#define UI_WNDSTYLE_FRAME      (WS_VISIBLE | WS_OVERLAPPEDWINDOW)
#define UI_WNDSTYLE_CHILD      (WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define UI_WNDSTYLE_DIALOG     (WS_VISIBLE | WS_POPUPWINDOW | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

#define UI_WNDSTYLE_EX_FRAME   (WS_EX_WINDOWEDGE)
#define UI_WNDSTYLE_EX_DIALOG  (WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

#define UI_CLASSSTYLE_CONTAINER  (0)
#define UI_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
#define UI_CLASSSTYLE_CHILD      (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define UI_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)


/////////////////////////////////////////////////////////////////////////////////////
//
#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

#ifdef _DEBUG
#ifndef TRACE
#define TRACE __Trace
#endif
#define TRACEMSG __TraceMsg
#else
#ifndef TRACE
#define TRACE
#endif
#define TRACEMSG _T("")
#endif

void UILIB_API __Trace(LPCTSTR pstrFormat, ...);
LPCTSTR UILIB_API __TraceMsg(UINT uMsg);

/////////////////////////////////////////////////////////////////////////////////////
//

// #define DuiLib_NAMESPACE 1
// 
// #if DuiLib_NAMESPACE
// #define  CDuiRect CDuiRect
// #define  CDuiPoint CDuiPoint
// #define  CDuiSize CDuiSize
// #else
// #define CDuiRect ::CDuiRect
// #define  CDuiPoint ::CDuiPoint
// #define  CDuiSize ::CDuiSize
// #endif

class STRINGorID
{
public:
    STRINGorID(LPCTSTR lpString) : m_lpstr(lpString)
    { }
    STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
    { }
    LPCTSTR m_lpstr;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CDuiPoint : public tagPOINT
{
public:
    CDuiPoint();
    CDuiPoint(const POINT& src);
    CDuiPoint(int x, int y);
    CDuiPoint(LPARAM lParam);

	// translate the point
	void Offset(int xOffset, int yOffset) throw();
	void Offset(POINT point) throw();
	void Offset(SIZE size) throw();
	void SetPoint(int X, int Y) throw();

	void operator+=(POINT point) throw();
	void operator-=(POINT point) throw();

	// Operators returning CPoint values
	CDuiPoint operator+(SIZE size) const throw();
	CDuiPoint operator+(POINT point) const throw();
	CDuiPoint operator-(SIZE size) const throw();
	CDuiPoint operator-(POINT point) const throw();
	CDuiPoint operator-() const throw();
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CDuiSize : public tagSIZE
{
public:
    CDuiSize();
    CDuiSize(const SIZE& src);
    CDuiSize(const RECT rc);
    CDuiSize(int cx, int cy);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CDuiRect : public tagRECT
{
public:
    CDuiRect();
    CDuiRect(const RECT& src);
	CDuiRect(POINT topLeft, POINT bottomRight) throw();
    CDuiRect(int iLeft, int iTop, int iRight, int iBottom);

    int GetWidth() const;
    int GetHeight() const;
    void Empty();
    bool IsNull() const;
    void Join(const RECT& rc);
    void ResetOffset();
    void Normalize();
    void Offset(int cx, int cy);
    void Inflate(int cx, int cy);
    void Deflate(int cx, int cy);
    void Union(CDuiRect& rc);
	BOOL Intersect(LPCRECT lpRect1, LPCRECT lpRect2);
	// returns TRUE if rectangle has no area
	BOOL IsRectEmpty() const throw();

	// TRUE if exactly the same as another rectangle
	BOOL EqualRect(LPCRECT lpRect) const throw();

	void InflateRect(int l, int t, int r, int b) throw();

	// reference to the top-left point
	POINT& TopLeft() throw();
	// reference to the bottom-right point
	POINT& BottomRight() throw();
	// const reference to the top-left point
	const POINT& TopLeft() const throw();
	// const reference to the bottom-right point
	const POINT& BottomRight() const throw();
	// the geometric center point of the rectangle
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CStdPtrArray
{
public:
    CStdPtrArray(int iPreallocSize = 0);
	CStdPtrArray(const CStdPtrArray& src);
    ~CStdPtrArray();

    void Empty();
    void Resize(int iSize);
    bool IsEmpty() const;
    int Find(LPVOID iIndex) const;
    bool Add(LPVOID pData);
    bool SetAt(int iIndex, LPVOID pData);
    bool InsertAt(int iIndex, LPVOID pData);
    bool Remove(int iIndex);
    int GetSize() const;
    LPVOID* GetData();

    LPVOID GetAt(int iIndex) const;
    LPVOID operator[] (int nIndex) const;

protected:
    LPVOID* m_ppVoid;
    int m_nCount;
    int m_nAllocated;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CStdValArray
{
public:
    CStdValArray(int iElementSize, int iPreallocSize = 0);
    ~CStdValArray();

    void Empty();
    bool IsEmpty() const;
    bool Add(LPCVOID pData);
    bool Remove(int iIndex);
    int GetSize() const;
    LPVOID GetData();

    LPVOID GetAt(int iIndex) const;
    LPVOID operator[] (int nIndex) const;

protected:
    LPBYTE m_pVoid;
    int m_iElementSize;
    int m_nCount;
    int m_nAllocated;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CStdString
{
public:
    enum { MAX_LOCAL_STRING_LEN = 63 };

    CStdString();
    CStdString(const TCHAR ch);
    CStdString(const CStdString& src);
    CStdString(LPCTSTR lpsz, int nLen = -1);
    ~CStdString();

    void Empty();
    int GetLength() const;
    bool IsEmpty() const;
    TCHAR GetAt(int nIndex) const;
    void Append(LPCTSTR pstr);
    void Assign(LPCTSTR pstr, int nLength = -1);
    LPCTSTR GetData();

    void SetAt(int nIndex, TCHAR ch);
    operator LPCTSTR() const;

    TCHAR operator[] (int nIndex) const;
    const CStdString& operator=(const CStdString& src);
    const CStdString& operator=(const TCHAR ch);
    const CStdString& operator=(LPCTSTR pstr);
#ifdef _UNICODE
    const CStdString& CStdString::operator=(LPCSTR lpStr);
    const CStdString& CStdString::operator+=(LPCSTR lpStr);
#else
    const CStdString& CStdString::operator=(LPCWSTR lpwStr);
    const CStdString& CStdString::operator+=(LPCWSTR lpwStr);
#endif
    CStdString operator+(const CStdString& src) const;
    CStdString operator+(LPCTSTR pstr) const;
    const CStdString& operator+=(const CStdString& src);
    const CStdString& operator+=(LPCTSTR pstr);
    const CStdString& operator+=(const TCHAR ch);

    bool operator == (LPCTSTR str) const;
    bool operator != (LPCTSTR str) const;
    bool operator <= (LPCTSTR str) const;
    bool operator <  (LPCTSTR str) const;
    bool operator >= (LPCTSTR str) const;
    bool operator >  (LPCTSTR str) const;

    int Compare(LPCTSTR pstr) const;
    int CompareNoCase(LPCTSTR pstr) const;

    void MakeUpper();
    void MakeLower();

    CStdString Left(int nLength) const;
    CStdString Mid(int iPos, int nLength = -1) const;
    CStdString Right(int nLength) const;

    int Find(TCHAR ch, int iPos = 0) const;
    int Find(LPCTSTR pstr, int iPos = 0) const;
    int ReverseFind(TCHAR ch) const;
    int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);

    int __cdecl Format(LPCTSTR pstrFormat, ...);
    int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);

protected:
    LPTSTR m_pstr;
    TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
};


/////////////////////////////////////////////////////////////////////////////////////
//

struct TITEM
{
    CStdString Key;
    LPVOID Data;
    struct TITEM* pNext;
};

class UILIB_API CStdStringPtrMap
{
public:
    CStdStringPtrMap(int nSize = 83);
    ~CStdStringPtrMap();

    void Resize(int nSize = 83);
    LPVOID Find(LPCTSTR key) const;
    bool Insert(LPCTSTR key, LPVOID pData);
    LPVOID Set(LPCTSTR key, LPVOID pData);
    bool Remove(LPCTSTR key);
    int GetSize() const;
    LPCTSTR GetAt(int iIndex) const;
    LPCTSTR operator[] (int nIndex) const;

protected:
    TITEM** m_aT;
    int m_nBuckets;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CWindowWnd
{
public:
    CWindowWnd();
    virtual ~CWindowWnd();

public:
    HWND GetHWND() const;
    operator HWND() const;

    bool RegisterWindowClass();
    bool RegisterSuperclass();

    HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
    HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
    HWND Subclass(HWND hWnd);
    void Unsubclass();
    void ShowWindow(bool bShow = true, bool bTakeFocus = true);
    UINT ShowModal(int nFlag = MSGF_DIALOGBOX); //默认1是对话框。2是菜单
    void Close(UINT nRet = IDOK);
    void CenterWindow();
    void SetIcon(UINT nRes);
	void SetIcon(LPCTSTR lpath);
	BOOL SetForegroundWindow();
	void MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE);

	void MoveWindow(int x,int y,int w, int h, BOOL bRepaint = TRUE);

	BOOL SetWindowText(LPCTSTR lptext);
	BOOL SetWindowText(HWND hwnd , LPCTSTR lptext);

	void BringWindowToTop();

    LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
    LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
    void ResizeClient(int cx = -1, int cy = -1);

	void EndDialog(int iResult = IDOK);

	int GetWindowFlag();

	HWND SetParent(HWND hwndNewParent);
	
protected:
	int RunModalLoop(DWORD dwFlags = 0);
	virtual BOOL ContinueModal();
	virtual void EndModalLoop(int nResult);
    virtual LPCTSTR GetWindowClassName() const = 0;
    virtual LPCTSTR GetSuperClassName() const;
    virtual UINT GetClassStyle() const;

	DWORD CWindowWnd::GetStyle() const;

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void OnFinalMessage(HWND hWnd);

    static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    HWND m_hWnd;
    WNDPROC m_OldWndProc;
    bool m_bSubclassed;
	LONG m_nModalResult; 
	int	m_nShowType;//菜单。还是模式对话框呢

public:
	HWND m_hWndOwner;   // implementation of SetOwner and GetOwner
	UINT m_nFlags;      // see WF_ flags above
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CWaitCursor
{
public:
    CWaitCursor();
    ~CWaitCursor();

protected:
    HCURSOR m_hOrigCursor;
};

} // namespace DuiLib

#endif // __UIBASE_H__
