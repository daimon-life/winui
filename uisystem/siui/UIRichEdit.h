#ifndef __UIRICHEDIT_H__
#define __UIRICHEDIT_H__

#pragma once
#include <stdio.h>
#include <Richole.h>
class ITextServices;
using namespace std;

namespace DuiLib {

class CTxtWinHost;

namespace OleLib{
	class CImageOleObject;
};

class CRichEditUI;
class RichEditEvent
{
public:
	//DragDrop
	virtual bool OnDragDropOver( const duistring& pFilePath ) = 0;
	virtual bool OnDropDown( const duistring& pFilePath,int nPos ) = 0;
	virtual bool OnDropLeave( const duistring& pFilePath ) = 0 ;	
	/*---------------------------------------------------------------------*\
			decription : 鼠标或键盘点击ole对象
			parament:
					pObj	—— 文件路径 或 ControlUI
					dwType	—— 值为1时pObj指向文件路径，值为2时指向ControlUI
					bRight	—— true表示鼠标右键，false表示鼠标左键
	\*---------------------------------------------------------------------*/
	virtual void OnClickOleObject(void* pObj,DWORD dwType,long pt,bool bRight ) = 0;
	/*---------------------------------------------------------------------*\
			decription : 删除ole对象
			parament:
					pObj	—— 文件路径 或 ControlUI
					dwType	—— 值为1时pObj指向文件路径，值为2时指向ControlUI
	\*---------------------------------------------------------------------*/
	virtual void OnDeleteOleObject(void* pObj,DWORD dwType) = 0;

	virtual void OnUrl(void* pObj,DWORD dwType,long lPos) = 0;

	virtual void OnPasteHBITMAP(CRichEditUI* pRichEdit) = 0; // ^ + V粘贴HBITMAP
};




class UILIB_API CRichEditUI : public CContainerUI, public IMessageFilterUI,public IDragDropEvent
{
public:
    CRichEditUI();
    ~CRichEditUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

	ITextServices* GetTextServices();
	HRESULT		TxGetText(BSTR *pbstrText);
	bool IsWantTab();
    void SetWantTab(bool bWantTab = true);
    bool IsWantReturn();
    void SetWantReturn(bool bWantReturn = true);
    bool IsWantCtrlReturn();
    void SetWantCtrlReturn(bool bWantCtrlReturn = true);
    bool IsRich();
    void SetRich(bool bRich = true);
    bool IsReadOnly();
    void SetReadOnly(bool bReadOnly = true);
    bool GetWordWrap();
    void SetWordWrap(bool bWordWrap = true);
    int GetFont();
    void SetFont(int index);
    void SetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    LONG GetWinStyle();
    void SetWinStyle(LONG lStyle);
    DWORD GetTextColor();
    void SetTextColor(DWORD dwTextColor);
    int GetLimitText();
    void SetLimitText(int iChars);
    long GetTextLength(DWORD dwFlags = GTL_DEFAULT) const;
    CStdString GetText() const;
    void SetText(LPCTSTR pstrText);
    bool GetModify() const;
    void SetModify(bool bModified = true) const;
    void GetSel(CHARRANGE &cr) const;
    void GetSel(long& nStartChar, long& nEndChar) const;
    int SetSel(CHARRANGE &cr);
    int SetSel(long nStartChar, long nEndChar);
    void ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo = false);
    void ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo = false);
    CStdString GetSelText() const;
    int SetSelAll();
    int SetSelNone();
    WORD GetSelectionType() const;
    bool GetZoom(int& nNum, int& nDen) const;
    bool SetZoom(int nNum, int nDen);
    bool SetZoomOff();
    bool GetAutoURLDetect() const;
    bool SetAutoURLDetect(bool bAutoDetect = true);
    DWORD GetEventMask() const;
    DWORD SetEventMask(DWORD dwEventMask);
    CStdString GetTextRange(long nStartChar, long nEndChar) const;
    void HideSelection(bool bHide = true, bool bChangeStyle = false);
    void ScrollCaret();
    
	virtual HRESULT  OnContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg, HMENU FAR * lphmenu) {
		return S_OK;
	};
	virtual int InsertText(LPCTSTR text , CHARFORMAT2 cf, BOOL bParse = FALSE );
	virtual int InsertText(LPCTSTR text ,COLORREF cr = RGB( 200 , 46 , 56 ));

	virtual int InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo = false);
    int AppendText(LPCTSTR lpstrText, bool bCanUndo = false);
    DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const;
    bool SetDefaultCharFormat(CHARFORMAT2 &cf);
    DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const;
    bool SetSelectionCharFormat(CHARFORMAT2 &cf);
    bool SetWordCharFormat(CHARFORMAT2 &cf);
    DWORD GetParaFormat(PARAFORMAT2 &pf) const;
    bool SetParaFormat(PARAFORMAT2 &pf);
    bool Redo();
    bool Undo();
    void Clear();
    void Copy();
    void Cut();
    void Paste();

	virtual BOOL OnPaste();
	void ClearAll();
	int GetLineCount() const;
	BOOL CanPaste(UINT nFormat = 0) const;
	CStdString GetLine(int nIndex, int nMaxLength) const;
	int LineIndex(int nLine = -1) const;
	int LineLength(int nLine = -1) const;
    bool LineScroll(int nLines, int nChars = 0);
    bool ScrollEndLine();
	CDuiPoint GetCharPos(long lChar) const;
    long LineFromChar(long nIndex) const;
    CDuiPoint PosFromChar(UINT nChar) const;
    int CharFromPos(CDuiPoint pt) const;
    void EmptyUndoBuffer();
    UINT SetUndoLimit(UINT nLimit);
    long StreamIn(int nFormat, EDITSTREAM &es);
    long StreamOut(int nFormat, EDITSTREAM &es);

    void DoInit();
    // 注意：TxSendMessage和SendMessage是有区别的，TxSendMessage没有multibyte和unicode自动转换的功能，
    // 而richedit2.0内部是以unicode实现的，在multibyte程序中，必须自己处理unicode到multibyte的转换
    virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const; 
    IDropTarget* GetTxDropTarget();
    virtual bool OnTxViewChanged();
    virtual void OnTxNotify(DWORD iNotify, void *pv);

    void SetScrollPos(SIZE szPos);
    void LineUp();
    void LineDown();
    void PageUp();
    void PageDown();
    void HomeUp();
    void EndDown();
    void LineLeft();
    void LineRight();
    void PageLeft();
    void PageRight();
    void HomeLeft();
    void EndRight();

    SIZE EstimateSize(SIZE szAvailable);
    void SetPos(RECT rc);
    void DoEvent(TEventUI& event);
    void DoPaint(HDC hDC, const RECT& rcPaint);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);


	RichEditEvent* GetEventSink(void);
	void RegisterEventSink(RichEditEvent* pEvenSink);
	// for ole object
	IRichEditOle* GetIRichEditOle();
	BOOL SetOLECallback(IRichEditOleCallback* pCallback);

	BOOL InsertControlUI(const CControlUI *pUI,const SIZE& sz,long lPos,bool bClick);

	BOOL InsertGif(LPCTSTR szFilePath,long lPos,bool bClick);

	BOOL InsertThumbnailImage(LPCTSTR szFilePath,long lPos,bool bClick);

	BOOL InsertImage(LPCTSTR szFilePath,long lPos,bool bClick , int faceid= -1);


	//For DropDrag
	bool EnableDropDrag( void );
	virtual IDragDropEvent* getDragEvent();
	duistring*	getRtf(void);
	//
	void ReleaseRes(void* pRes);

	int		getEndCharPos( void );

	LPCTSTR GetFilePath(LPOLEOBJECT);

	void	clearAllOleObj( void );


	void PasteSpecial(UINT uClipFormat, DWORD dwAspect = 0, HMETAFILE hMF = 0);

	virtual void OnCopy(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj)
	{};

protected:
	BOOL InsertOleImage(LPCTSTR szFilePath,DWORD dwUserData,long lPos , int faceid = -1);

	HRESULT DragEnter(IDataObject *pDataObj,DWORD grfKeyState,
		POINTL pt,POINT ptClient,DWORD *pdwEffect);

	HRESULT DragOver(IDataObject *pDataObj,DWORD grfKeyState,POINTL pt,POINT ptClient,
		DWORD *pdwEffect);

	HRESULT DragLeave(IDataObject *pDataObj);

	HRESULT Drop(IDataObject *pDataObj,DWORD grfKeyState,POINTL pt,POINT ptClient,
		DWORD *pdwEffect);

	BOOL InsertThumbnail(LPCTSTR szFilePath,long lPos,SIZE& sz,bool bClick);

	CControlUI* GetInternalControlUI(LPOLEOBJECT lpdata);
	void AdjustSubCtrlUIPos();
	
	int GetFaceId(LPOLEOBJECT);

	BOOL OnDBClick(POINT pt);

protected:
    CTxtWinHost* m_pTwh;
    bool m_bVScrollBarFixing;
    bool m_bWantTab;
    bool m_bWantReturn;
    bool m_bWantCtrlReturn;
    bool m_bRich;
    bool m_bReadOnly;
    bool m_bWordWrap;
    DWORD m_dwTextColor;
    int m_iFont;
    int m_iLimitText;
    LONG m_lTwhStyle;
};

} // namespace DuiLib

#endif // __UIRICHEDIT_H__
