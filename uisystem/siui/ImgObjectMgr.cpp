#include "stdafx.h"
#include "ImgObjectMgr.h"
#include "UIManager.h"

namespace DuiLib{
	CImgObjectMgr::CImgObjectMgr(void)
	{
		::InitializeCriticalSection(&m_Lock);
	}

	CImgObjectMgr::~CImgObjectMgr(void)
	{
		//这里直接清完。不用看。

		RemoveAllImage();
		::DeleteCriticalSection(&m_Lock);
	}

	void CImgObjectMgr::RemoveAllImage()
	{

		XMutex tmp(m_Lock);
		//如果这里的size真的大于0了。肯定是有窗体未删除。去查下哪个窗口没释放
		ATLASSERT(m_ImgMap.size() <= 0);
		MAP_IMAGE::iterator it = m_ImgMap.begin();
		while( it!=m_ImgMap.end())
		{	

#ifdef _DEBUG
			MAP_PAINTMGR Map;
			it->second->TRACEINFO(Map);

			for (MAP_PAINTMGR::iterator itinfo = Map.begin(); itinfo != Map.end(); itinfo++)
			{
				static_cast<CPaintManagerUI*>(itinfo->second)->TRACEINFO();
			}
#endif
			delete it->second;
			m_ImgMap.erase(it++);
		}
	}

	CImgObjectMgr* CImgObjectMgr::GetImgMgr()
	{

		static CImgObjectMgr s_instance;
		return &s_instance;
	}

	TImageInfo* CImgObjectMgr::GetImage(CPaintManagerUI* pMgr, LPCTSTR StrImage,LPCTSTR type, DWORD mask)
	{
		if (pMgr == NULL || StrImage == _T(""))
		{
			return NULL;
		}
		TImageInfo* ImageInfo = NULL;
		XMutex tmp(m_Lock);
		MAP_IMAGE::iterator it = m_ImgMap.find(StrImage);
		if (it != m_ImgMap.end())
		{
			ImageInfo = it->second->GetReal(pMgr);
		}
		else
		{
			AddImage(StrImage,type,mask);
			it = m_ImgMap.find(StrImage);
			if (it != m_ImgMap.end())
				ImageInfo = it->second->GetReal(pMgr);
		}

		return ImageInfo;
	}

	const TImageInfo* CImgObjectMgr::AddImage(CStdString bitmap, LPCTSTR type, DWORD mask)
	{
		if (bitmap.IsEmpty())
		{
			return NULL;
		}
		LPImage data = NULL;
		TImageInfo* ptmp = NULL;
		if( type != NULL ) {
			if( isdigit(*bitmap) ) {
				LPTSTR pstr = NULL;
				int iIndex = _tcstol(bitmap, &pstr, 10);
				ptmp = (CRenderEngine::LoadImage(iIndex, type, mask));
			}
		}
		else {
			ptmp = (CRenderEngine::LoadImage(bitmap.GetData(), NULL, mask));
		}

		if (ptmp == NULL)
		{
			return NULL;
		}
		data = new CXImgObject;

		data->hBitmap = ptmp->hBitmap;
		data->alphaChannel = ptmp->alphaChannel;
		data->hGrayBitmap = ptmp->hGrayBitmap;
		data->nX = ptmp->nX;
		data->nY = ptmp->nY;
		delete ptmp;


		if( !data ) return NULL;
		if( type != NULL ) data->sResType = type;
		data->dwMask = mask;
		XMutex tmp(m_Lock);
		m_ImgMap[bitmap] = data ;
		return data;
	}

	TImageInfo* CImgObjectMgr::GetGrayImage(CPaintManagerUI* pMgr, LPCTSTR StrImage,LPCTSTR type, DWORD mask)
	{

		if (pMgr == NULL || StrImage == _T(""))
		{
			return NULL;
		}
		TImageInfo* ImageInfo = NULL;
		XMutex tmp(m_Lock);
		MAP_IMAGE::iterator it = m_ImgMap.find(StrImage);
		if (it != m_ImgMap.end())
		{
			ImageInfo = it->second->GetReal(pMgr);
		}
		else
		{
			AddImage(StrImage,type,mask);
			it = m_ImgMap.find(StrImage);
			if (it != m_ImgMap.end())
				ImageInfo = it->second->GetReal(pMgr);
		}

		if (ImageInfo->hGrayBitmap == NULL)
		{
			ImageInfo->hGrayBitmap = CRenderEngine::ConverColToGray(ImageInfo->hBitmap);
		}

		return ImageInfo;
	}

	void CImgObjectMgr::ReleaseImage(CPaintManagerUI* pMgr)
	{
		XMutex tmp(m_Lock);
		MAP_IMAGE::iterator it = m_ImgMap.begin();
		while( it!=m_ImgMap.end())
		{
			if (it->second->ReleaseMgr(pMgr))
			{
				//delete it->second;
				m_ImgMap.erase(it++);
			}
			else
				it++;
		}
	}


	bool CImgObjectMgr::RemoveImage(CPaintManagerUI* pMgr, LPCTSTR StrImage)
	{
		XMutex tmp(m_Lock);
		MAP_IMAGE::iterator it = m_ImgMap.find(StrImage);
		if (it != m_ImgMap.end())
		{
			if (it->second->ReleaseMgr(pMgr))
			{
				m_ImgMap.erase(it);
			}
		}
		else
			return false;

		return true;
	}
};