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
	// 窗口消息处理
	void		OnMsg_EscKey(LPARAM lParam);
	void		OnMsg_SpaceKey(LPARAM lParam);
	void		OnMsg_DropFiles(WPARAM wParam, LPARAM lParam);
	void		OnMsg_OpenMedia(WPARAM wParam, LPARAM lParam);
	void		OnMsg_CloseMedia(WPARAM wParam, LPARAM lParam);
	void		OnMsg_UpdateMedia(WPARAM wParam, LPARAM lParam);
	// 控件通知处理
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
	// UI对象-------------------------------------------------------------------
	CHorizontalLayoutUI*		m_pTitleBarLayout;					// 标题栏
	CButtonUI*					m_pOpenVideoSrcBtn;					// 打开视频源按钮
	CTabLayoutUI*				m_pVideoLayoutTab;					// 视频模式切换布局
	CHorizontalLayoutUI*		m_pMovieLayout;						// 电源模式布局容器
	CVideoWindowUI*				m_pMovieVideoWnd;					// 电影模式一个窗口
	CMultiWndLayoutUI*			m_pResourceLayout;					// 资源模式布局容器
	CVerticalLayoutUI*			m_pPlayCtrlArea;					// 播放控制区
	CSliderUI*					m_pPlayProgressSlider;				// 播放进度滑动条
	CLabelUI*					m_pPlayTimeLbl;						// 播放时间显示
	CButtonUI*					m_pVideoSrcInfoBtn;					// 视频源信息按钮
	CCheckBoxUI*				m_pResourceModelChk;				// 资源模式单选框
	CCheckBoxUI*				m_pMovieModelChk;					// 电影模式单选框
	CButtonUI*					m_pStopBtn;							// 停止按钮
	CButtonUI*					m_pPlayBtn;							// 播放按钮
	CButtonUI*					m_pPauseBtn;						// 暂停按钮
	CButtonUI*					m_pFullscreenBtn;					// 全屏按钮
	CButtonUI*					m_pVolumeBtn;						// 音量按钮
	CButtonUI*					m_pMuteBtn;							// 静音按钮
	CSliderUI*					m_pVolumeSizeSlider;				// 音量大小滑动条
	// 界面参数-----------------------------------------------------------------
	RECT						m_sBeforeFullWndRect;				// 全屏前窗口尺寸
	RECT						m_sCaptionRect;						// 全屏前标题尺寸
	RECT						m_sSizeBoxRect;						// 全屏前移动尺寸
};

#endif // _MainWnd_H_