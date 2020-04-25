// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <atlbase.h>
#include <atlstr.h>

#include <olectl.h>
#include <shlwapi.h>
#include <shellapi.h>


#include "UIlib.h"
#include "MsGdiplus.h"
#include "MsImage.h"


// #define USE_GDIPlUS_DRAW 1
#define lengthof(x) (sizeof(x)/sizeof(*x))
#define MAX max
#define MIN min
#define CLAMP(x,a,b) (MIN(b,MAX(a,x)))
#define CHECKRESULT(sub) { HRESULT _hr; if ( FAILED( _hr=sub ) ) _com_issue_error(_hr); }


#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib, "Msimg32.lib")
#pragma warning(disable:4482 4996)
