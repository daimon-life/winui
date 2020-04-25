#include "stdafx.h"
#include "OwnerDrawButton.h"

namespace DuiLib
{
	COwnerDrawButton::COwnerDrawButton(void)
	{
	}

	COwnerDrawButton::~COwnerDrawButton(void)
	{
	}

	LPCTSTR COwnerDrawButton::GetClass() const
	{
		return _T("OwnerDrawButton");
	}

	LPVOID COwnerDrawButton::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("OwnerDrawButton")) == 0 ) return static_cast<COwnerDrawButton*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	void COwnerDrawButton::PaintText(HDC hDC)
	{

		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		if( m_sText.IsEmpty() ) return;
		int nLinks = 0;
		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		DWORD clrColor = IsEnabled()?m_dwTextColor:m_dwDisabledTextColor;

		if( ((m_uButtonState & UISTATE_PUSHED) != 0) && (GetPushedTextColor() != 0) )
			clrColor = GetPushedTextColor();
		else if( ((m_uButtonState & UISTATE_HOT) != 0) && (GetHotTextColor() != 0) )
			clrColor = GetHotTextColor();
		else if( ((m_uButtonState & UISTATE_FOCUSED) != 0) && (GetFocusedTextColor() != 0) )
			clrColor = GetFocusedTextColor();


		if( m_bShowHtml )
			CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, clrColor, \
			NULL, NULL, nLinks, m_uTextStyle);
		else
		{
			int length = 0;
			int des = 0;
			SIZE size = {0};

			CStdString StrText = m_sText;

			RECT rclast = m_rcItem;
			rclast.left += m_rcTextPadding.left;
			rclast.right -= m_rcTextPadding.right;
			rclast.top += m_rcTextPadding.top ;
			rclast.bottom -= m_rcTextPadding.bottom;

			while(true)
			{
				::GetTextExtentExPoint(hDC,StrText,StrText.GetLength(),this->GetWidth(),&length,0,&size);

				CStdString strtmp;
				if (length < StrText.GetLength())
				{
					strtmp = StrText.Left(length);
				}
				else
					strtmp = StrText;
				CRenderEngine::DrawText(hDC, m_pManager, rclast, strtmp, clrColor, \
					m_iFont, m_uTextStyle);
				if (length > StrText.GetLength() || StrText.GetLength() <= 0)
				{
					break;
				}
				StrText = StrText.Right(StrText.GetLength() - length);
				rclast.top += 20;
			}
		}
	}
};