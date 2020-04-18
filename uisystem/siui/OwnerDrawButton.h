#pragma once

namespace DuiLib{

	class UILIB_API  COwnerDrawButton : public CButtonUI
	{
	public:
		COwnerDrawButton(void);
		~COwnerDrawButton(void);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void PaintText(HDC hDC);
	};

};