#include "StdAfx.h"
#include "XLayout.h"

CXLayout::CXLayout(void)
: m_UserWnd(NULL)
{
}

CXLayout::~CXLayout(void)
{
}

void CXLayout::SetUserHwnd(HWND hwnd)
{
	if (m_UserWnd == hwnd)
		return;
	if (hwnd == NULL)
		return;
	if (!IsWindow(hwnd))
		return;
	m_UserWnd = hwnd;
}
void CXLayout::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);
	if(m_UserWnd != NULL && IsWindow(m_UserWnd))
	{

		TRACE(_T("ËÄ¸öÎ»ÖÃ:%d,%d,%d,%d"),m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
		::MoveWindow(m_UserWnd, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, TRUE);
	}
}

void CXLayout::SetVisible(bool bVisible)
{
	CControlUI::SetVisible(bVisible);
	if(m_UserWnd != NULL && IsWindow(m_UserWnd))
		::ShowWindow(m_UserWnd, IsVisible() ? SW_SHOW : SW_HIDE);
}

void CXLayout::SetInternVisible(bool bVisible)
{
	CControlUI::SetInternVisible(bVisible);
	if(m_UserWnd != NULL && IsWindow(m_UserWnd)) 
		::ShowWindow(m_UserWnd, IsVisible() ? SW_SHOW : SW_HIDE);
}

LPCTSTR CXLayout::GetClass() const
{
	return _T("XLayout");
}

LPVOID CXLayout::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("XLayout")) == 0 ) return static_cast<CXLayout*>(this);
	return CControlUI::GetInterface(pstrName);
}