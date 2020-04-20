﻿#ifndef __UITILELAYOUT_H__
#define __UITILELAYOUT_H__

#pragma once

namespace DuiLib {
	class UILIB_API CTileLayoutUI: public CContainerUI {
		DECLARE_DUICONTROL (CTileLayoutUI)
	public:
		CTileLayoutUI ();

		faw::string_view_t GetClass () const;
		LPVOID GetInterface (faw::string_view_t pstrName);

		void SetPos (RECT rc, bool bNeedInvalidate = true);

		SIZE GetItemSize () const;
		void SetItemSize (SIZE szItem);
		int GetColumns () const;
		void SetColumns (int nCols);

		void SetAttribute (faw::string_view_t pstrName, faw::string_view_t pstrValue);

	protected:
		SIZE	m_szItem	= { 0 };
		int		m_nColumns	= 1;
	};
}
#endif // __UITILELAYOUT_H__
