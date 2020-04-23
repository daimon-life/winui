#include "resource.h"
#include "MainWnd.h"


//
const wchar_t Name_wszOpenVideoSrcBtn[] = L"OpenFileBtn";
const wchar_t Name_wszVideoLayoutTab[] = L"VideoLayoutTab";
const wchar_t Name_wszMovieLayout[] = L"MovieLayout";
const wchar_t Name_wszMovieVideoWnd[] = L"MovieVideoWnd";
const wchar_t Name_wszResourceLayout[] = L"ResourceLayout";
const wchar_t Name_wszPlayCtrlArea[] = L"PlayCtrlArea";
const wchar_t Name_wszPlayProgreSlider[] = L"PlayProgreSlider";
const wchar_t Name_wszPlayTimeLbl[] = L"PlayTimeLbl";
const wchar_t Name_wszVideoSrcInfoBtn[] = L"VideoSrcInfoBtn";
const wchar_t Name_wszResourceModelChk[] = L"ResourceModelChk";
const wchar_t Name_wszMovieModelChk[] = L"MovieModelChk";
const wchar_t Name_wszStopBtn[] = L"StopBtn";
const wchar_t Name_wszPlayBtn[] = L"PlayBtn";
const wchar_t Name_wszPauseBtn[] = L"PauseBtn";
const wchar_t Name_wszFullscreenBtn[] = L"FullscreenBtn";
const wchar_t Name_wszVolumeBtn[] = L"VolumeBtn";
const wchar_t Name_wszMuteBtn[] = L"MuteBtn";
const wchar_t Name_wszVolumeSizeSlider[] = L"VolumeSizeSlider";


CMainWnd::CMainWnd()
{
	
}

CMainWnd::~CMainWnd()
{

}

CControlUI* CMainWnd::CreateControl(LPCTSTR pstrClass)
{
	if (_tcscmp(pstrClass, _T("VideoWindow")) == 0)
	{
		//return new CVideoWindowUI();
		return nullptr;
	}
	else if (_tcscmp(pstrClass, _T("MultiWndLayout")) == 0)
	{
		//return new CMultiWndLayoutUI();
		return nullptr;
	}
	return __super::CreateControl(pstrClass);
}

UINT CMainWnd::GetSkinResourceID()
{
	return IDR_MAINWND_XML;
}

void CMainWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	PostQuitMessage(0);
}

LRESULT	CMainWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_CHAR)
	{
		if (wParam == VK_ESCAPE) // Esc¼ü
		{
			OnMsg_EscKey(lParam);
		}
		else if (wParam == VK_RETURN)
		{
			
		}
		else if (wParam == VK_SPACE) // Space¼ü
		{
			OnMsg_SpaceKey(lParam);
		}
		else if (wParam== 0x46 || wParam == 0x66) // f¼ü
		{
			OnNotify_FullscreenBtn();
		}
		else if (wParam == 0x4F || wParam == 0x6F) // o¼ü
		{
			OnNotify_OpenVideoSrcBtn();
		}
	}
	else if (uMsg == WM_DROPFILES)
	{
		OnMsg_DropFiles(wParam, lParam);
	}
	return __super::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

LRESULT	CMainWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

LRESULT CMainWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return __super::OnDestroy(uMsg, wParam, lParam, bHandled);
}

void CMainWnd::Init()
{
	__super::Init();

	m_pTitleBarLayout = static_cast<CHorizontalLayoutUI*>(paint_manager_.FindControl(Name_wszTitleBarLayout));
	assert(m_pTitleBarLayout != nullptr);
	m_pOpenVideoSrcBtn = static_cast<CButtonUI*>(paint_manager_.FindControl(Name_wszOpenVideoSrcBtn));
	assert(m_pOpenVideoSrcBtn != nullptr);
	m_pVideoLayoutTab = static_cast<CTabLayoutUI*>(paint_manager_.FindControl(Name_wszVideoLayoutTab));
	assert(m_pVideoLayoutTab != nullptr);
	//m_pMovieLayout = static_cast<CHorizontalLayoutUI*>(paint_manager_.FindControl(Name_wszMovieLayout));
	//assert(m_pMovieLayout != nullptr);
	//m_pMovieVideoWnd = static_cast<CVideoWindowUI*>(paint_manager_.FindControl(Name_wszMovieVideoWnd));
	//assert(m_pMovieVideoWnd != nullptr);
	//m_pMovieVideoWnd->SetVisible(false);
	//m_pResourceLayout = static_cast<CMultiWndLayoutUI*>(paint_manager_.FindControl(Name_wszResourceLayout));
	//assert(m_pResourceLayout != nullptr);
	m_pPlayCtrlArea = static_cast<CVerticalLayoutUI*>(paint_manager_.FindControl(Name_wszPlayCtrlArea));
	assert(m_pPlayCtrlArea);
	m_pPlayProgressSlider = static_cast<CSliderUI*>(paint_manager_.FindControl(Name_wszPlayProgreSlider));
	assert(m_pPlayProgressSlider != nullptr);
	m_pPlayTimeLbl = static_cast<CLabelUI*>(paint_manager_.FindControl(Name_wszPlayTimeLbl));
	assert(m_pPlayTimeLbl != nullptr);
	m_pVideoSrcInfoBtn = static_cast<CButtonUI*>(paint_manager_.FindControl(Name_wszVideoSrcInfoBtn));
	assert(m_pVideoSrcInfoBtn != nullptr);
	m_pResourceModelChk = static_cast<CCheckBoxUI*>(paint_manager_.FindControl(Name_wszResourceModelChk));
	assert(m_pResourceModelChk != nullptr);
	m_pMovieModelChk = static_cast<CCheckBoxUI*>(paint_manager_.FindControl(Name_wszMovieModelChk));
	assert(m_pMovieModelChk != nullptr);
	m_pStopBtn = static_cast<CButtonUI*>(paint_manager_.FindControl(Name_wszStopBtn));
	assert(m_pStopBtn != nullptr);
	m_pPlayBtn = static_cast<CButtonUI*>(paint_manager_.FindControl(Name_wszPlayBtn));
	assert(m_pPlayBtn != nullptr);
	m_pPauseBtn = static_cast<CButtonUI*>(paint_manager_.FindControl(Name_wszPauseBtn));
	assert(m_pPauseBtn != nullptr);
	m_pFullscreenBtn = static_cast<CButtonUI*>(paint_manager_.FindControl(Name_wszFullscreenBtn));
	assert(m_pFullscreenBtn != nullptr);
	m_pVolumeBtn = static_cast<CButtonUI*>(paint_manager_.FindControl(Name_wszVolumeBtn));
	assert(m_pVolumeBtn != nullptr);
	m_pMuteBtn = static_cast<CButtonUI*>(paint_manager_.FindControl(Name_wszMuteBtn));
	assert(m_pMuteBtn != nullptr);
	m_pVolumeSizeSlider = static_cast<CSliderUI*>(paint_manager_.FindControl(Name_wszVolumeSizeSlider));
	assert(m_pVolumeSizeSlider != nullptr);
}

void CMainWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == L"windowinit")
	{
		OnNotify_WindowInit();
	}
	else if (msg.sType == L"click")
	{
		if (msg.pSender == m_pOpenVideoSrcBtn)
		{
 			OnNotify_OpenVideoSrcBtn();
		}
		else if (msg.pSender == m_pVideoSrcInfoBtn)
		{
			OnNotify_VideoSrcInfoBtn(m_pVideoSrcInfoBtn);
		}
		else if (msg.pSender == m_pResourceModelChk)
		{
			;
		}
		else if (msg.pSender == m_pMovieModelChk)
		{
			;
		}
		else if (msg.pSender == m_pStopBtn)
		{
			OnNotify_StopBtn(true);
		}
		else if (msg.pSender == m_pPlayBtn)
		{
			OnNotify_PlayBtn(0, true);
		}
		else if (msg.pSender == m_pPauseBtn)
		{
			OnNotify_PauseBtn();
		}
		else if (msg.pSender == m_pFullscreenBtn)
		{
			OnNotify_FullscreenBtn();
		}
		else if (msg.pSender == m_pVolumeBtn)
		{
			OnNotify_VolumeBtn();
		}
		else if (msg.pSender == m_pMuteBtn)
		{
			OnNotify_MuteBtn();
		}
	}
	else if (msg.sType == L"selectchanged")
	{
		if (msg.pSender == m_pResourceModelChk)
		{
			m_pVideoLayoutTab->SelectItem(0);
		}
		else if (msg.pSender == m_pMovieModelChk)
		{
			m_pVideoLayoutTab->SelectItem(1);
		}
	}
	else if (msg.sType == _T("valuechanged"))
	{
		if (msg.pSender == m_pPlayProgressSlider)
		{
			OnNotify_ProgressSlider();
		}
		else if (msg.pSender == m_pVolumeSizeSlider)
		{
			OnNotify_VolumeSlider();
		}
	}
	__super::Notify(msg);
}//NOLINT

void CMainWnd::InitPlayUI()
{

}

void CMainWnd::UpdatePlayProgressUI(uint32_t uCurPlayTime)
{

}

void CMainWnd::UpdatePlayTimeUI(uint32_t uCurPlayTime)
{

}

void CMainWnd::ExitFullScreen()
{

}

bool CMainWnd::IsMediaUrl(string szMediaUrl)
{
	return true;
}

void CMainWnd::LoadMediaUrl()
{

}

void CMainWnd::OpenMediaUrl(string szMediaUrl)
{

}

void CMainWnd::SetMediaVolume(uint32_t uValue)
{

}

void CMainWnd::OnMsg_EscKey(LPARAM lParam)
{

}

void CMainWnd::OnMsg_SpaceKey(LPARAM lParam)
{

}

void CMainWnd::OnMsg_DropFiles(WPARAM wParam, LPARAM lParam)
{

}

void CMainWnd::OnMsg_OpenMedia(WPARAM wParam, LPARAM lParam)
{

}

void CMainWnd::OnMsg_CloseMedia(WPARAM wParam, LPARAM lParam)
{

}

void CMainWnd::OnMsg_UpdateMedia(WPARAM wParam, LPARAM lParam)
{

}

void CMainWnd::OnNotify_WindowInit()
{

}

void CMainWnd::OnNotify_OpenVideoSrcBtn()
{

}

void CMainWnd::OnNotify_VideoSrcInfoBtn(CControlUI* pUI)
{

}

void CMainWnd::OnNotify_StopBtn(bool bActive)
{

}

void CMainWnd::OnNotify_PlayBtn(uint32_t uMilliSecond, bool bActive)
{

}

void CMainWnd::OnNotify_PauseBtn()
{

}

void CMainWnd::OnNotify_FullscreenBtn()
{

}

void CMainWnd::OnNotify_VolumeBtn()
{

}

void CMainWnd::OnNotify_MuteBtn()
{

}

void CMainWnd::OnNotify_ProgressSlider()
{

}

void CMainWnd::OnNotify_VolumeSlider()
{

}
