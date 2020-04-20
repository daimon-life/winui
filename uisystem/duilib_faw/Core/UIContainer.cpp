﻿#include "StdAfx.h"

namespace DuiLib {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL (CContainerUI)

	CContainerUI::CContainerUI () {}

	CContainerUI::~CContainerUI () {
		m_bDelayedDestroy = false;
		RemoveAll ();
		if (m_pVerticalScrollBar) {
			delete m_pVerticalScrollBar;
			m_pVerticalScrollBar = nullptr;
		}
		if (m_pHorizontalScrollBar) {
			delete m_pHorizontalScrollBar;
			m_pHorizontalScrollBar = nullptr;
		}
	}

	faw::string_view_t CContainerUI::GetClass () const {
		return _T ("ContainerUI");
	}

	LPVOID CContainerUI::GetInterface (faw::string_view_t pstrName) {
		if (pstrName == _T ("IContainer")) return static_cast<IContainerUI*>(this);
		else if (pstrName == DUI_CTRL_CONTAINER) return static_cast<CContainerUI*>(this);
		return CControlUI::GetInterface (pstrName);
	}

	CControlUI* CContainerUI::GetItemAt (int iIndex) const {
		if (iIndex < 0 || iIndex >= m_items.GetSize ()) return nullptr;
		return static_cast<CControlUI*>(m_items[iIndex]);
	}

	int CContainerUI::GetItemIndex (CControlUI* pControl) const {
		for (int it = 0; it < m_items.GetSize (); it++) {
			if (static_cast<CControlUI*>(m_items[it]) == pControl) {
				return it;
			}
		}

		return -1;
	}

	bool CContainerUI::SetItemIndex (CControlUI* pControl, int iIndex) {
		for (int it = 0; it < m_items.GetSize (); it++) {
			if (static_cast<CControlUI*>(m_items[it]) == pControl) {
				NeedUpdate ();
				m_items.Remove (it);
				return m_items.InsertAt (iIndex, pControl);
			}
		}

		return false;
	}

	int CContainerUI::GetCount () const {
		return m_items.GetSize ();
	}

	bool CContainerUI::Add (CControlUI* pControl) {
		if (!pControl) return false;

		if (m_pManager) m_pManager->InitControls (pControl, this);
		if (IsVisible ()) NeedUpdate ();
		else pControl->SetInternVisible (false);
		return m_items.Add (pControl);
	}

	bool CContainerUI::AddAt (CControlUI* pControl, int iIndex) {
		if (!pControl) return false;

		if (m_pManager) m_pManager->InitControls (pControl, this);
		if (IsVisible ()) NeedUpdate ();
		else pControl->SetInternVisible (false);
		return m_items.InsertAt (iIndex, pControl);
	}

	bool CContainerUI::Remove (CControlUI* pControl) {
		if (!pControl) return false;

		for (int it = 0; it < m_items.GetSize (); it++) {
			if (static_cast<CControlUI*>(m_items[it]) == pControl) {
				NeedUpdate ();
				if (m_bAutoDestroy) {
					if (m_bDelayedDestroy && m_pManager) m_pManager->AddDelayedCleanup (pControl);
					else delete pControl;
				}
				return m_items.Remove (it);
			}
		}
		return false;
	}

	bool CContainerUI::RemoveAt (int iIndex) {
		CControlUI* pControl = GetItemAt (iIndex);
		if (pControl) {
			return CContainerUI::Remove (pControl);
		}

		return false;
	}

	void CContainerUI::RemoveAll () {
		for (int it = 0; m_bAutoDestroy && it < m_items.GetSize (); it++) {
			CControlUI* pItem = static_cast<CControlUI*>(m_items[it]);
			if (m_bDelayedDestroy && m_pManager) {
				m_pManager->AddDelayedCleanup (pItem);
			} else {
				delete pItem;
				pItem = nullptr;
			}
		}
		m_items.Empty ();
		NeedUpdate ();
	}

	bool CContainerUI::IsAutoDestroy () const {
		return m_bAutoDestroy;
	}

	void CContainerUI::SetAutoDestroy (bool bAuto) {
		m_bAutoDestroy = bAuto;
	}

	bool CContainerUI::IsDelayedDestroy () const {
		return m_bDelayedDestroy;
	}

	void CContainerUI::SetDelayedDestroy (bool bDelayed) {
		m_bDelayedDestroy = bDelayed;
	}

	RECT CContainerUI::GetInset () const {
		if (m_pManager) return m_pManager->GetDPIObj ()->Scale (m_rcInset);
		return m_rcInset;
	}

	void CContainerUI::SetInset (RECT rcInset) {
		m_rcInset = rcInset;
		NeedUpdate ();
	}

	int CContainerUI::GetChildPadding () const {
		if (m_pManager) return m_pManager->GetDPIObj ()->Scale (m_iChildPadding);
		return m_iChildPadding;
	}


	void CContainerUI::SetChildPadding (int iPadding) {
		m_iChildPadding = iPadding;
		NeedUpdate ();
	}

	UINT CContainerUI::GetChildAlign (CControlUI *pControl) const {
		if (pControl) {
			UINT iChildFloatAlign = pControl->GetFloatAlign ();
			return (iChildFloatAlign == DT_LEFT ? m_iChildAlign : iChildFloatAlign);
		}
		return m_iChildAlign;
	}

	void CContainerUI::SetChildAlign (UINT iAlign) {
		m_iChildAlign = iAlign;
		NeedUpdate ();
	}

	UINT CContainerUI::GetChildVAlign () const {
		return m_iChildVAlign;
	}

	void CContainerUI::SetChildVAlign (UINT iVAlign) {
		m_iChildVAlign = iVAlign;
		NeedUpdate ();
	}

	bool CContainerUI::IsMouseChildEnabled () const {
		return m_bMouseChildEnabled;
	}

	void CContainerUI::SetMouseChildEnabled (bool bEnable) {
		m_bMouseChildEnabled = bEnable;
	}

	void CContainerUI::SetVisible (bool bVisible) {
		if (m_bVisible == bVisible) return;
		CControlUI::SetVisible (bVisible);
		for (int it = 0; it < m_items.GetSize (); it++) {
			static_cast<CControlUI*>(m_items[it])->SetInternVisible (IsVisible ());
		}
	}

	// 逻辑上，对于Container控件不公开此方法
	// 调用此方法的结果是，内部子控件隐藏，控件本身依然显示，背景等效果存在
	void CContainerUI::SetInternVisible (bool bVisible) {
		CControlUI::SetInternVisible (bVisible);
		if (m_items.empty ()) return;
		for (int it = 0; it < m_items.GetSize (); it++) {
			// 控制子控件显示状态
			// InternVisible状态应由子控件自己控制
			static_cast<CControlUI*>(m_items[it])->SetInternVisible (IsVisible ());
		}
	}

	void CContainerUI::SetEnabled (bool bEnabled) {
		if (m_bEnabled == bEnabled) return;

		m_bEnabled = bEnabled;

		for (int it = 0; it < m_items.GetSize (); it++) {
			static_cast<CControlUI*>(m_items[it])->SetEnabled (m_bEnabled);
		}

		Invalidate ();
	}

	void CContainerUI::SetMouseEnabled (bool bEnabled) {
		if (m_pVerticalScrollBar) m_pVerticalScrollBar->SetMouseEnabled (bEnabled);
		if (m_pHorizontalScrollBar) m_pHorizontalScrollBar->SetMouseEnabled (bEnabled);
		CControlUI::SetMouseEnabled (bEnabled);
	}

	void CContainerUI::DoEvent (TEventUI& event) {
		if (!IsMouseEnabled () && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pParent) m_pParent->DoEvent (event);
			else CControlUI::DoEvent (event);
			return;
		}

		if (event.Type == UIEVENT_SETFOCUS) {
			m_bFocused = true;
			return;
		}
		if (event.Type == UIEVENT_KILLFOCUS) {
			m_bFocused = false;
			return;
		}
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible () && m_pVerticalScrollBar->IsEnabled ()) {
			if (event.Type == UIEVENT_KEYDOWN) {
				switch (event.chKey) {
				case VK_DOWN:
					LineDown ();
					return;
				case VK_UP:
					LineUp ();
					return;
				case VK_NEXT:
					PageDown ();
					return;
				case VK_PRIOR:
					PageUp ();
					return;
				case VK_HOME:
					HomeUp ();
					return;
				case VK_END:
					EndDown ();
					return;
				}
			} else if (event.Type == UIEVENT_SCROLLWHEEL) {
				switch (LOWORD (event.wParam)) {
				case SB_LINEUP:
					LineUp ();
					return;
				case SB_LINEDOWN:
					LineDown ();
					return;
				}
			}
		}
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible () && m_pHorizontalScrollBar->IsEnabled ()) {
			if (event.Type == UIEVENT_KEYDOWN) {
				switch (event.chKey) {
				case VK_DOWN:
					LineRight ();
					return;
				case VK_UP:
					LineLeft ();
					return;
				case VK_NEXT:
					PageRight ();
					return;
				case VK_PRIOR:
					PageLeft ();
					return;
				case VK_HOME:
					HomeLeft ();
					return;
				case VK_END:
					EndRight ();
					return;
				}
			} else if (event.Type == UIEVENT_SCROLLWHEEL) {
				switch (LOWORD (event.wParam)) {
				case SB_LINEUP:
					LineLeft ();
					return;
				case SB_LINEDOWN:
					LineRight ();
					return;
				}
			}
		}
		CControlUI::DoEvent (event);
	}

	SIZE CContainerUI::GetScrollPos () const {
		SIZE sz = { 0, 0 };
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible ()) sz.cy = m_pVerticalScrollBar->GetScrollPos ();
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible ()) sz.cx = m_pHorizontalScrollBar->GetScrollPos ();
		return sz;
	}

	SIZE CContainerUI::GetScrollRange () const {
		SIZE sz = { 0, 0 };
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible ()) sz.cy = m_pVerticalScrollBar->GetScrollRange ();
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible ()) sz.cx = m_pHorizontalScrollBar->GetScrollRange ();
		return sz;
	}

	void CContainerUI::SetScrollPos (SIZE szPos, bool bMsg) {
		int cx = 0;
		int cy = 0;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible ()) {
			int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos ();
			m_pVerticalScrollBar->SetScrollPos (szPos.cy);
			cy = m_pVerticalScrollBar->GetScrollPos () - iLastScrollPos;
		}

		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible ()) {
			int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos ();
			m_pHorizontalScrollBar->SetScrollPos (szPos.cx);
			cx = m_pHorizontalScrollBar->GetScrollPos () - iLastScrollPos;
		}

		if (cx == 0 && cy == 0) return;

		RECT rcPos = { 0 };
		for (int it2 = 0; it2 < m_items.GetSize (); it2++) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if (!pControl->IsVisible ()) continue;
			if (pControl->IsFloat ()) continue;

			rcPos = pControl->GetPos ();
			rcPos.left -= cx;
			rcPos.right -= cx;
			rcPos.top -= cy;
			rcPos.bottom -= cy;
			pControl->SetPos (rcPos);
		}

		Invalidate ();

		if (m_pVerticalScrollBar) {
			// 发送滚动消息
			if (m_pManager && bMsg) {
				int nPage = (m_pVerticalScrollBar->GetScrollPos () + m_pVerticalScrollBar->GetLineSize ()) / m_pVerticalScrollBar->GetLineSize ();
				m_pManager->SendNotify (this, DUI_MSGTYPE_SCROLL, (WPARAM) nPage);
			}
		}
	}

	void CContainerUI::SetScrollStepSize (int nSize) {
		if (nSize > 0)
			m_nScrollStepSize = nSize;
	}

	int CContainerUI::GetScrollStepSize () const {
		if (m_pManager)return m_pManager->GetDPIObj ()->Scale (m_nScrollStepSize);

		return m_nScrollStepSize;
	}

	void CContainerUI::LineUp () {
		int cyLine = GetScrollStepSize ();
		if (cyLine == 0) {
			cyLine = 8;
			if (m_pManager) cyLine = m_pManager->GetDefaultFontInfo ()->tm.tmHeight + 8;
		}

		SIZE sz = GetScrollPos ();
		sz.cy -= cyLine;
		SetScrollPos (sz);
	}

	void CContainerUI::LineDown () {
		int cyLine = GetScrollStepSize ();
		if (cyLine == 0) {
			cyLine = 8;
			if (m_pManager) cyLine = m_pManager->GetDefaultFontInfo ()->tm.tmHeight + 8;
		}

		SIZE sz = GetScrollPos ();
		sz.cy += cyLine;
		SetScrollPos (sz);
	}

	void CContainerUI::PageUp () {
		SIZE sz = GetScrollPos ();
		int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible ()) iOffset -= m_pHorizontalScrollBar->GetFixedHeight ();
		sz.cy -= iOffset;
		SetScrollPos (sz);
	}

	void CContainerUI::PageDown () {
		SIZE sz = GetScrollPos ();
		int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible ()) iOffset -= m_pHorizontalScrollBar->GetFixedHeight ();
		sz.cy += iOffset;
		SetScrollPos (sz);
	}

	void CContainerUI::HomeUp () {
		SIZE sz = GetScrollPos ();
		sz.cy = 0;
		SetScrollPos (sz);
	}

	void CContainerUI::EndDown () {
		if (m_pManager) {
			::UpdateWindow (m_pManager->GetPaintWindow ());
		}
		SIZE sz = GetScrollPos ();
		sz.cy = GetScrollRange ().cy;
		SetScrollPos (sz);
	}

	void CContainerUI::LineLeft () {
		int nScrollStepSize = GetScrollStepSize ();
		int cxLine = nScrollStepSize == 0 ? 8 : nScrollStepSize;

		SIZE sz = GetScrollPos ();
		sz.cx -= cxLine;
		SetScrollPos (sz);
	}

	void CContainerUI::LineRight () {
		int nScrollStepSize = GetScrollStepSize ();
		int cxLine = nScrollStepSize == 0 ? 8 : nScrollStepSize;

		SIZE sz = GetScrollPos ();
		sz.cx += cxLine;
		SetScrollPos (sz);
	}

	void CContainerUI::PageLeft () {
		SIZE sz = GetScrollPos ();
		int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible ()) iOffset -= m_pVerticalScrollBar->GetFixedWidth ();
		sz.cx -= iOffset;
		SetScrollPos (sz);
	}

	void CContainerUI::PageRight () {
		SIZE sz = GetScrollPos ();
		int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible ()) iOffset -= m_pVerticalScrollBar->GetFixedWidth ();
		sz.cx += iOffset;
		SetScrollPos (sz);
	}

	void CContainerUI::HomeLeft () {
		SIZE sz = GetScrollPos ();
		sz.cx = 0;
		SetScrollPos (sz);
	}

	void CContainerUI::EndRight () {
		if (m_pManager) {
			::UpdateWindow (m_pManager->GetPaintWindow ());
		}
		SIZE sz = GetScrollPos ();
		sz.cx = GetScrollRange ().cx;
		SetScrollPos (sz);
	}

	void CContainerUI::EnableScrollBar (bool bEnableVertical, bool bEnableHorizontal) {
		if (bEnableVertical && !m_pVerticalScrollBar) {
			m_pVerticalScrollBar = new CScrollBarUI;
			m_pVerticalScrollBar->SetOwner (this);
			m_pVerticalScrollBar->SetManager (m_pManager, nullptr, false);
			if (m_pManager) {
				faw::string_view_t pDefaultAttributes = m_pManager->GetDefaultAttributeList (_T ("VScrollBar"));
				if (!pDefaultAttributes.empty ()) {
					m_pVerticalScrollBar->ApplyAttributeList (pDefaultAttributes);
				}
			}
		} else if (!bEnableVertical && m_pVerticalScrollBar) {
			delete m_pVerticalScrollBar;
			m_pVerticalScrollBar = nullptr;
		}

		if (bEnableHorizontal && !m_pHorizontalScrollBar) {
			m_pHorizontalScrollBar = new CScrollBarUI;
			m_pHorizontalScrollBar->SetHorizontal (true);
			m_pHorizontalScrollBar->SetOwner (this);
			m_pHorizontalScrollBar->SetManager (m_pManager, nullptr, false);

			if (m_pManager) {
				faw::string_view_t pDefaultAttributes = m_pManager->GetDefaultAttributeList (_T ("HScrollBar"));
				if (!pDefaultAttributes.empty ()) {
					m_pHorizontalScrollBar->ApplyAttributeList (pDefaultAttributes);
				}
			}
		} else if (!bEnableHorizontal && m_pHorizontalScrollBar) {
			delete m_pHorizontalScrollBar;
			m_pHorizontalScrollBar = nullptr;
		}

		NeedUpdate ();
	}

	CScrollBarUI* CContainerUI::GetVerticalScrollBar () const {
		return m_pVerticalScrollBar;
	}

	CScrollBarUI* CContainerUI::GetHorizontalScrollBar () const {
		return m_pHorizontalScrollBar;
	}

	int CContainerUI::FindSelectable (int iIndex, bool bForward /*= true*/) const {
		// NOTE: This is actually a helper-function for the list/combo/ect controls
		//       that allow them to find the next enabled/available selectable item
		if (GetCount () == 0) return -1;
		iIndex = CLAMP (iIndex, 0, GetCount () - 1);
		if (bForward) {
			for (int i = iIndex; i < GetCount (); i++) {
				if (GetItemAt (i)->GetInterface (_T ("ListItem"))
					&& GetItemAt (i)->IsVisible ()
					&& GetItemAt (i)->IsEnabled ()) return i;
			}
			return -1;
		} else {
			for (int i = iIndex; i >= 0; --i) {
				if (GetItemAt (i)->GetInterface (_T ("ListItem"))
					&& GetItemAt (i)->IsVisible ()
					&& GetItemAt (i)->IsEnabled ()) return i;
			}
			return FindSelectable (0, true);
		}
	}

	RECT CContainerUI::GetClientPos () const {
		RECT rc = m_rcItem;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible ()) {
			rc.top -= m_pVerticalScrollBar->GetScrollPos ();
			rc.bottom -= m_pVerticalScrollBar->GetScrollPos ();
			rc.bottom += m_pVerticalScrollBar->GetScrollRange ();
			rc.right -= m_pVerticalScrollBar->GetFixedWidth ();
		}
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible ()) {
			rc.left -= m_pHorizontalScrollBar->GetScrollPos ();
			rc.right -= m_pHorizontalScrollBar->GetScrollPos ();
			rc.right += m_pHorizontalScrollBar->GetScrollRange ();
			rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight ();
		}
		return rc;
	}

	void CContainerUI::Move (SIZE szOffset, bool bNeedInvalidate) {
		CControlUI::Move (szOffset, bNeedInvalidate);
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible ()) m_pVerticalScrollBar->Move (szOffset, false);
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible ()) m_pHorizontalScrollBar->Move (szOffset, false);
		for (int it = 0; it < m_items.GetSize (); it++) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if (pControl && pControl->IsVisible ()) pControl->Move (szOffset, false);
		}
	}

	void CContainerUI::SetPos (RECT rc, bool bNeedInvalidate) {
		CControlUI::SetPos (rc, bNeedInvalidate);
		if (m_items.empty ()) return;

		rc = m_rcItem;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible ()) {
			rc.top -= m_pVerticalScrollBar->GetScrollPos ();
			rc.bottom -= m_pVerticalScrollBar->GetScrollPos ();
			rc.bottom += m_pVerticalScrollBar->GetScrollRange ();
			rc.right -= m_pVerticalScrollBar->GetFixedWidth ();
		}
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible ()) {
			rc.left -= m_pHorizontalScrollBar->GetScrollPos ();
			rc.right -= m_pHorizontalScrollBar->GetScrollPos ();
			rc.right += m_pHorizontalScrollBar->GetScrollRange ();
			rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight ();
		}

		for (int it = 0; it < m_items.GetSize (); it++) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if (!pControl->IsVisible ()) continue;
			if (pControl->IsFloat ()) {
				SetFloatPos (it);
			} else {
				SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };
				if (sz.cx < pControl->GetMinWidth ()) sz.cx = pControl->GetMinWidth ();
				if (sz.cx > pControl->GetMaxWidth ()) sz.cx = pControl->GetMaxWidth ();
				if (sz.cy < pControl->GetMinHeight ()) sz.cy = pControl->GetMinHeight ();
				if (sz.cy > pControl->GetMaxHeight ()) sz.cy = pControl->GetMaxHeight ();
				RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy };
				pControl->SetPos (rcCtrl, false);
			}
		}
	}

	void CContainerUI::SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue) {
		if (pstrName == _T ("inset")) {
			RECT rcInset = FawTools::parse_rect (pstrValue);
			SetInset (rcInset);
		} else if (pstrName == _T ("mousechild")) SetMouseChildEnabled (FawTools::parse_bool (pstrValue));
		else if (pstrName == _T ("vscrollbar")) {
			EnableScrollBar (FawTools::parse_bool (pstrValue), GetHorizontalScrollBar ());
		} else if (pstrName == _T ("vscrollbarstyle")) {
			m_sVerticalScrollBarStyle = pstrValue;
			EnableScrollBar (TRUE, GetHorizontalScrollBar ());
			if (GetVerticalScrollBar ()) {
				faw::string_view_t pStyle = m_pManager->GetStyle (m_sVerticalScrollBarStyle.str_view ());
				if (!pStyle.empty ()) {
					GetVerticalScrollBar ()->ApplyAttributeList (pStyle);
				} else {
					GetVerticalScrollBar ()->ApplyAttributeList (pstrValue);
				}
			}
		} else if (pstrName == _T ("hscrollbar")) {
			EnableScrollBar (GetVerticalScrollBar (), FawTools::parse_bool (pstrValue));
		} else if (pstrName == _T ("hscrollbarstyle")) {
			m_sHorizontalScrollBarStyle = pstrValue;
			EnableScrollBar (TRUE, GetHorizontalScrollBar ());
			if (GetHorizontalScrollBar ()) {
				faw::string_view_t pStyle = m_pManager->GetStyle (m_sHorizontalScrollBarStyle.str_view ());
				if (!pStyle.empty ()) {
					GetHorizontalScrollBar ()->ApplyAttributeList (pStyle);
				} else {
					GetHorizontalScrollBar ()->ApplyAttributeList (pstrValue);
				}
			}
		} else if (pstrName == _T ("childpadding")) SetChildPadding (_ttoi (pstrValue.data ()));
		else if (pstrName == _T ("childalign")) {
			if (pstrValue == _T ("left")) m_iChildAlign = DT_LEFT;
			else if (pstrValue == _T ("center")) m_iChildAlign = DT_CENTER;
			else if (pstrValue == _T ("right")) m_iChildAlign = DT_RIGHT;
		} else if (pstrName == _T ("childvalign")) {
			if (pstrValue == _T ("top")) m_iChildVAlign = DT_TOP;
			else if (pstrValue == _T ("vcenter")) m_iChildVAlign = DT_VCENTER;
			else if (pstrValue == _T ("bottom")) m_iChildVAlign = DT_BOTTOM;
		} else if (pstrName == _T ("scrollstepsize")) SetScrollStepSize (_ttoi (pstrValue.data ()));
		else CControlUI::SetAttribute (pstrName, pstrValue);
	}

	void CContainerUI::SetManager (CPaintManagerUI* pManager, CControlUI* pParent, bool bInit) {
		for (int it = 0; it < m_items.GetSize (); it++) {
			static_cast<CControlUI*>(m_items[it])->SetManager (pManager, this, bInit);
		}

		if (m_pVerticalScrollBar) m_pVerticalScrollBar->SetManager (pManager, this, bInit);
		if (m_pHorizontalScrollBar) m_pHorizontalScrollBar->SetManager (pManager, this, bInit);
		CControlUI::SetManager (pManager, pParent, bInit);
	}

	CControlUI* CContainerUI::FindControl (FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags) {
		if ((uFlags & UIFIND_VISIBLE) != 0 && !IsVisible ()) return nullptr;
		if ((uFlags & UIFIND_ENABLED) != 0 && !IsEnabled ()) return nullptr;
		if ((uFlags & UIFIND_HITTEST) != 0 && !::PtInRect (&m_rcItem, *(static_cast<LPPOINT>(pData)))) return nullptr;
		if ((uFlags & UIFIND_UPDATETEST) != 0 && Proc (this, pData)) return nullptr;

		CControlUI* pResult = nullptr;
		if ((uFlags & UIFIND_ME_FIRST) != 0) {
			if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled ()) pResult = Proc (this, pData);
		}
		if (!pResult && m_pVerticalScrollBar) {
			if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled ()) pResult = m_pVerticalScrollBar->FindControl (Proc, pData, uFlags);
		}
		if (!pResult && m_pHorizontalScrollBar) {
			if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled ()) pResult = m_pHorizontalScrollBar->FindControl (Proc, pData, uFlags);
		}
		if (pResult) return pResult;

		if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseChildEnabled ()) {
			RECT rc = m_rcItem;
			rc.left += m_rcInset.left;
			rc.top += m_rcInset.top;
			rc.right -= m_rcInset.right;
			rc.bottom -= m_rcInset.bottom;
			if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible ()) rc.right -= m_pVerticalScrollBar->GetFixedWidth ();
			if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible ()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight ();
			if ((uFlags & UIFIND_TOP_FIRST) != 0) {
				for (int it = m_items.GetSize () - 1; it >= 0; it--) {
					pResult = static_cast<CControlUI*>(m_items[it])->FindControl (Proc, pData, uFlags);
					if (pResult) {
						if ((uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat () && !::PtInRect (&rc, *(static_cast<LPPOINT>(pData))))
							continue;
						else
							return pResult;
					}
				}
			} else {
				for (int it = 0; it < m_items.GetSize (); it++) {
					pResult = static_cast<CControlUI*>(m_items[it])->FindControl (Proc, pData, uFlags);
					if (pResult) {
						if ((uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat () && !::PtInRect (&rc, *(static_cast<LPPOINT>(pData))))
							continue;
						else
							return pResult;
					}
				}
			}
		}

		pResult = nullptr;
		if (!pResult && (uFlags & UIFIND_ME_FIRST) == 0) {
			if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled ()) pResult = Proc (this, pData);
		}
		return pResult;
	}

	bool CContainerUI::DoPaint (HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
		RECT rcTemp = { 0 };
		if (!::IntersectRect (&rcTemp, &rcPaint, &m_rcItem)) return true;

		CRenderClip clip;
		CRenderClip::GenerateClip (hDC, rcTemp, clip);
		CControlUI::DoPaint (hDC, rcPaint, pStopControl);

		if (m_items.GetSize () > 0) {
			RECT rcInset = GetInset ();
			RECT rc = m_rcItem;
			rc.left += rcInset.left;
			rc.top += rcInset.top;
			rc.right -= rcInset.right;
			rc.bottom -= rcInset.bottom;
			if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible ()) rc.right -= m_pVerticalScrollBar->GetFixedWidth ();
			if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible ()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight ();

			if (!::IntersectRect (&rcTemp, &rcPaint, &rc)) {
				for (int it = 0; it < m_items.GetSize (); it++) {
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if (pControl == pStopControl) return false;
					if (!pControl->IsVisible ()) continue;
					if (!::IntersectRect (&rcTemp, &rcPaint, &pControl->GetPos ())) continue;
					if (pControl->IsFloat ()) {
						if (!::IntersectRect (&rcTemp, &m_rcItem, &pControl->GetPos ())) continue;
						if (!pControl->Paint (hDC, rcPaint, pStopControl)) return false;
					}
				}
			} else {
				CRenderClip childClip;
				CRenderClip::GenerateClip (hDC, rcTemp, childClip);
				for (int it = 0; it < m_items.GetSize (); it++) {
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if (pControl == pStopControl) return false;
					if (!pControl->IsVisible ()) continue;
					if (!::IntersectRect (&rcTemp, &rcPaint, &pControl->GetPos ())) continue;
					if (pControl->IsFloat ()) {
						if (!::IntersectRect (&rcTemp, &m_rcItem, &pControl->GetPos ())) continue;
						CRenderClip::UseOldClipBegin (hDC, childClip);
						if (!pControl->Paint (hDC, rcPaint, pStopControl)) return false;
						CRenderClip::UseOldClipEnd (hDC, childClip);
					} else {
						if (!::IntersectRect (&rcTemp, &rc, &pControl->GetPos ())) continue;
						if (!pControl->Paint (hDC, rcPaint, pStopControl)) return false;
					}
				}
			}
		}

		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible ()) {
			if (m_pVerticalScrollBar == pStopControl) return false;
			if (::IntersectRect (&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos ())) {
				if (!m_pVerticalScrollBar->Paint (hDC, rcPaint, pStopControl)) return false;
			}
		}

		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible ()) {
			if (m_pHorizontalScrollBar == pStopControl) return false;
			if (::IntersectRect (&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos ())) {
				if (!m_pHorizontalScrollBar->Paint (hDC, rcPaint, pStopControl)) return false;
			}
		}
		return true;
	}

	void CContainerUI::SetFloatPos (int iIndex) {
		// 因为CControlUI::SetPos对float的操作影响，这里不能对float组件添加滚动条的影响
		if (iIndex < 0 || iIndex >= m_items.GetSize ()) return;

		CControlUI* pControl = static_cast<CControlUI*>(m_items[iIndex]);

		if (!pControl->IsVisible ()) return;
		if (!pControl->IsFloat ()) return;

		SIZE szXY = pControl->GetFixedXY ();
		SIZE sz = { pControl->GetFixedWidth (), pControl->GetFixedHeight () };

		int nParentWidth = m_rcItem.right - m_rcItem.left;
		int nParentHeight = m_rcItem.bottom - m_rcItem.top;

		UINT uAlign = pControl->GetFloatAlign ();
		if (uAlign != 0) {
			RECT rcCtrl = { 0, 0, sz.cx, sz.cy };
			if ((uAlign & DT_CENTER) != 0) {
				::OffsetRect (&rcCtrl, (nParentWidth - sz.cx) / 2, 0);
			} else if ((uAlign & DT_RIGHT) != 0) {
				::OffsetRect (&rcCtrl, nParentWidth - sz.cx, 0);
			} else {
				::OffsetRect (&rcCtrl, szXY.cx, 0);
			}

			if ((uAlign & DT_VCENTER) != 0) {
				::OffsetRect (&rcCtrl, 0, (nParentHeight - sz.cy) / 2);
			} else if ((uAlign & DT_BOTTOM) != 0) {
				::OffsetRect (&rcCtrl, 0, nParentHeight - sz.cy);
			} else {
				::OffsetRect (&rcCtrl, 0, szXY.cy);
			}

			::OffsetRect (&rcCtrl, m_rcItem.left, m_rcItem.top);
			pControl->SetPos (rcCtrl, false);
		} else {
			TPercentInfo rcPercent = pControl->GetFloatPercent ();
			LONG width = m_rcItem.right - m_rcItem.left;
			LONG height = m_rcItem.bottom - m_rcItem.top;
			RECT rcCtrl = { 0 };
			rcCtrl.left = (LONG) (width*rcPercent.left) + szXY.cx + m_rcItem.left;
			rcCtrl.top = (LONG) (height*rcPercent.top) + szXY.cy + m_rcItem.top;
			rcCtrl.right = (LONG) (width*rcPercent.right) + szXY.cx + sz.cx + m_rcItem.left;
			rcCtrl.bottom = (LONG) (height*rcPercent.bottom) + szXY.cy + sz.cy + m_rcItem.top;
			pControl->SetPos (rcCtrl, false);
		}
	}

	void CContainerUI::ProcessScrollBar (RECT rc, int cxRequired, int cyRequired) {
		while (m_pHorizontalScrollBar) {
			// Scroll needed
			if (cxRequired > rc.right - rc.left && !m_pHorizontalScrollBar->IsVisible ()) {
				m_pHorizontalScrollBar->SetVisible (true);
				m_pHorizontalScrollBar->SetScrollRange (cxRequired - (rc.right - rc.left));
				m_pHorizontalScrollBar->SetScrollPos (0);
				SetPos (m_rcItem);
				break;
			}

			// No scrollbar required
			if (!m_pHorizontalScrollBar->IsVisible ()) break;

			// Scroll not needed anymore?
			int cxScroll = cxRequired - (rc.right - rc.left);
			if (cxScroll <= 0) {
				m_pHorizontalScrollBar->SetVisible (false);
				m_pHorizontalScrollBar->SetScrollPos (0);
				m_pHorizontalScrollBar->SetScrollRange (0);
				SetPos (m_rcItem);
			} else {
				RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight () };
				m_pHorizontalScrollBar->SetPos (rcScrollBarPos);

				if (m_pHorizontalScrollBar->GetScrollRange () != cxScroll) {
					int iScrollPos = m_pHorizontalScrollBar->GetScrollPos ();
					m_pHorizontalScrollBar->SetScrollRange (::abs (cxScroll)); // if scrollpos>range then scrollpos=range
					if (iScrollPos > m_pHorizontalScrollBar->GetScrollPos ()) {
						SetPos (m_rcItem);
					}
				}
			}
			break;
		}

		while (m_pVerticalScrollBar) {
			// Scroll needed
			if (cyRequired > rc.bottom - rc.top && !m_pVerticalScrollBar->IsVisible ()) {
				m_pVerticalScrollBar->SetVisible (true);
				m_pVerticalScrollBar->SetScrollRange (cyRequired - (rc.bottom - rc.top));
				m_pVerticalScrollBar->SetScrollPos (0);
				SetPos (m_rcItem);
				break;
			}

			// No scrollbar required
			if (!m_pVerticalScrollBar->IsVisible ()) break;

			// Scroll not needed anymore?
			int cyScroll = cyRequired - (rc.bottom - rc.top);
			if (cyScroll <= 0) {
				m_pVerticalScrollBar->SetVisible (false);
				m_pVerticalScrollBar->SetScrollPos (0);
				m_pVerticalScrollBar->SetScrollRange (0);
				SetPos (m_rcItem);
				break;
			}

			RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth (), rc.bottom };
			m_pVerticalScrollBar->SetPos (rcScrollBarPos);

			if (m_pVerticalScrollBar->GetScrollRange () != cyScroll) {
				int iScrollPos = m_pVerticalScrollBar->GetScrollPos ();
				m_pVerticalScrollBar->SetScrollRange (::abs (cyScroll)); // if scrollpos>range then scrollpos=range
				if (iScrollPos > m_pVerticalScrollBar->GetScrollPos ()) {
					SetPos (m_rcItem);
				}
			}
			break;
		}
	}

	bool CContainerUI::SetSubControlText (faw::string_view_t pstrSubControlName, faw::string_view_t pstrText) {
		CControlUI* pSubControl = nullptr;
		pSubControl = this->FindSubControl (pstrSubControlName);
		if (pSubControl) {
			pSubControl->SetText (pstrText);
			return TRUE;
		} else
			return FALSE;
	}

	bool CContainerUI::SetSubControlFixedHeight (faw::string_view_t pstrSubControlName, int cy) {
		CControlUI* pSubControl = nullptr;
		pSubControl = this->FindSubControl (pstrSubControlName);
		if (pSubControl) {
			pSubControl->SetFixedHeight (cy);
			return TRUE;
		} else
			return FALSE;
	}

	bool CContainerUI::SetSubControlFixedWdith (faw::string_view_t pstrSubControlName, int cx) {
		CControlUI* pSubControl = nullptr;
		pSubControl = this->FindSubControl (pstrSubControlName);
		if (pSubControl) {
			pSubControl->SetFixedWidth (cx);
			return TRUE;
		} else
			return FALSE;
	}

	bool CContainerUI::SetSubControlUserData (faw::string_view_t pstrSubControlName, faw::string_view_t pstrText) {
		CControlUI* pSubControl = nullptr;
		pSubControl = this->FindSubControl (pstrSubControlName);
		if (pSubControl) {
			pSubControl->SetUserData (pstrText);
			return TRUE;
		} else
			return FALSE;
	}

	faw::String CContainerUI::GetSubControlText (faw::string_view_t pstrSubControlName) {
		CControlUI* pSubControl = nullptr;
		pSubControl = this->FindSubControl (pstrSubControlName);
		if (!pSubControl)
			return _T ("");
		else
			return pSubControl->GetText ();
	}

	int CContainerUI::GetSubControlFixedHeight (faw::string_view_t pstrSubControlName) {
		CControlUI* pSubControl = nullptr;
		pSubControl = this->FindSubControl (pstrSubControlName);
		if (!pSubControl)
			return -1;
		else
			return pSubControl->GetFixedHeight ();
	}

	int CContainerUI::GetSubControlFixedWdith (faw::string_view_t pstrSubControlName) {
		CControlUI* pSubControl = nullptr;
		pSubControl = this->FindSubControl (pstrSubControlName);
		if (!pSubControl)
			return -1;
		else
			return pSubControl->GetFixedWidth ();
	}

	const faw::String CContainerUI::GetSubControlUserData (faw::string_view_t pstrSubControlName) {
		CControlUI* pSubControl = nullptr;
		pSubControl = this->FindSubControl (pstrSubControlName);
		if (!pSubControl)
			return _T ("");
		else
			return pSubControl->GetUserData ();
	}

	CControlUI* CContainerUI::FindSubControl (faw::string_view_t pstrSubControlName) {
		return static_cast<CControlUI*>(GetManager ()->FindSubControlByName (this, pstrSubControlName));
	}

} // namespace DuiLib
