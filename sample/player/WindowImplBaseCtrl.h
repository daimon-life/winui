#pragma once


#ifndef _X_CWindowImplBaseCtrl_H_
#define _X_CWindowImplBaseCtrl_H_

// ATL
#include <atlbase.h>
#include <atlstr.h>
// Duilib��
#include "doui\UIlib.h"

// �ؼ����¼����������壨ֻ֧��wchar_t��
const wchar_t Name_wszWindowInit[]          = L"windowinit";
const wchar_t Name_wszClick[]               = L"click";
// ��������ؿؼ���
const wchar_t Name_wszTitleBarLayout[]      = L"TitleBarLayout";
const wchar_t Name_wszTitleBarName[]        = L"TitleBarName";
const wchar_t Name_wszSysSkinChangeBtn[]    = L"Sys_SkinChangeBtn";
const wchar_t Name_wszSysMinBtn[]           = L"Sys_MinBtn";
const wchar_t Name_wszSysMaxBtn[]           = L"Sys_MaxBtn";
const wchar_t Name_wszSysRestoreBtn[]       = L"Sys_RestoreBtn";
const wchar_t Name_wszSysCloseBtn[]         = L"Sys_CloseBtn";


// �߱����ڵĻ��������뻻������
class CWindowImplBaseCtrl : public WindowImplBase
{
public:
    CWindowImplBaseCtrl();
    virtual ~CWindowImplBaseCtrl();

public:
    virtual CStdString  GetSkinResourceType();
    virtual void        Init();
    virtual void        Notify(DuiLib::TNotifyUI& msg); // ֪ͨ
    virtual void        OnFinalMessage(HWND hWnd);

private:
    bool                        m_bIsWndInit;                       // �����Ƿ��Ѿ���ʼ����
};


#endif // _X_CWindowImplBaseCtrl_H_
