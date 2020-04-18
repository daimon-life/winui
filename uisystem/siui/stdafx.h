// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

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
