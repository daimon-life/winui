#pragma once

namespace DuiLib
{
	//状态小图片
	namespace GroupBtnStatus
	{
		const TCHAR statesaway[]	= _T("file=\'Res/common/status/away.png\' align=\'rightbottom\'");
		const TCHAR statesbusy[]	= _T("file=\'Res/common/status/busy.png\' align=\'rightbottom\'");
		const TCHAR statesNull[]	= _T(""); //下面的小方框设为空
		const TCHAR stateimageindex[] = _T("bkimagenum1");
	}	
}

namespace DuiLib{
	class UILIB_API CShowAnimation :public CLabelUI,public CEUIAniHelper
{
public:
	CShowAnimation();
	~CShowAnimation();
	void PaintBkImage(HDC hDC);
	void OnFrame();
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
};


	class UILIB_API CGrouopButtonUI :public CButtonUI
	{
	public:
		CGrouopButtonUI(void);
		~CGrouopButtonUI(void);
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		 void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		 void SetBKImageItem(LPCTSTR pstrName, LPCTSTR pStrImage);
		 void PaintBkImage(HDC hDC);
		 void SetGrayBkImage(bool bgray=false);

	private:
		CStdStringPtrMap m_bkimage;
		bool m_bkgray;
	};


}
