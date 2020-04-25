#if !defined(__MICROSOFT_GIDPLUS_IMAGE_BY_JADE_)
#define  __MICROSOFT_GIDPLUS_IMAGE_BY_JADE_

#include "MsGdiplus.h"
#include "Ole2.h"
#include <map>
class MsImage
{
public:
	//多帧图片
	class FramesAndPages
	{
	public:		
		typedef std::map<GUID*,unsigned long>	MAP_FRAME;
		FramesAndPages(Image *pImage);
		~FramesAndPages();
		inline unsigned long Total(void)
		{	return m_unTotal;			}	
		inline unsigned short LoopCount()
		{	return	m_LoopCount;		}
		inline long	getRealDelay()
		{
			long lRet = 0;
            std::vector<LONG>::iterator vtIt;
           
            for (vtIt = m_vtDelay.begin(); vtIt != m_vtDelay.end(); vtIt++)
            {
                lRet += (*vtIt);
            }
            if (lRet)
            {
                lRet = lRet / m_vtDelay.size();
            }
			return lRet;
		}
		UINT ActiveNext(void);
		UINT ActiveFrame(unsigned long dwIndex);
		void getFrameOrPage(unsigned long dwIndex,GUID **pOut,unsigned long &dwReal,long &lDelay);		
	protected:
		void InitFrameDelay(void);
		void InitFrameLoopCount(void);		
	private:
		unsigned int m_unTotal;	
		GUID		 *m_pGuids;		//每帧图片对象GUID
		Image		 *m_pImage;		//
		MAP_FRAME	 m_mapGuid;
		std::vector<LONG>	m_vtDelay;
		unsigned short		m_LoopCount;
		UINT		m_nIndex;		//存储当前帧
	};
	MsImage(MsGdiplus *pPlus);
	~MsImage();
	bool	Initialize(const WCHAR *filename,unsigned int *unFrames);
	bool	Initialize(const char *Imagedata,DWORD dataLen,unsigned int *unFrames);
	bool	DeterminingFormat(bool bEncoders);
	void	AutoRender(HDC hdc,int x,int y);
	void	AutoRender(HDC hdc,int x,int y,int nWidth,int nHeigt);
	void	AutoRender(Graphics *pGdi,int x,int y,int nWidth,int nHeigt);
	void	AutoRender(Graphics *pGdi,int x,int y);
	void	Render(Graphics *pGdi,int x,int y,unsigned int unFrame);
	void	Render(HDC hdc,int x,int y,unsigned int unFrame);

	Image* 	GetThumbnail(const SIZE &sz);

	bool	getPhysicalDimension(float &Width,float &Height);
	inline Image*  getRawImageInterface()
	{	return m_ImageImp;				}
	inline long	getRealDelay()
	{
		if( m_pFrames )
			return (m_pFrames->getRealDelay() * 10);
		return 0;
	}
	inline unsigned short getLoopCount()
	{
		if( m_pFrames )
			return m_pFrames->LoopCount();
		return 0;
	}
	inline	UINT GetWidth()
	{
		if( m_ImageImp )
			return m_ImageImp->GetWidth();
		return -1;
	}
	inline	UINT GetHeight()
	{
		if( m_ImageImp )
			return m_ImageImp->GetHeight();
		return -1;
	}
protected:
	void	ClearStream();
private:
	MsGdiplus	*m_pPlus;		//gdiplus
	Image		*m_ImageImp;
	CComPtr<IStream>	m_pStream;
	FramesAndPages		*m_pFrames;
};

#endif //__MICROSOFT_GIDPLUS_IMAGE_BY_JADE_