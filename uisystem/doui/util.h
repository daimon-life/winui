#pragma  once

namespace DuiLib {
	typedef enum EVENTTYPE_UI
	{
		UIEVENT__FIRST = 1,
		UIEVENT__KEYBEGIN,
		UIEVENT_KEYDOWN,
		UIEVENT_KEYUP,
		UIEVENT_CHAR,
		UIEVENT_SYSKEY,
		UIEVENT__KEYEND,
		UIEVENT__MOUSEBEGIN,
		UIEVENT_MOUSEMOVE,
		UIEVENT_MOUSELEAVE,
		UIEVENT_MOUSEENTER,
		UIEVENT_MOUSEHOVER,
		UIEVENT_BUTTONDOWN,
		UIEVENT_BUTTONUP,
		UIEVENT_RBUTTONDOWN,
		UIEVENT_DBLCLICK,
		UIEVENT_CONTEXTMENU,
		UIEVENT_SCROLLWHEEL,
		UIEVENT__MOUSEEND,
		UIEVENT_KILLFOCUS,
		UIEVENT_SETFOCUS,
		UIEVENT_WINDOWSIZE,
		UIEVENT_SETCURSOR,
		UIEVENT_TIMER,
		UIEVENT_NOTIFY,
		UIEVENT_COMMAND,
		UIEVENT__LAST,
		UIEVENT_ITEMKILLFOCUS,
	}EVENTTYPE_UI;

	/////////////////////////////////////////////////////////////////////////////////////
	//

	// Flags for CControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

	// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_TOP_FIRST     0x00000008
#define UIFIND_ME_FIRST      0x80000000

	// Flags for the CDialogLayout stretching
#define UISTRETCH_NEWGROUP   0x00000001
#define UISTRETCH_NEWLINE    0x00000002
#define UISTRETCH_MOVE_X     0x00000004
#define UISTRETCH_MOVE_Y     0x00000008
#define UISTRETCH_SIZE_X     0x00000010
#define UISTRETCH_SIZE_Y     0x00000020

	// Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_READONLY     0x00000020
#define UISTATE_CAPTURED     0x00000040



	/////////////////////////////////////////////////////////////////////////////////////
	//

	typedef struct tagTFontInfo
	{
		HFONT hFont;
		CStdString sFontName;
		int iSize;
		bool bBold;
		bool bUnderline;
		bool bItalic;
		TEXTMETRIC tm;
	} TFontInfo;

	typedef struct tagTImageInfo
	{
		HBITMAP hBitmap;
		HBITMAP hGrayBitmap;
		int nX;
		int nY;
		bool alphaChannel;
		CStdString sResType;
		DWORD dwMask;
	} TImageInfo;

	// Structure for notifications from the system
	// to the control implementation.
	typedef struct tagTEventUI
	{
		int Type;
		CControlUI* pSender;
		DWORD dwTimestamp;
		POINT ptMouse;
		TCHAR chKey;
		WORD wKeyState;
		WPARAM wParam;
		LPARAM lParam;
	} TEventUI;

	typedef struct tagSUBTEventUI
	{
		int Type;
		CControlUI* pSender;
		DWORD dwTimestamp;
		POINT ptMouse;
		TCHAR chKey;
		WORD wKeyState;
		WPARAM wParam;
		LPARAM lParam;
	} TSUBEventUI;

	// Structure for notifications to the outside world
	typedef struct tagTNotifyUI 
	{
		CStdString sType;
		CControlUI* pSender;
		DWORD dwTimestamp;
		POINT ptMouse;
		WPARAM wParam;
		LPARAM lParam;
	} TNotifyUI;

	// Structure for relative position to the parent
	typedef struct tagTRelativePosUI
	{
		bool bRelative;
		SIZE szParent;
		int nMoveXPercent;
		int nMoveYPercent;
		int nZoomXPercent;
		int nZoomYPercent;
	}TRelativePosUI;

	// Listener interface
	class INotifyUI
	{
	public:
		virtual void Notify(TNotifyUI& msg) = 0;
	};

	// MessageFilter interface
	class IMessageFilterUI
	{
	public:
		virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
	};


	class XMutex
	{
	public:

		virtual void Lock()
		{
			::EnterCriticalSection(&m_lock);
		}
		virtual void UnLock()
		{
			::LeaveCriticalSection(&m_lock);
		}
		XMutex(CRITICAL_SECTION& lck)
		{
			m_lock = lck;
			Lock();
		}
		~XMutex()
		{
			UnLock();
		}

	private:
		CRITICAL_SECTION m_lock;
	};

};
