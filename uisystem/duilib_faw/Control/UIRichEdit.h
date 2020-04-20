﻿#ifndef __UIRICHEDIT_H__
#define __UIRICHEDIT_H__

#pragma once

namespace DuiLib {

	class CTxtWinHost;

	class UILIB_API CRichEditUI: public CContainerUI, public IMessageFilterUI {
		DECLARE_DUICONTROL (CRichEditUI)
	public:
		CRichEditUI ();
		virtual ~CRichEditUI ();

		faw::string_view_t GetClass () const;
		LPVOID GetInterface (faw::string_view_t pstrName);
		UINT GetControlFlags () const;

		void SetEnabled (bool bEnabled);
		bool IsMultiLine ();
		void SetMultiLine (bool bMultiLine);
		bool IsWantTab ();
		void SetWantTab (bool bWantTab = true);
		bool IsWantReturn ();
		void SetWantReturn (bool bWantReturn = true);
		bool IsWantCtrlReturn ();
		void SetWantCtrlReturn (bool bWantCtrlReturn = true);
		bool IsTransparent ();
		void SetTransparent (bool bTransparent = true);
		bool IsRich ();
		void SetRich (bool bRich = true);
		bool IsReadOnly ();
		void SetReadOnly (bool bReadOnly = true);
		bool IsWordWrap ();
		void SetWordWrap (bool bWordWrap = true);
		int GetFont ();
		void SetFont (int index);
		void SetFont (faw::string_view_t pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
		LONG GetWinStyle ();
		void SetWinStyle (LONG lStyle);
		DWORD GetTextColor ();
		void SetTextColor (DWORD dwTextColor);
		int GetLimitText ();
		void SetLimitText (int iChars);
		long GetTextLength (DWORD dwFlags = GTL_DEFAULT) const;
		faw::String GetText () const;
		void SetText (faw::string_view_t pstrText);
		bool IsModify () const;
		void SetModify (bool bModified = true) const;
		void GetSel (CHARRANGE &cr) const;
		void GetSel (long& nStartChar, long& nEndChar) const;
		int SetSel (CHARRANGE &cr);
		int SetSel (long nStartChar, long nEndChar);
		void ReplaceSel (faw::string_view_t lpszNewText, bool bCanUndo);
		void ReplaceSelW (LPCWSTR lpszNewText, bool bCanUndo = false);
		faw::String GetSelText () const;
		int SetSelAll ();
		int SetSelNone ();
		WORD GetSelectionType () const;
		bool GetZoom (int& nNum, int& nDen) const;
		bool SetZoom (int nNum, int nDen);
		bool SetZoomOff ();
		bool GetAutoURLDetect () const;
		bool SetAutoURLDetect (bool bAutoDetect = true);
		DWORD GetEventMask () const;
		DWORD SetEventMask (DWORD dwEventMask);
		faw::String GetTextRange (long nStartChar, long nEndChar) const;
		void HideSelection (bool bHide = true, bool bChangeStyle = false);
		void ScrollCaret ();
		int InsertText (long nInsertAfterChar, faw::string_view_t lpstrText, bool bCanUndo = false);
		int AppendText (faw::string_view_t lpstrText, bool bCanUndo = false);
		DWORD GetDefaultCharFormat (CHARFORMAT2 &cf) const;
		bool SetDefaultCharFormat (CHARFORMAT2 &cf);
		DWORD GetSelectionCharFormat (CHARFORMAT2 &cf) const;
		bool SetSelectionCharFormat (CHARFORMAT2 &cf);
		bool SetWordCharFormat (CHARFORMAT2 &cf);
		DWORD GetParaFormat (PARAFORMAT2 &pf) const;
		bool SetParaFormat (PARAFORMAT2 &pf);
		bool CanUndo ();
		bool CanRedo ();
		bool CanPaste ();
		bool Redo ();
		bool Undo ();
		void Clear ();
		void Copy ();
		void Cut ();
		void Paste ();
		int GetLineCount () const;
		faw::String GetLine (int nIndex, int nMaxLength) const;
		int LineIndex (int nLine = -1) const;
		int LineLength (int nLine = -1) const;
		bool LineScroll (int nLines, int nChars = 0);
		POINT GetCharPos (long lChar) const;
		long LineFromChar (long nIndex) const;
		POINT PosFromChar (UINT nChar) const;
		int CharFromPos (POINT pt) const;
		void EmptyUndoBuffer ();
		UINT SetUndoLimit (UINT nLimit);
		long StreamIn (int nFormat, EDITSTREAM &es);
		long StreamOut (int nFormat, EDITSTREAM &es);
		void SetAccumulateDBCMode (bool bDBCMode);
		bool IsAccumulateDBCMode ();

		RECT GetTextPadding () const;
		void SetTextPadding (RECT rc);
		faw::string_view_t GetNormalImage ();
		void SetNormalImage (faw::string_view_t pStrImage);
		faw::string_view_t GetHotImage ();
		void SetHotImage (faw::string_view_t pStrImage);
		faw::string_view_t GetFocusedImage ();
		void SetFocusedImage (faw::string_view_t pStrImage);
		faw::string_view_t GetDisabledImage ();
		void SetDisabledImage (faw::string_view_t pStrImage);
		void PaintStatusImage (HDC hDC);

		void SetTipValue (faw::string_view_t pStrTipValue);
		faw::string_view_t GetTipValue ();
		void SetTipValueColor (faw::string_view_t pStrColor);
		DWORD GetTipValueColor ();
		void SetTipValueAlign (UINT uAlign);
		UINT GetTipValueAlign ();

		void DoInit ();
		bool SetDropAcceptFile (bool bAccept);
		// 注意：TxSendMessage和SendMessage是有区别的，TxSendMessage没有multibyte和unicode自动转换的功能，
		// 而richedit2.0内部是以unicode实现的，在multibyte程序中，必须自己处理unicode到multibyte的转换
		virtual HRESULT TxSendMessage (UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const;
		IDropTarget* GetTxDropTarget ();
		virtual bool OnTxViewChanged ();
		virtual void OnTxNotify (DWORD iNotify, void *pv);

		void SetScrollPos (SIZE szPos, bool bMsg = true);
		void LineUp ();
		void LineDown ();
		void PageUp ();
		void PageDown ();
		void HomeUp ();
		void EndDown ();
		void LineLeft ();
		void LineRight ();
		void PageLeft ();
		void PageRight ();
		void HomeLeft ();
		void EndRight ();

		SIZE EstimateSize (SIZE szAvailable);
		void SetPos (RECT rc, bool bNeedInvalidate = true);
		void Move (SIZE szOffset, bool bNeedInvalidate = true);
		void DoEvent (TEventUI& event);
		bool DoPaint (HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

		void SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue);

		LRESULT MessageHandler (UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	protected:
		enum {
			DEFAULT_TIMERID = 20,
		};

		CTxtWinHost	*m_pTwh				= nullptr;
		bool		m_bVScrollBarFixing	= false;
		bool		m_bWantTab			= true;
		bool		m_bWantReturn		= true;
		bool		m_bWantCtrlReturn	= true;
		bool		m_bTransparent		= true;
		bool		m_bRich				= true;
		bool		m_bReadOnly			= false;
		bool		m_bWordWrap			= false;
		DWORD		m_dwTextColor		= 0;
		int			m_iFont				= -1;
		int			m_iLimitText;
		LONG		m_lTwhStyle;
		bool		m_bDrawCaret		= true;
		bool		m_bInited			= false;

		bool		m_fAccumulateDBC; // TRUE - need to cumulate ytes from 2 WM_CHAR msgs
		// we are in this mode when we receive VK_PROCESSKEY
		UINT		m_chLeadByte		= 0; // use when we are in _fAccumulateDBC mode

		RECT		m_rcTextPadding;
		UINT		m_uButtonState		= 0;
		faw::String	m_sNormalImage;
		faw::String	m_sHotImage;
		faw::String	m_sFocusedImage;
		faw::String	m_sDisabledImage;
		faw::String	m_sTipValue;
		DWORD		m_dwTipValueColor	= 0xFFBAC0C5;
		UINT		m_uTipValueAlign	= DT_SINGLELINE | DT_LEFT;
	};

} // namespace DuiLib

#endif // __UIRICHEDIT_H__
