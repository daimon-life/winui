#include "stdafx.h"
#include "ControlEx.h"

namespace DuiLib{
	CShowAnimation::CShowAnimation()
	{
	}

	CShowAnimation::~CShowAnimation()
	{
	}

	void CShowAnimation::PaintBkImage(HDC hDC)
	{
		if( m_sBkImage.IsEmpty() ) 
			return;
		if( !DrawImageEx(hDC, m_pManager, m_rcItem, m_rcPaint, (LPCTSTR)m_sBkImage) ) 
			m_sBkImage.Empty();
	}

	void CShowAnimation::OnFrame()
	{
		Invalidate();
	}

	LPCTSTR CShowAnimation::GetClass() const
	{
		return _T("AnimationUI");
	}

	LPVOID CShowAnimation::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("Animation")) == 0 ) return static_cast<CShowAnimation*>(this);
		return CControlUI::GetInterface(pstrName);
	}
/************************************************************************/
/* CGroupButton µœ÷                                                                     */
/************************************************************************/


	CGrouopButtonUI::CGrouopButtonUI(void):m_bkgray(false)
	{
		m_bkimage.Resize(10);
	}

	CGrouopButtonUI::~CGrouopButtonUI(void)
	{
	}
	LPCTSTR CGrouopButtonUI::GetClass() const
	{
		return _T("GroupButtonUI");
	}

	LPVOID CGrouopButtonUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("GroupButton")) == 0 ) return static_cast<CGrouopButtonUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	void CGrouopButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	 {
		 CStdString strname(pstrName);
		 if (strname.Find( _T("bkimagenum")) != -1)
		 {
			 SetBKImageItem(pstrName,pstrValue);
		 }
		 else CButtonUI::SetAttribute(pstrName, pstrValue);
	 }

	void CGrouopButtonUI::SetBKImageItem(LPCTSTR pstrName, LPCTSTR pStrImage)
	{
		if ( !m_bkimage.Find(pstrName))
		{
			m_bkimage.Insert(pstrName,LPVOID(pStrImage));
		}
		else
		{
			m_bkimage.Set(pstrName,LPVOID(pStrImage));
		}
		
		Invalidate();
	}

	void CGrouopButtonUI::PaintBkImage(HDC hDC)
	{
		if( !m_sBkImage.IsEmpty() ){
			if(!m_bkgray)
			{
				if( !DrawImage(hDC, (LPCTSTR)m_sBkImage) ) m_sBkImage.Empty();
			}
			else
			{
				CRenderEngine::DrawImageString(hDC, m_pManager, m_rcItem, m_rcPaint, m_sBkImage, NULL,true);
			}
		}

		if (!m_sBkImagebuddy.IsEmpty())
		{
			if( !DrawImage(hDC, (LPCTSTR)m_sBkImagebuddy) ) m_sBkImagebuddy.Empty();
		}

 		for( int i = 0; i< m_bkimage.GetSize(); i++ ) {
 			if(LPCTSTR key = m_bkimage.GetAt(i)) {
 				if (!DrawImage(hDC,static_cast<LPCTSTR>(m_bkimage.Find(key))))
 				m_bkimage.Remove(key);
 			}
 		}
	}

	void CGrouopButtonUI::SetGrayBkImage(bool bgray)
	{
		if (m_bkgray == bgray)
			return;
		m_bkgray = bgray;
		Invalidate();
	}
}
