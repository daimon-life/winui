#pragma once


namespace DuiLib {
    struct UILIB_API SpliteItem{
        CControlUI* pcontrl;
        int	length;//这里是高度。或宽度
        BOOL bVariabled;//是否是可变长的
        SpliteItem()
        {
            pcontrl = NULL;
            length = 0;
            bVariabled = FALSE;
        }
    };

    /*
class  ISplitbase: public DuiLib::CControlUI
{
public:
ISplitbase() {}
virtual ~ISplitbase() {}

virtual VOID SetSplitMinSize(INT nItem1, INT nItem2) = 0;
virtual	BOOL GetSplitMinSize(INT& nItem1, INT& nItem2) = 0;

virtual	VOID SetSplitPos(INT nPos) = 0;
virtual INT GetSplitPos() const = 0;

virtual VOID SetResizeStyle(DWORD dwStyle) = 0;
virtual DWORD GetResizeStyle() const = 0;
};
*/

    class UILIB_API CXHitTestControl : public DuiLib::CControlUI
    {
    public:
        CXHitTestControl(void);
        ~CXHitTestControl(void);

    private:

        LPVOID GetInterface(LPCTSTR pstrName);
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

        UINT GetControlFlags() const;

        virtual void DoEvent(TEventUI& event);

        //IDUISplit
        virtual VOID SetSplitMinSize(INT nItem1, INT nItem2);
        virtual	BOOL GetSplitMinSize(INT& nItem1, INT& nItem2);

        virtual	VOID SetSplitPos(INT nPos);
        virtual INT GetSplitPos() const;

        virtual VOID SetResizeStyle(DWORD dwStyle);
        virtual DWORD GetResizeStyle() const;

    protected:
        virtual BOOL IsVertical();//是否垂直呢

        void SetVertical(BOOL bvert);


        VOID LayoutSplit(INT nPos);
        VOID SaveCurrentRadtio();
        VOID GetSplitterRect(RECT& rtSplitter);

        LRESULT OnLButtonDown(const TEventUI& info);
        LRESULT OnLButtonUp(const  TEventUI& info);
        LRESULT OnMouseMove(const TEventUI& info);

        void SetStartPoint(int x, int y);
        void SetStartPoint(POINT pt);

        void SetItems(CControlUI* pcontrol1, CControlUI* pcontrol2);

        void SetItems(LPCTSTR lp1, LPCTSTR lp2);

        void SetOperItem(LPCTSTR strvalue);

    protected:
        INT m_nSplitPos;
        INT m_nMinItem1;
        INT m_nMinItem2;

        BOOL m_bCanResize;   // 分隔条是否可以拖动
        BOOL m_bVert;

        DWORD m_dwStyle;
        double m_dbRatio;
        BOOL m_bTracking;
        DWORD m_dwTrackStatus;
        DWORD m_dwResizeStyle;

        POINT m_Startpt;

        BOOL m_bFirst;
        SpliteItem m_item1;
        SpliteItem m_item2;
        CStdString m_stritem1;
        CStdString m_stritem2;
    };
};