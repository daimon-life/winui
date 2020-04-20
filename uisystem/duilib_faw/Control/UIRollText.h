﻿#ifndef __UITEXTSCROLLH__
#define __UITEXTSCROLLH__

#pragma once

namespace DuiLib {
#define ROLLTEXT_LEFT		0
#define ROLLTEXT_RIGHT		1
#define ROLLTEXT_UP 		2
#define ROLLTEXT_DOWN		3

#define ROLLTEXT_TIMERID			20
#define ROLLTEXT_TIMERID_SPAN		50U

#define ROLLTEXT_ROLL_END			21
#define ROLLTEXT_ROLL_END_SPAN		1000*6U

	class UILIB_API CRollTextUI: public CLabelUI {
		DECLARE_DUICONTROL (CRollTextUI)
	public:
		CRollTextUI (void);
		virtual ~CRollTextUI (void);

	public:
		faw::string_view_t GetClass () const;
		LPVOID GetInterface (faw::string_view_t pstrName);

	public:
		virtual void PaintText (HDC hDC);
		virtual void DoEvent (TEventUI& event);
		virtual void SetPos (RECT rc);
		virtual void SetText (faw::String pstrText);

	public:
		void BeginRoll (int nDirect = ROLLTEXT_RIGHT, LONG lTimeSpan = ROLLTEXT_TIMERID_SPAN, LONG lMaxTimeLimited = 60);
		void EndRoll ();

	private:
		int		m_nStep				= 5;
		int		m_nScrollPos		= 0;
		BOOL	m_bUseRoll			= FALSE;
		int		m_nRollDirection	= ROLLTEXT_LEFT;
		int		m_nText_W_H			= 0;
	};

}	// namespace DuiLib

#endif // __UITEXTSCROLLH__