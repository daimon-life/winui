#ifndef __UICOMMONCONTROLS_H__
#define __UICOMMONCONTROLS_H__

#pragma once

namespace DuiLib {//NOLINT
/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CLabelUI : public CControlUI//NOLINT
{
public:
    CLabelUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetTextStyle(UINT uStyle);
	UINT GetTextStyle() const;
    void SetTextColor(DWORD dwTextColor);
	DWORD GetTextColor() const;
    void SetDisabledTextColor(DWORD dwTextColor);
	DWORD GetDisabledTextColor() const;
    void SetFont(int index);
	int GetFont() const;
    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    bool IsShowHtml();
    void SetShowHtml(bool bShowHtml = true);

    SIZE EstimateSize(SIZE szAvailable);
    void DoEvent(TEventUI& event);//NOLINT
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintText(HDC hDC);

	void PaintStatusImage(HDC hDC);

	LPCTSTR GetDisabledImage();
	void SetDisabledImage(LPCTSTR pStrImage);

    virtual SIZE TestTextSize();

protected:
    DWORD m_dwTextColor;
    DWORD m_dwDisabledTextColor;
    int m_iFont;
    UINT m_uTextStyle;
    RECT m_rcTextPadding;
    bool m_bShowHtml;
	CStdString m_sDisabledImage;
    UINT m_uTextAngle;
    bool m_bIsAdaptiveSize;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CButtonUI : public CLabelUI//NOLINT
{
public:
    CButtonUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    bool Activate();
    void SetEnabled(bool bEnable = true);
    void DoEvent(TEventUI& event);//NOLINT

    LPCTSTR GetNormalImage();
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage();
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage();
    void SetPushedImage(LPCTSTR pStrImage);
	LPCTSTR GetFocusedImage();
	void SetFocusedImage(LPCTSTR pStrImage);
	
	void SetBKImageBuddy(LPCTSTR pStrImage);//����������ϱ���Ч��
	LPCTSTR GetBKImageBuddy();


    void SetHotTextColor(DWORD dwColor);
	DWORD GetHotTextColor() const;
    void SetPushedTextColor(DWORD dwColor);
	DWORD GetPushedTextColor() const;
    void SetFocusedTextColor(DWORD dwColor);
	DWORD GetFocusedTextColor() const;
    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);
	void PaintBkImage(HDC hDC);
    void PaintBorder(HDC hDC);
	virtual void DoPaint(HDC hDC, const RECT& rcPaint);
	void SetPaintbkfirst(bool _istrue=true);
	//Ϊ�˴����еĲ�����״̬�ȱ���״̸̬�����ʱ��Ͳ����Ʊ�����
	void Setpaintbkwhenenabled(bool _istrue=true);

protected:
    UINT m_uButtonState;

	DWORD m_dwHotTextColor;
	DWORD m_dwPushedTextColor;
	DWORD m_dwFocusedTextColor;
	bool m_bkfirst;
	bool m_bkenabled;
    bool m_bIsMouseHand;

    CStdString m_sNormalImage;
    CStdString m_sHotImage;
    CStdString m_sPushedImage;
    CStdString m_sFocusedImage;
    

	CStdString m_sBkImagebuddy;//�������ڶ���ͼ��
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API COptionUI : public CButtonUI//NOLINT
{
public:
    COptionUI();
    virtual ~COptionUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);

    bool Activate();
    void SetEnabled(bool bEnable = true);

	bool ActivateEx();
	void SelectedEx(bool bSelected);

    LPCTSTR GetSelectedImage();
    void SetSelectedImage(LPCTSTR pStrImage);

	LPCTSTR GetSelectedDisabledImage();
	void SetSelectedDisabledImage(LPCTSTR pStrImage);

	void SetSelectedTextColor(DWORD dwTextColor);
	DWORD GetSelectedTextColor();
		
	LPCTSTR GetForeImage();
	void SetForeImage(LPCTSTR pStrImage);

	LPCTSTR GetSelectedHightImage();
	void SetSelectedHightImage(LPCTSTR pStrImage);

	LPCTSTR GetSelectedBKImage();
	void SetSelectedBKImage(LPCTSTR pStrImage);

    LPCTSTR GetGroup() const;
    void SetGroup(LPCTSTR pStrGroupName = NULL);
    bool IsSelected() const;
    void Selected(bool bSelected);

    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintStatusImage(HDC hDC);
	void PaintBkImage(HDC hDC);
	void PaintText(HDC hDC);

	void SetNormalToolTip(LPCTSTR lptext);
	void SetSelectToolTip(LPCTSTR lptext);//����ѡ����ʾ
protected:
    bool m_bSelected;
    CStdString m_sGroupName;

	DWORD m_dwSelectedTextColor;

	CStdString m_selecttooltip;
	CStdString m_normaltooltip;

    CStdString m_sSelectedImage;
	CStdString m_sSelectedDisabledImage;
	CStdString m_sForeImage;
	CStdString m_sSelectedhightImage;
	CStdString m_sSelectedBkImage;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CTextUI : public CLabelUI//NOLINT
{
public:
    CTextUI();
    ~CTextUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    CStdString* GetLinkContent(int iIndex);

    void DoEvent(TEventUI& event);//NOLINT
    SIZE EstimateSize(SIZE szAvailable);

    void PaintText(HDC hDC);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void SetOwnerDraw(bool btrue = false);

protected:
    enum { MAX_LINK = 8 };
    int m_nLinks;
    RECT m_rcLinks[MAX_LINK];
    CStdString m_sLinks[MAX_LINK];
    int m_nHoverLink;
	bool m_bOwnerDrawText;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CProgressUI : public CLabelUI//NOLINT
{
public:
    CProgressUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    bool IsHorizontal();
    void SetHorizontal(bool bHorizontal = true);
    int GetMinValue() const;
    void SetMinValue(int nMin);
    int GetMaxValue() const;
    void SetMaxValue(int nMax);
    int GetValue() const;
    void SetValue(int nValue);
    LPCTSTR GetForeImage() const;
    void SetForeImage(LPCTSTR pStrImage);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void PaintStatusImage(HDC hDC);

	void SetTileRect();

	void SetShowForeImageTiled(bool bshow);

protected:
    bool m_bHorizontal;
    int m_nMax;
    int m_nMin;
    int m_nValue;

	RECT m_rectForeImage;
	bool m_bShowTileForeimage;

    CStdString m_sForeImage;
    CStdString m_sForeImageModify;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CSliderUI : public CProgressUI//NOLINT
{
public:
    CSliderUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetEnabled(bool bEnable = true);

    int GetChangeStep();
    void SetChangeStep(int step);
    void SetThumbSize(SIZE szXY);
    RECT GetThumbRect() const;
    LPCTSTR GetThumbImage() const;
    void SetThumbImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbHotImage() const;
    void SetThumbHotImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbPushedImage() const;
    void SetThumbPushedImage(LPCTSTR pStrImage);

    void DoEvent(TEventUI& event);//NOLINT
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void PaintStatusImage(HDC hDC);

protected:
    SIZE m_szThumb;
    UINT m_uButtonState;
    int m_nStep;
	

    CStdString m_sThumbImage;
    CStdString m_sThumbHotImage;
    CStdString m_sThumbPushedImage;

    CStdString m_sImageModify;

    bool       m_bIsThumbMDown;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class CEditWnd;

class UILIB_API CEditUI : public CLabelUI//NOLINT
{
    friend class CEditWnd;
public:
    CEditUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    void SetEnabled(bool bEnable = true);
    void SetText(LPCTSTR pstrText);
    void SetMaxChar(UINT uMax);
    UINT GetMaxChar();
    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly() const;
    void SetPasswordMode(bool bPasswordMode);
    bool IsPasswordMode() const;
    void SetDisabledIME(bool bIsDisabledIME);
    bool IsDisabledIME() const;
    void SetPasswordChar(TCHAR cPasswordChar);
    TCHAR GetPasswordChar() const;
	void SetNumberOnly(bool bNumberOnly);
	bool IsNumberOnly() const;
	int GetWindowStyls() const;

    LPCTSTR GetNormalImage();
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage();
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage();
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage();
    void SetDisabledImage(LPCTSTR pStrImage);
    void SetNativeEditBkColor(DWORD dwBkColor);
    DWORD GetNativeEditBkColor() const;

    void SetSel(long nStartChar, long nEndChar);
    void SetSelAll();
    void SetReplaceSel(LPCTSTR lpszReplace);

    void SetPos(RECT rc);
    void SetVisible(bool bVisible = true);
    void SetInternVisible(bool bVisible = true);
    SIZE EstimateSize(SIZE szAvailable);
    void DoEvent(TEventUI& event);//NOLINT
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintStatusImage(HDC hDC);
    void PaintText(HDC hDC);
    void PaintBorder(HDC hDC);

	HWND GetEditHwnd();
	
	void SetDefaultString(LPCTSTR lptext);

protected:
    CEditWnd* m_pWindow;

    UINT m_uMaxChar;
    bool m_bReadOnly;
    bool m_bPasswordMode;
    bool m_bIsDisabledIME;
    TCHAR m_cPasswordChar;
    UINT m_uButtonState;
    CStdString m_sNormalImage;
    CStdString m_sHotImage;
    CStdString m_sFocusedImage;
    CStdString m_sDisabledImage;
    DWORD m_dwEditbkColor;
	int m_iWindowStyls;
	DWORD m_dwDefaultTextColor; //Ĭ���������ɫ
	CStdString m_sDefaultString; //Ĭ���ַ���������û�����ĵ�ʱ����ʾ��
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CScrollBarUI : public CControlUI//NOLINT
{
public:
    CScrollBarUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    CContainerUI* GetOwner() const;
    void SetOwner(CContainerUI* pOwner);

    void SetVisible(bool bVisible = true);
    void SetEnabled(bool bEnable = true);
    void SetFocus();

    bool IsHorizontal();
    void SetHorizontal(bool bHorizontal = true);
    int GetScrollRange() const;
    void SetScrollRange(int nRange);
    int GetScrollPos() const;
    void SetScrollPos(int nPos);
    int GetLineSize() const;
    void SetLineSize(int nSize);
    bool IsScroll() const;

    bool GetShowButton1();
    void SetShowButton1(bool bShow);
    LPCTSTR GetButton1NormalImage();
    void SetButton1NormalImage(LPCTSTR pStrImage);
    LPCTSTR GetButton1HotImage();
    void SetButton1HotImage(LPCTSTR pStrImage);
    LPCTSTR GetButton1PushedImage();
    void SetButton1PushedImage(LPCTSTR pStrImage);
    LPCTSTR GetButton1DisabledImage();
    void SetButton1DisabledImage(LPCTSTR pStrImage);

    bool GetShowButton2();
    void SetShowButton2(bool bShow);
    LPCTSTR GetButton2NormalImage();
    void SetButton2NormalImage(LPCTSTR pStrImage);
    LPCTSTR GetButton2HotImage();
    void SetButton2HotImage(LPCTSTR pStrImage);
    LPCTSTR GetButton2PushedImage();
    void SetButton2PushedImage(LPCTSTR pStrImage);
    LPCTSTR GetButton2DisabledImage();
    void SetButton2DisabledImage(LPCTSTR pStrImage);

    LPCTSTR GetThumbNormalImage();
    void SetThumbNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbHotImage();
    void SetThumbHotImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbPushedImage();
    void SetThumbPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbDisabledImage();
    void SetThumbDisabledImage(LPCTSTR pStrImage);

    LPCTSTR GetRailNormalImage();
    void SetRailNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetRailHotImage();
    void SetRailHotImage(LPCTSTR pStrImage);
    LPCTSTR GetRailPushedImage();
    void SetRailPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetRailDisabledImage();
    void SetRailDisabledImage(LPCTSTR pStrImage);

    LPCTSTR GetBkNormalImage();
    void SetBkNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetBkHotImage();
    void SetBkHotImage(LPCTSTR pStrImage);
    LPCTSTR GetBkPushedImage();
    void SetBkPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetBkDisabledImage();
    void SetBkDisabledImage(LPCTSTR pStrImage);

    void SetPos(RECT rc);
    void DoEvent(TEventUI& event);//NOLINT
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void DoPaint(HDC hDC, const RECT& rcPaint);

    void PaintBk(HDC hDC);
    void PaintButton1(HDC hDC);
    void PaintButton2(HDC hDC);
    void PaintThumb(HDC hDC);
    void PaintRail(HDC hDC);

protected:

    enum { 
        DEFAULT_SCROLLBAR_SIZE = 16,
        DEFAULT_TIMERID = 10,
    };

    bool m_bHorizontal;
    int m_nRange;
    int m_nScrollPos;
    int m_nLineSize;
    CContainerUI* m_pOwner;
    POINT ptLastMouse;
    int m_nLastScrollPos;
    int m_nLastScrollOffset;
    int m_nScrollRepeatDelay;

    CStdString m_sBkNormalImage;
    CStdString m_sBkHotImage;
    CStdString m_sBkPushedImage;
    CStdString m_sBkDisabledImage;

    bool m_bShowButton1;
    RECT m_rcButton1;
    UINT m_uButton1State;
    CStdString m_sButton1NormalImage;
    CStdString m_sButton1HotImage;
    CStdString m_sButton1PushedImage;
    CStdString m_sButton1DisabledImage;

    bool m_bShowButton2;
    RECT m_rcButton2;
    UINT m_uButton2State;
    CStdString m_sButton2NormalImage;
    CStdString m_sButton2HotImage;
    CStdString m_sButton2PushedImage;
    CStdString m_sButton2DisabledImage;

    RECT m_rcThumb;
    UINT m_uThumbState;
    CStdString m_sThumbNormalImage;
    CStdString m_sThumbHotImage;
    CStdString m_sThumbPushedImage;
    CStdString m_sThumbDisabledImage;

    CStdString m_sRailNormalImage;
    CStdString m_sRailHotImage;
    CStdString m_sRailPushedImage;
    CStdString m_sRailDisabledImage;

    CStdString m_sImageModify;
};

/// ����ͨ�ĵ�ѡ��ť�ؼ���ֻ���ǡ������ֽ��
/// ������COptionUI��ֻ��ÿ��ֻ��һ����ť���ѣ�����Ϊ�գ������ļ�Ĭ�����Ծ�����
/// <CheckBox name="CheckBox" value="height='20' align='left' textpadding='24,0,0,0' normalimage='file='sys_check_btn.png' source='0,0,20,20' dest='0,0,20,20'' selectedimage='file='sys_check_btn.png' source='20,0,40,20' dest='0,0,20,20'' disabledimage='file='sys_check_btn.png' source='40,0,60,20' dest='0,0,20,20''"/>

class UILIB_API CCheckBoxUI : public COptionUI//NOLINT
{
public:
    CCheckBoxUI();

	LPCTSTR GetClass() const;
    // ��������
    int GetStatusIconOffset() const;
    void SetStatusIconOffset(int nOffset);
    int GetStatusIconSpace() const;
    void SetStatusIconSpace(int nSpace);
    int GetStatusIconWidth() const;
    void SetStatusIconWidth(int nWidth);
    int GetStatusIconHeight() const;
    void SetStatusIconHeight(int nHeight);

    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void SetCheck(bool bCheck);
	bool GetCheck() const;

    // ����
    void PaintStatusImage(HDC hDC);

private:
    inline void SetTextLeftOffset();

protected://NOLINT
    int m_nStatusIconOffset;
    int m_nStatusIconSpace;
    int m_nStatusIconWidth;
    int m_nStatusIconHeight;
};


/// ��չ�����б��
/// ����arrowimage����,һ��ͼƬƽ���ֳ�5��,Normal/Hot/Pushed/Focused/Disabled(������source����)
/// <Default name="ComboBox" value="arrowimage=&quot;file='sys_combo_btn.png' source='0,0,16,16'&quot; "/>
class UILIB_API CComboBoxUI : public CComboUI//NOLINT
{
public:
	CComboBoxUI();
	LPCTSTR GetClass() const;

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void PaintText(HDC hDC);
	void PaintStatusImage(HDC hDC);

protected:
	CStdString m_sArrowImage;
	int        m_nArrowWidth;
};


class CDateTimeWnd;

/// ʱ��ѡ��ؼ�
class UILIB_API CDateTimeUI : public CLabelUI ,public IMessageFilterUI//NOLINT
{
	friend class CDateTimeWnd;
public:
	CDateTimeUI();
	~CDateTimeUI();
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	SYSTEMTIME& GetTime();
	void SetTime(SYSTEMTIME* pst);

	void SetReadOnly(bool bReadOnly);
	bool IsReadOnly() const;

	void UpdateText();

	void DoEvent(TEventUI& event);//NOLINT

	void DoInit();
    virtual void SetPos(RECT rc);

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);//NOLINT


protected:
	SYSTEMTIME m_sysTime;
	int        m_nDTUpdateFlag;
	bool       m_bReadOnly;

	CDateTimeWnd* m_pWindow;
};

} // namespace DuiLib

#endif // __UICOMMONCONTROLS_H__

