#pragma once


#ifndef _MainWnd_H_
#define _MainWnd_H_


#include "WindowImplBaseCtrl.h"
#include <stdint.h>

class CVideoWindowUI;
class CVideoInfoFloatWnd;
class CMultiWndLayoutUI;
class IAVCoreCtrl;

class CMainWnd : public CWindowImplBaseCtrl
{
public:
	CMainWnd();
	virtual ~CMainWnd();

public:
	CControlUI* CreateControl(LPCTSTR pstrClass);
	UINT		GetSkinResourceID();
	void        OnFinalMessage(HWND hWnd);
	LRESULT		HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);//NOLINT
	LRESULT		OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);//NOLINT
	LRESULT		OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);//NOLINT
	void		Init();
	void		Notify(TNotifyUI& msg);//NOLINT

protected:
	void		InitPlayUI();
	void		UpdatePlayProgressUI(uint32_t uCurPlayTime);
	void		UpdatePlayTimeUI(uint32_t uCurPlayTime);
	void        ExitFullScreen();
	bool		IsMediaUrl(string szMediaUrl);
	void		LoadMediaUrl();
	void		OpenMediaUrl(string szMediaUrl);
	void		SetMediaVolume(uint32_t uValue);

protected:
	// ������Ϣ����
	void		OnMsg_EscKey(LPARAM lParam);
	void		OnMsg_SpaceKey(LPARAM lParam);
	void		OnMsg_DropFiles(WPARAM wParam, LPARAM lParam);
	void		OnMsg_OpenMedia(WPARAM wParam, LPARAM lParam);
	void		OnMsg_CloseMedia(WPARAM wParam, LPARAM lParam);
	void		OnMsg_UpdateMedia(WPARAM wParam, LPARAM lParam);
	// �ؼ�֪ͨ����
	void		OnNotify_WindowInit();
	void		OnNotify_OpenVideoSrcBtn();
	void		OnNotify_VideoSrcInfoBtn(CControlUI* pUI);
	void		OnNotify_StopBtn(bool bActive);
	void		OnNotify_PlayBtn(uint32_t uMilliSecond, bool bActive);
	void		OnNotify_PauseBtn();
	void		OnNotify_FullscreenBtn();
	void		OnNotify_VolumeBtn();
	void		OnNotify_MuteBtn();
	void		OnNotify_ProgressSlider();
	void		OnNotify_VolumeSlider();

	
private:
	// UI����-------------------------------------------------------------------
	CHorizontalLayoutUI*		m_pTitleBarLayout;					// ������
	CButtonUI*					m_pOpenVideoSrcBtn;					// ����ƵԴ��ť
	CTabLayoutUI*				m_pVideoLayoutTab;					// ��Ƶģʽ�л�����
	CHorizontalLayoutUI*		m_pMovieLayout;						// ��Դģʽ��������
	CVideoWindowUI*				m_pMovieVideoWnd;					// ��Ӱģʽһ������
	CMultiWndLayoutUI*			m_pResourceLayout;					// ��Դģʽ��������
	CVerticalLayoutUI*			m_pPlayCtrlArea;					// ���ſ�����
	CSliderUI*					m_pPlayProgressSlider;				// ���Ž��Ȼ�����
	CLabelUI*					m_pPlayTimeLbl;						// ����ʱ����ʾ
	CButtonUI*					m_pVideoSrcInfoBtn;					// ��ƵԴ��Ϣ��ť
	CCheckBoxUI*				m_pResourceModelChk;				// ��Դģʽ��ѡ��
	CCheckBoxUI*				m_pMovieModelChk;					// ��Ӱģʽ��ѡ��
	CButtonUI*					m_pStopBtn;							// ֹͣ��ť
	CButtonUI*					m_pPlayBtn;							// ���Ű�ť
	CButtonUI*					m_pPauseBtn;						// ��ͣ��ť
	CButtonUI*					m_pFullscreenBtn;					// ȫ����ť
	CButtonUI*					m_pVolumeBtn;						// ������ť
	CButtonUI*					m_pMuteBtn;							// ������ť
	CSliderUI*					m_pVolumeSizeSlider;				// ������С������
	// �������-----------------------------------------------------------------
	RECT						m_sBeforeFullWndRect;				// ȫ��ǰ���ڳߴ�
	RECT						m_sCaptionRect;						// ȫ��ǰ����ߴ�
	RECT						m_sSizeBoxRect;						// ȫ��ǰ�ƶ��ߴ�
};

#endif // _MainWnd_H_