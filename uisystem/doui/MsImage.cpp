#include "stdafx.h"
#include "MsImage.h"

MsImage::FramesAndPages::FramesAndPages(Image *pImage)
		:m_pGuids(0),m_unTotal(0),m_LoopCount(0),m_nIndex(0)
{
	if( pImage )
	{
        UINT n = 0;
        UINT unTmp = 0;
        GUID tgFormat;
		UINT unFrameCount = pImage->GetFrameDimensionsCount();
		
		m_pImage = pImage;
		if( unFrameCount )
		{
			if( m_pGuids = new GUID[unFrameCount] )
			{
				//得到子帧的对象列表
				pImage->GetFrameDimensionsList(m_pGuids,unFrameCount);
				for(n=0; n < unFrameCount; n++)
				{
					unTmp = m_pImage->GetFrameCount(&(m_pGuids[n]));
					m_mapGuid.insert(std::make_pair(&(m_pGuids[n]),unTmp));
					m_unTotal += unTmp;					
				}
				if( m_unTotal > 1)
				{
					InitFrameDelay();
					pImage->GetRawFormat(&tgFormat);
					if( tgFormat == ImageFormatGIF )
						InitFrameLoopCount();
				}
			}					
		}
	}
}

MsImage::FramesAndPages::~FramesAndPages()
{
	MAP_FRAME::iterator	  mapIt = m_mapGuid.begin();
	if( m_pGuids )
	{				
		delete []m_pGuids;
		m_pGuids = 0;
		m_unTotal = 0;
		m_mapGuid.clear();
	}
}
UINT MsImage::FramesAndPages::ActiveFrame(unsigned long dwIndex)
{
	GUID *pObj = 0;
	unsigned long dwReal = 0;
	long lDelay = 0;
	if( m_pImage == 0 )	return -1;
	if( dwIndex > m_unTotal  )
		return -1;
	getFrameOrPage(dwIndex,&pObj,dwReal,lDelay);
	if( pObj )
	{
		m_pImage->SelectActiveFrame(pObj,dwReal);
		return 0;
	}
	return -1;
}
UINT MsImage::FramesAndPages::ActiveNext(void)
{
	GUID *pObj = 0;
	unsigned long dwReal = 0;
	long lDelay = 0;
	if( m_pImage == 0 )	return -1;
	if( m_unTotal == 1) return 1;	//唯一数据图
	if( m_nIndex > m_unTotal  )
		m_nIndex = 0;
	getFrameOrPage(m_nIndex,&pObj,dwReal,lDelay);
	if( pObj )
	{
		m_pImage->SelectActiveFrame(pObj,dwReal);
		m_nIndex++;
	}
	return m_nIndex;
}	
void MsImage::FramesAndPages::getFrameOrPage(unsigned long dwIndex,GUID **pOut,unsigned long &dwReal,long &lDelay)
{
	unsigned long dwCount = dwReal,dwTmp = 0 ;
	MAP_FRAME::iterator	  mapIt = m_mapGuid.begin();
	for(mapIt;mapIt != m_mapGuid.end();mapIt++)
	{
		dwTmp = mapIt->second - dwCount;
		if( dwTmp == -1 )
		{
			dwCount = dwCount - mapIt->second;
			continue;
		}
		else
		{
			*pOut = mapIt->first;
			dwReal= dwIndex;
			break;
		}
	}
	if( dwReal < m_vtDelay.size() )
		lDelay = m_vtDelay[dwReal];
	return;
}
void MsImage::FramesAndPages::InitFrameDelay(void)
{
	PropertyItem *pPropertyItem = 0;
	int nSize = m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
	pPropertyItem = (PropertyItem*) malloc(nSize);
	if( pPropertyItem == 0 )	return ;
	if( Ok == m_pImage->GetPropertyItem(PropertyTagFrameDelay,nSize,pPropertyItem) )
	{
		LONG *pValue = (LONG*)pPropertyItem->value;				
		m_vtDelay.clear();
		for(LONG n = 0; n < (LONG)(pPropertyItem->length / sizeof(LONG)); n++)
			m_vtDelay.push_back(pValue[n]);
	}
	free(pPropertyItem);
	pPropertyItem = 0;
}
void MsImage::FramesAndPages::InitFrameLoopCount(void)
{
	PropertyItem *pPropertyItem = 0;
	int nSize = m_pImage->GetPropertyItemSize(PropertyTagLoopCount);
	pPropertyItem = (PropertyItem*) malloc(nSize);
	if( pPropertyItem == 0 )	return ;
	if( Ok == m_pImage->GetPropertyItem(PropertyTagLoopCount,nSize,pPropertyItem) )
	{
		unsigned short *pValue = (unsigned short*)pPropertyItem->value;				
		m_LoopCount = *pValue;
	}
	free(pPropertyItem);
	pPropertyItem = 0;
}
MsImage::MsImage(MsGdiplus *pPlus) throw() : m_pPlus(pPlus),m_pFrames(0)
{
	m_ImageImp = 0;
}
MsImage::~MsImage()
{
	if( m_pFrames )
	{
		delete m_pFrames;
		m_pFrames = 0;
	}
	if( m_ImageImp )
	{
		delete m_ImageImp;
		m_ImageImp = 0;
	}
	ClearStream();
}
bool MsImage::Initialize(const WCHAR *filename,unsigned int *unFrames)
{
	bool	bRet = false;
	UINT	unFrameCount = 0;
	if( m_pPlus == 0 )	return false;
	if( m_ImageImp )	return false;
	if( m_ImageImp = new Image(filename) )
	{
		if( false == DeterminingFormat(false) )
		{
			delete m_ImageImp;
			m_ImageImp = 0; 
			bRet =  false;
		}
		else
		{
			bRet =  true;
			if( m_pFrames = new FramesAndPages(m_ImageImp) )	
			{
				if( unFrames )
					*unFrames = m_pFrames->Total();
			}
		}
	}
	return bRet;
}
void	MsImage::ClearStream()
{
	if( m_pStream.p )
	{
		HGLOBAL hMemory = 0;
		GetHGlobalFromStream(m_pStream,&hMemory);
		if( hMemory )
		{
			GlobalFree(hMemory);
			hMemory = 0;
		}
		m_pStream.Release();
	}
}
bool	MsImage::Initialize(const char *Imagedata,DWORD dataLen,unsigned int *unFrames)
{
	bool bRet = false;
	if( m_pPlus == 0 )	return false;
	if( m_ImageImp )	return false;
	ClearStream();
	HGLOBAL hMemory = GlobalAlloc(GHND,dataLen);
	if(hMemory)
		CreateStreamOnHGlobal(hMemory,TRUE,&m_pStream);
	else
		return false;
	char *pData = (char*)Imagedata;
	for(ULONG n=0,retry=0;(n <dataLen);)
	{
		if( S_OK != (m_pStream->Write(pData+n,dataLen-n,&n)) )
		{
			if( retry == 3)
			{
				bRet = false;
				break;
			}
			retry++;
		}
	}
	if( bRet == false )
	{
		ClearStream();
		return bRet;
	}
	if( m_ImageImp = new Image(m_pStream) )
	{
		if( false == DeterminingFormat(false) )
		{
			delete m_ImageImp;
			m_ImageImp = 0; 
			bRet =  false;
			ClearStream();
		}
		else
		{
			bRet =  true;
			if( m_pFrames = new FramesAndPages(m_ImageImp) )	
			{
				if( unFrames )
					*unFrames = m_pFrames->Total();
			}
		}
	}
	return bRet;
}
bool	MsImage::DeterminingFormat(bool bEncoders)
{
	bool bRet = false;
	if( m_pPlus == 0 )	return false;
	if( m_ImageImp == 0 ) return false;
	GUID tgGuid;
	if( Ok == m_ImageImp->GetRawFormat(&tgGuid) )
		bRet = true;
	return bRet;
}
void	MsImage::AutoRender(Graphics *pGdi,int x,int y)
{
	if( m_ImageImp && pGdi )
	{
		pGdi->DrawImage(m_ImageImp,x,y);
		if( m_pFrames )
			m_pFrames->ActiveNext();
	}
}
void	MsImage::AutoRender(Graphics *pGdi,int x,int y,int nWidth,int nHeigt)
{
	if( m_ImageImp && pGdi )
	{
		pGdi->DrawImage(m_ImageImp,x,y,nWidth,nHeigt);
		if( m_pFrames )
			m_pFrames->ActiveNext();
	}
}
void	MsImage::Render(Graphics *pGdi,int x,int y,unsigned int unFrame)
{
	if( m_ImageImp && pGdi )
	{
		if( m_pFrames )
			m_pFrames->ActiveFrame(unFrame);
		pGdi->DrawImage(m_ImageImp,x,y);
	}
}
void	MsImage::AutoRender(HDC hdc,int x,int y)
{
	Graphics    graphics(hdc);
	if( m_ImageImp )
	{
		graphics.DrawImage(m_ImageImp,x,y);
		if( m_pFrames )
			m_pFrames->ActiveNext();
	}
}
void	MsImage::AutoRender(HDC hdc,int x,int y,int nWidth,int nHeigt)
{
	Graphics    graphics(hdc);
	Rect		tgClipRc;
	if( m_ImageImp )
	{
		tgClipRc.X = x;
		tgClipRc.Y = y;
		tgClipRc.Width = nWidth;
		tgClipRc.Height= nHeigt; 
		graphics.SetClip(tgClipRc);
		graphics.DrawImage(m_ImageImp,x,y,nWidth,nHeigt);
		if( m_pFrames )
			m_pFrames->ActiveNext();
	}
}
void	MsImage::Render(HDC hdc,int x,int y,unsigned int unFrame)
{
	Graphics    graphics(hdc);
	if( m_ImageImp )
	{
		if( m_pFrames )
			m_pFrames->ActiveFrame(unFrame);
		graphics.DrawImage(m_ImageImp,x,y);
	}
}
bool MsImage::getPhysicalDimension(float &Width,float &Height)
{
	SizeF	tgSize;
	if( m_ImageImp )
	{
		if( Ok == m_ImageImp->GetPhysicalDimension(&tgSize) )
		{
			Height = tgSize.Height;
			Width  = tgSize.Width;
			return true;
		}
	}
	return false;
}
Image* 	MsImage::GetThumbnail(const SIZE &sz)
{
	Image* pRet = 0;
	if( m_ImageImp )
	{
		pRet = m_ImageImp->GetThumbnailImage(sz.cx,sz.cy,NULL,NULL);
		if( m_pFrames )
			m_pFrames->ActiveNext();
	}
	return pRet;
}
