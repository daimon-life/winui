// Windows API 头文件
#include <windows.h>
#include <tchar.h>
// GDI+库
#include <gdiplus.h>
// ATL
#include <atlbase.h>
#include <atlstr.h>
// Duilib库
#include "doui\UIlib.h"
// ui
#include "MainWnd.h"

// 执行起点
int APIENTRY _tWinMain(_In_	 HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_	 LPTSTR    lpCmdLine,
	_In_	 int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	// 绘图初始化   ---  GDI+环境初始化
	ULONG_PTR GdiplusToken;
	Gdiplus::GdiplusStartupInput StartupInput;
	GdiplusStartup(&GdiplusToken, &StartupInput, nullptr);

	do
	{
		// 初始化xDui环境
		CPaintManagerUI::SetInstance(hInstance);
		CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + L"skin");
		// 创建主界面
		CMainWnd* pMainWnd = new CMainWnd();
		pMainWnd->Create(NULL, L"录播播放器", UI_WNDSTYLE_FRAME, WS_EX_ACCEPTFILES);
		pMainWnd->CenterWindow();
		pMainWnd->ShowWindow();
		// 进行消息循环
		CPaintManagerUI::MessageLoop();

	} while (false);

	// 释放GDI+环境
	Gdiplus::GdiplusShutdown(GdiplusToken);

	return 0;
}