#pragma once
#include "Util.h"
#include "ImgObject.h"
namespace DuiLib{
	class CPaintManagerUI;

	class CXImgObject : public CImgObject<TImageInfo>
	{
		void OnDelete()
		{
			if (this->hBitmap)
			{
				::DeleteObject(this->hBitmap);
				this->hBitmap = NULL;
			}

			if (this->hGrayBitmap)
			{
				::DeleteObject(this->hGrayBitmap);
				this->hGrayBitmap = NULL;
			}
			delete this;
		}

	public:
		void TRACEINFO(MAP_PAINTMGR& mapp)
		{

			mapp = this->m_mapInfo;
			TRACE(_T("Ãû×Ö£º%d\r\n ¶ÔÏó:%d"),this->hBitmap,this->m_mapInfo.size());

		}
	};

	typedef CXImgObject* LPImage;
	typedef std::map<CStdString,LPImage> MAP_IMAGE;

	
	class CImgObjectMgr
	{
	public:
		CImgObjectMgr(void);
		~CImgObjectMgr(void);

		static CImgObjectMgr* GetImgMgr();

		TImageInfo* GetImage(CPaintManagerUI* pMgr, LPCTSTR StrImage,LPCTSTR type, DWORD mask);

		TImageInfo* GetGrayImage(CPaintManagerUI* pMgr, LPCTSTR StrImage,LPCTSTR type, DWORD mask);

		void ReleaseImage(CPaintManagerUI* pMgr);

		bool RemoveImage(CPaintManagerUI* pMgr, LPCTSTR StrImage);

	private:
		void RemoveAllImage();

	private:
		const TImageInfo* AddImage(CStdString bitmap, LPCTSTR type, DWORD mask);

	private:
		MAP_IMAGE m_ImgMap;

		CRITICAL_SECTION m_Lock;
	};
};