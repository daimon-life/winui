#include "WindowImplBaseCtrl.h"
#include "resource.h"


CWindowImplBaseCtrl::CWindowImplBaseCtrl()
{
    m_bIsWndInit = false;
}

CWindowImplBaseCtrl::~CWindowImplBaseCtrl()
{
}

CStdString CWindowImplBaseCtrl::GetSkinResourceType()
{
    return L"UI_XML";
}

void CWindowImplBaseCtrl::Init()
{
    m_bIsWndInit = true;

    SetIcon(IDI_LOGO_ICON);
}

void CWindowImplBaseCtrl::Notify(DuiLib::TNotifyUI& msg)
{
    if (_wcsicmp(msg.sType, Name_wszClick) == 0)
    {
        if (_wcsicmp(msg.pSender->GetName(), Name_wszSysMinBtn) == 0)
        {
            // ��С��
            SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
        }
        else if (_wcsicmp(msg.pSender->GetName(), Name_wszSysMaxBtn) == 0)
        {
            // ���
            SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        }
        else if (_wcsicmp(msg.pSender->GetName(), Name_wszSysRestoreBtn) == 0)
        {
            // �ָ�
            SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
        }
        else if (_wcsicmp(msg.pSender->GetName(), Name_wszSysCloseBtn) == 0)
        {
            // �ر�
            Close();
        }
    }
}

void CWindowImplBaseCtrl::OnFinalMessage(HWND hWnd)
{
    __super::OnFinalMessage(hWnd);
    delete this;
}
