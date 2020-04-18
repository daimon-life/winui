#include "StdAfx.h"
#include "XHitTestControl.h"

#define SPLIT_BAR_WIDTH	5
#define SPLIT_ZERO_RATIO 0.0001

#define SPLIT_TRACK_NO_HIT 0
#define SPLIT_TRACK_HIT_MIN 1
#define SPLIT_TRACK_HIT_MAX 2


#define SPLIT_STYLE_SIZE_LEFT		0x00000001				//size_left
#define SPLIT_STYLE_SIZE_RIGHT		0x00000002				//size_right
#define SPLIT_STYLE_SIZE_TOP		SPLIT_STYLE_SIZE_LEFT   //size_top
#define SPLIT_STYLE_SIZE_BOTTOM		SPLIT_STYLE_SIZE_RIGHT	//size_bottom
#define SPLIT_STYLE_SIZE_ALL		0x00000003				//size_all

namespace DuiLib {

	CXHitTestControl::CXHitTestControl(void)
		: m_bFirst(TRUE)
	{
		m_dwStyle = 0;

		m_bVert = TRUE;//默认为垂直。。。

		m_nSplitPos = 0;
		m_nMinItem1 = 0;
		m_nMinItem2 = 0;

		m_dbRatio = 0.0;

		m_bTracking = FALSE;
		m_dwTrackStatus = SPLIT_TRACK_NO_HIT;
		m_dwResizeStyle = 0;
        m_bCanResize = FALSE;

		m_stritem1.Empty();
		m_stritem2.Empty();
		SetStartPoint(0,0);
	}

	CXHitTestControl::~CXHitTestControl(void)
	{
	}

	void CXHitTestControl::SetStartPoint(int x, int y)
	{
		m_Startpt.x = x;
		m_Startpt.y = y;
	}
	void CXHitTestControl::SetStartPoint(POINT pt)
	{
		SetStartPoint(pt.x,pt.y);
	}

	LPVOID CXHitTestControl::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("SpliteWnd")) == 0 ) return static_cast<CXHitTestControl*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	void CXHitTestControl::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		BOOL bHandled(FALSE);
		if(_tcsicmp(pstrName , _T("split_style")) == 0)
		{
			DWORD dwSplitStyle(0);
			if(_tcsicmp(pstrValue,_T("size_left")) != 0)
			{
				dwSplitStyle = SPLIT_STYLE_SIZE_LEFT;
			}
			else if(_tcsicmp(pstrValue,_T("size_right")) != 0)
			{
				dwSplitStyle = SPLIT_STYLE_SIZE_RIGHT;
			}
			else if(_tcsicmp(pstrValue,_T("size_top")) != 0)
			{
				dwSplitStyle = SPLIT_STYLE_SIZE_TOP;
			}
			else if(_tcsicmp(pstrValue,_T("size_bottom")) != 0)
			{
				dwSplitStyle = SPLIT_STYLE_SIZE_BOTTOM;
			}
			else if(_tcsicmp(pstrValue,_T("size_all")) != 0)
			{
				dwSplitStyle = SPLIT_STYLE_SIZE_ALL;
			}


			SetResizeStyle(dwSplitStyle);
			bHandled = TRUE;
		}
		else if(_tcsicmp(pstrName ,_T("split_pos")) == 0)
		{
			INT nPos = _ttol(pstrValue);


			if(nPos < 0) nPos = 0;

			SetSplitPos(nPos);
			bHandled = TRUE;
		}
		else if(_tcsicmp(pstrName ,_T("split_min")) == 0)
		{
			TCHAR szValue[128] = {0};
			lstrcpyn(szValue, pstrValue, sizeof(szValue) / sizeof(TCHAR) - 1);
			LPTSTR pstr = szValue;
			INT nItem1 = _tcstol(pstr, &pstr, 10);    
			INT nItem2 = _tcstol(pstr + 1, &pstr, 10); 		

			SetSplitMinSize(nItem1, nItem2);
			bHandled = TRUE;
		}
		else if(_tcsicmp(pstrName ,_T("split_vertical")) == 0)
		{
			SetVertical(_tcscmp(pstrValue, _T("true")) == 0);
		}
		else if(_tcsicmp(pstrName ,_T("split_items")) == 0)
		{
			SetOperItem(pstrValue);
		} 
        else if (_tcsicmp(pstrName ,_T("CanResize")) == 0)
        {
            int  nCan = _ttol(pstrValue);
            if (0 == nCan)
            {
                m_bCanResize = FALSE;
            }
            else
            {
                m_bCanResize = TRUE;
            }
        }    
	}


	void CXHitTestControl::SetOperItem(LPCTSTR strvalue)
	{

		CStdString sValue;
		CStdString sItem;
		LPCTSTR pStrImage;


		pStrImage = strvalue;


		while( *pStrImage != _T('\0') ) {
			sItem.Empty();
			sValue.Empty();
			while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
			while( *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ') ) {
				LPTSTR pstrTemp = ::CharNext(pStrImage);
				while( pStrImage < pstrTemp) {
					sItem += *pStrImage++;
				}
			}
			while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
			if( *pStrImage++ != _T('=') ) break;
			while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
			if( *pStrImage++ != _T('\'') ) break;
			while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) {
				LPTSTR pstrTemp = ::CharNext(pStrImage);
				while( pStrImage < pstrTemp) {
					sValue += *pStrImage++;
				}
			}
			if( *pStrImage++ != _T('\'') ) break;
			if( !sValue.IsEmpty() ) {
				if( sItem == _T("items1")) {
					m_stritem1 = sValue;
				}
				else if( sItem == _T("items2")) {
					m_stritem2 = sValue;
				}
			}
		}
	}
	VOID CXHitTestControl::SetSplitPos(INT nPos)
	{
		//if(nPos < 0) nPos = 0;

		m_nSplitPos = nPos;

		if(IsRectEmpty(&m_rcItem)) return;

		LayoutSplit(nPos);
	}

	INT CXHitTestControl::GetSplitPos() const
	{
		return m_nSplitPos;
	}

	BOOL CXHitTestControl::GetSplitMinSize(INT& nItem1, INT& nItem2)
	{
		nItem1 = m_nMinItem1;
		nItem2 = m_nMinItem2;

		return TRUE;
	}

	VOID CXHitTestControl::SetSplitMinSize(INT nItem1, INT nItem2)
	{
		if(nItem1 < 0) nItem1 = 0;
		if(nItem2 < 0) nItem2 = 0;

		m_nMinItem1 = nItem1;
		m_nMinItem2 = nItem2;
	}


	VOID CXHitTestControl::LayoutSplit(INT nPos)
	{

		if (m_item1.pcontrl == NULL || m_item2.pcontrl == NULL)
			return;
		
		int height1 , height2 ;
		height1 = m_item1.length;
		height2 = m_item2.length;

		
		TRACE(_T("现在进来的POS是%d \r\n"),nPos);
		if (IsVertical())
		{
			if (height1 == 0 )
			{
				int height = m_item1.pcontrl->GetHeight();
				if (height <= 0)
				{
					return;
				}
				if (height + nPos <= m_nMinItem1)
				{
					TRACE(_T("判断heght1是%d 我在这返回了第一步这\r\n"),height1 + nPos);
					return;
				}
			}
			else 
			{
				if (height1 + nPos <= m_nMinItem1)
				{
					TRACE(_T("判断heght1是%d 我在这退出了\r\n"),height1 + nPos);
					return;
				}
			}

			if (height2 == 0)
			{
				int height = m_item2.pcontrl->GetHeight();
				if (height <= 0)
				{
					return;
				}
				if (height - nPos <= m_nMinItem2)
				{
					TRACE(_T("判断heght2是%d 我在这返回了\r\n"),height - nPos);
					return;
				}
			}
			else
			{
				if (height2 - nPos <= m_nMinItem2)
				{
					TRACE(_T("判断heght2是%d 我在这退出了\r\n"),height2 - nPos);
					return;
				}
			}
		}
		if (!m_item1.bVariabled && !m_item1.pcontrl->IsFloat())
		{
			if (IsVertical())
			{
				m_item1.pcontrl->SetFixedHeight( m_item1.length + nPos);

				TRACE(_T("现在画的是高度item1是%d \r\n"),m_item1.length + nPos);
			}
			else
				m_item1.pcontrl->SetFixedWidth( m_item1.length + nPos);
		}

		if (!m_item2.bVariabled && !m_item2.pcontrl->IsFloat())
		{
			if (IsVertical())
			{
				m_item2.pcontrl->SetFixedHeight( m_item2.length - nPos);
				TRACE(_T("现在画的是高度item2是%d \r\n"),m_item2.length - nPos);
			}
			else
				m_item2.pcontrl->SetFixedWidth( m_item2.length - nPos);
		}
	}

	

	LRESULT CXHitTestControl::OnLButtonDown(const TEventUI& info)
	{
		if (!m_stritem1.IsEmpty() && !m_stritem2.IsEmpty())
		{
			SetItems(m_stritem1,m_stritem2);
		}
		m_dwTrackStatus = SPLIT_TRACK_NO_HIT;
		m_bTracking = TRUE;

		SetStartPoint(info.ptMouse);
		return S_OK;
	}

	LRESULT CXHitTestControl::OnLButtonUp(const TEventUI& info)
	{
		m_dwTrackStatus = SPLIT_TRACK_NO_HIT;
		m_bTracking = FALSE;

		return S_OK;
	}

	VOID CXHitTestControl::GetSplitterRect(RECT& rtSplitter)
	{

		if(IsVertical())
		{
			rtSplitter.left = m_rcItem.left;
			rtSplitter.right = m_rcItem.right;
			rtSplitter.top = m_rcItem.top;
			rtSplitter.bottom = rtSplitter.top + SPLIT_BAR_WIDTH;
		}
		else
		{
			rtSplitter.left = m_rcItem.left;
			rtSplitter.right = rtSplitter.left + SPLIT_BAR_WIDTH;
			rtSplitter.top = m_rcItem.top;
			rtSplitter.bottom = m_rcItem.bottom;
		}
	}

	LRESULT CXHitTestControl::OnMouseMove(const TEventUI& info)
	{
		if(!m_bTracking) return S_OK;
		if(!IsVertical())
		{
			INT nOffset = info.ptMouse.x - m_Startpt.x;
			SetSplitPos(nOffset);
		}
		else
		{
			INT nOffset = info.ptMouse.y - m_Startpt.y;
			SetSplitPos(nOffset);
		}
		return S_OK;
	}

	VOID CXHitTestControl::SaveCurrentRadtio()
	{
		if(!(GetResizeStyle() & SPLIT_STYLE_SIZE_ALL)) return;
		RECT rtAvailable = m_rcItem;
		if (!IsRectEmpty(&rtAvailable))
		{
			return;
		}
		if(IsVertical())
		{
			m_dbRatio = m_nSplitPos / (double)(rtAvailable.bottom - rtAvailable.top);
		}
		else
		{
			m_dbRatio = m_nSplitPos / (double)(rtAvailable.right - rtAvailable.left);
		}

	}

	VOID CXHitTestControl::SetResizeStyle(DWORD dwStyle)
	{
		m_dwResizeStyle = dwStyle;
	}

	DWORD CXHitTestControl::GetResizeStyle() const
	{
		return m_dwResizeStyle;
	}
	void CXHitTestControl::SetVertical(BOOL bvert)
	{
		m_bVert = bvert;
	}

	BOOL CXHitTestControl::IsVertical()
	{
		return m_bVert;
	}

	UINT CXHitTestControl::GetControlFlags() const
	{
		return(IsEnabled() ? UIFLAG_SETCURSOR : 0);
	}


	void CXHitTestControl::SetItems(CControlUI* pcontrol1 , CControlUI* pcontrol2 )
	{
		ASSERT(pcontrol1 != NULL && pcontrol2 != NULL);
		m_item1.pcontrl = pcontrol1;
		m_item2.pcontrl = pcontrol2;

		if (IsVertical())
		{
			m_item1.length = pcontrol1->GetFixedHeight();
			if (m_item1.length == 0)
			{
				m_item1.bVariabled = TRUE;
				m_item1.length = pcontrol1->GetHeight();
			}
			else
				m_item1.bVariabled = FALSE;
			m_item2.length = pcontrol2->GetFixedHeight();
			if (m_item2.length == 0)
			{
				m_item2.bVariabled = TRUE;
				m_item2.length = pcontrol2->GetHeight();
			}
			else
				m_item2.bVariabled = FALSE;
		}
		else
		{

			m_item1.length = pcontrol1->GetFixedWidth();
			if (m_item1.length == 0)
			{
				m_item1.bVariabled = TRUE;
				m_item1.length = pcontrol1->GetWidth();
			}
			else
				m_item1.bVariabled = FALSE;
			m_item2.length = pcontrol2->GetFixedWidth();
			if (m_item2.length == 0)
			{
				m_item2.bVariabled = TRUE;
				m_item2.length = pcontrol2->GetWidth();
			}
			else
				m_item2.bVariabled = FALSE;
		}
	}

	void CXHitTestControl::SetItems(LPCTSTR lp1 , LPCTSTR lp2)
	{
		CPaintManagerUI* pmanger = this->GetManager();
		if (pmanger)
		{
			CControlUI* pui1 = pmanger->FindControl(lp1);
			CControlUI* pui2 = pmanger->FindControl(lp2);

			if (pui1 == NULL || pui2 == NULL)
				return;
			SetItems(pui1,pui2);
		}
	}

	void CXHitTestControl::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_BUTTONDOWN ) 
		{
			RECT rtSplitter = {0};
			GetSplitterRect(rtSplitter);

			if(::PtInRect(&rtSplitter, event.ptMouse))
			{
				OnLButtonDown(event);
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP) 
		{
			OnLButtonUp(event);
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{

			OnMouseMove(event);
			return;
		}


		if( event.Type == UIEVENT_SETCURSOR )
		{
			RECT rtSplitter = {0};
			GetSplitterRect(rtSplitter);

            if (!m_bCanResize)
                return;
             
			if(::PtInRect(&rtSplitter, event.ptMouse) )
			{
				if(IsVertical())
				{
					::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
				}
				else
				{
					::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
				}
				return;
			}
		}
		CControlUI::DoEvent(event);
	}

};