#include "stdafx.h"
#include "MsGdiplus.h"
#include <string>

MsGdiplus* MsGdiplus::pGdiplus = NULL; 
MsGdiplus::MsGdiplus() throw()
{
	Status nStu;
	m_gdiplusStartupInput.GdiplusVersion = 1;
	m_gdiplusStartupInput.DebugEventCallback = NULL;
	m_gdiplusStartupInput.SuppressExternalCodecs = FALSE;
	m_gdiplusStartupInput.SuppressBackgroundThread = FALSE;
	nStu = GdiplusStartup(&m_gdiplusToken,&m_gdiplusStartupInput,&m_gdiplusStartupOutput);
	if( nStu != Ok )
		m_gdiplusToken = -1;
	GetImageInfo(m_ImageEncoders.m_pCodecInfo,m_ImageEncoders.m_Count,Encoders);
	GetImageInfo(m_ImageDecoders.m_pCodecInfo,m_ImageDecoders.m_Count,Decoders);
}
MsGdiplus::~MsGdiplus()
{
	if(m_ImageEncoders.m_pCodecInfo)
	{
		free(m_ImageEncoders.m_pCodecInfo);
		m_ImageEncoders.m_pCodecInfo = 0;
	}
	if(m_ImageDecoders.m_pCodecInfo)
	{
		free(m_ImageDecoders.m_pCodecInfo);
		m_ImageDecoders.m_pCodecInfo = 0;
	}
	if( m_gdiplusToken != -1 )
	{
		GdiplusShutdown(m_gdiplusToken);
		m_gdiplusToken = -1;
	}
}
ImageCodecInfo*	MsGdiplus::getCodecEncodersFirst(void)
{
	if( getCodecEncodersTotal() )
		return &(m_ImageEncoders.m_pCodecInfo[0]);
	return 0;
}
ImageCodecInfo* MsGdiplus::getCodecDecodersFirst(void)
{
	if( getCodecDecodersTotal() )
		return &(m_ImageDecoders.m_pCodecInfo[0]);
	return 0;
}
ImageCodecInfo*	MsGdiplus::getCodecEncoders(unsigned int nIndex)
{
	if( nIndex < getCodecEncodersTotal() )
		return &(m_ImageEncoders.m_pCodecInfo[nIndex]);
	return 0;
}
ImageCodecInfo* MsGdiplus::getCodecDecoders(unsigned int nIndex)
{
	if( nIndex < getCodecDecodersTotal() )
		return &(m_ImageDecoders.m_pCodecInfo[nIndex]);
	return 0;
}
unsigned int	MsGdiplus::getCodecEncodersTotal(void)
{
	return m_ImageEncoders.m_pCodecInfo ? m_ImageEncoders.m_Count:0;
}
unsigned int	MsGdiplus::getCodecDecodersTotal(void)
{
	return m_ImageDecoders.m_pCodecInfo ? m_ImageDecoders.m_Count:0;
}
bool	MsGdiplus::DeterminingFormat(const WCHAR *pExtern,bool bEncoders)
{
	unsigned int unNums = getCodecDecodersTotal();
	ImageCodecInfo	*pCodec = 0;
	bool bRet = false;
	if( pExtern == 0 )	return false;
	std::wstring	szEx,SupportsEx,szTmp(pExtern);
	szEx = _wcsupr((WCHAR*)szTmp.data());
	if( bEncoders == false )
	{
        for (unsigned int n = 0; n<unNums; n++)
		{
			if( pCodec = getCodecEncoders(n) )
			{
				szTmp = pCodec->FilenameExtension;
				SupportsEx = _wcsupr((WCHAR*)szTmp.c_str());
				if( DuiLib::duistring::npos !=  SupportsEx.find(szEx)  )
				{
					bRet = true;
					break;
				}
			}
		}
	}
	else
	{
        for (unsigned int n = 0; n<unNums; n++)
		{
			if( pCodec = getCodecDecoders(n) )
			{
				szTmp = pCodec->FilenameExtension;
				SupportsEx = _wcsupr((WCHAR*)szTmp.c_str());
				if( DuiLib::duistring::npos !=  SupportsEx.find(szEx)  )
				{
					bRet = true;
					break;
				}
			}
		}
	}
	return bRet;
}