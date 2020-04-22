// Windows API ͷ�ļ�
#include <windows.h>
#include <tchar.h>
// GDI+��
#include <gdiplus.h>
// ATL
#include <atlbase.h>
#include <atlstr.h>
// Duilib��
#include "..\..\uisystem\siui\UIlib.h"

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
		CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + L"Skin");
		// ������Ϣѭ��
		CPaintManagerUI::MessageLoop();

	} while (false);

	// �ͷ�GDI+����
	Gdiplus::GdiplusShutdown(GdiplusToken);

	return 0;
}