#pragma once

class UILIB_API CXLayout : public CControlUI
{
public:
	CXLayout(void);
	~CXLayout(void);
	void SetUserHwnd(HWND hwnd);
	void SetPos(RECT rc);
	void SetVisible(bool bVisible = true);
	void SetInternVisible(bool bVisible = true);
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
private: 
	HWND m_UserWnd;
};
