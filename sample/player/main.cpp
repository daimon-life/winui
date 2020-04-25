// Windows API ͷ�ļ�
#include <windows.h>
#include <tchar.h>
// GDI+��
#include <gdiplus.h>
// ATL
#include <atlbase.h>
#include <atlstr.h>
// Duilib��
#include "doui\UIlib.h"
// ui
#include "MainWnd.h"

// ִ�����
int APIENTRY _tWinMain(_In_	 HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_	 LPTSTR    lpCmdLine,
	_In_	 int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	// ��ͼ��ʼ��   ---  GDI+������ʼ��
	ULONG_PTR GdiplusToken;
	Gdiplus::GdiplusStartupInput StartupInput;
	GdiplusStartup(&GdiplusToken, &StartupInput, nullptr);

	do
	{
		// ��ʼ��xDui����
		CPaintManagerUI::SetInstance(hInstance);
		CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + L"skin");
		// ����������
		CMainWnd* pMainWnd = new CMainWnd();
		pMainWnd->Create(NULL, L"¼��������", UI_WNDSTYLE_FRAME, WS_EX_ACCEPTFILES);
		pMainWnd->CenterWindow();
		pMainWnd->ShowWindow();
		// ������Ϣѭ��
		CPaintManagerUI::MessageLoop();

	} while (false);

	// �ͷ�GDI+����
	Gdiplus::GdiplusShutdown(GdiplusToken);

	return 0;
}