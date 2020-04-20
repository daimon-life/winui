﻿#ifdef UILIB_STATIC
#define UILIB_API 
#else
#if defined(UILIB_EXPORTS)
#	if defined(_MSC_VER)
#		define UILIB_API __declspec(dllexport)
#	else
#		define UILIB_API 
#	endif
#else
#	if defined(_MSC_VER)
#		define UILIB_API __declspec(dllimport)
#	else
#		define UILIB_API 
#	endif
#endif
#endif
#define UILIB_COMDAT __declspec(selectany)

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include "3rd/FawLib/FawLib.hpp"

#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <tuple>
#include <thread>
#include <type_traits>
#include <variant>
#include <cstdint>
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <comdef.h>
#include <Gdiplus.h>

#pragma warning (disable : 4100)
#pragma warning (disable : 4244)
#pragma warning (disable : 4302)
#pragma warning (disable : 4311)
#pragma warning (disable : 4312)
#pragma warning (disable : 4505)

//#define USE_XIMAGE_EFFECT

#ifndef _FAW_STRING_TYPE
#define _FAW_STRING_TYPE
#ifdef UNICODE
typedef std::wstring faw::String;
#else
typedef std::string faw::String;
#endif

#define _USE_STRING_VIEW
#ifdef _USE_STRING_VIEW
#include <string_view>
#ifdef UNICODE
typedef std::wstring_view faw::string_view_t;
#else
typedef std::string_view faw::string_view_t;
#endif
#else //_USE_STRING_VIEW
namespace std {
	typedef string string_view;
	typedef wstring wstring_view;
}
typedef faw::String faw::string_view_t;
#endif //_USE_STRING_VIEW
#endif

#include "Utils/Utils.h"
#include "Utils/unzip.h"
#include "Utils/VersionHelpers.h"
#include "Core/UIMarkup.h"
#include "Utils/observer_impl_base.h"
#include "Utils/UIShadow.h"
#include "Utils/UIDelegate.h"
#include "Utils/DragDropImpl.h"
#include "Utils/TrayIcon.h"
#include "Utils/DPI.h"

#include "Core/UIDefine.h"
#include "Core/UIResourceManager.h"
#include "Core/UIManager.h"
#include "Core/UIBase.h"
#include "Core/ControlFactory.h"
#include "Core/UIControl.h"
#include "Core/UIContainer.h"

#include "Core/UIDlgBuilder.h"
#include "Core/UIRender.h"
#include "Utils/WinImplBase.h"

#include "Layout/UIVerticalLayout.h"
#include "Layout/UIHorizontalLayout.h"
#include "Layout/UITileLayout.h"
#include "Layout/UITabLayout.h"
#include "Layout/UIChildLayout.h"

#include "Control/UIList.h"
#include "Control/UICombo.h"
#include "Control/UIScrollBar.h"
#include "Control/UITreeView.h"

#include "Control/UILabel.h"
#include "Control/UIText.h"
#include "Control/UIEdit.h"
#include "Control/UIGifAnim.h"
#include "Control/UIGifAnimEx.h"

#include "Control/UIAnimation.h"
#include "Layout/UIAnimationTabLayout.h"
#include "Control/UIButton.h"
#include "Control/UIOption.h"

#include "Control/UIProgress.h"
#include "Control/UISlider.h"

#include "Control/UIComboBox.h"
#include "Control/UIRichEdit.h"
#include "Control/UIDateTime.h"
#include "Control/UIIPAddress.h"
#include "Control/UIIPAddressEx.h"

#include "Control/UIActiveX.h"
#include "Control/UIWebBrowser.h"
#include "Control/UIFlash.h"

#include "Control/UIMenu.h"
#include "Control/UIGroupBox.h"
#include "Control/UIRollText.h"
#include "Control/UIColorPalette.h"
#include "Control/UIListEx.h"
#include "Control/UIHotKey.h"
#include "Control/UIFadeButton.h"
#include "Control/UIRing.h"

#include "Utils/FawTools.hpp"
#include "Bind/BindBase.h"
#include "Bind/BindCtrls.hpp"

#pragma comment (lib, "comctl32.lib")
#pragma comment (lib, "GdiPlus.lib")
#pragma comment (lib, "Imm32.lib")