#pragma once


#ifndef _X_CWindowImplBaseCtrl_H_
#define _X_CWindowImplBaseCtrl_H_

// ATL
#include <atlbase.h>
#include <atlstr.h>
// Duilib库
#include "doui\UIlib.h"

// 控件与事件名常量定义（只支持wchar_t）
const wchar_t Name_wszWindowInit[]          = L"windowinit";
const wchar_t Name_wszClick[]               = L"click";
// 标题栏相关控件名
const wchar_t Name_wszTitleBarLayout[]      = L"TitleBarLayout";
const wchar_t Name_wszTitleBarName[]        = L"TitleBarName";
const wchar_t Name_wszSysSkinChangeBtn[]    = L"Sys_SkinChangeBtn";
const wchar_t Name_wszSysMinBtn[]           = L"Sys_MinBtn";
const wchar_t Name_wszSysMaxBtn[]           = L"Sys_MaxBtn";
const wchar_t Name_wszSysRestoreBtn[]       = L"Sys_RestoreBtn";
const wchar_t Name_wszSysCloseBtn[]         = L"Sys_CloseBtn";


// 具备窗口的基本控制与换肤功能
class CWindowImplBaseCtrl : public WindowImplBase
{
public:
    CWindowImplBaseCtrl();
    virtual ~CWindowImplBaseCtrl();

public:
    virtual CStdString  GetSkinResourceType();
    virtual void        Init();
    virtual void        Notify(DuiLib::TNotifyUI& msg); // 通知
    virtual void        OnFinalMessage(HWND hWnd);

private:
    bool                        m_bIsWndInit;                       // 窗口是否已经初始化了
};


#endif // _X_CWindowImplBaseCtrl_H_
