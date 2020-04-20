﻿#ifndef __UITRAICON_H__
#define __UITRAICON_H__

#pragma once
#include <ShellAPI.h>

namespace DuiLib {
	class UILIB_API CTrayIcon {
	public:
		CTrayIcon (void);
		virtual ~CTrayIcon (void);

	public:
		void CreateTrayIcon (HWND _RecvHwnd, UINT _IconIDResource, faw::string_view_t _ToolTipText = _T (""), UINT _Message = 0);
		void DeleteTrayIcon ();
		bool SetTooltipText (faw::string_view_t _ToolTipText);
		bool SetTooltipText (UINT _IDResource);
		faw::String GetTooltipText () const;

		bool SetIcon (HICON _Hicon);
		bool SetIcon (faw::string_view_t _IconFile);
		bool SetIcon (UINT _IDResource);
		HICON GetIcon () const;
		void SetHideIcon ();
		void SetShowIcon ();
		void RemoveIcon ();
		bool Enabled () {
			return m_bEnabled;
		};
		bool IsVisible () {
			return !m_bVisible;
		};

	private:
		bool			m_bEnabled	= false;
		bool			m_bVisible	= false;
		HWND			m_hWnd		= NULL;
		UINT			m_uMessage;
		HICON			m_hIcon		= NULL;
		NOTIFYICONDATA	m_trayData	= { 0 };
	};
}
#endif // 

