#include "StdAfx.h"
#ifdef _DEBUG
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

void UILIB_API __Trace(LPCTSTR pstrFormat, ...)
{
#ifdef _DEBUG
    TCHAR szBuffer[300] = { 0 };
    va_list args;
    va_start(args, pstrFormat);
    ::wvnsprintf(szBuffer, lengthof(szBuffer) - 2, pstrFormat, args);
    _tcscat(szBuffer, _T("\n"));
    va_end(args);
    ::OutputDebugString(szBuffer);
#endif
}

LPCTSTR __TraceMsg(UINT uMsg)
{
#define MSGDEF(x) if(uMsg==x) return _T(#x)
    MSGDEF(WM_SETCURSOR);
    MSGDEF(WM_NCHITTEST);
    MSGDEF(WM_NCPAINT);
    MSGDEF(WM_PAINT);
    MSGDEF(WM_ERASEBKGND);
    MSGDEF(WM_NCMOUSEMOVE);  
    MSGDEF(WM_MOUSEMOVE);
    MSGDEF(WM_MOUSELEAVE);
    MSGDEF(WM_MOUSEHOVER);   
    MSGDEF(WM_NOTIFY);
    MSGDEF(WM_COMMAND);
    MSGDEF(WM_MEASUREITEM);
    MSGDEF(WM_DRAWITEM);   
    MSGDEF(WM_LBUTTONDOWN);
    MSGDEF(WM_LBUTTONUP);
    MSGDEF(WM_LBUTTONDBLCLK);
    MSGDEF(WM_RBUTTONDOWN);
    MSGDEF(WM_RBUTTONUP);
    MSGDEF(WM_RBUTTONDBLCLK);
    MSGDEF(WM_SETFOCUS);
    MSGDEF(WM_KILLFOCUS);  
    MSGDEF(WM_MOVE);
    MSGDEF(WM_SIZE);
    MSGDEF(WM_SIZING);
    MSGDEF(WM_MOVING);
    MSGDEF(WM_GETMINMAXINFO);
    MSGDEF(WM_CAPTURECHANGED);
    MSGDEF(WM_WINDOWPOSCHANGED);
    MSGDEF(WM_WINDOWPOSCHANGING);   
    MSGDEF(WM_NCCALCSIZE);
    MSGDEF(WM_NCCREATE);
    MSGDEF(WM_NCDESTROY);
    MSGDEF(WM_TIMER);
    MSGDEF(WM_KEYDOWN);
    MSGDEF(WM_KEYUP);
    MSGDEF(WM_CHAR);
    MSGDEF(WM_SYSKEYDOWN);
    MSGDEF(WM_SYSKEYUP);
    MSGDEF(WM_SYSCOMMAND);
    MSGDEF(WM_SYSCHAR);
    MSGDEF(WM_VSCROLL);
    MSGDEF(WM_HSCROLL);
    MSGDEF(WM_CHAR);
    MSGDEF(WM_SHOWWINDOW);
    MSGDEF(WM_PARENTNOTIFY);
    MSGDEF(WM_CREATE);
    MSGDEF(WM_NCACTIVATE);
    MSGDEF(WM_ACTIVATE);
    MSGDEF(WM_ACTIVATEAPP);   
    MSGDEF(WM_CLOSE);
    MSGDEF(WM_DESTROY);
    MSGDEF(WM_GETICON);   
    MSGDEF(WM_GETTEXT);
    MSGDEF(WM_GETTEXTLENGTH); 
	MSGDEF(WM_NCLBUTTONDBLCLK); 
	MSGDEF(WM_NCLBUTTONDOWN); 
	MSGDEF(WM_MOUSEACTIVATE);
	MSGDEF(WM_SYNCPAINT);
	MSGDEF(WM_IME_SETCONTEXT);
	MSGDEF(WM_IME_NOTIFY);
	MSGDEF(WM_APP + 1);
	

	

	
    static TCHAR szMsg[10];
    ::wsprintf(szMsg, _T("0x%04X"), uMsg);
    return szMsg;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CDuiPoint::CDuiPoint()
{
    x = y = 0;
}

CDuiPoint::CDuiPoint(const POINT& src)
{
    x = src.x;
    y = src.y;
}

CDuiPoint::CDuiPoint(int _x, int _y)
{
    x = _x;
    y = _y;
}

CDuiPoint::CDuiPoint(LPARAM lParam)
{
    x = GET_X_LPARAM(lParam);
    y = GET_Y_LPARAM(lParam);
}

ATLTYPES_INLINE void CDuiPoint::Offset(int xOffset, int yOffset) throw()
{ x += xOffset; y += yOffset; }
ATLTYPES_INLINE void CDuiPoint::Offset(POINT point) throw()
{ x += point.x; y += point.y; }
ATLTYPES_INLINE void CDuiPoint::Offset(SIZE size) throw()
{ x += size.cx; y += size.cy; }

ATLTYPES_INLINE void CDuiPoint::operator-=(POINT point) throw()
{ x -= point.x; y -= point.y; }

ATLTYPES_INLINE CDuiPoint CDuiPoint::operator+(SIZE size) const throw()
{ return CDuiPoint(x + size.cx, y + size.cy); }
ATLTYPES_INLINE CDuiPoint CDuiPoint::operator-(SIZE size) const throw()
{ return CDuiPoint(x - size.cx, y - size.cy); }
ATLTYPES_INLINE CDuiPoint CDuiPoint::operator-(POINT point) const throw()
{ return CDuiPoint(x - point.x, y - point.y); }

ATLTYPES_INLINE CDuiPoint CDuiPoint::operator+(POINT point) const throw()
{ return CDuiPoint(x + point.x, y + point.y); }



/////////////////////////////////////////////////////////////////////////////////////
//
//

CDuiSize::CDuiSize()
{
    cx = cy = 0;
}

CDuiSize::CDuiSize(const SIZE& src)
{
    cx = src.cx;
    cy = src.cy;
}

CDuiSize::CDuiSize(const RECT rc)
{
    cx = rc.right - rc.left;
    cy = rc.bottom - rc.top;
}

CDuiSize::CDuiSize(int _cx, int _cy)
{
    cx = _cx;
    cy = _cy;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CDuiRect::CDuiRect()
{
    left = top = right = bottom = 0;
}

CDuiRect::CDuiRect(const RECT& src)
{
    left = src.left;
    top = src.top;
    right = src.right;
    bottom = src.bottom;
}

CDuiRect::CDuiRect(POINT topLeft, POINT bottomRight) throw()
{ 
	left = topLeft.x; 
	top = topLeft.y;
	right = bottomRight.x;
	bottom = bottomRight.y; 
}

CDuiRect::CDuiRect(int iLeft, int iTop, int iRight, int iBottom)
{
    left = iLeft;
    top = iTop;
    right = iRight;
    bottom = iBottom;
}

int CDuiRect::GetWidth() const
{
    return right - left;
}

int CDuiRect::GetHeight() const
{
    return bottom - top;
}

void CDuiRect::Empty()
{
    left = top = right = bottom = 0;
}

bool CDuiRect::IsNull() const
{
    return (left == 0 && right == 0 && top == 0 && bottom == 0); 
}

void CDuiRect::Join(const RECT& rc)
{
    if( rc.left < left ) left = rc.left;
    if( rc.top < top ) top = rc.top;
    if( rc.right > right ) right = rc.right;
    if( rc.bottom > bottom ) bottom = rc.bottom;
}

void CDuiRect::ResetOffset()
{
    ::OffsetRect(this, -left, -top);
}

void CDuiRect::Normalize()
{
    if( left > right ) { int iTemp = left; left = right; right = iTemp; }
    if( top > bottom ) { int iTemp = top; top = bottom; bottom = iTemp; }
}

void CDuiRect::Offset(int cx, int cy)
{
    ::OffsetRect(this, cx, cy);
}

void CDuiRect::Inflate(int cx, int cy)
{
    ::InflateRect(this, cx, cy);
}

void CDuiRect::Deflate(int cx, int cy)
{
    ::InflateRect(this, -cx, -cy);
}

void CDuiRect::Union(CDuiRect& rc)
{
    ::UnionRect(this, this, &rc);
}

BOOL CDuiRect::Intersect(LPCRECT lpRect1, LPCRECT lpRect2)
{
	return ::IntersectRect(this, lpRect1, lpRect2);
}

BOOL CDuiRect::IsRectEmpty() const throw()
{
	return ::IsRectEmpty(this);
}

inline BOOL CDuiRect::EqualRect(LPCRECT lpRect) const throw()
{
	return ::EqualRect(this, lpRect); 
}

inline void CDuiRect::InflateRect(int l, int t, int r, int b) throw()
{
	left -= l;			top -= t;
	right += r;			bottom += b;
}

ATLTYPES_INLINE POINT& CDuiRect::TopLeft() throw()
{ return *((CDuiPoint*)this); }
ATLTYPES_INLINE POINT& CDuiRect::BottomRight() throw()
{ return *((CDuiPoint*)this+1); }
ATLTYPES_INLINE const POINT& CDuiRect::TopLeft() const throw()
{ return *((CDuiPoint*)this); }
ATLTYPES_INLINE const POINT& CDuiRect::BottomRight() const throw()
{ return *((CDuiPoint*)this+1); }

/////////////////////////////////////////////////////////////////////////////////////
//
//

CStdPtrArray::CStdPtrArray(int iPreallocSize) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize)
{
    ASSERT(iPreallocSize>=0);
    if( iPreallocSize > 0 ) m_ppVoid = static_cast<LPVOID*>(malloc(iPreallocSize * sizeof(LPVOID)));
}

CStdPtrArray::CStdPtrArray(const CStdPtrArray& src) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(0)
{
	for(int i=0; i<src.GetSize(); i++)
		Add(src.GetAt(i));
}

CStdPtrArray::~CStdPtrArray()
{
    if( m_ppVoid != NULL ) free(m_ppVoid);
}

void CStdPtrArray::Empty()
{
    if( m_ppVoid != NULL ) free(m_ppVoid);
    m_ppVoid = NULL;
    m_nCount = m_nAllocated = 0;
}

void CStdPtrArray::Resize(int iSize)
{
    Empty();
    m_ppVoid = static_cast<LPVOID*>(malloc(iSize * sizeof(LPVOID)));
    ::ZeroMemory(m_ppVoid, iSize * sizeof(LPVOID));
    m_nAllocated = iSize;
    m_nCount = iSize;
}

bool CStdPtrArray::IsEmpty() const
{
    return m_nCount == 0;
}

bool CStdPtrArray::Add(LPVOID pData)
{
    if( ++m_nCount >= m_nAllocated) {
        int nAllocated = m_nAllocated * 2;
        if( nAllocated == 0 ) nAllocated = 11;
        LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
        if( ppVoid != NULL ) {
            m_nAllocated = nAllocated;
            m_ppVoid = ppVoid;
        }
        else {
            --m_nCount;
            return false;
        }
    }
    m_ppVoid[m_nCount - 1] = pData;
    return true;
}

bool CStdPtrArray::InsertAt(int iIndex, LPVOID pData)
{
    if( iIndex == m_nCount ) return Add(pData);
    if( iIndex < 0 || iIndex > m_nCount ) return false;
    if( ++m_nCount >= m_nAllocated) {
        int nAllocated = m_nAllocated * 2;
        if( nAllocated == 0 ) nAllocated = 11;
        LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
        if( ppVoid != NULL ) {
            m_nAllocated = nAllocated;
            m_ppVoid = ppVoid;
        }
        else {
            --m_nCount;
            return false;
        }
    }
    memmove(&m_ppVoid[iIndex + 1], &m_ppVoid[iIndex], (m_nCount - iIndex - 1) * sizeof(LPVOID));
    m_ppVoid[iIndex] = pData;
    return true;
}

bool CStdPtrArray::SetAt(int iIndex, LPVOID pData)
{
    if( iIndex < 0 || iIndex >= m_nCount ) return false;
    m_ppVoid[iIndex] = pData;
    return true;
}

bool CStdPtrArray::Remove(int iIndex)
{
    if( iIndex < 0 || iIndex >= m_nCount ) return false;
    if( iIndex < --m_nCount ) ::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + 1, (m_nCount - iIndex) * sizeof(LPVOID));
    return true;
}

int CStdPtrArray::Find(LPVOID pData) const
{
    for( int i = 0; i < m_nCount; i++ ) if( m_ppVoid[i] == pData ) return i;
    return -1;
}

int CStdPtrArray::GetSize() const
{
    return m_nCount;
}

LPVOID* CStdPtrArray::GetData()
{
    return m_ppVoid;
}

LPVOID CStdPtrArray::GetAt(int iIndex) const
{
    if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
    return m_ppVoid[iIndex];
}

LPVOID CStdPtrArray::operator[] (int iIndex) const
{
    ASSERT(iIndex>=0 && iIndex<m_nCount);
    return m_ppVoid[iIndex];
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CStdValArray::CStdValArray(int iElementSize, int iPreallocSize /*= 0*/) : 
m_pVoid(NULL), 
m_nCount(0), 
m_iElementSize(iElementSize), 
m_nAllocated(iPreallocSize)
{
    ASSERT(iElementSize>0);
    ASSERT(iPreallocSize>=0);
    if( iPreallocSize > 0 ) m_pVoid = static_cast<LPBYTE>(malloc(iPreallocSize * m_iElementSize));
}

CStdValArray::~CStdValArray()
{
    if( m_pVoid != NULL ) free(m_pVoid);
}

void CStdValArray::Empty()
{   
    m_nCount = 0;  // NOTE: We keep the memory in place
}

bool CStdValArray::IsEmpty() const
{
    return m_nCount == 0;
}

bool CStdValArray::Add(LPCVOID pData)
{
    if( ++m_nCount >= m_nAllocated) {
        int nAllocated = m_nAllocated * 2;
        if( nAllocated == 0 ) nAllocated = 11;
        LPBYTE pVoid = static_cast<LPBYTE>(realloc(m_pVoid, nAllocated * m_iElementSize));
        if( pVoid != NULL ) {
            m_nAllocated = nAllocated;
            m_pVoid = pVoid;
        }
        else {
            --m_nCount;
            return false;
        }
    }
    ::CopyMemory(m_pVoid + ((m_nCount - 1) * m_iElementSize), pData, m_iElementSize);
    return true;
}

bool CStdValArray::Remove(int iIndex)
{
    if( iIndex < 0 || iIndex >= m_nCount ) return false;
    if( iIndex < --m_nCount ) ::CopyMemory(m_pVoid + (iIndex * m_iElementSize), m_pVoid + ((iIndex + 1) * m_iElementSize), (m_nCount - iIndex) * m_iElementSize);
    return true;
}

int CStdValArray::GetSize() const
{
    return m_nCount;
}

LPVOID CStdValArray::GetData()
{
    return static_cast<LPVOID>(m_pVoid);
}

LPVOID CStdValArray::GetAt(int iIndex) const
{
    if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
    return m_pVoid + (iIndex * m_iElementSize);
}

LPVOID CStdValArray::operator[] (int iIndex) const
{
    ASSERT(iIndex>=0 && iIndex<m_nCount);
    return m_pVoid + (iIndex * m_iElementSize);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CStdString::CStdString() : m_pstr(m_szBuffer)
{
    m_szBuffer[0] = '\0';
}

CStdString::CStdString(const TCHAR ch) : m_pstr(m_szBuffer)
{
    m_szBuffer[0] = ch;
    m_szBuffer[1] = '\0';
}

CStdString::CStdString(LPCTSTR lpsz, int nLen) : m_pstr(m_szBuffer)
{      
    ASSERT(!::IsBadStringPtr(lpsz,-1) || lpsz==NULL);
    m_szBuffer[0] = '\0';
    Assign(lpsz, nLen);
}

CStdString::CStdString(const CStdString& src) : m_pstr(m_szBuffer)
{
    m_szBuffer[0] = '\0';
    Assign(src.m_pstr);
}

CStdString::~CStdString()
{
    if( m_pstr != m_szBuffer ) free(m_pstr);
}

int CStdString::GetLength() const
{ 
    return (int) _tcslen(m_pstr); 
}

CStdString::operator LPCTSTR() const 
{ 
    return m_pstr; 
}

void CStdString::Append(LPCTSTR pstr)
{
    int nNewLength = GetLength() + (int) _tcslen(pstr);
    if( nNewLength >= MAX_LOCAL_STRING_LEN ) {
        if( m_pstr == m_szBuffer ) {
            m_pstr = static_cast<LPTSTR>(malloc((nNewLength + 1) * sizeof(TCHAR)));
            _tcscpy(m_pstr, m_szBuffer);
            _tcscat(m_pstr, pstr);
        }
        else {
            m_pstr = static_cast<LPTSTR>(realloc(m_pstr, (nNewLength + 1) * sizeof(TCHAR)));
            _tcscat(m_pstr, pstr);
        }
    }
    else {
        if( m_pstr != m_szBuffer ) {
            free(m_pstr);
            m_pstr = m_szBuffer;
        }
        _tcscat(m_szBuffer, pstr);
    }
}

void CStdString::Assign(LPCTSTR pstr, int cchMax)
{
    if( pstr == NULL ) pstr = _T("");
    cchMax = (cchMax < 0 ? (int) _tcslen(pstr) : cchMax);
    if( cchMax < MAX_LOCAL_STRING_LEN ) {
        if( m_pstr != m_szBuffer ) {
            free(m_pstr);
            m_pstr = m_szBuffer;
        }
    }
    else if( cchMax > GetLength() || m_pstr == m_szBuffer ) {
        if( m_pstr == m_szBuffer ) m_pstr = NULL;
        m_pstr = static_cast<LPTSTR>(realloc(m_pstr, (cchMax + 1) * sizeof(TCHAR)));
    }
    _tcsncpy(m_pstr, pstr, cchMax);
    m_pstr[cchMax] = '\0';
}

bool CStdString::IsEmpty() const 
{ 
    return m_pstr[0] == '\0'; 
}

void CStdString::Empty() 
{ 
    if( m_pstr != m_szBuffer ) free(m_pstr);
    m_pstr = m_szBuffer;
    m_szBuffer[0] = '\0'; 
}

LPCTSTR CStdString::GetData()
{
    return m_pstr;
}

TCHAR CStdString::GetAt(int nIndex) const
{
    return m_pstr[nIndex];
}

TCHAR CStdString::operator[] (int nIndex) const
{ 
    return m_pstr[nIndex];
}   

const CStdString& CStdString::operator=(const CStdString& src)
{      
    Assign(src);
    return *this;
}

const CStdString& CStdString::operator=(LPCTSTR lpStr)
{      
    ASSERT(!::IsBadStringPtr(lpStr,-1));
    Assign(lpStr);
    return *this;
}

#ifdef _UNICODE

const CStdString& CStdString::operator=(LPCSTR lpStr)
{
    ASSERT(!::IsBadStringPtrA(lpStr,-1));
    int cchStr = (int) strlen(lpStr) + 1;
    LPWSTR pwstr = (LPWSTR) _alloca(cchStr);
    if( pwstr != NULL ) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr) ;
    Assign(pwstr);
    return *this;
}

const CStdString& CStdString::operator+=(LPCSTR lpStr)
{
    ASSERT(!::IsBadStringPtrA(lpStr,-1));
    int cchStr = (int) strlen(lpStr) + 1;
    LPWSTR pwstr = (LPWSTR) _alloca(cchStr);
    if( pwstr != NULL ) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr) ;
    Append(pwstr);
    return *this;
}

#else

const CStdString& CStdString::operator=(LPCWSTR lpwStr)
{      
    ASSERT(!::IsBadStringPtrW(lpwStr,-1));
    int cchStr = ((int) wcslen(lpwStr) * 2) + 1;
    LPSTR pstr = (LPSTR) _alloca(cchStr);
    if( pstr != NULL ) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
    Assign(pstr);
    return *this;
}

const CStdString& CStdString::operator+=(LPCWSTR lpwStr)
{
    ASSERT(!::IsBadStringPtrW(lpwStr,-1));
    int cchStr = ((int) wcslen(lpwStr) * 2) + 1;
    LPSTR pstr = (LPSTR) _alloca(cchStr);
    if( pstr != NULL ) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
    Append(pstr);
    return *this;
}

#endif // _UNICODE

const CStdString& CStdString::operator=(const TCHAR ch)
{
    Empty();
    m_szBuffer[0] = ch;
    m_szBuffer[1] = '\0';
    return *this;
}

CStdString CStdString::operator+(const CStdString& src) const
{
    CStdString sTemp = *this;
    sTemp.Append(src);
    return sTemp;
}

CStdString CStdString::operator+(LPCTSTR lpStr) const
{
    ASSERT(!::IsBadStringPtr(lpStr,-1));
    CStdString sTemp = *this;
    sTemp.Append(lpStr);
    return sTemp;
}

const CStdString& CStdString::operator+=(const CStdString& src)
{      
    Append(src);
    return *this;
}

const CStdString& CStdString::operator+=(LPCTSTR lpStr)
{      
    ASSERT(!::IsBadStringPtr(lpStr,-1));
    Append(lpStr);
    return *this;
}

const CStdString& CStdString::operator+=(const TCHAR ch)
{      
    TCHAR str[] = { ch, '\0' };
    Append(str);
    return *this;
}

bool CStdString::operator == (LPCTSTR str) const { return (Compare(str) == 0); };
bool CStdString::operator != (LPCTSTR str) const { return (Compare(str) != 0); };
bool CStdString::operator <= (LPCTSTR str) const { return (Compare(str) <= 0); };
bool CStdString::operator <  (LPCTSTR str) const { return (Compare(str) <  0); };
bool CStdString::operator >= (LPCTSTR str) const { return (Compare(str) >= 0); };
bool CStdString::operator >  (LPCTSTR str) const { return (Compare(str) >  0); };

void CStdString::SetAt(int nIndex, TCHAR ch)
{
    ASSERT(nIndex>=0 && nIndex<GetLength());
    m_pstr[nIndex] = ch;
}

int CStdString::Compare(LPCTSTR lpsz) const 
{ 
    return _tcscmp(m_pstr, lpsz); 
}

int CStdString::CompareNoCase(LPCTSTR lpsz) const 
{ 
    return _tcsicmp(m_pstr, lpsz); 
}

void CStdString::MakeUpper() 
{ 
    _tcsupr(m_pstr); 
}

void CStdString::MakeLower() 
{ 
    _tcslwr(m_pstr); 
}

CStdString CStdString::Left(int iLength) const
{
    if( iLength < 0 ) iLength = 0;
    if( iLength > GetLength() ) iLength = GetLength();
    return CStdString(m_pstr, iLength);
}

CStdString CStdString::Mid(int iPos, int iLength) const
{
    if( iLength < 0 ) iLength = GetLength() - iPos;
    if( iPos + iLength > GetLength() ) iLength = GetLength() - iPos;
    if( iLength <= 0 ) return CStdString();
    return CStdString(m_pstr + iPos, iLength);
}

CStdString CStdString::Right(int iLength) const
{
    int iPos = GetLength() - iLength;
    if( iPos < 0 ) {
        iPos = 0;
        iLength = GetLength();
    }
    return CStdString(m_pstr + iPos, iLength);
}

int CStdString::Find(TCHAR ch, int iPos /*= 0*/) const
{
    ASSERT(iPos>=0 && iPos<=GetLength());
    if( iPos != 0 && (iPos < 0 || iPos >= GetLength()) ) return -1;
    LPCTSTR p = _tcschr(m_pstr + iPos, ch);
    if( p == NULL ) return -1;
    return (int)(p - m_pstr);
}

int CStdString::Find(LPCTSTR pstrSub, int iPos /*= 0*/) const
{
    ASSERT(!::IsBadStringPtr(pstrSub,-1));
    ASSERT(iPos>=0 && iPos<=GetLength());
    if( iPos != 0 && (iPos < 0 || iPos > GetLength()) ) return -1;
    LPCTSTR p = _tcsstr(m_pstr + iPos, pstrSub);
    if( p == NULL ) return -1;
    return (int)(p - m_pstr);
}

int CStdString::ReverseFind(TCHAR ch) const
{
    LPCTSTR p = _tcsrchr(m_pstr, ch);
    if( p == NULL ) return -1;
    return (int)(p - m_pstr);
}

int CStdString::Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo)
{
    CStdString sTemp;
    int nCount = 0;
    int iPos = Find(pstrFrom);
    if( iPos < 0 ) return 0;
    int cchFrom = (int) _tcslen(pstrFrom);
    int cchTo = (int) _tcslen(pstrTo);
    while( iPos >= 0 ) {
        sTemp = Left(iPos);
        sTemp += pstrTo;
        sTemp += Mid(iPos + cchFrom);
        Assign(sTemp);
        iPos = Find(pstrFrom, iPos + cchTo);
        nCount++;
    }
    return nCount;
}

int CStdString::Format(LPCTSTR pstrFormat, ...)
{
    CStdString sFormat = pstrFormat;
    // Do ordinary printf replacements
    // NOTE: Documented max-length of wvsprintf() is 1024
    TCHAR szBuffer[1025] = { 0 };
    va_list argList;
    va_start(argList, pstrFormat);
    int iRet = ::wvsprintf(szBuffer, sFormat, argList);
    va_end(argList);
    Assign(szBuffer);
    return iRet;
}

int CStdString::SmallFormat(LPCTSTR pstrFormat, ...)
{
    CStdString sFormat = pstrFormat;
    TCHAR szBuffer[64] = { 0 };
    va_list argList;
    va_start(argList, pstrFormat);
    int iRet = ::wvsprintf(szBuffer, sFormat, argList);
    va_end(argList);
    Assign(szBuffer);
    return iRet;
}

/////////////////////////////////////////////////////////////////////////////
//
//

static UINT HashKey(LPCTSTR Key)
{
    UINT i = 0;
    SIZE_T len = _tcslen(Key);
    while( len-- > 0 ) i = (i << 5) + i + Key[len];
    return i;
}

static UINT HashKey(const CStdString& Key)
{
    return HashKey((LPCTSTR)Key);
};

CStdStringPtrMap::CStdStringPtrMap(int nSize)
{
    if( nSize < 16 ) nSize = 16;
    m_nBuckets = nSize;
    m_aT = new TITEM*[nSize];
    memset(m_aT, 0, nSize * sizeof(TITEM*));
}

CStdStringPtrMap::~CStdStringPtrMap()
{
	if( m_aT ) {
		int len = m_nBuckets;
		while( len-- ) {
			TITEM* pItem = m_aT[len];
			while( pItem ) {
				TITEM* pKill = pItem;
				pItem = pItem->pNext;
				delete pKill;
			}
		}
		delete [] m_aT;
		m_aT = NULL;
	}
}

void CStdStringPtrMap::Resize(int nSize)
{
    if( m_aT ) {
        int len = m_nBuckets;
        while( len-- ) {
            TITEM* pItem = m_aT[len];
            while( pItem ) {
                TITEM* pKill = pItem;
                pItem = pItem->pNext;
                delete pKill;
            }
        }
        delete [] m_aT;
        m_aT = NULL;
    }
    
    if( nSize < 0 ) nSize = 0;
    if( nSize > 0 ) {
        m_aT = new TITEM*[nSize];
        memset(m_aT, 0, nSize * sizeof(TITEM*));
    } 
    m_nBuckets = nSize;
}

LPVOID CStdStringPtrMap::Find(LPCTSTR key) const
{
    if( m_nBuckets == 0 ) return NULL;

    UINT slot = HashKey(key) % m_nBuckets;
    for( const TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) {
        if( pItem->Key == key ) {
            return pItem->Data;
        }        
    }

    return NULL;
}

bool CStdStringPtrMap::Insert(LPCTSTR key, LPVOID pData)
{
    if( m_nBuckets == 0 ) return false;
    if( Find(key) ) return false;

    // Add first in bucket
    UINT slot = HashKey(key) % m_nBuckets;
    TITEM* pItem = new TITEM;
    pItem->Key = key;
    pItem->Data = pData;
    pItem->pNext = m_aT[slot];
    m_aT[slot] = pItem;
    return true;
}

LPVOID CStdStringPtrMap::Set(LPCTSTR key, LPVOID pData)
{
    if( m_nBuckets == 0 ) return pData;

    UINT slot = HashKey(key) % m_nBuckets;
    // Modify existing item
    for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) {
        if( pItem->Key == key ) {
            LPVOID pOldData = pItem->Data;
            pItem->Data = pData;
            return pOldData;
        }
    }

    Insert(key, pData);
    return NULL;
}

bool CStdStringPtrMap::Remove(LPCTSTR key)
{
    if( m_nBuckets == 0 ) return false;

    UINT slot = HashKey(key) % m_nBuckets;
    TITEM** ppItem = &m_aT[slot];
    while( *ppItem ) {
        if( (*ppItem)->Key == key ) {
            TITEM* pKill = *ppItem;
            *ppItem = (*ppItem)->pNext;
            delete pKill;
            return true;
        }
        ppItem = &((*ppItem)->pNext);
    }

    return false;
}

int CStdStringPtrMap::GetSize() const
{
    int nCount = 0;
    int len = m_nBuckets;
    while( len-- ) {
        for( const TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) nCount++;
    }

    return nCount;
}

LPCTSTR CStdStringPtrMap::GetAt(int iIndex) const
{
    int pos = 0;
    int len = m_nBuckets;
    while( len-- ) {
        for( TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) {
            if( pos++ == iIndex ) {
                return pItem->Key.GetData();
            }
        }
    }

    return NULL;
}

LPCTSTR CStdStringPtrMap::operator[] (int nIndex) const
{
    return GetAt(nIndex);
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CWindowWnd::CWindowWnd() : m_hWnd(NULL), m_OldWndProc(::DefWindowProc), m_bSubclassed(false),m_nShowType(MSGF_DIALOGBOX)
{
	m_nFlags = 0;
	m_nModalResult = 0;
}

CWindowWnd::~CWindowWnd()
{
}

HWND CWindowWnd::GetHWND() const 
{ 
    return m_hWnd; 
}

UINT CWindowWnd::GetClassStyle() const
{
    return 0;
}

LPCTSTR CWindowWnd::GetSuperClassName() const
{
    return NULL;
}

CWindowWnd::operator HWND() const
{
    return m_hWnd;
}

HWND CWindowWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu)
{
    return Create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMenu);
}

HWND CWindowWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu)
{
    if( GetSuperClassName() != NULL && !RegisterSuperclass() ) return NULL;
    if( GetSuperClassName() == NULL && !RegisterWindowClass() ) return NULL;
    m_hWnd = ::CreateWindowEx(dwExStyle, GetWindowClassName(), pstrName, dwStyle, x, y, cx, cy, hwndParent, hMenu, CPaintManagerUI::GetInstance(), this);
    ASSERT(m_hWnd!=NULL);
    return m_hWnd;
}

HWND CWindowWnd::Subclass(HWND hWnd)
{
    ASSERT(::IsWindow(hWnd));
    ASSERT(m_hWnd==NULL);
    m_OldWndProc = SubclassWindow(hWnd, __WndProc);
    if( m_OldWndProc == NULL ) return NULL;
    m_bSubclassed = true;
    m_hWnd = hWnd;
    ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
    return m_hWnd;
}


HWND CWindowWnd::SetParent(HWND hwndNewParent)
{
	if (::IsWindow(*this))
	return ::SetParent(*this,hwndNewParent);
	else 
		return HWND_DESKTOP;
}
void CWindowWnd::Unsubclass()
{
    ASSERT(::IsWindow(m_hWnd));
    if( !::IsWindow(m_hWnd) ) return;
    if( !m_bSubclassed ) return;
    SubclassWindow(m_hWnd, m_OldWndProc);
    m_OldWndProc = ::DefWindowProc;
    m_bSubclassed = false;
}

void CWindowWnd::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
{
    ASSERT(::IsWindow(m_hWnd));
    if( !::IsWindow(m_hWnd) ) return;
    ::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
}
int CWindowWnd::GetWindowFlag()
{
	return m_nShowType;
}

UINT CWindowWnd::ShowModal(int nFlag)
{

	m_nShowType = nFlag;
	m_nModalResult = -1;
	m_nFlags |= WF_CONTINUEMODAL;
	ASSERT(::IsWindow(m_hWnd));
	HWND hWndParent = GetWindowOwner(m_hWnd);

	BOOL bEnableParent = FALSE;

	BOOL bEnableMainWnd = FALSE;

	if (hWndParent && hWndParent != ::GetDesktopWindow() && ::IsWindowEnabled(hWndParent))
	{
		::EnableWindow(hWndParent, FALSE);
		bEnableParent = TRUE;
	}


	if (m_nFlags & WF_CONTINUEMODAL)
	{
		// enter modal loop
		DWORD dwFlags = MLF_SHOWONIDLE;
		if (GetStyle() & DS_NOIDLEMSG)
			dwFlags |= MLF_NOIDLEMSG;
		//ASSERT(RunModalLoop(dwFlags) == m_nModalResult);
		RunModalLoop(dwFlags);
	}

	// hide the window before enabling the parent, etc.
	if (m_hWnd != NULL)
		SetWindowPos(m_hWnd,NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
		SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);

	if (bEnableParent)
		::EnableWindow(hWndParent, TRUE);
    /*
	if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
		::SetActiveWindow(hWndParent);
        */
	int nResult = m_nModalResult;
	// destroy modal window
	DestroyWindow(m_hWnd);


	return nResult;
}

void CWindowWnd::Close(UINT nRet)
{
    ASSERT(::IsWindow(m_hWnd));
    if( !::IsWindow(m_hWnd) ) return;
    PostMessage(WM_CLOSE, (WPARAM)nRet, 0L);
}

void CWindowWnd::EndDialog(int iResult)
{
	ASSERT(::IsWindow(m_hWnd));
	::ShowWindow(m_hWnd, SW_HIDE);

	if (m_nFlags & (WF_MODALLOOP|WF_CONTINUEMODAL))
		EndModalLoop(iResult);

	::EndDialog(m_hWnd, iResult);
}

void CWindowWnd::CenterWindow()
{
    ASSERT(::IsWindow(m_hWnd));
    ASSERT((GetWindowStyle(m_hWnd)&WS_CHILD)==0);
    RECT rcDlg = { 0 };
    ::GetWindowRect(m_hWnd, &rcDlg);
    RECT rcArea = { 0 };
    RECT rcCenter = { 0 };
    HWND hWndParent = ::GetParent(m_hWnd);
    HWND hWndCenter = ::GetWindowOwner(m_hWnd);
    ::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
    if( hWndCenter == NULL ) rcCenter = rcArea; else ::GetWindowRect(hWndCenter, &rcCenter);

    int DlgWidth = rcDlg.right - rcDlg.left;
    int DlgHeight = rcDlg.bottom - rcDlg.top;

    // Find dialog's upper left based on rcCenter
    int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
    int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

    // The dialog is outside the screen, move it inside
    if( xLeft < rcArea.left ) xLeft = rcArea.left;
    else if( xLeft + DlgWidth > rcArea.right ) xLeft = rcArea.right - DlgWidth;
    if( yTop < rcArea.top ) yTop = rcArea.top;
    else if( yTop + DlgHeight > rcArea.bottom ) yTop = rcArea.bottom - DlgHeight;
    ::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void CWindowWnd::SetIcon(UINT nRes)
{
	SetIcon(MAKEINTRESOURCE(nRes));
}

void CWindowWnd::SetIcon(LPCTSTR lpath)
{
	HICON hIcon = (HICON)::LoadIcon(CPaintManagerUI::GetInstance(), lpath);
	ASSERT(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) TRUE, (LPARAM) hIcon);
	hIcon = (HICON)::LoadIcon(CPaintManagerUI::GetInstance(), lpath);
	ASSERT(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) FALSE, (LPARAM) hIcon);
}

BOOL CWindowWnd::SetForegroundWindow()
{
	ASSERT(::IsWindow(m_hWnd));
	return ::SetForegroundWindow(m_hWnd);
}

bool CWindowWnd::RegisterWindowClass()
{
    WNDCLASS wc = { 0 };
    wc.style = GetClassStyle();
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = NULL;
    wc.lpfnWndProc = CWindowWnd::__WndProc;
    wc.hInstance = CPaintManagerUI::GetInstance();
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = GetWindowClassName();
    ATOM ret = ::RegisterClass(&wc);
    ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}


DWORD CWindowWnd::GetStyle() const
{ 
	ASSERT(::IsWindow(m_hWnd)); return (DWORD)GetWindowLong(m_hWnd, GWL_STYLE);
}

static BOOL IsIdleMessage(MSG* pMsg)
{
	// These messages should NOT cause idle processing
	switch(pMsg->message)
	{
	case WM_MOUSEMOVE:
#ifndef _WIN32_WCE
	case WM_NCMOUSEMOVE:
#endif // !_WIN32_WCE
	case WM_PAINT:
	case 0x0118:	// WM_SYSTIMER (caret blink)
		return FALSE;
	}

	return TRUE;
}

int CWindowWnd::RunModalLoop(DWORD dwFlags)
{
	ASSERT(::IsWindow(m_hWnd)); // window must be created
	ASSERT(!(m_nFlags & WF_MODALLOOP)); // window must not already be in modal state

	// for tracking the idle time state
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;
	BOOL bShowIdle = (dwFlags & MLF_SHOWONIDLE) && !(GetStyle() & WS_VISIBLE);
	HWND hWndParent = ::GetParent(m_hWnd);
	m_nFlags |= (WF_MODALLOOP|WF_CONTINUEMODAL);
	MSG pMsg ={0};
BOOL bRet;
	// acquire and dispatch messages until the modal state is done
	for (;;)
	{
		ASSERT(ContinueModal());

		// phase1: check to see if we can do idle work
		while (bIdle &&
			!::PeekMessage(&pMsg, NULL, NULL, NULL, PM_NOREMOVE))
		{
			ASSERT(ContinueModal());

			// show the dialog when the message queue goes idle
			if (bShowIdle)
			{
				ShowWindow(SW_SHOWNORMAL);
				UpdateWindow(m_hWnd);
				bShowIdle = FALSE;
			}

			// call OnIdle while in bIdle state
			if (!(dwFlags & MLF_NOIDLEMSG) && hWndParent != NULL && lIdleCount == 0)
			{
				// send WM_ENTERIDLE to the parent
				::SendMessage(hWndParent, WM_ENTERIDLE, m_nShowType, (LPARAM)m_hWnd);
			}
			if ((dwFlags & MLF_NOKICKIDLE) ||
				!SendMessage(WM_KICKIDLE, m_nShowType, lIdleCount++))
			{
				// stop idle processing next time
				bIdle = FALSE;
			}
		}

		// phase2: pump messages while available
		do
		{
			ASSERT(ContinueModal());

			// pump message, but quit on WM_QUIT
			bRet = ::GetMessage(&pMsg, NULL, 0, 0);

			if(bRet == -1)
			{
				continue;   // error, don't process
			}
			else if(!bRet)
			{
				break;   // WM_QUIT, exit message loop
			}

			if( !CPaintManagerUI::TranslateMessage(&pMsg) ) 
			{	
				::TranslateMessage(&pMsg);

				::DispatchMessage(&pMsg);
			}

			// show the window when certain special messages rec'd
			if (bShowIdle &&
				(pMsg.message == 0x118 || pMsg.message == WM_SYSKEYDOWN))
			{
				ShowWindow(SW_SHOWNORMAL);
				UpdateWindow(m_hWnd);
				bShowIdle = FALSE;
			}

			if (!ContinueModal())
				goto ExitModal;

			// reset "no idle" state after pumping "normal" message
 			if (IsIdleMessage(&pMsg))
 			{
 				bIdle = TRUE;
 				lIdleCount = 0;
 			}

		} while (::PeekMessage(&pMsg, NULL, NULL, NULL, PM_NOREMOVE));
	}

ExitModal:
	m_nFlags &= ~(WF_MODALLOOP|WF_CONTINUEMODAL);
	return m_nModalResult;
}

BOOL CWindowWnd::ContinueModal()
{
	return m_nFlags & WF_CONTINUEMODAL;
}

void CWindowWnd::EndModalLoop(int nResult)
{
	ASSERT(::IsWindow(m_hWnd));

	// this result will be returned from CWnd::RunModalLoop
	m_nModalResult = nResult;

	// make sure a message goes through to exit the modal loop
	if (m_nFlags & WF_CONTINUEMODAL)
	{
		m_nFlags &= ~WF_CONTINUEMODAL;
		PostMessage(WM_NULL);
	}
}

bool CWindowWnd::RegisterSuperclass()
{
    // Get the class information from an existing
    // window so we can subclass it later on...
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    if( !::GetClassInfoEx(NULL, GetSuperClassName(), &wc) ) {
        if( !::GetClassInfoEx(CPaintManagerUI::GetInstance(), GetSuperClassName(), &wc) ) {
            ASSERT(!"Unable to locate window class");
            return NULL;
        }
    }
    m_OldWndProc = wc.lpfnWndProc;
    wc.lpfnWndProc = CWindowWnd::__ControlProc;
    wc.hInstance = CPaintManagerUI::GetInstance();
    wc.lpszClassName = GetWindowClassName();
    ATOM ret = ::RegisterClassEx(&wc);
    ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LRESULT CALLBACK CWindowWnd::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWindowWnd* pThis = NULL;
    if( uMsg == WM_NCCREATE ) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<CWindowWnd*>(lpcs->lpCreateParams);
        pThis->m_hWnd = hWnd;
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
    } 
    else {
        pThis = reinterpret_cast<CWindowWnd*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if( uMsg == WM_NCDESTROY && pThis != NULL ) {
            LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
            ::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
            if( pThis->m_bSubclassed ) pThis->Unsubclass();
            pThis->m_hWnd = NULL;
            pThis->OnFinalMessage(hWnd);
			//pThis->m_hWnd = NULL;
			pThis = NULL;
            return lRes;
        }
    }
    if( pThis != NULL ) {
		if ((DWORD)(pThis->m_hWnd) == 0||(DWORD)(pThis->m_hWnd) == 0xfeeefeee || (DWORD)(pThis->m_hWnd) == 0xcdcdcdcd)
		{
			return 0;
		}
        return pThis->HandleMessage(uMsg, wParam, lParam);
    } 
    else {
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

LRESULT CALLBACK CWindowWnd::__ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWindowWnd* pThis = NULL;
    if( uMsg == WM_NCCREATE ) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<CWindowWnd*>(lpcs->lpCreateParams);
        ::SetProp(hWnd, _T("CXWndX"), (HANDLE) pThis);
        pThis->m_hWnd = hWnd;
    } 
    else {
        pThis = reinterpret_cast<CWindowWnd*>(::GetProp(hWnd, _T("CXWndX")));
        if( uMsg == WM_NCDESTROY && pThis != NULL ) {
            LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
            if( pThis->m_bSubclassed ) pThis->Unsubclass();
            ::SetProp(hWnd, _T("CXWndX"), NULL);
            pThis->m_hWnd = NULL;
            pThis->OnFinalMessage(hWnd);
			//pThis->m_hWnd = NULL;
            return lRes;
        }
    }
    if( pThis != NULL ) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    } 
    else {
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

LRESULT CWindowWnd::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
    ASSERT(::IsWindow(m_hWnd));
	if(!::IsWindow(m_hWnd))
		return 0;
    return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
} 

LRESULT CWindowWnd::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
    ASSERT(::IsWindow(m_hWnd));
    return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
}

void CWindowWnd::ResizeClient(int cx /*= -1*/, int cy /*= -1*/)
{
    ASSERT(::IsWindow(m_hWnd));
    RECT rc = { 0 };
    if( !::GetClientRect(m_hWnd, &rc) ) return;
    if( cx != -1 ) rc.right = cx;
    if( cy != -1 ) rc.bottom = cy;
    if( !::AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd), (!(GetWindowStyle(m_hWnd) & WS_CHILD) && (::GetMenu(m_hWnd) != NULL)), GetWindowExStyle(m_hWnd)) ) return;
    ::SetWindowPos(m_hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

LRESULT CWindowWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
}

void CWindowWnd::OnFinalMessage(HWND /*hWnd*/)
{
}

void CWindowWnd::MoveWindow(LPCRECT lpRect, BOOL bRepaint)
{ 
	::MoveWindow(m_hWnd,lpRect->left, lpRect->top, lpRect->right - lpRect->left,
			 lpRect->bottom - lpRect->top, bRepaint); 
}

void CWindowWnd::MoveWindow(int x,int y,int w, int h, BOOL bRepaint)
{
	::MoveWindow(m_hWnd,x, y, w,
		h, bRepaint); 
}
BOOL CWindowWnd::SetWindowText(LPCTSTR lptext)
{
	ASSERT(::IsWindow(m_hWnd));
	return ::SetWindowText(m_hWnd, lptext);
}

BOOL CWindowWnd::SetWindowText(HWND hwnd ,LPCTSTR lptext)
{
	ASSERT(::IsWindow(hwnd));
	return ::SetWindowText(hwnd, lptext);
}

void CWindowWnd::BringWindowToTop()
{ ASSERT(::IsWindow(m_hWnd)); ::BringWindowToTop(m_hWnd); }



/////////////////////////////////////////////////////////////////////////////////////
//
//

CWaitCursor::CWaitCursor()
{
    m_hOrigCursor = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}

CWaitCursor::~CWaitCursor()
{
    ::SetCursor(m_hOrigCursor);
}

} // namespace DuiLib
